#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_main.h>
#include <SDL_thread.h>

#include "osal/imports_osal.h"
#include "cheat.h"
#include "debugger.h"
#include "interface.h"
#include "main.h"
#include "plugin.h"
#include "version.h"

#ifdef VIDEXT_HEADER
#define xstr(s) str(s)
#define str(s) #s
#include xstr(VIDEXT_HEADER)
#endif

/* Version number for UI-Console config section parameters */
#define CONFIG_PARAM_VERSION     1.00

/** global variables **/
int    g_Verbose = 0;

/** static (local) variables **/
m64p_handle l_ConfigCore = NULL;
m64p_handle l_ConfigVideo = NULL;
m64p_handle l_ConfigUI = NULL;
m64p_handle l_ConfigTransferPak = NULL;
m64p_handle l_Config64DD = NULL;

const char *l_CoreLibPath = NULL;
const char *l_ConfigDirPath = NULL;
const char *l_ROMFilepath = NULL;       // filepath of ROM to load & run at startup
const char *l_SaveStatePath = NULL;     // save state to load at startup

#if defined(SHAREDIR)
  const char *l_DataDirPath = SHAREDIR;
#else
  const char *l_DataDirPath = NULL;
#endif

static int  *l_TestShotList = NULL;      // list of screenshots to take for regression test support
static int   l_TestShotIdx = 0;          // index of next screenshot frame in list
       int   l_SaveOptions = 1;          // save command-line options in configuration file (enabled by default)
static int   l_LaunchDebugger = 0;

static eCheatMode l_CheatMode = CHEAT_DISABLE;
static char      *l_CheatNumList = NULL;

/*********************************************************************************************************
 *  Callback functions from the core
 */

void DebugMessage(int level, const char *message, ...)
{
  char msgbuf[1024];
  va_list args;

  va_start(args, message);
  vsnprintf(msgbuf, 1024, message, args);

  DebugCallback((void*)"M64N", level, msgbuf);

  va_end(args);
}

void DebugCallback(void *Context, int level, const char *message)
{
#ifdef ANDROID
    if (level == M64MSG_ERROR)
        __android_log_print(ANDROID_LOG_ERROR, (const char *) Context, "%s", message);
    else if (level == M64MSG_WARNING)
        __android_log_print(ANDROID_LOG_WARN, (const char *) Context, "%s", message);
    else if (level == M64MSG_INFO)
        __android_log_print(ANDROID_LOG_INFO, (const char *) Context, "%s", message);
    else if (level == M64MSG_STATUS)
        __android_log_print(ANDROID_LOG_DEBUG, (const char *) Context, "%s", message);
    else if (level == M64MSG_VERBOSE)
    {
        if (g_Verbose)
            __android_log_print(ANDROID_LOG_VERBOSE, (const char *) Context, "%s", message);
    }
    else
        __android_log_print(ANDROID_LOG_ERROR, (const char *) Context, "Unknown: %s", message);
#else
    if (level == M64MSG_ERROR)
        printf("%s Error: %s\n", (const char *) Context, message);
    else if (level == M64MSG_WARNING)
        printf("%s Warning: %s\n", (const char *) Context, message);
    else if (level == M64MSG_INFO)
        printf("%s: %s\n", (const char *) Context, message);
    else if (level == M64MSG_STATUS)
        printf("%s Status: %s\n", (const char *) Context, message);
    else if (level == M64MSG_VERBOSE)
    {
        if (g_Verbose)
            printf("%s: %s\n", (const char *) Context, message);
    }
    else
        printf("%s Unknown: %s\n", (const char *) Context, message);
#endif
}

static void FrameCallback(unsigned int FrameIndex)
{
    // take a screenshot if we need to
    if (l_TestShotList != NULL)
    {
        int nextshot = l_TestShotList[l_TestShotIdx];
        if (nextshot == FrameIndex)
        {
            (*CoreDoCommand)(M64CMD_TAKE_NEXT_SCREENSHOT, 0, NULL);  /* tell the core take a screenshot */
            // advance list index to next screenshot frame number.  If it's 0, then quit
            l_TestShotIdx++;
        }
        else if (nextshot == 0)
        {
            (*CoreDoCommand)(M64CMD_STOP, 0, NULL);  /* tell the core to shut down ASAP */
            free(l_TestShotList);
            l_TestShotList = NULL;
        }
    }
}

