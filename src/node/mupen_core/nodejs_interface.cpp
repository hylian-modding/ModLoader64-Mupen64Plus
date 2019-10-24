#include <string.h>

#include "nodejs_interface.h"

#include "../../mupen_core/core_interface.h"
#include "../../mupen_core/main_mupen_core.h"
#include "../../mupen_core/plugin.h"

// #########################################################
// ## Paths
// #########################################################

Value npmSetConfigDir(const CallbackInfo& info) {
    std::string input = info[0].As<String>().Utf8Value();
    l_ConfigDirPath = strdup(input.c_str());
    return info.Env().Undefined();
}

Value npmSetDataDir(const CallbackInfo& info) {
#if !defined(SHAREDIR)
    std::string input = info[0].As<String>().Utf8Value();
	l_DataDirPath = strdup(input.c_str());
#endif
    return info.Env().Undefined();
}

Value npmSetPluginDir(const CallbackInfo& info) {
    std::string input = info[0].As<String>().Utf8Value();
    g_PluginDir = strdup(input.c_str());
    return info.Env().Undefined();
}

Value npmSetSaveDir(const CallbackInfo& info) {
    std::string input = info[0].As<String>().Utf8Value();
    (*CoreSaveOverride)(strdup(input.c_str()));
    return info.Env().Undefined();
}

Value npmIsMupenReady(const CallbackInfo& info) {
	u8 value = (*IsMupenReady)();
	if (value == 1) return Boolean::New(info.Env(), true);
	else return Boolean::New(info.Env(), false);
}

// #########################################################
// ## General Functions
// #########################################################

Value npmSetOsdEnabled(const CallbackInfo& info) {
    bool boolVal = info[0].As<Boolean>();
	(*ConfigSetParameter)(l_ConfigCore, "OnScreenDisplay", M64TYPE_BOOL, &boolVal);
    return info.Env().Undefined();
}

Value npmSetFullscreen(const CallbackInfo& info) {
    bool boolVal = info[0].As<Boolean>();
	(*ConfigSetParameter)(l_ConfigVideo, "Fullscreen", M64TYPE_BOOL, &boolVal);
    return info.Env().Undefined();
}

Value npmSetNoSpeedLimit(const CallbackInfo& info) {
	int EnableSpeedLimit = 0;
	if (g_CoreAPIVersion < 0x020001)
		DebugMessage(M64MSG_WARNING, "core library doesn't support --nospeedlimit");
	else if ((*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_SPEED_LIMITER, &EnableSpeedLimit) != M64ERR_SUCCESS)
			DebugMessage(M64MSG_ERROR, "core gave error while setting --nospeedlimit option");	
    return info.Env().Undefined();
}

Value npmSetResolution(const CallbackInfo& info) {
    int xres = info[0].As<Number>().Uint32Value();
    int yres = info[1].As<Number>().Uint32Value();
	if (xres <= 0 || yres <= 0)
		DebugMessage(M64MSG_WARNING, "Resolution values must be greater than 0!");

	(*ConfigSetParameter)(l_ConfigVideo, "ScreenWidth", M64TYPE_INT, &xres);
	(*ConfigSetParameter)(l_ConfigVideo, "ScreenHeight", M64TYPE_INT, &yres);
    return info.Env().Undefined();
}

Value npmSetVerboseLog(const CallbackInfo& info) {
    bool boolVal = info[0].As<Boolean>();
	g_Verbose = boolVal;
    return info.Env().Undefined();
}

// #########################################################
// ## Special Functions
// #########################################################

Number npmCoreEmuState(const CallbackInfo& info) {
    uint32_t value;
    uint32_t err = (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);

    if (!err) return Number::New(info.Env(), value);
    else return Number::New(info.Env(), -1);
}

Number npmInitEmu(const CallbackInfo& info) {
    int ret = Initialize();
    return Number::New(info.Env(), ret);
}

Number npmLoadRom(const CallbackInfo& info) {
    std::string input = info[0].As<String>().Utf8Value();
    l_ROMFilepath = strdup(input.c_str());
	int ret = LoadGame();
    return Number::New(info.Env(), ret);
}

Number npmRunEmulator(const CallbackInfo& info) {
    int ret = Boot();
    return Number::New(info.Env(), ret);
}

Value npmPauseEmulator(const CallbackInfo& info) {
    (*CoreDoCommand)(M64CMD_PAUSE, 0, NULL);
    return info.Env().Undefined();
}

Value npmResumeEmulator(const CallbackInfo& info) {
    (*CoreDoCommand)(M64CMD_RESUME, 0, NULL);
    return info.Env().Undefined();
}

