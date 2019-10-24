#ifndef _M64B_CALLBACKS_
#define _M64B_CALLBACKS_
    #include <napi.h>
    using namespace Napi;

    void M64P_Callback_Frame(unsigned int frameIndex);

    // #########################################################
    // ## Node Functions
    // #########################################################

    Number npmGetFrameCount(const CallbackInfo& info);
    Value npmSetFrameCount(const CallbackInfo& info);

    // #########################################################
    // ## NAPI Export
    // #########################################################

    Object M64P_Callback_Init(Env env, Object exports);
#endif