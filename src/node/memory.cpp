#include "memory.h"

#include <chrono>
#include <ctime>
#include <thread>
#include <fstream>
#include <memory>
#include <iostream>
#include <string>
#include <cstdio>
using namespace std;

#include "../frontend/interface.h"
#include "callback.h"

bool debugPrinting = false;

template<typename ... Args>
void log(const std::string& format, Args ... args) {
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
    unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    string input = string( buf.get(), buf.get() + size - 1 );

    time_t now = time(0);
    string output = input + string(": ") + string(ctime(&now));
    std::ofstream outfile;
    outfile.open("m64p_memory-log.txt", std::ios_base::app);
    outfile << output;
}

// #########################################################
// ## Bit Utility
// #########################################################

const uint8_t* setBitsBuffer(Uint8Array bitField, size_t len) {
    const uint8_t* arr = (const uint8_t*)malloc(len);
    uint8_t value;

    for (size_t i = 0; i < len; i++) {
        value = 0;
        for (size_t n = 0; n < 8; n++)
            if (bitField[i * 8 + n] != 0) value |= (1 << (7 - n));
        memcpy((void*)(arr + i), (void*)(&value), 1);
    }

    return arr;
}

uint8_t setBits8(Uint8Array bitField) {
    uint8_t value;
    for (size_t i = 0; i < 8; i++)
        if (bitField[i] != 0) value |= (1 << (7 - i));
    return value;
}

uint8_t setBit8(uint8_t value, size_t offset, bool set) {
    if (set) value |= (1 << (7 - offset));
    else value &= ~(1 << (7 - offset));
    return value;
}

// #########################################################
// ## RDRam Memory
// #########################################################

Number npmDereferencePointer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint32_t value = (*RdRamRead32)(addr) & 0x00FFFFFF;   
    return Number::New(info.Env(), value);
}

// -------------------------------------------

Buffer<uint8_t> npmRdRamReadBuffer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    size_t len = info[1].As<Number>().Uint32Value();
    
    const uint8_t* mem = (*RdRamReadBuffer)(addr, len);
    return Buffer<uint8_t>::New(info.Env(), (uint8_t*)mem, len);
}

Number npmRdRamReadF32(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint32_t val = (*RdRamRead32)(addr);    
    float value;
    memcpy((void*)(&value), (void*)(&val), 4);
    return Number::New(info.Env(), value);
}

