#ifndef VERSION_H
#define VERSION_H

    #define CONSOLE_UI_NAME        "Mupen64Plus Frontend"
    #define CONSOLE_UI_VERSION     0x020509
    #define CORE_API_VERSION       0x020500
    #define CONFIG_API_VERSION     0x020000
    #define MINIMUM_CORE_VERSION   0x020500
    #define VERSION_PRINTF_SPLIT(x) (((x) >> 16) & 0xffff), (((x) >> 8) & 0xff), ((x) & 0xff)

#endif