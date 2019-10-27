#include "nodejs_callback.h"

#include <chrono>
#include <thread>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

using namespace std;

// #########################################################
// ## Memory Tools
// #########################################################



// #########################################################
// ## Threading Handlers
// #########################################################

static SDL_mutex *frame_lock;
unsigned int count_frame = -1;
bool refresh_invoked = false;

int GetFrameCount() {
    int count;
	SDL_LockMutex(frame_lock);
    count = count_frame;
	SDL_UnlockMutex(frame_lock);
    return count;
}

void SetFrameCount(int count) {
	SDL_LockMutex(frame_lock);
    count_frame = count;
	SDL_UnlockMutex(frame_lock);
}

// #########################################################
// ## Mupen Frame Callback Function
// #########################################################

void M64P_Callback_Init(void) { frame_lock = SDL_CreateMutex(); }

void M64P_Callback_Frame(unsigned int frameIndex) {
    // //Init memory tools if necessary
    // if (frameIndex == 0) {
	// 	winHex = new HexWindow();
    // }

    // // Update memory tools
    // winHex->runOnce();

    // Allow modloader access
    SetFrameCount(frameIndex);
    
    // Wait for modloader to finish
    while (GetFrameCount() != -1)
        this_thread::sleep_for(chrono::milliseconds(1));
}

void M64P_Callback_Destroy(void) { SDL_DestroyMutex(frame_lock); }

// #########################################################
// ## Node Functions
// #########################################################

Number npmGetFrameCount(const CallbackInfo& info) {
    return Number::New(info.Env(), GetFrameCount());
}

Value npmSetFrameCount(const CallbackInfo& info) {
    uint32_t value = info[0].As<Number>().Uint32Value();
    SetFrameCount(value);
    return info.Env().Undefined();
}

Object M64P_Callback_Init(Env env, Object exports) {
    exports.Set("getFrameCount", Function::New(env, npmGetFrameCount));
    exports.Set("setFrameCount", Function::New(env, npmSetFrameCount));
    return exports;
}