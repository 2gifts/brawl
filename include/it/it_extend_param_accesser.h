#pragma once

#include <so/so_value_accesser.h>

class itExtendParamAccesser : public soParamAccesser {
public:
    virtual float getParamFloat(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual s32 getParamInt(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual void* getParamIndefinite(soModuleAccesser* moduleAccesser, s32 id, s32 index);
};
