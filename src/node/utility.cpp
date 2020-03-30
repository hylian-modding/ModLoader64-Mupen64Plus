#include "utility.h"

// #########################################################
// ## Bit Counters
// #########################################################

Number npmUtilBitCountBuffer(const CallbackInfo& info)
{
    Uint8Array arr = info[0].As<Uint8Array>();
    uint32_t offset = info[1].As<Number>().Uint32Value();
    uint32_t length = info[2].As<Number>().Uint32Value();

    if (length <= 0 || offset + length > arr.ByteLength())
        length = arr.ByteLength() - offset;

    size_t count = 0;
    for (size_t i = 0; i < length; i++) 
        for (size_t n = 0; n < 8; n++) 
            if (arr[i] & (1 << n)) count++; 
    return Number::New(info.Env(), count);
}

Number npmUtilBitCount32(const CallbackInfo& info)
{
    uint32_t value = info[0].As<Number>().Uint32Value();
    size_t count = 0;
    for (size_t i = 0; i < 32; i++) 
        if (value & (1 << i)) count++; 
    return Number::New(info.Env(), count);
} 

Number npmUtilBitCount16(const CallbackInfo& info)
{
    uint16_t value = info[0].As<Number>().Uint32Value();
    size_t count = 0;
    for (size_t i = 0; i < 16; i++) 
        if (value & (1 << i)) count++;
    return Number::New(info.Env(), count);
} 

Number npmUtilBitCount8(const CallbackInfo& info)
{
    uint8_t value = info[0].As<Number>().Uint32Value();
    size_t count = 0;
    for (size_t i = 0; i < 8; i++) 
        if (value & (1 << i)) count++;
    return Number::New(info.Env(), count);
} 

// #########################################################
// ## NAPI Export
// #########################################################

Object M64P_Utility_Init(Env env, Object exports) {
    // Bit Counters
    exports.Set("utilBitCountBuffer", Function::New(env, npmUtilBitCountBuffer));
    exports.Set("utilBitCount32", Function::New(env, npmUtilBitCount32));
    exports.Set("utilBitCount16", Function::New(env, npmUtilBitCount16));
    exports.Set("utilBitCount8", Function::New(env, npmUtilBitCount8));

    return exports;
}