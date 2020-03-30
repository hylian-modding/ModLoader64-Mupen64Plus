#include <napi.h>
#include "node/callback.h"
#include "node/interface.h"
#include "node/memory.h"
#include "node/utility.h"
#include "yaz0/yaz0.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Yaz0_Init(env, exports);
  M64P_Callback_Init(env, exports);
  M64P_Memory_Init(env, exports);  
  M64P_Utility_Init(env, exports);
  return M64P_Interface_Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)