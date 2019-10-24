#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL_main.h>
#include "core_compare.h"
#include "core_interface.h"
#include <m64p_types.h>
#include "main_mupen_core.h"
#include "osal_preproc.h"
#include "plugin.h"
#include "version.h"
#include "../node/mupen_core/nodejs_callbacks.h"

  /* Version number for Binding config section parameters */
#define CONFIG_PARAM_VERSION     1.00

/* Fix for legacy functions called by SDLmain.lib */
#ifdef WIN32
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

/** global variables **/
int    g_Verbose = 0;

/** static (local) variables **/
m64p_handle l_ConfigCore = NULL;
m64p_handle l_ConfigVideo = NULL;
m64p_handle l_ConfigUI = NULL;

// Platform specific core preloader
#if WIN32
const char *l_CoreLibPath = ".\\emulator\\mupen64plus.dll";
#else
const char *l_CoreLibPath = "./emulator/mupen64plus.so";
#endif

const char *l_ConfigDirPath = NULL;
const char *l_ROMFilepath = NULL;       // filepath of ROM to load & run at startup

#if defined(SHAREDIR)
const char *l_DataDirPath = SHAREDIR;
#else
const char *l_DataDirPath = NULL;
#endif

int   l_SaveOptions = 1;          // save command-line options in configuration file (enabled by default)
int   l_CoreCompareMode = 0;      // 0 = disable, 1 = send, 2 = receive

/*********************************************************************************************************
 *  Callback functions from the core
 */

void DebugMessage(int level, const char *message, ...)
{
	char msgbuf[1024];
	va_list args;

	va_start(args, message);
	vsnprintf(msgbuf, 1024, message, args);

	DebugCallback("Node", level, msgbuf);

	va_end(args);
}

void DebugCallback(void *Context, int level, const char *message)
{
	if (level == M64MSG_ERROR)
		printf("%s Error: %s\n", (const char *)Context, message);
	else if (level == M64MSG_WARNING)
		printf("%s Warning: %s\n", (const char *)Context, message);
	else if (level == M64MSG_INFO)
		printf("%s: %s\n", (const char *)Context, message);
	else if (level == M64MSG_STATUS)
		printf("%s Status: %s\n", (const char *)Context, message);
	else if (level == M64MSG_VERBOSE)
	{
		if (g_Verbose)
			printf("%s: %s\n", (const char *)Context, message);
	}
	else
		printf("%s Unknown: %s\n", (const char *)Context, message);
}

/*********************************************************************************************************
 *  Configuration handling
 */

