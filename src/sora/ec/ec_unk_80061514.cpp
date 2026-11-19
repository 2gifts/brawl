#include <gr/collision/gr_collision_handle.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <ec/ec_mgr.h>
#include <ef/ef_effect.h>
#include <gf/gf_model.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>
#include <ut/ut_nw.h>
#include <gf/gf_archive.h>

void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* mdl, u32 nodeId, const Matrix* mtx);

void ecEffectWork::render() {
    if (unk24 == 2) {
        unk24 = 3;
    }
}

void ecEffectWork::release(bool p1) {
    if (unk44 != NULL) {
        if (unk40 != NULL) {
            unk40->Remove(unk44);
            unk40 = NULL;
        }
        if (unk4C != NULL) {
            if (unk4C != NULL) {
                delete unk4C;
                unk4C = NULL;
            }
        }
        if (unk48 != NULL) {
            gfModelAnimation::unbind(unk44);
            gfModelAnimation* anim = unk48;
            if (anim->m_anmObjChrRes != NULL) {
                anim->m_anmObjChrRes->Destroy();
            }
            if (anim->m_anmObjVisRes != NULL) {
                anim->m_anmObjVisRes->Destroy();
            }
            if (anim->m_anmObjTexPatRes != NULL) {
                anim->m_anmObjTexPatRes->Destroy();
            }
            if (anim->m_anmObjTexSrtRes != NULL) {
                anim->m_anmObjTexSrtRes->Destroy();
            }
            if (anim->m_anmObjMatClrRes != NULL) {
                anim->m_anmObjMatClrRes->Destroy();
            }
            if (anim->m_anmObjShpRes != NULL) {
                anim->m_anmObjShpRes->Destroy();
            }
            if (anim->m_resFile.ptr() != NULL) {
                if ((u8)anim->_spacer[0] != 0) {
                    gfHeapManager::free(anim->m_resFile.ptr());
                    anim->m_resFile = nw4r::g3d::ResFile((void*)NULL);
                }
            }
            anim->m_anmObjChrRes = NULL;
            anim->m_anmObjVisRes = NULL;
            anim->m_anmObjTexPatRes = NULL;
            anim->m_anmObjTexSrtRes = NULL;
            anim->m_anmObjMatClrRes = NULL;
            anim->m_anmObjShpRes = NULL;
            gfHeapManager::free(unk48);
            unk48 = NULL;
        }
        unk44->Destroy();
        unk44 = NULL;
    }
    if (p1 == true && unk34_ != NULL) {
        for (u32 i = 0; i < unk38; i++) {
            if (unk34_[i] != NULL) {
                unk34_[i]->endForce();
                unk34_[i] = NULL;
            }
        }
    }
    if (unk34_ != NULL) {
        delete[] unk34_;
        unk34_ = NULL;
    }
}

void ecEffectWork::pause() {
    if (!(unk58 & 8)) {
        unk58 = (unk58 & ~2) | 4;
    }
}

void ecEffectWork::play() {
    if (!(unk58 & 8)) {
        unk58 = (unk58 & ~4) | 2;
    }
}

void ecEffectWork::end(bool p1, bool p2) {
    if (!(unk58 & 0x10)) {
        if (p1) {
            if (unk34_ != NULL) {
                for (u32 i = 0; i < unk38; i++) {
                    if (unk34_[i] != NULL) {
                        unk34_[i]->endForce();
                        unk34_[i] = NULL;
                    }
                }
            }
        }
        if (p2 || unk3C < 0) {
            unk3C = 0;
        }
        unk58 = 8;
        if (p1 == true && p2 == true) {
            if (unk44 != NULL) {
                nwSMSetVisibility(unk44, false);
                if (unk4C != NULL) {
                    unk4C->SetCallbackCondition(false);
                }
                if (unk40 != NULL) {
                    unk40->Remove(unk44);
                    unk40 = NULL;
                }
            }
            unk58 = (unk58 | 0x10) & ~0x20;
        } else if (!p1) {
            unk58 |= 0x20;
        }
    }
}

void ecEffectWork::set_parent(nw4r::g3d::ScnMdl* p1, const char* p2, bool p3) {
    nw4r::g3d::ResMdl mdl = p1->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(p2);
    u32 idx = node.IsValid() ? (u16)node.GetID() : 0;
    set_parent(p1, idx, p3);
}

void ecEffectWork::set_parent(nw4r::g3d::ScnMdl* p1, u32 p2, bool p3) {
    unk50 = p1;
    if (p1 != NULL) {
        unk54 = p2;
        unk56 &= ~1;
        if (p3) {
            unk56 |= 1;
        }
        init_attach();
    }
}