Number npmRdRamRead32(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint32_t value = (*RdRamRead32)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamRead16(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint16_t value = (*RdRamRead16)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamRead8(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint8_t value = (*RdRamRead8)(addr);    
    return Number::New(info.Env(), value);
}

// -------------------------------------------

Buffer<uint8_t> npmRdRamReadBitsBuffer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    size_t len = info[1].As<Number>().Uint32Value();

    const uint8_t* mem = (*RdRamReadBuffer)(addr, len);
    Buffer<uint8_t> arr = Buffer<uint8_t>::New(info.Env(), len * 8);

    for (size_t i = 0; i < len; i++)
        for (size_t n = 0; n < 8; n++)
            arr.Set(i * 8 + n, (mem[i] & (1 << (7 - n))) != 0);

    return arr;
}

Buffer<uint8_t> npmRdRamReadBits8(const CallbackInfo& info) {
    Buffer<uint8_t> arr = Buffer<uint8_t>::New(info.Env(), 8);
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint8_t value = (*RdRamRead8)(addr);

    for (size_t i = 0; i < 8; i++)
        arr.Set(i, (value & (1 << (7 - i))) != 0);

    return arr;
}

Boolean npmRdRamReadBit8(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint32_t offset = info[1].As<Number>().Uint32Value();
    uint8_t value = (*RdRamRead8)(addr);
    return Boolean::New(info.Env(), (value & (1 << (7 - offset))) != 0);
}

// -------------------------------------------

Number npmRdRamReadS32(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    int32_t value = (*RdRamRead32)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamReadS16(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    int16_t value = (*RdRamRead16)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamReadS8(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    int8_t value = (*RdRamRead8)(addr);    
    return Number::New(info.Env(), value);
}

// -------------------------------------------

Value npmRdRamWriteBuffer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    Uint8Array arr = info[1].As<Uint8Array>();
    size_t len = arr.ByteLength();

    if (debugPrinting)
        log("RdRamWriteBuffer{ addr[%#010x], len[%i] }", addr, len);

    for (size_t i = 0; i < len; i++)
        (*RdRamWrite8)(addr + i, arr[i]);

    return info.Env().Undefined();
}

Value npmRdRamWriteF32(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    float value = info[1].As<Number>().FloatValue();

    if (debugPrinting)
        log("RdRamWriteF32{ addr[%#010x], val[%f] }", addr, value);

    uint32_t val;
    memcpy((void*)(&val), (void*)(&value), 4);
    (*RdRamWrite32)(addr, val);  
    return info.Env().Undefined();
}

Value npmRdRamWrite32(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint32_t value = info[1].As<Number>().Uint32Value();

    if (debugPrinting)
        log("RdRamWrite32{ addr[%#010x], val[%#010x] }", addr, value);
    
    (*RdRamWrite32)(addr, value);    
    return info.Env().Undefined();
}

Value npmRdRamWrite16(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint16_t value = info[1].As<Number>().Uint32Value();

    if (debugPrinting)
        log("RdRamWrite16{ addr[%#010x], val[%#06x] }", addr, value);
    
    (*RdRamWrite16)(addr, value);    
    return info.Env().Undefined();
}

Value npmRdRamWrite8(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    uint8_t value = info[1].As<Number>().Uint32Value();

    if (debugPrinting)
        log("RdRamWrite8{ addr[%#010x], val[%#04x] }", addr, value);
    
    (*RdRamWrite8)(addr, value);    
    return info.Env().Undefined();
}

// -------------------------------------------

Value npmRdRamWriteBitsBuffer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    Uint8Array arr = info[1].As<Uint8Array>();
    size_t len = arr.ByteLength() / 8;
    const uint8_t* val = setBitsBuffer(arr, len);

    if (debugPrinting)
        log("RdRamWriteBitsBuffer{ addr[%#010x], val[%i] }", addr, val);
        
    for (size_t i = 0; i < len; i++) 
        (*RdRamWrite8)(addr + i, val[i]);    
    return info.Env().Undefined();
}

Value npmRdRamWriteBits8(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    Uint8Array value = info[1].As<Uint8Array>();
    uint8_t val = setBits8(value);

    if (debugPrinting)
        log("RdRamWriteBits8{ addr[%#010x], val[%#04x] }", addr, val);
    
    (*RdRamWrite8)(addr, val);    
    return info.Env().Undefined();
}

Value npmRdRamWriteBit8(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    size_t offset = info[1].As<Number>().Uint32Value();
    bool set = info[2].As<Number>().ToBoolean();
    uint8_t val = (*RdRamRead8)(addr);

    if (debugPrinting)
        log("RdRamWriteBit8{ addr[%#010x], val[%#04x] }", addr, val);
    
    (*RdRamWrite8)(addr, setBit8(val, offset, set));    
    return info.Env().Undefined();
}

// #########################################################
// ## RDRam Memory (Pointer Based)
// #########################################################

Buffer<uint8_t> npmRdRamReadPtrBuffer(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    size_t len = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    const uint8_t* mem = (*RdRamReadBuffer)(addr, len);
    return Buffer<uint8_t>::New(info.Env(), (uint8_t*)mem, len);;
}

Number npmRdRamReadPtrF32(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    const uint8_t* mem = (*RdRamReadBuffer)(addr, 4);
    uint32_t val = (*RdRamRead32)(addr);    
    float value;
    memcpy((void*)(&value), (void*)(&val), 4);
    return Number::New(info.Env(), value);
}

Number npmRdRamReadPtr32(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    uint32_t value = (*RdRamRead32)(addr);
    return Number::New(info.Env(), value);
}

Number npmRdRamReadPtr16(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    uint16_t value = (*RdRamRead16)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamReadPtr8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    uint8_t value = (*RdRamRead8)(addr);    
    return Number::New(info.Env(), value);
}

// -------------------------------------------

Buffer<uint8_t> npmRdRamReadPtrBitsBuffer(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    size_t len = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    const uint8_t* mem = (*RdRamReadBuffer)(addr, len);
    Buffer<uint8_t> arr = Buffer<uint8_t>::New(info.Env(), len * 8);

    for (size_t i = 0; i < len; i++)
        for (size_t n = 0; n < 8; n++)
            arr.Set(i * 8 + n, (mem[i] & (1 << (7 - n))) != 0);

    return arr;
}

Buffer<uint8_t> npmRdRamReadPtrBits8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    Buffer<uint8_t> arr = Buffer<uint8_t>::New(info.Env(), 8);
    uint8_t value = (*RdRamRead8)(addr);

    for (size_t i = 0; i < 8; i++)
        arr.Set(i, (value & (1 << (7 - i))) != 0);

    return arr;
}

Boolean npmRdRamReadPtrBit8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t offset = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    uint8_t value = (*RdRamRead8)(addr);
    return Boolean::New(info.Env(), (value & (1 << (7 - offset))) != 0);
}

