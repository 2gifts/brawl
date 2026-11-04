#pragma once

#include <StaticAssert.h>
#include <types.h>

class Vec2f;

// HYPOTHESIS: grHitCircleSubject (sora_melee gr_hit_circle_subject.o). notifyHitCircle is the pure slot
// (the subject's vtable stores 0 there); the two bytes are the circle kind and the enabled flag.
class grHitCircleSubject {
public:
    virtual void notifyHitCircle(void* circle) = 0;
    virtual ~grHitCircleSubject();

    grHitCircleSubject() : unk4(0), unk5(0) {}

    // HYPOTHESIS: signatures from grAdventureBlockSympatheticManager (pos, kind, flag, radius).
    void* addHitCircle(Vec2f* pos, u8 kind, int flag, float radius);
    void enableHitCircle(int unk1, int unk2, void* circle);

    u8 unk4;
    u8 unk5;
};
static_assert(sizeof(grHitCircleSubject) == 8, "Class is wrong size!");
