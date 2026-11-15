#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/gr/gr_stadium.h): the scale is three floats at 0x150 and
// the frame callback is updateCallBack(float) (sora_melee gr_stadium.o vtable).

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>

class grStadium : public grYakumono {
protected:
    float m_scaleX; // 0x150
    float m_scaleY; // 0x154
    float m_scaleZ; // 0x158
public:
    grStadium(const char* taskName);
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setScaleY(float scaleY);
    virtual float getScaleY();
    virtual ~grStadium();
};
static_assert(sizeof(grStadium) == 348, "Class is wrong size!");