static m64p_error OpenConfigurationHandles(void)
{
	float fConfigParamsVersion;
	int bSaveConfig = 0;
	m64p_error rval;

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

	rval = (*ConfigOpenSection)("Node", &l_ConfigUI);
	if (rval != M64ERR_SUCCESS)
	{
		DebugMessage(M64MSG_ERROR, "failed to open 'Node' configuration section");
		return rval;
	}

	if ((*ConfigGetParameter)(l_ConfigUI, "Version", M64TYPE_FLOAT, &fConfigParamsVersion, sizeof(float)) != M64ERR_SUCCESS)
	{
		DebugMessage(M64MSG_WARNING, "No version number in 'Node' config section. Setting defaults.");
		(*ConfigDeleteSection)("Node");
		(*ConfigOpenSection)("Node", &l_ConfigUI);
		bSaveConfig = 1;
	}
	else if (((int)fConfigParamsVersion) != ((int)CONFIG_PARAM_VERSION))
	{
		DebugMessage(M64MSG_WARNING, "Incompatible version %.2f in 'Node' config section: current is %.2f. Setting defaults.", fConfigParamsVersion, (float)CONFIG_PARAM_VERSION);
		(*ConfigDeleteSection)("Node");
		(*ConfigOpenSection)("Node", &l_ConfigUI);
		bSaveConfig = 1;
	}
	else if ((CONFIG_PARAM_VERSION - fConfigParamsVersion) >= 0.0001f)
	{
		/* handle upgrades */
		float fVersion = CONFIG_PARAM_VERSION;
		ConfigSetParameter(l_ConfigUI, "Version", M64TYPE_FLOAT, &fVersion);
		DebugMessage(M64MSG_INFO, "Updating parameter set version in 'Node' config section to %.2f", fVersion);
		bSaveConfig = 1;
	}

	/* Set default values for my Config parameters */
	(*ConfigSetDefaultFloat)(l_ConfigUI, "Version", CONFIG_PARAM_VERSION, "Mupen64Plus Node config parameter set version number.  Please don't change this version number.");
	(*ConfigSetDefaultString)(l_ConfigUI, "PluginDir", OSAL_CURRENT_DIR, "Directory in which to search for plugins");
	(*ConfigSetDefaultString)(l_ConfigUI, "VideoPlugin", "mupen64plus-video-gliden64" OSAL_DLL_EXTENSION, "Filename of video plugin");
	(*ConfigSetDefaultString)(l_ConfigUI, "AudioPlugin", "mupen64plus-audio-sdl" OSAL_DLL_EXTENSION, "Filename of audio plugin");
	(*ConfigSetDefaultString)(l_ConfigUI, "InputPlugin", "mupen64plus-input-sdl" OSAL_DLL_EXTENSION, "Filename of input plugin");
	(*ConfigSetDefaultString)(l_ConfigUI, "RspPlugin", "mupen64plus-rsp-hle" OSAL_DLL_EXTENSION, "Filename of RSP plugin");
	if (bSaveConfig && ConfigSaveSection != NULL) /* ConfigSaveSection was added in Config API v2.1.0 */
		(*ConfigSaveSection)("Node");

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
	if (g_PluginDir != NULL) {
		m64p_error rval;
		m64p_handle glidencfg = NULL;
		rval = (*ConfigOpenSection)("Video-GLideN64", &glidencfg);
		if (rval == M64ERR_SUCCESS) {
			(*ConfigSetParameter)(glidencfg, "txCachePath", M64TYPE_STRING, g_PluginDir);
			DebugMessage(M64MSG_WARNING, "Overwriting txtCachePath in GLideN64.");
		}
	}
	return (*ConfigSaveFile)();
}

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

	/* free allocated memory */
	if (l_TestShotList != NULL)
		free(l_TestShotList);
}

#ifdef WIN32
DWORD WINAPI ExecuteM64PThread(void* data) {
	(*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);
	KillEmuProc();
	return 0;
}
#else
	#include <pthread.h>

/* 
 * Allow external modules to call the main function as a library method.  This is useful for user
 * interfaces that simply layer on top of (rather than re-implement) Node (e.g. mupen64plus-ae).
 */
__attribute__((visibility("default")))

pthread_t threadId;

void* ExecuteM64PThread(void* data) {
	(*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);
	KillEmuProc();
	auto err = pthread_join(threadId, NULL);
	if (err) printf("Failed to join Thread : %s\n", strerror(err));
	return NULL;
}
#endif

int Initialize() {
	/* load the Mupen64Plus core library */
	if (AttachCoreLib(l_CoreLibPath) != M64ERR_SUCCESS)
		return 1;

	/* start the Mupen64Plus core library, load the configuration file */
	m64p_error rval = (*CoreStartup)(CORE_API_VERSION, l_ConfigDirPath, l_DataDirPath, "Core", DebugCallback, NULL, CALLBACK_FUNC);
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

	/* Handle the core comparison feature */
	if (l_CoreCompareMode != 0 && !(g_CoreCapabilities & M64CAPS_CORE_COMPARE))
	{
		DebugMessage(M64MSG_ERROR, "can't use --core-compare feature with this Mupen64Plus core library.");
		DetachCoreLib();
		return 4;
	}
	compare_core_init(l_CoreCompareMode);

	/* save the given command-line options in configuration file if requested */
	if (l_SaveOptions)
		SaveConfigurationOptions();
	
	return 0;
}

int LoadGame() {
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

	// Hook Modloader frame callback
	(*CoreDoCommand)(M64CMD_SET_FRAME_CALLBACK, 0, &M64P_Callback_Frame);

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
