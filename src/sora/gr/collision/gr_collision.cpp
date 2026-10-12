#include <types.h>
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

#include <gf/gf_heap_manager.h>
#include <gr/collision/gr_collision.h>

struct grCollisionBuffers {
    char unk0[0x10];
    void* unk10;
    void* unk14;
    void* unk18;
};

extern "C" void fn_80110A40();
extern "C" void fn_80112D64(s32, grCollision*);
extern "C" void* Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(void*, void*);
extern s32 lbl_805A0360;

grCollision::~grCollision() {
    grCollisionBuffers* buffers = (grCollisionBuffers*)this;

    _spacer[0] |= 0x08;
    fn_80110A40();
    fn_80112D64(lbl_805A0360, this);
    Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(&g_grCollisionList, &m_next);
    gfHeapManager::free(buffers->unk10);
    buffers->unk10 = NULL;
    gfHeapManager::free(buffers->unk14);
    buffers->unk14 = NULL;
    gfHeapManager::free(buffers->unk18);
    buffers->unk18 = NULL;
}
