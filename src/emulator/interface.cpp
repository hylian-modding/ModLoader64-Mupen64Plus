#include <string.h>

#ifdef GUI
    #include <m64p-frontend-gui/src/interface.h>
    #include <m64p-frontend-gui/src/main.h>
#else
    #include <m64p-frontend-cli/src/interface.h>
    #include <m64p-frontend-cli/src/main.h>
#endif

#include "callback.h"

// #########################################################
// ## Paths
// #########################################################

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
// ## Functions
// #########################################################

Number npmCoreEmuState(const CallbackInfo& info) {
    uint32_t value;
    uint32_t err = (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);

    if (!err) return Number::New(info.Env(), value);
    else return Number::New(info.Env(), -1);
}

Number npmInitEmu(const CallbackInfo& info) {
    int ret = Main_ModLoader();
    return Number::New(info.Env(), ret);
}

Number npmLoadRom(const CallbackInfo& info) {
    std::string input = info[0].As<String>().Utf8Value();
	int ret = LoadGame(input);
    return Number::New(info.Env(), ret);
}

Number npmRunEmulator(const CallbackInfo& info) {
    int ret = Boot();
    if (ret != 0) return Number::New(info.Env(), ret);

	// Hook Modloader frame callback
	M64P_Callback_Create();
	(*CoreDoCommand)(M64CMD_SET_FRAME_CALLBACK, 0, &M64P_Callback_Frame);
    
    ret = BootThread();
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

// #########################################################
// ## NAPI Export
// #########################################################

Object M64P_Interface_Init(Env env, Object exports) {    
    // Paths
    exports.Set("setSaveDir", Function::New(env, npmSetSaveDir));
    exports.Set("isMupenReady", Function::New(env, npmIsMupenReady));
    
    // Functions
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