// -------------------------------------------

Number npmRdRamReadPtrS32(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    int32_t value = (*RdRamRead32)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamReadPtrS16(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    int16_t value = (*RdRamRead16)(addr);    
    return Number::New(info.Env(), value);
}

Number npmRdRamReadPtrS8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    int8_t value = (*RdRamRead8)(addr);    
    return Number::New(info.Env(), value);
}

// -------------------------------------------

Value npmRdRamWritePtrBuffer(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    Uint8Array arr = info[2].As<Uint8Array>();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    size_t len = arr.ByteLength();

    if (debugPrinting)
        log("RdRamWritePtrBuffer{ ptr[%#010x], addr[%#010x], len[%i] }", ptr, addr, len);

    for (size_t i = 0; i < len; i++)
        (*RdRamWrite8)(addr + i, arr[i]);
    
    return info.Env().Undefined();
}

Value npmRdRamWritePtrF32(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    float value = info[2].As<Number>().FloatValue();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    if (debugPrinting)
        log("RdRamWritePtrF32{ ptr[%#010x], addr[%#010x], val[%f] }", ptr, addr, value);

    uint32_t val;
    memcpy((void*)(&val), (void*)(&value), 4);
    (*RdRamWrite32)(addr, val);  
    return info.Env().Undefined();
}

Value npmRdRamWritePtr32(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint32_t value = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    if (debugPrinting)
        log("RdRamWritePtr32{ ptr[%#010x], addr[%#010x], val[%#010x] }", ptr, addr, value);

    (*RdRamWrite32)(addr, value);    
    return info.Env().Undefined();
}

Value npmRdRamWritePtr16(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint16_t value = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    if (debugPrinting)
        log("RdRamWritePtr16{ ptr[%#010x], addr[%#010x], val[%#06x] }", ptr, addr, value);

    (*RdRamWrite16)(addr, value);    
    return info.Env().Undefined();
}

Value npmRdRamWritePtr8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    uint8_t value = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    if (debugPrinting)
        log("RdRamWritePtr8{ ptr[%#010x], addr[%#010x], val[%#04x] }", ptr, addr, value);

    (*RdRamWrite8)(addr, value);    
    return info.Env().Undefined();
}

// -------------------------------------------

Value npmRdRamWritePtrBitsBuffer(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    Uint8Array arr = info[2].As<Uint8Array>();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    size_t len = arr.ByteLength() / 8;
    const uint8_t* val = setBitsBuffer(arr, len);
    
    if (debugPrinting)
        log("RdRamWritePtrBitsBuffer{ ptr[%#010x], addr[%#010x], val[%i] }", ptr, addr, val);
        
    for (size_t i = 0; i < len; i++) 
        (*RdRamWrite8)(addr + i, val[i]);
    
    return info.Env().Undefined();
}

Value npmRdRamWritePtrBits8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    Uint8Array value = info[2].As<Uint8Array>();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;

    if (debugPrinting)
        log("RdRamWritePtrBits8{ ptr[%#010x], addr[%#010x], val[%#04x] }", ptr, addr, value);
    
    (*RdRamWrite8)(addr, setBits8(value));    
    return info.Env().Undefined();
}

Value npmRdRamWritePtrBit8(const CallbackInfo& info) {
    uint32_t ptr = info[0].As<Number>().Uint32Value();
    uint32_t ptr_offset = info[1].As<Number>().Uint32Value();
    size_t bit_offset = info[2].As<Number>().Uint32Value();
    uint32_t addr = (((*RdRamRead32)(ptr) & ~0xFF000000) | 0x00) + ptr_offset;
    bool set = info[3].As<Number>().ToBoolean();
    uint8_t val = (*RdRamRead8)(addr);

    if (debugPrinting)
        log("RdRamWritePtrBit8{ ptr[%#010x], addr[%#010x], val[%#04x] }", ptr, addr, val);
    
    (*RdRamWrite8)(addr, setBit8(val, bit_offset, set));
    return info.Env().Undefined();
}

// #########################################################
// ## Rom Memory
// #########################################################

Buffer<uint8_t> npmRomReadBuffer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    size_t len = info[1].As<Number>().Uint32Value();

    const uint8_t* mem = (*RomReadBuffer)(addr, len);
    return Buffer<uint8_t>::New(info.Env(), (uint8_t*)mem, len);;
}

Number npmRomRead32(const CallbackInfo& info) {
	uint32_t addr = info[0].As<Number>().Uint32Value();
	uint32_t value = (*RomRead32)(addr);
	return Number::New(info.Env(), value);
}

