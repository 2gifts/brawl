#pragma once

#include <so/so_value_accesser.h>

// The item parameters past the constant tables (4000.. floats, 24000.. ints): per-variation tables of the item resource
// (sora_melee it_extend_param_accesser.o).
class itExtendParamAccesser : public soParamAccesser {
public:
    itExtendParamAccesser();
    virtual ~itExtendParamAccesser() { }
    virtual float getParamFloat(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual s32 getParamInt(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual void* getParamIndefinite(soModuleAccesser* moduleAccesser, s32 id, s32 index);
};
