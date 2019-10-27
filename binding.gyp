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
                'libraries': [ 
                    "<(module_root_dir)/libs/SDL2.lib",
                    "<(module_root_dir)/libs/SDL2main.lib",
                    "opengl32.lib" 
                ],
            }],
            ['OS=="linux"', {
                "cflags_cc+": [ "-fpermissive" ],
                'libraries': [ "/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so" ]
            }]
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "<(module_root_dir)/subprojects/m64p-core/src/api",
            "<(module_root_dir)/includes"
        ],
        "sources": [          
            "src/mupen_core/core_compare.cpp",
            "src/mupen_core/core_interface.cpp",
            "src/mupen_core/main_mupen_core.cpp",
            "src/mupen_core/osal_dynamiclib.cpp",
            "src/mupen_core/osal_files.cpp",            
            "src/mupen_core/plugin.cpp",

            "src/node/nodejs_main.cpp",
            "src/node/mupen_core/nodejs_callback.cpp",
            "src/node/mupen_core/nodejs_interface.cpp",
            "src/node/mupen_core/nodejs_memory.cpp",
            "src/node/mupen_core/nodejs_utility.cpp",
            "src/node/yaz0/nodejs_yaz0.cpp",

            "subprojects/yaz0/chunk.cpp",
            "subprojects/yaz0/base.cpp",
            "subprojects/yaz0/parser.cpp",
            "subprojects/yaz0/creator.cpp",
        ]
    }]
}
