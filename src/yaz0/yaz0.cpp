#include <m64p_types.h>

#include "yaz0.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

typedef unsigned char u8;
typedef unsigned int u32;

u32 toDWORD(u32 d)
{
    u8 w1 = d & 0xFF;
    u8 w2 = (d >> 8) & 0xFF;
    u8 w3 = (d >> 16) & 0xFF;
    u8 w4 = d >> 24;
    return (w1 << 24) | (w2 << 16) | (w3 << 8) | w4;
}

struct Ret
{
    int srcPos, dstPos;
};

void decode(u8 *src, u8 *dst, int uncompressedSize)
{
    int srcPlace = 0, dstPlace = 0; //current read/write positions

    u32 validBitCount = 0; //number of valid bits left in "code" byte
    u8 currCodeByte;
    while (dstPlace < uncompressedSize)
    {
        //read new "code" byte if the current one is used up
        if (validBitCount == 0)
        {
            currCodeByte = src[srcPlace];
            ++srcPlace;
            validBitCount = 8;
        }

        if ((currCodeByte & 0x80) != 0)
        {
            //straight copy
            dst[dstPlace] = src[srcPlace];
            dstPlace++;
            srcPlace++;
        }
        else
        {
            //RLE part
            u8 byte1 = src[srcPlace];
            u8 byte2 = src[srcPlace + 1];
            srcPlace += 2;

            u32 dist = ((byte1 & 0xF) << 8) | byte2;
            u32 copySource = dstPlace - (dist + 1);

            u32 numBytes = byte1 >> 4;
            if (numBytes == 0)
            {
                numBytes = src[srcPlace] + 0x12;
                srcPlace++;
            }
            else
                numBytes += 2;

            //copy run
            for (size_t i = 0; i < numBytes; ++i)
            {
                dst[dstPlace] = dst[copySource];
                copySource++;
                dstPlace++;
            }
        }

        //use next bit from "code" byte
        currCodeByte <<= 1;
        validBitCount -= 1;
    }
}

// simple and straight encoding scheme for Yaz0
u32 simpleEnc(u8 *src, int size, int pos, u32 *pMatchPos)
{
    int startPos = pos - 0x1000;
    u32 numBytes = 1;
    u32 matchPos = 0;

    if (startPos < 0)
        startPos = 0;
    for (size_t i = startPos; i < pos; i++)
    {
        size_t j = 0;
        for (j = 0; j < size - pos; j++)
        {
            if (src[i + j] != src[j + pos])
                break;
        }
        if (j > numBytes)
        {
            numBytes = j;
            matchPos = i;
        }
    }
    *pMatchPos = matchPos;
    if (numBytes == 2)
        numBytes = 1;
    return numBytes;
}

// a lookahead encoding scheme for ngc Yaz0
u32 nintendoEnc(u8 *src, int size, int pos, u32 *pMatchPos)
{
    int startPos = pos - 0x1000;
    u32 numBytes = 1;
    static u32 numBytes1;
    static u32 matchPos;
    static int prevFlag = 0;

    // if prevFlag is set, it means that the previous position was determined by look-ahead try.
    // so just use it. this is not the best optimization, but nintendo's choice for speed.
    if (prevFlag == 1)
    {
        *pMatchPos = matchPos;
        prevFlag = 0;
        return numBytes1;
    }
    prevFlag = 0;
    numBytes = simpleEnc(src, size, pos, &matchPos);
    *pMatchPos = matchPos;

    // if this position is RLE encoded, then compare to copying 1 byte and next position(pos+1) encoding
    if (numBytes >= 3)
    {
        numBytes1 = simpleEnc(src, size, pos + 1, &matchPos);
        // if the next position encoding is +2 longer than current position, choose it.
        // this does not guarantee the best optimization, but fairly good optimization with speed.
        if (numBytes1 >= numBytes + 2)
        {
            numBytes = 1;
            prevFlag = 1;
        }
    }
    return numBytes;
}

