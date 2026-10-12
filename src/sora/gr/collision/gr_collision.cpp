#include <types.h>
#include <gf/gf_task_scheduler.h>
#include <gr/collision/gr_collision.h>

extern "C" void fn_80110A40(grCollision* collision);

void grCollision::setDisable() {
    u8* bytes = (u8*)this;
    s32 var_r31;

    if (m_isEnabled) {
        u8 temp_r0 = _spacer[0];
        var_r31 = 0;
        temp_r0 &= ~0x40;
        _spacer[0] = temp_r0;
        s32 temp_r3 = *(s32*)gfTaskScheduler::getInstance();
        if ((temp_r3 >> 24) == 1 && ((s32)((temp_r3 << 8) & 0xFF800000) >> 24) < 6) {
            var_r31 = 1;
        }
        if (var_r31 != 0) {
            bytes[0x2C] |= 0x20;
            return;
        }
        fn_80110A40(this);
    }
}

#include <gr/collision/gr_collision.h>

void grCollision::getSegment(clSegment2D* outSegment, grCollisionLine* line) {
    // HYPOTHESIS: vertex positions are 16 bytes apart, so indices are doubled over Vec2f
    u16 index0 = line->m_point0Index;
    Vec2f* p0 = &m_vtxPositions[index0 * 2];
    u16 index1 = line->m_point1Index;
    Vec2f* p1 = &m_vtxPositions[index1 * 2];
    outSegment->m_p0Pos.m_x = p0->m_x;
    outSegment->m_p0Pos.m_y = p0->m_y;
    outSegment->m_p1Pos.m_x = p1->m_x;
    outSegment->m_p1Pos.m_y = p1->m_y;
}
