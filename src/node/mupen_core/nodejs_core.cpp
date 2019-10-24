#include <m64p_types.h>

#include "nodejs_core.h"
#include "nodejs_callbacks.h"

#include "../../mupen_core/core_interface.h"

Number npmCoreEmuState(const CallbackInfo& info) {
    uint32_t value;
    uint32_t err = (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);

    if (!err) return Number::New(info.Env(), value);
    else return Number::New(info.Env(), -1);
}

Value npmSetFrameCallback(const CallbackInfo& info) {
    auto cb = info[0].As<Function>();    
    Init_Callback_Frame(cb);
    return info.Env().Undefined();
}

Object M64P_Core_Init(Env env, Object exports) {
    exports.Set("coreEmuState", Function::New(env, npmCoreEmuState));
    exports.Set("setFrameCallback", Function::New(env, npmSetFrameCallback));
    return exports;
}