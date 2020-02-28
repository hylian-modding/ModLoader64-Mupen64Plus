#ifndef NODEJS_YAZ0_H
#define NODEJS_YAZ0_H
    #include <napi.h>
    using namespace Napi;

    // #########################################################
    // ## Yaz0 Functions
    // #########################################################

    Buffer<uint8_t> Yaz0_Encode(const CallbackInfo& info);
    Buffer<uint8_t> Yaz0_Decode(const CallbackInfo& info);

    // #########################################################
    // ## NAPI Export
    // #########################################################

    Object Yaz0_Init(Env env, Object exports);
#endif