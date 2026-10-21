#include <types.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task_scheduler.h>
#include <gr/collision/gr_collision.h>

void grCollision::setEnable() {
    u8* bytes = (u8*)this;
    u8 temp_r0 = _spacer[0];
    temp_r0 |= 0x40;
    m_isEnabled = (temp_r0 >> 6) & 1;
    _spacer[0] = temp_r0;
    if ((bytes[0x2C] >> 5U) & 1) {
        bytes[0x2C] &= ~0x20;
    }
}

struct grCollisionBuffers {
    char unk0[0x10];
    void* unk10;
    void* unk14;
    void* unk18;
};

extern "C" void fn_80110A40(grCollision* collision);
extern "C" void fn_80112D64(s32, grCollision*);
extern "C" void* Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(void*, void*);
extern s32 lbl_805A0360;

grCollision::~grCollision() {
    grCollisionBuffers* buffers = (grCollisionBuffers*)this;

    _spacer[0] |= 0x08;
    fn_80110A40(this);
    fn_80112D64(lbl_805A0360, this);
    Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(&g_grCollisionList, &m_next);
    gfHeapManager::free(buffers->unk10);
    buffers->unk10 = NULL;
    gfHeapManager::free(buffers->unk14);
    buffers->unk14 = NULL;
    gfHeapManager::free(buffers->unk18);
    buffers->unk18 = NULL;
}

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

// HYPOTHESIS: each vertex owns a 16-byte pair, current position first and previous position second
struct grCollVtxPair {
    Vec2f pos;
    Vec2f prev;
};

void grCollision::copyVtxToPrev() {
    for (u16 i = 0; i < (u16)m_vtxLen; i++) {
        grCollVtxPair* pair = &((grCollVtxPair*)m_vtxPositions)[i];
        pair->prev.m_x = pair->pos.m_x;
        pair->prev.m_y = pair->pos.m_y;
    }
}

extern "C" void fn_80111E84(grCollisionLineWork* work, const Vec2f& pos, Vec2f* tmp, Vec2f* out);
extern f32 lbl_805A2918;

void grCollision::calcLineVector(const u32& lineId, const Vec2f& pos, Vec2f* out) {
    grCollisionLineWork work;
    Vec2f tmp;
    u32 lineIdCopy = lineId;
    if (getLineWork(&lineIdCopy, &work)) {
        fn_80111E84(&work, pos, &tmp, out);
        return;
    }
    f32 fallback = lbl_805A2918;
    out->m_x = fallback;
    out->m_y = fallback;
}
