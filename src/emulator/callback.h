#ifndef _CALLBACKS_
#define _CALLBACKS_
    #include <napi.h>
    using namespace Napi;

    int GetFrameCount();
    
    void M64P_Callback_Create(void);
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