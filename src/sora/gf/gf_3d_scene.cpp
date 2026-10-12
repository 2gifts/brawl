#include <gf/gf_3d_scene.h>
#include <types.h>

// HYPOTHESIS: 0x54 points at an object whose virtual at 0x1C is called with no argument.
// MATCH-ONLY: the leading unused virtuals place unk1C at vtable offset 0x1C.
struct gfSceneRootAnmObj {
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk1C();
};

// HYPOTHESIS: each layer slot at 0x394 + i * 0x18 has a virtual at 0x1C that takes the frame.
struct gfSceneRootLayerObj {
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk1C(float frame);
};

void gfSceneRoot::setCurrentFrame(float frame) {
    gfSceneRootAnmObj* obj = *reinterpret_cast<gfSceneRootAnmObj**>(reinterpret_cast<char*>(this) + 0x54);
    if (obj != nullptr) {
        obj->unk1C();
        s32 i = 0;
        char* cur = reinterpret_cast<char*>(this);
        do {
            gfSceneRootLayerObj* slot = *reinterpret_cast<gfSceneRootLayerObj**>(cur + 0x394);
            if (slot != nullptr) {
                slot->unk1C(frame);
            }
            i += 1;
            cur += 0x18;
        } while (i < 9);
    }
}


namespace nw4r {
    namespace g3d {
        class ScnRoot;
    }
}

// HYPOTHESIS: the pointer at 0x40 is the ScnRoot that the anim scene was added to.
extern "C" void RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(nw4r::g3d::ScnRoot* scnRoot);

void gfSceneRoot::removeResAnmScn() {
    if (m_anmScnRes != NULL) {
        RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(*reinterpret_cast<nw4r::g3d::ScnRoot**>(reinterpret_cast<char*>(this) + 0x40));
        m_anmScnRes->Destroy();
    }
    if (*reinterpret_cast<s32*>(reinterpret_cast<char*>(this) + 0x4C) != 0) {
        *reinterpret_cast<s32*>(reinterpret_cast<char*>(this) + 0x4C) = 0;
    }
    m_anmScnRes = NULL;
    *reinterpret_cast<s32*>(reinterpret_cast<char*>(this) + 0x58) = 0;
}