static char *formatstr(const char *fmt, ...) ATTR_FMT(1, 2);

char *formatstr(const char *fmt, ...)
{
	int size = 128, ret;
	char *str = (char *)malloc(size), *newstr;
	va_list args;

	/* There are two implementations of vsnprintf we have to deal with:
	 * C99 version: Returns the number of characters which would have been written
	 *              if the buffer had been large enough, and -1 on failure.
	 * Windows version: Returns the number of characters actually written,
	 *                  and -1 on failure or truncation.
	 * NOTE: An implementation equivalent to the Windows one appears in glibc <2.1.
	 */
	while (str != NULL)
	{
		va_start(args, fmt);
		ret = vsnprintf(str, size, fmt, args);
		va_end(args);

		// Successful result?
		if (ret >= 0 && ret < size)
			return str;

		// Increment the capacity of the buffer
		if (ret >= size)
			size = ret + 1; // C99 version: We got the needed buffer size
		else
			size *= 2; // Windows version: Keep guessing

		newstr = (char *)realloc(str, size);
		if (newstr == NULL)
			free(str);
		str = newstr;
	}

	return NULL;
}

static int is_path_separator(char c)
{
    return strchr((const char*)OSAL_DIR_SEPARATOR, c) != NULL;
}

char* combinepath(const char* first, const char *second)
{
    size_t len_first, off_second = 0;

    if (first == NULL || second == NULL)
        return NULL;

    len_first = strlen(first);

    while (is_path_separator(first[len_first-1]))
        len_first--;

    while (is_path_separator(second[off_second]))
        off_second++;

    return formatstr("%.*s%c%s", (int) len_first, first, OSAL_DIR_SEPARATOR, second + off_second);
}


/*********************************************************************************************************
 *  Configuration handling
 */

static m64p_error OpenConfigurationHandles(void)
{
    float fConfigParamsVersion;
    int bSaveConfig = 0;
    m64p_error rval;
    unsigned int i;

    /* Open Configuration sections for core library and console User Interface */
    rval = (*ConfigOpenSection)("Core", &l_ConfigCore);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'Core' configuration section");
        return rval;
    }

    rval = (*ConfigOpenSection)("Video-General", &l_ConfigVideo);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'Video-General' configuration section");
        return rval;
    }

    rval = (*ConfigOpenSection)("Transferpak", &l_ConfigTransferPak);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'Transferpak' configuration section");
        return rval;
    }

    rval = (*ConfigOpenSection)("64DD", &l_Config64DD);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open '64DD' configuration section");
        return rval;
    }

    rval = (*ConfigOpenSection)("M64N", &l_ConfigUI);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'M64N' configuration section");
        return rval;
    }

    if ((*ConfigGetParameter)(l_ConfigUI, "Version", M64TYPE_FLOAT, &fConfigParamsVersion, sizeof(float)) != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_WARNING, "No version number in 'M64N' config section. Setting defaults.");
        (*ConfigDeleteSection)("M64N");
        (*ConfigOpenSection)("M64N", &l_ConfigUI);
        bSaveConfig = 1;
    }
    else if (((int) fConfigParamsVersion) != ((int) CONFIG_PARAM_VERSION))
    {
        DebugMessage(M64MSG_WARNING, "Incompatible version %.2f in 'M64N' config section: current is %.2f. Setting defaults.", fConfigParamsVersion, (float) CONFIG_PARAM_VERSION);
        (*ConfigDeleteSection)("M64N");
        (*ConfigOpenSection)("M64N", &l_ConfigUI);
        bSaveConfig = 1;
    }
    else if ((CONFIG_PARAM_VERSION - fConfigParamsVersion) >= 0.0001f)
    {
        /* handle upgrades */
        float fVersion = CONFIG_PARAM_VERSION;
        ConfigSetParameter(l_ConfigUI, "Version", M64TYPE_FLOAT, &fVersion);
        DebugMessage(M64MSG_INFO, "Updating parameter set version in 'UI-Console' config section to %.2f", fVersion);
        bSaveConfig = 1;
    }

    /* Set default values for my Config parameters */
    (*ConfigSetDefaultFloat)(l_ConfigUI, "Version", CONFIG_PARAM_VERSION,  "Mupen64Plus M64N config parameter set version number.  Please don't change this version number.");
    (*ConfigSetDefaultString)(l_ConfigUI, "PluginDir", OSAL_CURRENT_DIR, "Directory in which to search for plugins");
    (*ConfigSetDefaultString)(l_ConfigUI, "VideoPlugin", "mupen64plus-video-gliden64" OSAL_DLL_EXTENSION, "Filename of video plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "AudioPlugin", "mupen64plus-audio-sdl" OSAL_DLL_EXTENSION, "Filename of audio plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "InputPlugin", "mupen64plus-input-sdl" OSAL_DLL_EXTENSION, "Filename of input plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "RspPlugin", "mupen64plus-rsp-hle" OSAL_DLL_EXTENSION, "Filename of RSP plugin");

    for(i = 1; i < 5; ++i) {
        char key[64];
        char desc[2048];
#define SET_DEFAULT_STRING(key_fmt, default_value, desc_fmt) \
        do { \
            snprintf(key, sizeof(key), key_fmt, i); \
            snprintf(desc, sizeof(desc), desc_fmt, i); \
            (*ConfigSetDefaultString)(l_ConfigTransferPak, key, default_value, desc); \
        } while(0)

        SET_DEFAULT_STRING("GB-rom-%u", "", "Filename of the GB ROM to load into transferpak %u");
        SET_DEFAULT_STRING("GB-ram-%u", "", "Filename of the GB RAM to load into transferpak %u");
#undef SET_DEFAULT_STRING
    }

    (*ConfigSetDefaultString)(l_Config64DD, "IPL-ROM", "", "Filename of the 64DD IPL ROM");
    (*ConfigSetDefaultString)(l_Config64DD, "Disk", "", "Filename of the disk to load into Disk Drive");

    if (bSaveConfig && l_SaveOptions && ConfigSaveSection != NULL) { /* ConfigSaveSection was added in Config API v2.1.0 */
        (*ConfigSaveSection)("M64N");
        (*ConfigSaveSection)("Transferpak");
    }

    return M64ERR_SUCCESS;
}