void ecEffectWork::set_delay(int frames) {
    if (unk24 != 0) {
        return;
    }
    unk28 = -frames;
}

void ecEffectWork::set_visible(bool visible) {
    if (visible) {
        unk56 &= ~0x8000;
        if (unk28 < 0) {
            return;
        }
        if (unk44 != NULL && unk3C != 0) {
            nwSMSetVisibility(unk44, true);
        }
        if (unk34_ != NULL) {
            for (u32 i = 0; i < unk38; i++) {
                if (unk34_[i] != NULL) {
                    unk34_[i]->unkD_b6 = true;
                }
            }
        }
    } else {
        unk56 |= 0x8000;
        if (unk44 != NULL) {
            nwSMSetVisibility(unk44, false);
        }
        if (unk34_ != NULL) {
            for (u32 i = 0; i < unk38; i++) {
                if (unk34_[i] != NULL) {
                    unk34_[i]->unkD_b6 = false;
                }
            }
        }
    }
}

void ecEffectWork::set_rot(const Vec3f* rot) {
    m_rot = *rot;
    if (unk64 == 0 && unk50 == NULL && unk44 != NULL) {
        Matrix mtx(true);
        mtx.setSRT(m_scale, m_rot, m_pos);
        ScnMdl_SetNodeMtx(unk44, 0, &mtx);
    }
    if (unk30->unkC->unk10[unk2C].unk4 != 0 && unk34_ != NULL) {
        efEffect* effect = unk34_[unk38 - 1];
        if (effect != NULL) {
            effect->m_rot = m_rot;
        }
    }
}

void ecEffectWork::set_scl(const Vec3f* scale) {
    m_scale = *scale;
    if (unk64 == 0 && unk50 == NULL && unk44 != NULL) {
        Matrix mtx(true);
        mtx.setSRT(m_scale, m_rot, m_pos);
        ScnMdl_SetNodeMtx(unk44, 0, &mtx);
    }
    if (unk30->unkC->unk10[unk2C].unk4 != 0 && unk34_ != NULL) {
        efEffect* effect = unk34_[unk38 - 1];
        if (effect != NULL) {
            effect->m_scale = m_scale;
        }
    }
}

void ecEffectWork::set_mdl_texpat(u32 patIndex) {
    if (unk48 == NULL) {
        return;
    }
    if (unk48->m_anmObjTexPatRes == NULL) {
        return;
    }
    unk56 |= 8;
    unk48->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    unk48->m_anmObjTexPatRes->SetFrame((float)patIndex);
}

void ecEffectWork::set_mdl_frame(float frame, s8 flags) {
    if (unk48 == NULL) {
        return;
    }
    if ((u8)flags == 0xFF) {
        unk48->setFrame(frame);
    } else {
        if (flags & 1) {
            if (unk48->m_anmObjChrRes != NULL) {
                unk48->m_anmObjChrRes->SetFrame(frame);
            }
        }
        if (flags & 2) {
            if (unk48->m_anmObjVisRes != NULL) {
                unk48->m_anmObjVisRes->SetFrame(frame);
            }
        }
        if (flags & 4) {
            if (unk48->m_anmObjTexPatRes != NULL) {
                unk48->m_anmObjTexPatRes->SetFrame(frame);
            }
        }
        if (flags & 8) {
            if (unk48->m_anmObjTexSrtRes != NULL) {
                unk48->m_anmObjTexSrtRes->SetFrame(frame);
            }
        }
        if (flags & 0x10) {
            if (unk48->m_anmObjMatClrRes != NULL) {
                unk48->m_anmObjMatClrRes->SetFrame(frame);
            }
        }
        if (flags & 0x20) {
            if (unk48->m_anmObjShpRes != NULL) {
                unk48->m_anmObjShpRes->SetFrame(frame);
            }
        }
    }
    unk3C = -1;
}

void ecEffectWork::set_mdl_calc_mtx(void (*callback)(nw4r::g3d::ScnMdl*, void*), void* arg) {
    unk56 |= 0x80;
    if (callback == NULL) {
        return;
    }
    unk70 = callback;
    unk74 = arg;
}

void ecEffectWork::change_mdl_tex(const char* matName, const char* texName, nw4r::g3d::ResFile* resFile, int filter) {
    if (unk44 == NULL) {
        return;
    }
    unk30->change_tex(unk44, unk2C, matName, texName, resFile, filter);
}

void ecEffectWork::set_slow_rate(u32 rate) {
    unk5B = rate > 1 ? rate : 1;
}

void ScnObj_DisableCallbackExecOp(nw4r::g3d::ScnObj*, u32);
void ScnObj_DisableCallbackTiming(nw4r::g3d::ScnObj*, u32);