int encodeYaz0(u8 *src, int srcSize, uint8_t *tBuf)
{
    Ret r = {0, 0};
    u8 dst[24]; // 8 codes * 3 bytes maximum
    size_t bOffset = 0;
    int dstSize = 0;
    int percent = -1;

    u32 validBitCount = 0; //number of valid bits left in "code" byte
    u8 currCodeByte = 0;
    while (r.srcPos < srcSize)
    {
        u32 numBytes;
        u32 matchPos;
        u32 srcPosBak;

        numBytes = nintendoEnc(src, srcSize, r.srcPos, &matchPos);
        if (numBytes < 3)
        {
            //straight copy
            dst[r.dstPos] = src[r.srcPos];
            r.dstPos++;
            r.srcPos++;
            //set flag for straight copy
            currCodeByte |= (0x80 >> validBitCount);
        }
        else
        {
            //RLE part
            u32 dist = r.srcPos - matchPos - 1;
            u8 byte1, byte2, byte3;

            if (numBytes >= 0x12) // 3 byte encoding
            {
                byte1 = 0 | (dist >> 8);
                byte2 = dist & 0xff;
                dst[r.dstPos++] = byte1;
                dst[r.dstPos++] = byte2;
                // maximum runlength for 3 byte encoding
                if (numBytes > 0xff + 0x12)
                    numBytes = 0xff + 0x12;
                byte3 = numBytes - 0x12;
                dst[r.dstPos++] = byte3;
            }
            else // 2 byte encoding
            {
                byte1 = ((numBytes - 2) << 4) | (dist >> 8);
                byte2 = dist & 0xff;
                dst[r.dstPos++] = byte1;
                dst[r.dstPos++] = byte2;
            }
            r.srcPos += numBytes;
        }
        validBitCount++;
        //write eight codes
        if (validBitCount == (u32)8)
        {
            memcpy((void *)(tBuf + bOffset), (void *)&currCodeByte, 1);
            bOffset += 1;

            memcpy((void *)(tBuf + bOffset), (void *)dst, r.dstPos);
            bOffset += r.dstPos;

            dstSize += r.dstPos + 1;

            srcPosBak = r.srcPos;
            currCodeByte = 0;
            validBitCount = 0;
            r.dstPos = 0;
        }
        if ((r.srcPos + 1) * 100 / srcSize != percent)
        {
            percent = (r.srcPos + 1) * 100 / srcSize;
        }
    }
    if (validBitCount > 0)
    {
        memcpy((void *)(tBuf + bOffset), (void *)&currCodeByte, 1);
        bOffset += 1;

        memcpy((void *)(tBuf + bOffset), (void *)dst, r.dstPos);
        bOffset += r.dstPos;

        dstSize += r.dstPos + 1;

        currCodeByte = 0;
        validBitCount = 0;
        r.dstPos = 0;
    }

    return dstSize;
}

// #########################################################
// ## Yaz0 Node Functions
// #########################################################

#include <cmath>

Buffer<uint8_t> Yaz0_Encode(const CallbackInfo &info)
{
    Buffer<uint8_t> buf = info[0].As<Buffer<uint8_t>>();
    size_t len = buf.ByteLength();
    uint8_t *yBuf = (uint8_t *)buf.Data();

    uint8_t *tBuf = (uint8_t *)malloc(len * 2);
    uint32_t size = encodeYaz0(yBuf, len, tBuf);
    size_t alignSize = ceil((float)(size + 16) / 16) * 16;
    yBuf = (uint8_t *)calloc(alignSize, 1);

    const char *header = "Yaz0";
    u32 origSize = toDWORD(len);
    memcpy((void *)yBuf, (void *)header, 4);
    memcpy((void *)(yBuf + 4), (void *)&origSize, 4);
    memcpy((void *)(yBuf + 16), (void *)tBuf, size);
    //free(tBuf);

    return Buffer<uint8_t>::New(info.Env(), yBuf, alignSize);
}

Buffer<uint8_t> Yaz0_Decode(const CallbackInfo &info)
{
    Buffer<uint8_t> buf = info[0].As<Buffer<uint8_t>>();
    size_t len = buf.ByteLength();
    uint8_t *yBuf = (uint8_t *)buf.Data();

    uint32_t size = 0;
    memcpy((void *)&size, (void *)(yBuf + 4), 4);
    size = toDWORD(size);

    uint8_t *tBuf = (uint8_t *)malloc(size);
    yBuf += 16;
    decode(yBuf, tBuf, size);

    buf = Buffer<uint8_t>::New(info.Env(), tBuf, size);
    return buf;
}

// #########################################################
// ## NAPI Export
// #########################################################

Object Yaz0_Init(Env env, Object exports)
{
    exports.Set("yaz0Encode", Function::New(env, Yaz0_Encode));
    exports.Set("yaz0Decode", Function::New(env, Yaz0_Decode));

    return exports;
}