static m64p_error SaveConfigurationOptions(void)
{
    /* if shared data directory was given on the command line, write it into the config file */
    if (l_DataDirPath != NULL)
        (*ConfigSetParameter)(l_ConfigCore, "SharedDataPath", M64TYPE_STRING, l_DataDirPath);

    /* if any plugin filepaths were given on the command line, write them into the config file */
    if (g_PluginDir != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "PluginDir", M64TYPE_STRING, g_PluginDir);
    if (g_GfxPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "VideoPlugin", M64TYPE_STRING, g_GfxPlugin);
    if (g_AudioPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "AudioPlugin", M64TYPE_STRING, g_AudioPlugin);
    if (g_InputPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "InputPlugin", M64TYPE_STRING, g_InputPlugin);
    if (g_RspPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "RspPlugin", M64TYPE_STRING, g_RspPlugin);

    if ((*ConfigHasUnsavedChanges)(NULL))
        return (*ConfigSaveFile)();
    else
        return M64ERR_SUCCESS;
}

/*********************************************************************************************************
 *  Command-line parsing
 */

static char* media_loader_get_filename(void* cb_data, m64p_handle section_handle, const char* section, const char* key)
{
#define MUPEN64PLUS_CFG_NAME "mupen64plus.cfg"
    m64p_handle core_config;
    char value[4096];
    const char* configdir = NULL;
    char* cfgfilepath = NULL;

    /* reset filename */
    char* mem_filename = NULL;

    /* XXX: use external config API to force reload of file content */
    configdir = ConfigGetUserConfigPath();
    if (configdir == NULL) {
        DebugMessage(M64MSG_ERROR, "Can't get user config path !");
        return NULL;
    }

    cfgfilepath = combinepath(configdir, MUPEN64PLUS_CFG_NAME);
    if (cfgfilepath == NULL) {
        DebugMessage(M64MSG_ERROR, "Can't get config file path: %s + %s!", configdir, MUPEN64PLUS_CFG_NAME);
        return NULL;
    }

    if (ConfigExternalOpen(cfgfilepath, &core_config) != M64ERR_SUCCESS) {
        DebugMessage(M64MSG_ERROR, "Can't open config file %s!", cfgfilepath);
        free(cfgfilepath);
        return mem_filename;
    }

    if (ConfigExternalGetParameter(core_config, section, key, value, sizeof(value)) != M64ERR_SUCCESS) {
        DebugMessage(M64MSG_ERROR, "Can't get parameter %s", key);
        ConfigExternalClose(core_config);
        free(cfgfilepath);
        return mem_filename;
    }

    size_t len = strlen(value);
    if (len < 2 || value[0] != '"' || value[len-1] != '"') {
        DebugMessage(M64MSG_ERROR, "Invalid string format %s", value);
        ConfigExternalClose(core_config);
        free(cfgfilepath);
        return mem_filename;
    }

    value[len-1] = '\0';
    mem_filename = strdup(value + 1);

    ConfigSetParameter(section_handle, key, M64TYPE_STRING, mem_filename);
    ConfigExternalClose(core_config);
    free(cfgfilepath);
    return mem_filename;
}


