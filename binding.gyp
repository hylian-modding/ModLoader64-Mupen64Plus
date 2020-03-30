{
    "targets": [{
        "target_name": "mupen64plus",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        'dependencies': [ "<!(node -p \"require('node-addon-api').gyp\")" ],
                    
        'conditions': [        
            ['OS=="win"', {
                "defines": [ "WIN32", "NDEBUG", "_CONSOLE", "_CRT_SECURE_NO_DEPRECATE" ],
                'libraries': [ "<(module_root_dir)/subprojects/m64p-core/libs/x86/SDL2.lib" ],
            }],
            ['OS=="linux"', {
                "cflags_cc+": [ "-fpermissive" ],
                'libraries': [
                    "-Wl,-rpath,'$$ORIGIN'",
                    "-L/usr/local/lib -ldl -lSDL2 -lz"
                ],
            }]
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "<(module_root_dir)/subprojects/m64p-core/src/api",
            "<(module_root_dir)/subprojects/m64p-core/includes/SDL2-2.0.6",
        ],
        "sources": [
            # Frontend
            "src/frontend/osal/dynamiclib.cpp",
            "src/frontend/osal/files.cpp",
            "src/frontend/cheat.cpp",
            "src/frontend/common.cpp",
            "src/frontend/debugger.cpp",
            "src/frontend/interface.cpp",
            "src/frontend/plugin.cpp",

            # Node
            "src/node/callback.cpp",
            "src/node/interface.cpp",
            "src/node/memory.cpp",
            "src/node/utility.cpp",

            # Yazo
            "src/yaz0/yaz0.cpp",

            # Main
            "src/main.cpp",
        ]
    }]
}
