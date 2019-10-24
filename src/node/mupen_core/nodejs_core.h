#ifndef _NODEJS_CORE_
#define _NODEJS_CORE_
    #include <napi.h>
    using namespace Napi;

    Number npmCoreEmuState(const CallbackInfo& info);
    
    // #########################################################
    // ## NAPI Export
    // #########################################################

    Object M64P_Core_Init(Env env, Object exports);
#endif