static char* media_loader_get_gb_cart_mem_file(void* cb_data, const char* mem, int control_id)
{
    char key[64];

    snprintf(key, sizeof(key), "GB-%s-%u", mem, control_id + 1);
    return media_loader_get_filename(cb_data, l_ConfigTransferPak, "Transferpak", key);
}

static char* media_loader_get_gb_cart_rom(void* cb_data, int control_id)
{
    return media_loader_get_gb_cart_mem_file(cb_data, "rom", control_id);
}

static char* media_loader_get_gb_cart_ram(void* cb_data, int control_id)
{
    return media_loader_get_gb_cart_mem_file(cb_data, "ram", control_id);
}

static char* media_loader_get_dd_rom(void* cb_data)
{
    return media_loader_get_filename(cb_data, l_Config64DD, "64DD", "IPL-ROM");
}

static char* media_loader_get_dd_disk(void* cb_data)
{
    return media_loader_get_filename(cb_data, l_Config64DD, "64DD", "Disk");
}

static m64p_media_loader l_media_loader =
{
    NULL,
    media_loader_get_gb_cart_rom,
    media_loader_get_gb_cart_ram,
    media_loader_get_dd_rom,
    media_loader_get_dd_disk
};


/*********************************************************************************************************
* main function
*/

/* Allow state callback in external module to be specified via build flags (header and function name) */
#ifdef CALLBACK_HEADER
#define xstr(s) str(s)
#define str(s) #s
#include xstr(CALLBACK_HEADER)
#endif

#ifndef CALLBACK_FUNC
#define CALLBACK_FUNC NULL
#endif

/*********************************************************************************************************
* modloader functions
*/

void KillEmuProc(void) {
	int i;

	/* detach plugins from core and unload them */
	for (i = 0; i < 4; i++)
		(*CoreDetachPlugin)(g_PluginMap[i].type);
	PluginUnload();

	/* close the ROM image */
	(*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);

	/* save the configuration file again if --nosaveoptions was not specified, to keep any updated parameters from the core/plugins */
	if (l_SaveOptions)
		SaveConfigurationOptions();

	/* Shut down and release the Core library */
	(*CoreShutdown)();
	DetachCoreLib();
}

#ifdef WIN32
DWORD WINAPI ExecuteM64PThread(void* data) {
	(*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);
	KillEmuProc();
	return 0;
}
#else
	#include <pthread.h>

pthread_t threadId;

void* ExecuteM64PThread(void* data) {
	(*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);
	KillEmuProc();
	auto err = pthread_join(threadId, NULL);
	if (err) printf("Failed to join Thread : %s\n", strerror(err));
	return NULL;
}
#endif

