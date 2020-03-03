#ifndef FRONTEND_MAIN_H
#define FRONTEND_MAIN_H

    #ifdef WIN32
        #ifdef M64P_FRONTEND
            #define expose __declspec(dllexport)
        #else
            #define  expose __declspec(dllimport)
        #endif
    #else
        #define expose
    #endif

    #if defined(__GNUC__)
        #define ATTR_FMT(fmtpos, attrpos) __attribute__ ((format (printf, fmtpos, attrpos)))
    #else
        #define ATTR_FMT(fmtpos, attrpos)
    #endif

    #include <string>

    #include "imports_m64p.h"

    extern void DebugMessage(int level, const char *message, ...) ATTR_FMT(2,3);
    extern void DebugCallback(void *Context, int level, const char *message);

    extern int g_Verbose;

    extern m64p_handle l_ConfigCore;
    extern m64p_handle l_ConfigVideo;
    extern m64p_handle l_ConfigUI;

    extern const char *l_CoreLibPath;
    extern const char *l_ConfigDirPath;
    extern const char *l_ROMFilepath;
    extern const char *l_SaveStatePath;
    extern const char *l_DataDirPath;

    extern int l_SaveOptions;

    expose extern int Main_ModLoader();
    expose extern int LoadGame(std::string);
    expose extern int PreBoot();
    expose extern int PostBoot();

#endif