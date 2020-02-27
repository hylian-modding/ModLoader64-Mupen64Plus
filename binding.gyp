{
    "targets": [{
        "target_name": "mupen64plus",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS', 'nGUI' ],
        'dependencies': [ "<!(node -p \"require('node-addon-api').gyp\")" ],
                    
        'conditions': [        
            ['OS=="win"', {
                "defines": [ "WIN32" ],
                'libraries': [ 
                    "<(module_root_dir)/subprojects/m64p-core/libs/x86/SDL2.lib",
                    "<(module_root_dir)/subprojects/m64p-core/libs/x86/SDL2main.lib",
                    #"<(module_root_dir)/subprojects/m64p-frontend-gui/project/bin/mupen64plus-frontend.lib",
                    "<(module_root_dir)/subprojects/m64p-frontend-cli/project/bin/mupen64plus-frontend.lib",
                ],
            }],
            ['OS=="linux"', {
                "cflags_cc+": [ "-fpermissive" ],
                'libraries': [
                    "-Wl,-rpath,'$$ORIGIN'",
                    "-L/usr/local/lib -ldl -lSDL2 -lz",
                    #"<(module_root_dir)/subprojects/m64p-frontend-gui/project/bin/libmupen64plus-frontend.so",
                    "<(module_root_dir)/subprojects/m64p-frontend-cli/project/bin/libmupen64plus-frontend.so",
                ],
            }]
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "<(module_root_dir)/subprojects/",
            "<(module_root_dir)/subprojects/m64p-core/src/api",
            "<(module_root_dir)/subprojects/m64p-core/includes/SDL2-2.0.6",
        ],
        "sources": [
            # Mupen
            "src/emulator/callback.cpp",
            "src/emulator/interface.cpp",
            "src/emulator/memory.cpp",
            "src/emulator/utility.cpp",

            # Yazo
            "src/yaz0/yaz0.cpp",

            # Main
            "src/main.cpp",
        ]
    }]
}
