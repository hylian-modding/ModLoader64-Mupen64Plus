QT += core gui
CONFIG += c++11
QMAKE_PROJECT_DEPTH = 0

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = ../src/gfx/mupen64plus.ico
ICON = ../src/gfx/mupen64plus.icns
FORMS += ../src/gui/window_main.ui

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = obj

DEFINES += M64P_FRONTEND
TARGET = mupen64plus-frontend
TEMPLATE = lib # app # 

INCLUDEPATH += ../src \
    ../../m64p-core/includes/SDL2-2.0.6 \
    ../../m64p-core/src/api

SOURCES += \
    ../src/gui/component_settings.cpp \
    ../src/gui/dialog_cheat.cpp \
    ../src/gui/dialog_controller.cpp \
    ../src/gui/dialog_keyselect.cpp \
    ../src/gui/dialog_logviewer.cpp \
    ../src/gui/dialog_plugin.cpp \
    ../src/gui/dialog_settings.cpp \
    ../src/gui/extension_video.cpp \
    ../src/gui/thread_worker.cpp \
    ../src/gui/window_gl.cpp \
    ../src/gui/window_main.cpp \
    \
    ../src/osal/dynamiclib.cpp \
    ../src/osal/files.cpp \
    \
    ../src/cheat.cpp \
    ../src/common.cpp \
    ../src/interface.cpp \
    ../src/main.cpp \
    ../src/plugin.cpp \
    ../src/sdl_keys.cpp

HEADERS  += \
    ../src/gui/imports_gui.h \
    ../src/gui/component_settings.h \
    ../src/gui/dialog_cheat.h \
    ../src/gui/dialog_controller.h \
    ../src/gui/dialog_keyselect.h \
    ../src/gui/dialog_logviewer.h \
    ../src/gui/dialog_plugin.h \
    ../src/gui/dialog_settings.h \
    ../src/gui/extension_video.h \
    ../src/gui/thread_worker.h \
    ../src/gui/window_gl.h \
    ../src/gui/window_main.h \
    \
    ../src/osal/imports_osal.h \
    ../src/osal/dynamiclib.h \
    ../src/osal/files.h \
    ../src/osal/preproc.h \
    \
    ../src/imports_m64p.h \
    ../src/imports_qt.h \
    ../src/cheat.h \
    ../src/common.h \
    ../src/interface.h \
    ../src/main.h \
    ../src/plugin.h \
    ../src/sdl_keys.h \
    ../src/version.h

win32 {
    DEFINES += WIN32
    DEFINES -= UNICODE

    LIBS += ../../m64p-core/libs/x86/SDL2.lib

    CONFIG += shared
}
!win32 {
    QMAKE_CFLAGS += -std=c++11 -fpermissive
    QMAKE_CXXFLAGS += -std=c++11 -fpermissive
    QMAKE_LFLAGS += -std=c++11 -fpermissive
    
    LIBS += -L/usr/local/lib -ldl -lSDL2 -lz
    
    VERSION = 0

    QMAKE_POST_LINK = \
    cd ./bin && \
    rm libmupen64plus-frontend.so && \
    rm libmupen64plus-frontend.so.0 && \
    rm libmupen64plus-frontend.so.0.0 && \
    mv libmupen64plus-frontend.so.0.0.0 libmupen64plus-frontend.so.0 && \
    cd ..
}