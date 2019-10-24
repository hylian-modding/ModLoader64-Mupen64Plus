#include <napi.h>
#include "yaz0/nodejs_yaz0.h"
#include "mupen_core/nodejs_core.h"
#include "mupen_core/nodejs_interface.h"
#include "mupen_core/nodejs_memory.h"
#include "mupen_core/nodejs_utility.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Yaz0_Init(env, exports);
  M64P_Memory_Init(env, exports);
  M64P_Interface_Init(env, exports);
  M64P_Utility_Init(env, exports);
  return M64P_Core_Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)