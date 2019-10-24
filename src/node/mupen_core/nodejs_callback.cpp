#include "nodejs_callback.h"
#include <chrono>
#include <thread>
using namespace std;

// #########################################################
// ## Threading Handlers
// #########################################################

unsigned int count_frame = -1;

#ifdef WIN32
    #include <windows.h>

    HANDLE mutex_frame = CreateMutex(NULL, FALSE, NULL); 

    int mutex_frame_get() {
        int count;
        DWORD dWait = WaitForSingleObject(mutex_frame, INFINITE);
        switch (dWait) {
            case WAIT_OBJECT_0: 
                __try { count = count_frame; }
                __finally { 
                    if (! ReleaseMutex(mutex_frame)) 
                        printf("FRAME_CALLBACK_EXCEPTION\n"); 
                } break; 

            case WAIT_ABANDONED: 
                printf("FRAME_CALLBACK_EXCEPTION\n"); 
                break;
        } return count;
    }

    void mutex_frame_set(int count) {
        DWORD dWait = WaitForSingleObject(mutex_frame, INFINITE);
        switch (dWait) {
            case WAIT_OBJECT_0: 
                __try { count_frame = count; }
                __finally { 
                    if (! ReleaseMutex(mutex_frame)) 
                        printf("FRAME_CALLBACK_EXCEPTION\n"); 
                } break; 

            case WAIT_ABANDONED: 
                printf("FRAME_CALLBACK_EXCEPTION\n"); 
                break;
        }
    }
#else
    #include <pthread.h>

    pthread_mutex_t mutex_frame = PTHREAD_MUTEX_INITIALIZER;

    int mutex_frame_get() {
        int count;
        pthread_mutex_lock(&mutex_frame);
        count = count_frame;
        pthread_mutex_unlock(&mutex_frame);
        return count;
    }

    void mutex_frame_set(int count) {
        pthread_mutex_lock(&mutex_frame);
        count_frame = count;
        pthread_mutex_unlock(&mutex_frame);
    }
#endif

// #########################################################
// ## Memory Tools
// #########################################################

// #include "../../mem_tools/hex/HexWindow.h"
// HexWindow* winHex;

// #########################################################
// ## Mupen Frame Callback Function
// #########################################################

void M64P_Callback_Frame(unsigned int frameIndex) {
    mutex_frame_set(frameIndex);

    // //Init memory tools if necessary
    // if (frameIndex == 0) {
	// 	winHex = new HexWindow();
    // }

    // // Update memory tools
    // winHex->runOnce();
    
    while (mutex_frame_get() != -1)
        this_thread::sleep_for(chrono::milliseconds(1));
}

// #########################################################
// ## Node Functions
// #########################################################

Number npmGetFrameCount(const CallbackInfo& info) {
    return Number::New(info.Env(), mutex_frame_get());
}

Value npmSetFrameCount(const CallbackInfo& info) {
    uint32_t value = info[0].As<Number>().Uint32Value();
    mutex_frame_set(value);
    return info.Env().Undefined();
}

Object M64P_Callback_Init(Env env, Object exports) {
    exports.Set("getFrameCount", Function::New(env, npmGetFrameCount));
    exports.Set("setFrameCount", Function::New(env, npmSetFrameCount));
    return exports;
}