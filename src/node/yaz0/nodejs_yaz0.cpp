#include <m64p_types.h>

#include "nodejs_yaz0.h"

#include "../../../subprojects/yaz0/includes.h"

// #########################################################
// ## Yaz0 Functions
// #########################################################

Buffer<uint8_t> Yaz0_Encode(const CallbackInfo& info)
{
    Buffer<uint8_t> buf = info[0].As<Buffer<uint8_t>>();
    size_t len = buf.ByteLength();
    uint8_t* yBuf = (uint8_t*)buf.Data();

    auto yaz0 = Yaz0::Creator();
    if (!yaz0.encode(yBuf, len, -1))
        printf("Yazo failed to encode the data!\n");

    return Buffer<uint8_t>::New(info.Env(), yaz0.getData(), yaz0.getSize());;
}

Buffer<uint8_t> Yaz0_Decode(const CallbackInfo& info)
{
    Buffer<uint8_t> buf = info[0].As<Buffer<uint8_t>>();
    size_t len = buf.ByteLength();
    uint8_t* yBuf = (uint8_t*)buf.Data();

    auto yaz0 = Yaz0::Parser();
    if (!yaz0.decode(yBuf, len, -1))
        printf("Yazo failed to parse the data!\n");

    return Buffer<uint8_t>::New(info.Env(), yaz0.getData(), yaz0.getSize());;
}

// #########################################################
// ## NAPI Export
// #########################################################

Object Yaz0_Init(Env env, Object exports) {
    exports.Set("yaz0Encode", Function::New(env, Yaz0_Encode));
    exports.Set("yaz0Decode", Function::New(env, Yaz0_Decode));

    return exports;
}