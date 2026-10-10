#pragma once

#include <st_gw/gr_gw.h>

// The smoke of the fire scene: it plays while the scene plays and has no collision when it is hidden.
class grGWFireSmoke : public grGW {
public:
    static grGWFireSmoke* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    inline grGWFireSmoke(const char* taskName);
    virtual ~grGWFireSmoke();
    virtual void update(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
};
static_assert(sizeof(grGWFireSmoke) == 0x174, "Class is wrong size!");
