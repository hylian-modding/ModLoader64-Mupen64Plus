#ifndef _NODEJS_INTERFACE_
#define _NODEJS_INTERFACE_
    #include <napi.h>
    using namespace Napi;

    // #########################################################
    // ## Paths
    // #########################################################

    Value npmSetCoreLib(const CallbackInfo& info);
    Value npmSetConfigDir(const CallbackInfo& info);
    Value npmSetDataDir(const CallbackInfo& info);
    Value npmSetPluginDir(const CallbackInfo& info);
    Value npmSetSaveDir(const CallbackInfo& info);
    Value npmIsMupenReady(const CallbackInfo& info);

    // #########################################################
    // ## General Functions
    // #########################################################

    Value npmSetOsdEnabled(const CallbackInfo& info);
    Value npmSetFullscreen(const CallbackInfo& info);
    Value npmSetNoSpeedLimit(const CallbackInfo& info);
    Value npmSetResolution(const CallbackInfo& info);
    Value npmSetVerboseLog(const CallbackInfo& info);

    // #########################################################
    // ## Special Functions
    // #########################################################

    Number npmInitEmu(const CallbackInfo& info);
    Number npmLoadRom(const CallbackInfo& info);
    Number npmRunEmulator(const CallbackInfo& info);
    Value npmPauseEmulator(const CallbackInfo& info);
    Value npmResumeEmulator(const CallbackInfo& info);
    Value npmStopEmulator(const CallbackInfo& info);
    
    Value npmSoftReset(const CallbackInfo& info);
    Value npmHardReset(const CallbackInfo& info);

    Value npmSaveState(const CallbackInfo& info);
    Value npmLoadState(const CallbackInfo& info);
        
    // #########################################################
    // ## NAPI Export
    // #########################################################

    Object M64P_Interface_Init(Env env, Object exports);
#endif