Value npmStopEmulator(const CallbackInfo& info) {
    (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
    return info.Env().Undefined();
}

Value npmSoftReset(const CallbackInfo& info) {
    (*CoreDoCommand)(M64CMD_RESET, 0, NULL);
    return info.Env().Undefined();
}

Value npmHardReset(const CallbackInfo& info) {
    (*CoreDoCommand)(M64CMD_RESET, 1, NULL);
    return info.Env().Undefined();
}

Value npmSaveState(const CallbackInfo& info) {
	std::string napiStr = info[0].As<String>().ToString();
	const char* fileName = napiStr.c_str();
    (*CoreDoCommand)(M64CMD_STATE_SAVE, 1, (void*)fileName);
    return info.Env().Undefined();
}

Value npmLoadState(const CallbackInfo& info) {
	std::string napiStr = info[0].As<String>().ToString();
	const char* fileName = napiStr.c_str();
    (*CoreDoCommand)(M64CMD_STATE_LOAD, 0, (void*)fileName);
    return info.Env().Undefined();
}

/*
static m64p_error ParseCommandLineFinal(int argc, const char **argv)
{
	int i;

	for (i = 1; i < argc; i++)
	{
		
		else if (strcmp(argv[i], "--sshotdir") == 0 && ArgsLeft >= 1)
		{
			(*ConfigSetParameter)(l_ConfigCore, "ScreenshotPath", M64TYPE_STRING, argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "--emumode") == 0 && ArgsLeft >= 1)
		{
			int emumode = atoi(argv[i + 1]);
			i++;
			if (emumode < 0 || emumode > 2)
			{
				DebugMessage(M64MSG_WARNING, "invalid --emumode value '%i'", emumode);
				continue;
			}
			if (emumode == 2 && !(g_CoreCapabilities & M64CAPS_DYNAREC))
			{
				DebugMessage(M64MSG_WARNING, "Emulator core doesn't support Dynamic Recompiler.");
				emumode = 1;
			}
			(*ConfigSetParameter)(l_ConfigCore, "R4300Emulator", M64TYPE_INT, &emumode);
		}
		else if (strcmp(argv[i], "--lua") == 0 && ArgsLeft >= 1)
		{
			(*CoreDoCommand)(M64CMD_LOAD_LUA_SCRIPT, 0, (char*)argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "--python") == 0 && ArgsLeft >= 1)
		{
			(*CoreDoCommand)(M64CMD_LOAD_PYTHON_SCRIPT, 0, (char*)argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "--core-compare-send") == 0)
		{
			l_CoreCompareMode = 1;
		}
		else if (strcmp(argv[i], "--core-compare-recv") == 0)
		{
			l_CoreCompareMode = 2;
		}
		else if (strcmp(argv[i], "--nosaveoptions") == 0)
		{
			l_SaveOptions = 0;
		}
	}

}
*/

// #########################################################
// ## NAPI Export
// #########################################################

Object M64P_Interface_Init(Env env, Object exports) {    
    // Paths
    exports.Set("setConfigDir", Function::New(env, npmSetConfigDir));
    exports.Set("setDataDir", Function::New(env, npmSetDataDir));    
    exports.Set("setPluginDir", Function::New(env, npmSetPluginDir));
    exports.Set("setSaveDir", Function::New(env, npmSetSaveDir));
    exports.Set("isMupenReady", Function::New(env, npmIsMupenReady));
    
    // General Functions
    exports.Set("setOsdEnabled", Function::New(env, npmSetOsdEnabled));
    exports.Set("setFullscreen", Function::New(env, npmSetFullscreen));
    exports.Set("setNoSpeedLimit", Function::New(env, npmSetNoSpeedLimit));    
    exports.Set("setResolution", Function::New(env, npmSetResolution));
    exports.Set("setVerboseLog", Function::New(env, npmSetVerboseLog));
    
    // Special Functions
    exports.Set("coreEmuState", Function::New(env, npmCoreEmuState));
    exports.Set("initialize", Function::New(env, npmInitEmu));
    exports.Set("loadRom", Function::New(env, npmLoadRom));
    exports.Set("runEmulator", Function::New(env, npmRunEmulator));
    exports.Set("pauseEmulator", Function::New(env, npmPauseEmulator));
    exports.Set("resumeEmulator", Function::New(env, npmResumeEmulator));
    exports.Set("stopEmulator", Function::New(env, npmStopEmulator));
	
    exports.Set("softReset", Function::New(env, npmSoftReset));
    exports.Set("hardReset", Function::New(env, npmHardReset));
	
    exports.Set("saveState", Function::New(env, npmSaveState));
    exports.Set("loadState", Function::New(env, npmLoadState));
    
    return exports;
}