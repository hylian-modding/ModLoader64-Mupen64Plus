#ifndef FRONTEND_EXTENSION_VIDEO_H
#define FRONTEND_EXTENSION_VIDEO_H

    #include "../imports_m64p.h"
    #include "window_gl.h"

    extern WindowGL *my_window;

    m64p_error    qtVidExtFuncInit(void);
    m64p_error    qtVidExtFuncQuit(void);
    m64p_error    qtVidExtFuncListModes(m64p_2d_size *SizeArray, int *NumSizes);
    m64p_error    qtVidExtFuncSetMode(int Width, int Height, int, int, int);
    m64p_function qtVidExtFuncGLGetProc(const char*Proc);
    m64p_error    qtVidExtFuncGLSetAttr(m64p_GLattr Attr, int Value);
    m64p_error    qtVidExtFuncGLGetAttr(m64p_GLattr Attr, int *pValue);
    m64p_error    qtVidExtFuncGLSwapBuf(void);
    m64p_error    qtVidExtFuncSetCaption(const char *_title);
    m64p_error    qtVidExtFuncToggleFS(void);
    m64p_error    qtVidExtFuncResizeWindow(int, int);
    uint32_t      qtVidExtFuncGLGetDefaultFramebuffer(void);

#endif