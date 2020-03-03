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

    #include <string>

    /* modloader initialization */
    expose extern int Main_ModLoader();
    expose extern int LoadGame(std::string);
    expose extern int PreBoot();
    expose extern int PostBoot();

#endif