ecScnObjCallback::~ecScnObjCallback() {
    if (unk4 != NULL) {
        if (unk4 != NULL) {
            unk4->m_callback = NULL;
            ScnObj_DisableCallbackExecOp(unk4, 1);
            ScnObj_DisableCallbackExecOp(unk4, 4);
            ScnObj_DisableCallbackTiming(unk4, 4);
        }
        unk4 = NULL;
    }
}

void ecResource::set_file_img(void* image, int size, u32 id) {
    unk0 = new (static_cast<HeapType>(g_ecMgr->unk44)) gfArchive;
    if (unk0 == NULL) {
        release(false);
        return;
    }
    unk0->setFileImage(image, size, static_cast<HeapType>(g_ecMgr->unk44));
    unk14 = 2;
    regist(id);
}

void ecEffectWork::set_mdl_enable_update(bool enable, s8 flags) {
    if (unk48 == NULL) {
        return;
    }
    if ((u8)flags == 0xFF) {
        if (enable) {
            unk56 &= ~0x7E;
        } else {
            unk48->setUpdateRate(0.0f);
            unk56 |= 0x7E;
        }
    } else {
        if (flags & 1) {
            if (unk48->m_anmObjChrRes != NULL) {
                if (enable) {
                    unk56 &= ~2;
                } else {
                    unk48->m_anmObjChrRes->SetUpdateRate(0.0f);
                    unk56 |= 2;
                }
            }
        }
        if (flags & 2) {
            if (unk48->m_anmObjVisRes != NULL) {
                if (enable) {
                    unk56 &= ~4;
                } else {
                    unk48->m_anmObjVisRes->SetUpdateRate(0.0f);
                    unk56 |= 4;
                }
            }
        }
        if (flags & 4) {
            if (unk48->m_anmObjTexPatRes != NULL) {
                if (enable) {
                    unk56 &= ~8;
                } else {
                    unk48->m_anmObjTexPatRes->SetUpdateRate(0.0f);
                    unk56 |= 8;
                }
            }
        }
        if (flags & 8) {
            if (unk48->m_anmObjTexSrtRes != NULL) {
                if (enable) {
                    unk56 &= ~16;
                } else {
                    unk48->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
                    unk56 |= 16;
                }
            }
        }
        if (flags & 16) {
            if (unk48->m_anmObjMatClrRes != NULL) {
                if (enable) {
                    unk56 &= ~32;
                } else {
                    unk48->m_anmObjMatClrRes->SetUpdateRate(0.0f);
                    unk56 |= 32;
                }
            }
        }
        if (flags & 32) {
            if (unk48->m_anmObjShpRes != NULL) {
                if (enable) {
                    unk56 &= ~64;
                } else {
                    unk48->m_anmObjShpRes->SetUpdateRate(0.0f);
                    unk56 |= 64;
                }
            }
        }
    }
}

void ecEffectWork::update_odd(float frames) {
    if (unk5A > 1) {
        frames *= 1.0f / unk5A;
    }
    switch (unk24) {
    case 1:
        if (!(unk58 & 0x10)) {
            renew_attach();
            if (unk44 != NULL && unk3C != 0) {
                if (unk58 & 1) {
                    if (unk48 != NULL && !(unk56 & 0x7E)) {
                        unk48->setUpdateRate(0.0f);
                    }
                } else if (unk48 != NULL) {
                    if (!(unk56 & 0x7E)) {
                        unk48->setUpdateRate(frames);
                    } else {
                        if (!(unk56 & 2) && unk48->m_anmObjChrRes != NULL) {
                            unk48->m_anmObjChrRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 4) && unk48->m_anmObjVisRes != NULL) {
                            unk48->m_anmObjVisRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 8) && unk48->m_anmObjTexPatRes != NULL) {
                            unk48->m_anmObjTexPatRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 0x10) && unk48->m_anmObjTexSrtRes != NULL) {
                            unk48->m_anmObjTexSrtRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 0x20) && unk48->m_anmObjMatClrRes != NULL) {
                            unk48->m_anmObjMatClrRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 0x40) && unk48->m_anmObjShpRes != NULL) {
                            unk48->m_anmObjShpRes->SetUpdateRate(frames);
                        }
                    }
                }
                unk44->UpdateFrame();
                unk44->SetScnObjOption(2, 0);
                unk44->G3dProc(1, 0, 0);
                unk44->SetScnObjOption(2, 1);
                if ((unk56 & 0x80) && unk70 != NULL) {
                    unk70(unk44, unk74);
                }
            }
        }
        break;
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_8006544C(void) {}

extern "C" void fn_80065450(void) {}

extern "C" void fn_80065454(void) {}

