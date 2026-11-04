#pragma once

#include <so/so_value_accesser.h>

class wnValueAccesser : public soValueAccesser {
public:
    wnValueAccesser();
    virtual ~wnValueAccesser();
    virtual float getVariableFloatCore(soModuleAccesser* moduleAccesser, u32, u32 = 0);
    virtual int getVariableIntCore(soModuleAccesser* moduleAccesser, u32, u32 = 0);
    virtual float getConstantFloatCore(soModuleAccesser* moduleAccesser, u32, u32 = 0);
    virtual int getConstantIntCore(soModuleAccesser* moduleAccesser, u32, u32 = 0);
};
