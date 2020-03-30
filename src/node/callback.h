#ifndef NODEJS_CALLBACKS_H
#define NODEJS_CALLBACKS_H
    #include <napi.h>
    using namespace Napi;

    int GetFrameCount();
    
    void M64P_Callback_Init(void);
    void M64P_Callback_Frame(unsigned int frameIndex);
    void M64P_Callback_Destroy(void);

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