Number npmRomRead16(const CallbackInfo& info) {
	uint32_t addr = info[0].As<Number>().Uint32Value();
	uint16_t value = (*RomRead16)(addr);
	return Number::New(info.Env(), value);
}

Number npmRomRead8(const CallbackInfo& info) {
	uint32_t addr = info[0].As<Number>().Uint32Value();
	uint8_t value = (*RomRead8)(addr);
	return Number::New(info.Env(), value);
}

// -------------------------------------------

Value npmRomWriteBuffer(const CallbackInfo& info) {
    uint32_t addr = info[0].As<Number>().Uint32Value();
    Uint8Array arr = info[1].As<Uint8Array>();
    size_t len = arr.ByteLength();

    if (debugPrinting)
        log("RomWriteBuffer{ addr[%#010x], len[%i] }", addr, len);

    for (size_t i = 0; i < len; i++)
        (*RomWrite8)(addr + i, arr[i]);
    
    return info.Env().Undefined();
}

Value npmRomWrite32(const CallbackInfo& info) {
	uint32_t addr = info[0].As<Number>().Uint32Value();
	uint32_t value = info[1].As<Number>().Uint32Value();

    if (debugPrinting)
        log("RomWrite32{ addr[%#010x], val[%#010x] }", addr, value);
    
	(*RomWrite32)(addr, value);
    return info.Env().Undefined();
}

Value npmRomWrite16(const CallbackInfo& info) {
	uint32_t addr = info[0].As<Number>().Uint32Value();
	uint16_t value = info[1].As<Number>().Uint32Value();

    if (debugPrinting)
        log("RomWrite16{ addr[%#010x], val[%#06x] }", addr, value);
    
    (*RomWrite16)(addr, value);
    return info.Env().Undefined();
}

Value npmRomWrite8(const CallbackInfo& info) {
	uint32_t addr = info[0].As<Number>().Uint32Value();
	uint8_t value = info[1].As<Number>().Uint32Value();

    if (debugPrinting)
        log("RomWrite8{ addr[%#010x], val[%#04x] }", addr, value);
    
    (*RomWrite8)(addr, value);
    return info.Env().Undefined();
}

// #########################################################
// ## Cart Info
// #########################################################

Number npmCartGetClockRate(const CallbackInfo& info) {
	return Number::New(info.Env(), (*CartGetClockRate)());
}

Number npmCartGetPC(const CallbackInfo& info) {
	return Number::New(info.Env(), (*CartGetPC)());
}

Number npmCartGetRelease(const CallbackInfo& info) {
	return Number::New(info.Env(), (*CartGetRelease)());
}

// -------------------------------------------

Number npmCartGetName(const CallbackInfo& info) {
	return Number::New(info.Env(), (*CartGetName)());
}

Number npmCartGetManufacturerID(const CallbackInfo& info) {
	return Number::New(info.Env(), (*CartGetManufacturerID)());
}

Number npmCartGetSerial(const CallbackInfo& info) {
	return Number::New(info.Env(), (*CartGetSerial)());
}

Number npmCartGetCountryCode(const CallbackInfo& info) {
	return Number::New(info.Env(), 0);//(*CartGetCountryCode)());
}

// #########################################################
// ## SaveStates
// #########################################################

Value npmMemoryCacheRefresh(const CallbackInfo& info) {
    while (GetFrameCount() == -1)
        this_thread::sleep_for(chrono::milliseconds(1));
        
	(*MemoryCacheRefresh)();
    return info.Env().Undefined();
}

// #########################################################
// ## Misc
// #########################################################

Value npmMemoryDebugLogger(const CallbackInfo& info) {
	debugPrinting = info[0].As<Boolean>().ToBoolean();
    return info.Env().Undefined();
}

// #########################################################
// ## NAPI Export
// #########################################################

