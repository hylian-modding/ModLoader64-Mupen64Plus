#ifndef _INTERFACE_
#define _INTERFACE_
    #include <napi.h>
    using namespace Napi;

    // #########################################################
    // ## Paths
    // #########################################################

    Value npmSetSaveDir(const CallbackInfo& info);
    Value npmIsMupenReady(const CallbackInfo& info);

    // #########################################################
    // ## Functions
    // #########################################################

    Number npmCoreEmuState(const CallbackInfo& info);
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
        
    Object M64P_Interface_Init(Env env, Object exports);
#endif