// TODO: clean up, give variables real names
namespace jj_80064278 {
extern "C" {
void fn_80064278(int *p) {
    *p = 0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_80065300 {
struct Mtx34S { float m[3][4]; };

extern "C" {
extern Mtx34S lbl_80497810;
extern const float lbl_805A1980;
void fn_801EC158(Mtx34S*);
void fn_801EC1B8(Mtx34S*, Mtx34S*, Mtx34S*);
}

// TODO: clean up, give variables real names
extern "C" void fn_80065300(void*, int kind, void*, void*, Mtx34S* m) {
    switch (kind) {
    case 1: {
        Mtx34S tmp;
        lbl_80497810 = *m;
        fn_801EC158(&tmp);
        float s = lbl_805A1980;
        tmp.m[0][2] *= s;
        tmp.m[1][2] *= s;
        tmp.m[2][2] *= s;
        fn_801EC1B8(m, &tmp, m);
        break;
    }
    case 4:
        *m = lbl_80497810;
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_80063538 {

using namespace nw4r::g3d;

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5(float);
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10(ResMdl);
};

struct Owner {
    char pad[0x3c];
    int current;
    char pad2[4];
    ScnMdl* model;
    gfModelAnimation* animation;
};

extern "C" {
void* fn_8018D6D8(ResFile*, u32);
void* fn_8018D8E0(ResFile*, u32);
void* fn_8018DCF0(ResFile*, u32);
void* fn_8018DEF8(ResFile*, u32);
void* fn_8018DAE8(ResFile*, u32);
void* fn_8018E100(ResFile*, u32);
AnmObjShpRes* fn_8019BC04(MEMAllocator*, int*, ResAnmShp, ResMdl, bool);
extern float lbl_805A1958;
}

// TODO: clean up, give variables real names
inline void chrInner(u32 index, ResMdl model, gfModelAnimation* a) {
    int size;
    ResAnmChr anim(fn_8018D6D8(&a->m_resFile, index));
    MEMAllocator* alloc = gfHeapManager::getMEMAllocator((Heaps::HeapType)3);
    if (anim.IsValid()) {
        AnmObjChrRes* obj = AnmObjChrRes::Construct(alloc, &size, anim, model, false);
        if (obj) {
            obj->Bind(model);
            if (a->m_anmObjChrRes)
                a->m_anmObjChrRes->Destroy();
            a->m_anmObjChrRes = obj;
        }
    }
}

inline void chr(u32 index, ResMdl model, gfModelAnimation* a) {
    if (index < a->m_resFile.GetResAnmChrNumEntries())
        chrInner(index, model, a);
}

// TODO: clean up, give variables real names
inline void visInner(ResMdl model, u32 index, gfModelAnimation* a) {
    int size;
    AnmObjVisRes* obj;
    void* anim = fn_8018D8E0(&a->m_resFile, index);
    MEMAllocator* alloc = gfHeapManager::getMEMAllocator((Heaps::HeapType)3);
    if (anim) {
        obj = AnmObjVisRes::Construct(alloc, &size, ResAnmVis(anim), model);
        if (obj) {
            obj->Bind(model);
            if (a->m_anmObjVisRes)
                a->m_anmObjVisRes->Destroy();
            a->m_anmObjVisRes = obj;
        }
    }
}

inline void vis(gfModelAnimation* a, void* model, u32 index) {
    if (index < a->m_resFile.GetResAnmVisNumEntries())
        visInner(ResMdl(model), index, a);
}

#define HELPER(name, count, resource, resType, objType, member) \
inline void name##Inner(u32 index, ResMdl model, gfModelAnimation* a) { \
    int size; \
    resType anim(resource(&a->m_resFile, index)); \
    if (anim.IsValid()) { \
        MEMAllocator* alloc = gfHeapManager::getMEMAllocator((Heaps::HeapType)3); \
        objType* obj = objType::Construct(alloc, &size, anim, model, false); \
        if (obj) { \
            obj->Bind(model); \
            if (a->member) a->member->Destroy(); \
            a->member = obj; \
        } \
    } \
} \
inline void name(u32 index, void* model, gfModelAnimation* a) { \
    if (index < a->m_resFile.count()) \
        name##Inner(index, ResMdl(model), a); \
}

HELPER(pat, GetResAnmTexPatNumEntries, fn_8018DCF0, ResAnmTexPat,
       AnmObjTexPatRes, m_anmObjTexPatRes)
// TODO: clean up, give variables real names
HELPER(srt, GetResAnmTexSrtNumEntries, fn_8018DEF8, ResAnmTexSrt,
       AnmObjTexSrtRes, m_anmObjTexSrtRes)
HELPER(clr, GetResAnmClrNumEntries, fn_8018DAE8, ResAnmClr,
       AnmObjMatClrRes, m_anmObjMatClrRes)

inline void shpInner(u32& index, ResMdl model, gfModelAnimation* a) {
    int size;
    index = (u32)fn_8018E100(&a->m_resFile, index);
    if (index) {
        MEMAllocator* alloc = gfHeapManager::getMEMAllocator((Heaps::HeapType)3);
        AnmObjShpRes* obj = fn_8019BC04(alloc, &size, ResAnmShp((void*)index), model, false);
        if (obj) {
            obj->Bind(model);
            if (a->m_anmObjShpRes)
                obj->Destroy();
            a->m_anmObjShpRes = obj;
        }
    }
}

inline void shp(u32& index, void* model, gfModelAnimation* a) {
    if (index < a->m_resFile.GetResAnmShpNumEntries())
        shpInner(index, ResMdl(model), a);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80063538(Owner* self, u32 index, int flags) {
    void* visModel;
    if (self->animation) {
        if (flags & 1) {
            u32 count = self->animation->m_resFile.GetResAnmChrNumEntries();
            if ((count << __cntlzw(count ^ index)) >> 31) {
                self->animation->unbindNodeAnim(self->model);
                chr(index, *(ResMdl*)((char*)self->model + 0xe8), self->animation);
                self->animation->bindNodeAnim(self->model);
                self->animation->m_anmObjChrRes->SetFrame(lbl_805A1958);
            }
        }

        if (flags & 2) {
            u32 count = self->animation->m_resFile.GetResAnmVisNumEntries();
            if ((count << __cntlzw(count ^ index)) >> 31) {
                self->animation->unbindVisibleAnim(self->model);
                visModel = *(void**)((char*)self->model + 0xe8);
                vis(self->animation, visModel, index);
                self->animation->bindVisibleAnim(self->model);
                self->animation->m_anmObjVisRes->SetFrame(lbl_805A1958);
            }
        }

        if (flags & 4) {
            u32 count = self->animation->m_resFile.GetResAnmTexPatNumEntries();
            if ((count << __cntlzw(count ^ index)) >> 31) {
                self->animation->unbindTexAnim(self->model);
                visModel = *(void**)((char*)self->model + 0xe8);
                pat(index, visModel, self->animation);
                self->animation->bindTexAnim(self->model);
                self->animation->m_anmObjTexPatRes->SetFrame(lbl_805A1958);
            }
        }

        if (flags & 8) {
            u32 count = self->animation->m_resFile.GetResAnmTexSrtNumEntries();
            if ((count << __cntlzw(count ^ index)) >> 31) {
                self->animation->unbindTexSrtAnim(self->model);
                visModel = *(void**)((char*)self->model + 0xe8);
                srt(index, visModel, self->animation);
                self->animation->bindTexSrtAnim(self->model);
                self->animation->m_anmObjTexSrtRes->SetFrame(lbl_805A1958);
            }
        }

        if (flags & 16) {
            u32 count = self->animation->m_resFile.GetResAnmClrNumEntries();
            if ((count << __cntlzw(count ^ index)) >> 31) {
                self->animation->unbindMatColAnim(self->model);
                visModel = *(void**)((char*)self->model + 0xe8);
                clr(index, visModel, self->animation);
                self->animation->bindMatColAnim(self->model);
                self->animation->m_anmObjMatClrRes->SetFrame(lbl_805A1958);
            }
        }

        if (flags & 32) {
            u32 count = self->animation->m_resFile.GetResAnmShpNumEntries();
            if ((count << __cntlzw(count ^ index)) >> 31) {
                self->animation->unbindShapeAnim(self->model);
                visModel = *(void**)((char*)self->model + 0xe8);
                shp(index, visModel, self->animation);
                self->animation->bindShapeAnim(self->model);
                self->animation->m_anmObjShpRes->SetFrame(lbl_805A1958);
            }
        }

        self->current = -1;
    }
}
}

namespace jj_80061944 {

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1(int, int, int);
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6(int, int);
    virtual void slot7();
    virtual void slot8(float);
};

struct Entry {
    char pad00[12];
    unsigned int state : 8;
    unsigned int other : 24;
    float value;
    char pad14[8];
    Object_vptr_0* model;
    char pad20[2];
    u16 node;
};

struct Resource {
    char pad00[12];
    char* data;
};

struct TableEntry {
    char pad00[20];
    u32 field14;
    u32 field18;
};

struct AnimResource {
    char pad00[4];
    u8 count;
    char pad05[11];
};

struct AnimEntry {
    u32 name;
    u32 field04;
    u16 frame;
    u16 field0a;
    u32 data;
};

struct FlagBits {
    u16 hi : 6;
    u16 b9 : 1;
    u16 b8 : 1;
    u16 lo : 8;
};

struct Record {
    char pad00[0x24];
    int state;
    int frame;
    int index;
    Resource* resource;
    Entry** entries;
    u32 count;
    int timer;
    int field40;
    Object_vptr_0* model;
    gfModelAnimation* anim;
    void* effect;
    char pad50[6];
    u16 flags;
    u8 mode;
    u8 delay;
    u8 frameCount;
    u8 frameMax;
    grCollisionHandle collision;
    int collisionEnabled;
    void* collisionData;
    char pad6c[4];
    void (*callback)(Object_vptr_0*, void*);
    void* callbackArg;
    char pad78[4];
    int result;
};

extern "C" {
void fn_80062954(Record*, int, int);
void fn_8005A98C(Entry*);
Entry* fn_8005C650(void*, u8);
int fn_8005C8F0(Entry*);
void fn_8005C9B4(Entry*);
void fn_8006242C(Record*);
void fn_80062734(Record*, int);
void fn_8006526C(void*, int);
void fn_801AF9F4(void*);
void nwSMSetVisibility__FPQ34nw4r3g3d12ScnMdlSimpleb(void*, bool);
void* GetResNode__Q34nw4r3g3d6ResMdlCFPCc(
    const nw4r::g3d::ResMdl*, const char*);
}

extern const float lbl_805A1958;
extern float lbl_805A195C;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80061944(Record* self, float rate) {
    TableEntry* table =
        (TableEntry*)(self->resource->data + self->index * 16);
    AnimResource* res;
    u32 offset;
    union {
        u32 off;
        Entry* entry;
    } work;
    int waiting = 0;

    if (self->collisionEnabled) {
        if (!self->collision.isValid()) {
            fn_80062954(self, 1, 1);
        } else {
            void* col = *(void**)((char*)self + 0x5c);
            if (!((*(u8*)((char*)col + 0x2c) >> 7) & 1) ||
                !((*(u8*)((char*)self->collisionData + 0x54) >> 4) & 1)) {
                fn_80062954(self, 1, 1);
            } else if (*(int*)((char*)col + 0x10) == 0) {
                fn_80062954(self, 1, 1);
            }
        }
    }

    if (self->mode & 1) {
        if (self->mode & 2) {
            self->mode = 0;
            for (u32 i = 0; i < self->count; ++i) {
                Entry* entry = self->entries[i];
                if (entry)
                    entry->state = 2;
            }
        }
    } else if (self->mode & 4) {
        self->mode = 1;
        for (u32 i = 0; i < self->count; ++i) {
            if (self->entries[i])
                fn_8005A98C(self->entries[i]);
        }
    }

    if (!(self->mode & 1)) {
        if (self->delay) {
            --self->delay;
            waiting = 1;
        } else {
            if (self->frameCount != self->frameMax) {
                self->frameCount = self->frameMax;
                if (self->frameMax > 1)
                    self->delay = self->frameMax - 1;
                for (u32 i = 0; i < self->count; ++i) {
                    if (self->entries[i])
                        *((u8*)self->entries[i] + 0x0f) = self->frameCount;
                }
            } else if (self->frameCount > 1) {
                self->delay = self->frameCount - 1;
            }

            if ((u32)self->state < 2 && (self->mode & 0x28) != 8 &&
                self->model && table->field18) {
                u32 i = 0;
                offset = 0;
                res = (AnimResource*)(self->resource->data + table->field18);
                AnimEntry* item = (AnimEntry*)((char*)res + 0x10);
                for (; i < res->count;
                     offset += 4, ++i, item = (AnimEntry*)((char*)item + 0x10)) {
                    if (item->frame == self->frame && item->data) {
                        *(Entry**)((char*)self->entries + offset) =
                            fn_8005C650((char*)res + item->data,
                                (self->flags & 0x100) ? 2
                                    : (((FlagBits*)&self->flags)->b9 ^ 1));
                        if (*(Entry**)((char*)self->entries + offset)) {
                            if (self->flags & 0x400)
                                *((u8*)*(Entry**)((char*)self->entries + offset) + 0x0d) |= 0x20;

                            const char* name = (const char*)res + item->name;
                            work.entry = *(Entry**)((char*)self->entries + offset);
                            Object_vptr_0* model = self->model;
                            work.entry->model = model;
                            void* resource = *(void**)((char*)model + 0xe8);
                            void* node = GetResNode__Q34nw4r3g3d6ResMdlCFPCc(
                                (nw4r::g3d::ResMdl*)&resource, name);
                            work.entry->node = node ? *(u32*)((char*)node + 0x0c) : 0;

                            if (self->flags & 0x8000)
                                *((u8*)*(Entry**)((char*)self->entries + offset) + 0x0d) &= ~0x40;
                            *((u8*)*(Entry**)((char*)self->entries + offset) + 0x0f) = self->frameCount;
                        }
                    }
                }
            }
        }
    }

    if (self->frameCount > 1)
        rate *= lbl_805A195C / (float)self->frameCount;

    switch (self->state) {
    case 0:
        if ((self->mode & 0x28) != 8) {
            if (!waiting && !(self->mode & 1) && self->frame >= 0) {
                if (self->model && !(self->flags & 0x8000)) {
                    nwSMSetVisibility__FPQ34nw4r3g3d12ScnMdlSimpleb(
                        self->model, true);
                    if (self->effect)
                        fn_8006526C(self->effect, 1);
                }
                if (table->field14 && self->entries) {
                    Entry* entry = self->entries[self->count - 1];
                    if (entry) {
                        entry->state = 1;
                        entry->value = lbl_805A1958;
                        if (self->flags & 0x8000)
                            *((u8*)self->entries[self->count - 1] + 0x0d) &= ~0x40;
                    }
                }
                fn_8006242C(self);
                self->state = 1;
            }
            break;
        }
        self->state = 1;
    case 1: {
        if (self->mode & 0x20) {
            if (self->entries) {
                for (u32 i = 0; i < self->count; ++i) {
                    if (self->entries[i])
                        fn_8005C9B4(self->entries[i]);
                }
            }
            self->mode &= ~0x20;
        }

        int finished = 1;
        if (!(self->mode & 0x10)) {
            fn_8006242C(self);
            if (self->model) {
                if (self->timer) {
                    if (self->mode & 1) {
                        if (self->anim && !(self->flags & 0x7e))
                            self->anim->setUpdateRate(lbl_805A1958);
                    } else {
                        if (self->anim) {
                            if (!(self->flags & 0x7e)) {
                                self->anim->setUpdateRate(rate);
                            } else {
                                if (!(self->flags & 2) && self->anim->m_anmObjChrRes)
                                    self->anim->m_anmObjChrRes->SetUpdateRate(rate);
                                if (!(self->flags & 4) && self->anim->m_anmObjVisRes)
                                    self->anim->m_anmObjVisRes->SetUpdateRate(rate);
                                if (!(self->flags & 8) && self->anim->m_anmObjTexPatRes)
                                    self->anim->m_anmObjTexPatRes->SetUpdateRate(rate);
                                if (!(self->flags & 0x10) && self->anim->m_anmObjTexSrtRes)
                                    self->anim->m_anmObjTexSrtRes->SetUpdateRate(rate);
                                if (!(self->flags & 0x20) && self->anim->m_anmObjMatClrRes)
                                    self->anim->m_anmObjMatClrRes->SetUpdateRate(rate);
                                if (!(self->flags & 0x40) && self->anim->m_anmObjShpRes)
                                    self->anim->m_anmObjShpRes->SetUpdateRate(rate);
                            }
                        }
                        if (waiting) {
                            finished = 0;
                        } else if (self->timer > 0) {
                            if (--self->timer > 0)
                                finished = 0;
                        } else if (self->timer < 0) {
                            finished = 0;
                        }
                    }

                    fn_801AF9F4(self->model);
                    self->model->slot6(2, 0);
                    self->model->slot1(1, 0, 0);
                    self->model->slot6(2, 1);
                    if ((self->flags & 0x80) && self->callback)
                        self->callback(self->model, self->callbackArg);
                }

                if (!self->timer) {
                    nwSMSetVisibility__FPQ34nw4r3g3d12ScnMdlSimpleb(
                        self->model, false);
                    if (self->effect)
                        fn_8006526C(self->effect, 0);
                }
            }
        }

        if (!(self->mode & 1)) {
            if (self->entries) {
                for (u32 i = 0; i < self->count; ++i) {
                    if (self->entries[i] && fn_8005C8F0(self->entries[i])) {
                        finished = 0;
                        break;
                    }
                }
            }
            if (finished) {
                self->state = 2;
                self->mode |= 0x10;
            }
        }
        break;
    }
    case 3:
        fn_80062734(self, 1);
        self->result = 0;
        return 1;
    default:
        break;
    }

    if (!waiting && !(self->mode & 1))
        ++self->frame;
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_80062B78 {

namespace nw4r { namespace g3d { class ScnMdlSimple; } }
struct Node {
    char pad[0x1c];
    nw4r::g3d::ScnMdlSimple* model;
    char pad20[2];
    unsigned short index;
    Vec3f position;
    void set(nw4r::g3d::ScnMdlSimple* m, unsigned short i) { model = m; index = i; }
};
struct Entry {
    char pad[4];
    void* value;
    char rest[8];
};
struct Table {
    char pad[0xc];
    Entry* entries;
};
struct Object {
    Vec3f position;
    Vec3f rotation;
    Vec3f scale;
    char pad24[8];
    int entryIndex;
    Table* table;
    Node** nodes;
    int nodeIndex;
    char pad3c[8];
    void* target;
    char pad48[8];
    nw4r::g3d::ScnMdlSimple* model;
    unsigned short index;
    unsigned short flags;
};
extern "C" void fn_800613B8(Matrix*, nw4r::g3d::ScnMdlSimple*, unsigned short, const Vec3f*, const Vec3f*, const Vec3f*);
extern "C" void ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(void*, int, Matrix*);
inline void setPosition(Vec3f& p, float z, float y, float x) { p.m_y = y; p.m_x = x; p.m_z = z; }
extern "C" void init_attach__12ecEffectWorkFv(Object* self) {
    Entry* entry = self->table->entries + self->entryIndex;
    if (!(self->flags & 1)) {
        if (self->target) {
            Matrix m(true);
            fn_800613B8(&m, self->model, self->index, &self->scale, &self->rotation, &self->position);
            ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(self->target, 0, &m);
        }
        if (entry[1].value && self->nodes) {
            Node* node = *(Node**)((char*)(self->nodes + self->nodeIndex) - 4);
            if (node) {
                node->set(self->model, self->index);
            }
        }
    } else {
        Matrix m(true);
        fn_800613B8(&m, self->model, self->index, &self->scale, &self->rotation, &self->position);
        Vec3f pos;
        float z = m.m[2][3];
        float y = m.m[1][3];
        float x = m.m[0][3];
        setPosition(pos, z, y, x);
        if (self->target) {
            m.setSRT(self->scale, self->rotation, pos);
            ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(self->target, 0, &m);
        }
        if (entry[1].value && self->nodes) {
            Node* node = *(Node**)((char*)(self->nodes + self->nodeIndex) - 4);
            if (node) {
                node->position = pos;
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_80064284 {

extern "C" {

void __dla__FPv(void*);
void __dl__FPv(void*);
void __dt__9gfArchiveFv(void*, int);
void fn_8001AE1C(void*, char*);
int sprintf(char*, const char*, ...);
extern void* lbl_8059FFC8;
extern char lbl_8059DAE8;

struct ArchiveData {
    void* f00;
    void* f04;
    void* f08;
    int f0C;
    int f10;
    unsigned char f14;
};

void release__10ecResourceFb(ArchiveData* self, int flag) {
    void* archive = self->f00;
    if (archive != 0) {
        void* heap = lbl_8059FFC8;
        if (heap != 0) {
            void* result = ((gfArchive*)archive)->getData(Data_Type_Misc, 1, 0xFFFE);
            ((gfArchive*)self->f00)->getData(Data_Type_Misc, 2, 0xFFFE);
            if (result != 0) {
                char buf[16];
                sprintf(buf, &lbl_8059DAE8, self->f10);
                fn_8001AE1C(heap, buf);
            }
        }
        if (flag != 0) {
            self->f14 = 0xFF;
        }
    }

    void* ptr4 = self->f04;
    if (ptr4 != 0) {
        if (ptr4 != 0) {
            if (ptr4 != 0) {
                __dla__FPv((char*)ptr4 - 0x10);
            }
            self->f04 = 0;
        }
    }

    void* ptr8 = self->f08;
    if (ptr8 != 0) {
        __dl__FPv(ptr8);
        self->f08 = 0;
    }

    void* ptr0 = self->f00;
    if (ptr0 != 0) {
        __dt__9gfArchiveFv(ptr0, 1);
        self->f00 = 0;
    }
}

}
}

// TODO: clean up, give variables real names
namespace jj_8006526C {
extern "C" void ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_DisableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_EnableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_DisableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(void*, int);

extern "C" void SetCallbackCondition__16ecScnObjCallbackFb(void* arg0, int arg1) {
    void* temp_r5 = *(void**)((char*)arg0 + 0x4);
    if (temp_r5 != 0) {
        if (arg1 != 0) {
            *(void**)((char*)temp_r5 + 0xD4) = arg0;
            ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(*(void**)((char*)arg0 + 0x4), 1);
            ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(*(void**)((char*)arg0 + 0x4), 4);
            ScnObj_EnableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(*(void**)((char*)arg0 + 0x4), 4);
            return;
        }
        *(void**)((char*)temp_r5 + 0xD4) = 0;
        ScnObj_DisableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(*(void**)((char*)arg0 + 0x4), 1);
        ScnObj_DisableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(*(void**)((char*)arg0 + 0x4), 4);
        ScnObj_DisableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(*(void**)((char*)arg0 + 0x4), 4);
    }
}
}

