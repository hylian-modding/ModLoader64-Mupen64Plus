#ifndef FRONTEND_COMMON_H
#define FRONTEND_COMMON_H

    #include <string>

    #include "imports_m64p.h"
    #include "imports_qt.h"

    extern bool isModLoader;

    m64p_error openROM(std::string filename);
    int QT2SDL2MOD(Qt::KeyboardModifiers modifiers);
    int QT2SDL2(int qtKey);

    void DebugMessage(int level, const char *message, ...);
    void DebugCallback(void *Context, int level, const char *message);

#endif