int Main_ModLoader() {
    // Platform specific core preloader
    #if WIN32
        l_CoreLibPath = ".\\emulator\\mupen64plus.dll";
        l_ConfigDirPath = ".\\emulator";
        l_DataDirPath = ".\\emulator";
        g_PluginDir = ".\\emulator";
    #else
        l_CoreLibPath =  "./emulator/mupen64plus.so";
        l_ConfigDirPath = "./emulator";
        l_DataDirPath = "./emulator";
        g_PluginDir = "./emulator";
    #endif

	/* load the Mupen64Plus core library */
	if (AttachCoreLib(l_CoreLibPath) != M64ERR_SUCCESS)
		return 1;

	/* start the Mupen64Plus core library, load the configuration file */
	m64p_error rval = (*CoreStartup)(CORE_API_VERSION, l_ConfigDirPath, l_DataDirPath, (void*)"Core", DebugCallback, NULL, CALLBACK_FUNC);
	if (rval != M64ERR_SUCCESS)
	{
		DebugMessage(M64MSG_ERROR, "couldn't start Mupen64Plus core library.");
		DetachCoreLib();
		return 2;
	}

	/* Open configuration sections */
	rval = OpenConfigurationHandles();
	if (rval != M64ERR_SUCCESS)
	{
		(*CoreShutdown)();
		DetachCoreLib();
		return 3;
	}

	/* save the given command-line options in configuration file if requested */
	if (l_SaveOptions)
		SaveConfigurationOptions();
	
	return 0;
}

int LoadGame(std::string romPath) {
    l_ROMFilepath = romPath.c_str();

    /* load ROM image */
    FILE *fPtr = fopen(l_ROMFilepath, "rb");
    if (fPtr == NULL)
    {
        DebugMessage(M64MSG_ERROR, "couldn't open ROM file '%s' for reading.", l_ROMFilepath);
        (*CoreShutdown)();
        DetachCoreLib();
        return -1;
    }
    /* get the length of the ROM, allocate memory buffer, load it from disk */
    long romlength = 0;
    fseek(fPtr, 0L, SEEK_END);
    romlength = ftell(fPtr);
    fseek(fPtr, 0L, SEEK_SET);
    unsigned char *ROM_buffer = (unsigned char *)malloc(romlength);
    if (ROM_buffer == NULL)
    {
        DebugMessage(M64MSG_ERROR, "couldn't allocate %li-byte buffer for ROM image file '%s'.", romlength, l_ROMFilepath);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return -2;
    }
    else if (fread(ROM_buffer, 1, romlength, fPtr) != romlength)
    {
        DebugMessage(M64MSG_ERROR, "couldn't read %li bytes from ROM image file '%s'.", romlength, l_ROMFilepath);
        free(ROM_buffer);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return -3;
    }
    fclose(fPtr);

    size_t newLen = 1024 * 1024 * 64;
    unsigned char *newRom = (unsigned char *)malloc(newLen);
    memcpy(newRom, ROM_buffer, romlength);

    /* Try to load the ROM image into the core */
    if ((*CoreDoCommand)(M64CMD_ROM_OPEN, (int)newLen, newRom) != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "core failed to open ROM image file '%s'.", l_ROMFilepath);
        free(ROM_buffer);
        free(newRom);
        (*CoreShutdown)();
        DetachCoreLib();
        return -4;
    }
    free(ROM_buffer); /* the core copies the ROM image, so we can release this buffer immediately */
    free(newRom);
    return (int)romlength;
}

int Boot() {
	int i;

	/* search for and load plugins */
	m64p_error rval = PluginSearchLoad(l_ConfigUI);
	if (rval != M64ERR_SUCCESS)
	{
		(*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
		(*CoreShutdown)();
		DetachCoreLib();
		return 2;
	}

	/* attach plugins to core */
	for (i = 0; i < 4; i++)
	{
		if ((*CoreAttachPlugin)(g_PluginMap[i].type, g_PluginMap[i].handle) != M64ERR_SUCCESS)
		{
			DebugMessage(M64MSG_ERROR, "core error while attaching %s plugin.", g_PluginMap[i].name);
			(*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
			(*CoreShutdown)();
			DetachCoreLib();
			return 3;
		}
	}

    return 0;
}

int BootThread() {
	/* run the game */
#ifdef WIN32
	HANDLE thread = CreateThread(NULL, 0, ExecuteM64PThread, NULL, 0, NULL);
	if (thread == NULL) {
		printf("Failed to start M64P Async process.\n");
		return 1;
	}
#else
	int thread = pthread_create(&threadId, NULL, &ExecuteM64PThread, NULL);
	if (thread != 0) {
		printf("Failed to start M64P Async process.\n");
		return 1;
	}
#endif

	return 0;
}
