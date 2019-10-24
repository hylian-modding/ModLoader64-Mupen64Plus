#ifndef _NODEJS_MEMORY_
#define _NODEJS_MEMORY_
    #include <napi.h>
    using namespace Napi;

    // #########################################################
    // ## RDRam Memory
    // #########################################################
    Number npmDereferencePointer(const CallbackInfo& info);
    
    Buffer<uint8_t> npmRdRamReadBuffer(const CallbackInfo& info);
    Number npmRdRamReadF32(const CallbackInfo& info);
    Number npmRdRamRead32(const CallbackInfo& info);
    Number npmRdRamRead16(const CallbackInfo& info);
    Number npmRdRamRead8(const CallbackInfo& info);
    
    Buffer<uint8_t> npmRdRamReadBitsBuffer(const CallbackInfo& info);
    Buffer<uint8_t> npmRdRamReadBits8(const CallbackInfo& info);
    Boolean npmRdRamReadBit8(const CallbackInfo& info);

    Number npmRdRamReadS32(const CallbackInfo& info);
    Number npmRdRamReadS16(const CallbackInfo& info);
    Number npmRdRamReadS8(const CallbackInfo& info);

    Value npmRdRamWriteBuffer(const CallbackInfo& info);
    Value npmRdRamWriteF32(const CallbackInfo& info);
    Value npmRdRamWrite32(const CallbackInfo& info);
    Value npmRdRamWrite16(const CallbackInfo& info);
    Value npmRdRamWrite8(const CallbackInfo& info);

    Value npmRdRamWriteBitsBuffer(const CallbackInfo& info);
    Value npmRdRamWriteBits8(const CallbackInfo& info);
    Value npmRdRamWriteBit8(const CallbackInfo& info);

    // #########################################################
    // ## RDRam Memory (Pointer Based)
    // #########################################################

    Buffer<uint8_t> npmRdRamReadPtrBuffer(const CallbackInfo& info);
    Number npmRdRamReadPtrF32(const CallbackInfo& info);
    Number npmRdRamReadPtr32(const CallbackInfo& info);
    Number npmRdRamReadPtr16(const CallbackInfo& info);
    Number npmRdRamReadPtr8(const CallbackInfo& info);
    
    Buffer<uint8_t> npmRdRamReadPtrBitsBuffer(const CallbackInfo& info);
    Buffer<uint8_t> npmRdRamReadPtrBits8(const CallbackInfo& info);
    Boolean npmRdRamReadPtrBit8(const CallbackInfo& info);

    Number npmRdRamReadPtrS32(const CallbackInfo& info);
    Number npmRdRamReadPtrS16(const CallbackInfo& info);
    Number npmRdRamReadPtrS8(const CallbackInfo& info);

    Value npmRdRamWritePtrBuffer(const CallbackInfo& info);
    Value npmRdRamWritePtrF32(const CallbackInfo& info);
    Value npmRdRamWritePtr32(const CallbackInfo& info);
    Value npmRdRamWritePtr16(const CallbackInfo& info);
    Value npmRdRamWritePtr8(const CallbackInfo& info);

    Value npmRdRamWritePtrBitsBuffer(const CallbackInfo& info);
    Value npmRdRamWritePtrBits8(const CallbackInfo& info);
    Value npmRdRamWritePtrBit8(const CallbackInfo& info);

    // #########################################################
    // ## Rom Memory
    // #########################################################

    Buffer<uint8_t> npmRomReadBuffer(const CallbackInfo& info);
    Number npmRomRead32(const CallbackInfo& info);
    Number npmRomRead16(const CallbackInfo& info);
    Number npmRomRead8(const CallbackInfo& info);

    Value npmRomWriteBuffer(const CallbackInfo& info);
    Value npmRomWrite32(const CallbackInfo& info);
    Value npmRomWrite16(const CallbackInfo& info);
    Value npmRomWrite8(const CallbackInfo& info);

    // #########################################################
    // ## Cart Info
    // #########################################################

	Number npmCartGetClockRate(const CallbackInfo& info);
	Number npmCartGetPC(const CallbackInfo& info);
	Number npmCartGetRelease(const CallbackInfo& info);

	Number npmCartGetName(const CallbackInfo& info);
	Number npmCartGetManufacturerID(const CallbackInfo& info);
	Number npmCartGetSerial(const CallbackInfo& info);
	Number npmCartGetCountryCode(const CallbackInfo& info);

    // #########################################################
    // ## SaveStates
    // #########################################################

    Value npmMemoryCacheRefresh(const CallbackInfo& info);

    // #########################################################
    // ## Misc
    // #########################################################

    Value npmMemoryDebugLogger(const CallbackInfo& info);
    
    // #########################################################
    // ## NAPI Export
    // #########################################################

    Object M64P_Memory_Init(Env env, Object exports);
#endif