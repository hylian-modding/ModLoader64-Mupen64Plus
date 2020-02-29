QT += core gui
CONFIG += c++11
QMAKE_PROJECT_DEPTH = 0

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = obj

DEFINES += M64P_FRONTEND
TARGET = mupen64plus-frontend
TEMPLATE = lib

INCLUDEPATH += ../src \
    ../../m64p-core/includes/SDL2-2.0.6 \
    ../../m64p-core/src/api

SOURCES += \
    ../src/osal/dynamiclib.cpp \
    ../src/osal/files.cpp \
    \
    ../src/cheat.cpp \
    ../src/debugger.cpp \
    ../src/interface.cpp \
    ../src/main.cpp \
    ../src/plugin.cpp

HEADERS  += \
    ../src/osal/imports_osal.h \
    ../src/osal/dynamiclib.h \
    ../src/osal/files.h \
    ../src/osal/preproc.h \
    \
    ../src/imports_m64p.h \
    ../src/cheat.h \
    ../src/debugger.h \
    ../src/interface.h \
    ../src/main.h \
    ../src/plugin.h \
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