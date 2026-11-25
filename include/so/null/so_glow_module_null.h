#pragma once

#define SO_GLOW_MODULE_ATTACK_API
#include <so/glow/so_glow_module_simple.h>

class soGlowModuleNull : public soGlowModule {
public:
    virtual ~soGlowModuleNull();
    virtual void clear();
    virtual void update();
    virtual s32 getGlowAttack();
    virtual void setGlowAttack(s32 attack);
    virtual bool isGlow();
};