Object M64P_Memory_Init(Env env, Object exports) {
    // RDRam Memory
    exports.Set("dereferencePointer", Function::New(env, npmDereferencePointer));
    exports.Set("rdramReadBuffer", Function::New(env, npmRdRamReadBuffer));
    exports.Set("rdramReadF32", Function::New(env, npmRdRamReadF32));
    exports.Set("rdramRead32", Function::New(env, npmRdRamRead32));
    exports.Set("rdramRead16", Function::New(env, npmRdRamRead16));
    exports.Set("rdramRead8", Function::New(env, npmRdRamRead8));
    
    exports.Set("rdramReadBitsBuffer", Function::New(env, npmRdRamReadBitsBuffer));
    exports.Set("rdramReadBits8", Function::New(env, npmRdRamReadBits8));
    exports.Set("rdramReadBit8", Function::New(env, npmRdRamReadBit8));

    exports.Set("rdramReadS32", Function::New(env, npmRdRamReadS32));
    exports.Set("rdramReadS16", Function::New(env, npmRdRamReadS16));
    exports.Set("rdramReadS8", Function::New(env, npmRdRamReadS8));

    exports.Set("rdramWriteBuffer", Function::New(env, npmRdRamWriteBuffer));
    exports.Set("rdramWriteF32", Function::New(env, npmRdRamWriteF32));
    exports.Set("rdramWrite32", Function::New(env, npmRdRamWrite32));
    exports.Set("rdramWrite16", Function::New(env, npmRdRamWrite16));
    exports.Set("rdramWrite8", Function::New(env, npmRdRamWrite8));

    exports.Set("rdramWriteBitsBuffer", Function::New(env, npmRdRamWriteBitsBuffer));
    exports.Set("rdramWriteBits8", Function::New(env, npmRdRamWriteBits8));
    exports.Set("rdramWriteBit8", Function::New(env, npmRdRamWriteBit8));

    // RDRam Memory (Pointer Based)
    exports.Set("rdramReadPtrBuffer", Function::New(env, npmRdRamReadPtrBuffer));
    exports.Set("rdramReadPtrF32", Function::New(env, npmRdRamReadPtrF32));
    exports.Set("rdramReadPtr32", Function::New(env, npmRdRamReadPtr32));
    exports.Set("rdramReadPtr16", Function::New(env, npmRdRamReadPtr16));
    exports.Set("rdramReadPtr8", Function::New(env, npmRdRamReadPtr8));

    exports.Set("rdramReadPtrBitsBuffer", Function::New(env, npmRdRamReadPtrBitsBuffer));
    exports.Set("rdramReadPtrBits8", Function::New(env, npmRdRamReadPtrBits8));
    exports.Set("rdramReadPtrBit8", Function::New(env, npmRdRamReadPtrBit8));

    exports.Set("rdramReadPtrS32", Function::New(env, npmRdRamReadPtrS32));
    exports.Set("rdramReadPtrS16", Function::New(env, npmRdRamReadPtrS16));
    exports.Set("rdramReadPtrS8", Function::New(env, npmRdRamReadPtrS8));

    exports.Set("rdramWritePtrBuffer", Function::New(env, npmRdRamWritePtrBuffer));
    exports.Set("rdramWritePtrF32", Function::New(env, npmRdRamWritePtrF32));
    exports.Set("rdramWritePtr32", Function::New(env, npmRdRamWritePtr32));
    exports.Set("rdramWritePtr16", Function::New(env, npmRdRamWritePtr16));
    exports.Set("rdramWritePtr8", Function::New(env, npmRdRamWritePtr8));

    exports.Set("rdramWritePtrBitsBuffer", Function::New(env, npmRdRamWritePtrBitsBuffer));
    exports.Set("rdramWritePtrBits8", Function::New(env, npmRdRamWritePtrBits8));
    exports.Set("rdramWritePtrBit8", Function::New(env, npmRdRamWritePtrBit8));
    
    // Rom Memory
    exports.Set("romReadBuffer", Function::New(env, npmRomReadBuffer));
    exports.Set("romRead32", Function::New(env, npmRomRead32));
    exports.Set("romRead16", Function::New(env, npmRomRead16));
    exports.Set("romRead8", Function::New(env, npmRomRead8));

    exports.Set("romWriteBuffer", Function::New(env, npmRomWriteBuffer));
    exports.Set("romWrite32", Function::New(env, npmRomWrite32));
    exports.Set("romWrite16", Function::New(env, npmRomWrite16));
    exports.Set("romWrite8", Function::New(env, npmRomWrite8));

    // Cart Info
    exports.Set("cartGetClockRate", Function::New(env, npmCartGetClockRate));
    exports.Set("cartGetPC", Function::New(env, npmCartGetPC));
    exports.Set("cartGetRelease", Function::New(env, npmCartGetRelease));
    
    exports.Set("cartGetName", Function::New(env, npmCartGetName));
    exports.Set("cartGetManufacturerID", Function::New(env, npmCartGetManufacturerID));
    exports.Set("cartGetSerial", Function::New(env, npmCartGetSerial));
    exports.Set("cartGetCountryCode", Function::New(env, npmCartGetCountryCode));

    // SaveStates
    exports.Set("memoryCacheRefresh", Function::New(env, npmMemoryCacheRefresh));

    // Misc
    exports.Set("memoryDebugLogger", Function::New(env, npmMemoryDebugLogger));
    
    return exports;
}
