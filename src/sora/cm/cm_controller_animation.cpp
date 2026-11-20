#include <gf/gf_camera.h>
#include <types.h>
#include <cm/cm_controller_anm.h>
#include <nw4r/g3d/g3d_anmscn.h>

struct cmAnimationControllerAnm {
    char unk0[0xC];
    nw4r::g3d::G3dObj* unkC;
    nw4r::g3d::G3dObj* unk10;
};

void cmAnimationController::releaseScnAnmRes() {
    cmAnimationControllerAnm* self = (cmAnimationControllerAnm*) this;
    if (self->unk10 != NULL) {
        self->unk10->Destroy();
    }
    if (self->unkC != NULL) {
        self->unkC->Destroy();
    }
    self->unkC = NULL;
    self->unk10 = NULL;
}

// TODO: clean up, give variables real names
extern "C" void GetResAnmCameraNumEntries__Q34nw4r3g3d9ResAnmScnCFv(void*);

extern "C" void fn_800A6590(void* self) {
    GetResAnmCameraNumEntries__Q34nw4r3g3d9ResAnmScnCFv(reinterpret_cast<u8*>(self) + 0x20);
}

// TODO: clean up, give variables real names
namespace jj_800A63A0 {

extern unsigned char lbl_805A2010[8];
extern float lbl_805A2018;

class P {
public:
    char pad[0x30];
    u16 m30;
};

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual float slot6(P*);
    virtual void slot7(float);
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual u32 slot17();
    char pad[0x1c];
    P* m20;
};

class Self {
public:
    gfCamera* m0;
    char pad4[8];
    void* mC;
    Object_vptr_0* m10;
    char pad14[4];
    char* m18;
    s32 m1C;
    s32 m20;
};

extern "C" {
void calc__8gfCameraFv(gfCamera*);
void setFromNwCamera__8gfCameraFPQ34nw4r3g3d6Camera(gfCamera*, s32*);
s32 GetCamera__Q34nw4r3g3d7ScnRootFi(void*, s32);
void SetCurrentCamera__Q34nw4r3g3d7ScnRootFi(void*, s32);
void CalcWorld__Q34nw4r3g3d7ScnRootFv(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800A63A0(Self* self, float f) {
    gfCamera* t = self->m0;
    if (self->m20 == 1) {
        return;
    }
    s32 idx = self->m1C;
    if (idx < 0) {
        self->m1C = idx + 1;
        return;
    }
    Object_vptr_0* obj = self->m10;
    if (obj != 0) {
        if (obj->slot17() >= 1) {
            SetCurrentCamera__Q34nw4r3g3d7ScnRootFi(self->mC, 0);
            CalcWorld__Q34nw4r3g3d7ScnRootFv(self->mC);
            s32 sp8 = GetCamera__Q34nw4r3g3d7ScnRootFi(self->mC, 0);
            setFromNwCamera__8gfCameraFPQ34nw4r3g3d6Camera(t, &sp8);
            calc__8gfCameraFv(t);
            self->m10->slot7(f);
            self->m10->slot9();
            P* p = self->m10->m20;
            u16 v = p->m30;
            float r = self->m10->slot6(p);
            float vf = (float)(s32)v;
            float s = lbl_805A2018 + r;
            if (s >= vf) {
                self->m20 = 1;
            }
        }
    } else {
        char* tbl = self->m18;
        if (tbl != 0) {
            if ((u32)idx < *(u32*)tbl) {
                char* e = tbl + idx * 0x18;
                t->m_centerPos.m_x = *(float*)(e + 0x4);
                t->m_centerPos.m_y = *(float*)(e + 0x8);
                t->m_centerPos.m_z = *(float*)(e + 0xC);
                e = self->m18 + self->m1C * 0x18;
                t->m_targetPos.m_x = *(float*)(e + 0x10);
                t->m_targetPos.m_y = *(float*)(e + 0x14);
                t->m_targetPos.m_z = *(float*)(e + 0x18);
                t->unkFA.m_mask = t->unkFA.m_mask | 2;
                self->m1C = self->m1C + 1;
                t->m_transformFlag.m_mask = 0x43;
                calc__8gfCameraFv(t);
                return;
            }
            self->m20 = 1;
            return;
        }
        self->m20 = 1;
    }
}
}
}

