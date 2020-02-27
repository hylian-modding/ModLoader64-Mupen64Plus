#ifndef _UTILITY_
#define _UTILITY_
    #include <napi.h>
    using namespace Napi;

    // #########################################################
    // ## Bit Counters
    // #########################################################

    Number npmUtilBitCountBuffer(const CallbackInfo& info);
    Number npmUtilBitCount32(const CallbackInfo& info);
    Number npmUtilBitCount16(const CallbackInfo& info);
    Number npmUtilBitCount8(const CallbackInfo& info);

    // #########################################################
    // ## NAPI Export
    // #########################################################

    Object M64P_Utility_Init(Env env, Object exports);
#endif