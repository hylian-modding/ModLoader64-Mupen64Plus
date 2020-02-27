#include <napi.h>
#include "yaz0/yaz0.h"
#include "emulator/callback.h"
#include "emulator/interface.h"
#include "emulator/memory.h"
#include "emulator/utility.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Yaz0_Init(env, exports);
  M64P_Callback_Init(env, exports);
  M64P_Memory_Init(env, exports);  
  M64P_Utility_Init(env, exports);
  return M64P_Interface_Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)