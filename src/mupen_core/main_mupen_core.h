
#ifndef __MAIN_H__
#define __MAIN_H__

extern void DebugMessage(int level, const char *message, ...);
extern void DebugCallback(void *Context, int level, const char *message);

extern int  g_Verbose;

extern m64p_handle l_ConfigCore;
extern m64p_handle l_ConfigVideo;
extern m64p_handle l_ConfigUI;

extern const char *l_CoreLibPath;
extern const char *l_ConfigDirPath;
extern const char *l_ROMFilepath;
extern const char *l_SaveStatePath;
extern const char *l_DataDirPath;

extern int   l_SaveOptions;

extern int Initialize();
extern int LoadGame();
extern int Boot();

#endif