#include <types.h>

// TODO: clean up, give variables real names
namespace jj_272420 {
extern "C" {
extern void* lbl_27_bss_6148;
extern void enableHitCircle__19grHitCircleObserverFbUc(void*, s32, s32);
extern void __dl__FPv(void*);
extern u8 lbl_27_data_58060[];
}

extern "C" void* __dt__18grHitCircleSubjectFv(void* p, s32 arg2) {
    if (p) {
        *(void**)p = lbl_27_data_58060;
        u8 b5 = ((u8*)p)[5];
        void* bss = lbl_27_bss_6148;
        if (b5 && bss) {
            enableHitCircle__19grHitCircleObserverFbUc(bss, 0, ((u8*)p)[4]);
        }
        if (arg2 > 0)
            __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_27255C {
struct Owner {
    u8 pad[4];
    u8 unk4;
    u8 unk5;
};

extern Owner* lbl_27_bss_6148;

extern "C" void enableHitCircle__19grHitCircleObserverFbUciUl(Owner* self, void* p4, u32 a, u32 b, u32 c);

extern "C" void enableHitCircle__18grHitCircleSubjectFiiPv(Owner* self, void* p2, u32 a, u32 b, u32 c) {
    Owner* g = lbl_27_bss_6148;
    u32 x = a;
    if (self->unk5 == 0) {
        return;
    }
    if (g == 0) {
        return;
    }
    enableHitCircle__19grHitCircleObserverFbUciUl(g, p2, self->unk4, x, b);
}
}

// TODO: clean up, give variables real names
namespace jj_272594 {
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

struct FnOwner;

extern void* lbl_27_bss_6148;
extern "C" void setPos__19grHitCircleObserverFP5Vec2fUciUl(FnOwner* self, s32 a1, u32 key, s32 a3, s32 a4);

extern "C" void setHitCirclePos__18grHitCircleSubjectFP5Vec2fiPv(void* arg0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    u8* object = (u8*)arg0;
    void* owner = lbl_27_bss_6148;

    if (object[5] == 0)
        return;
    if (owner == 0)
        return;

    setPos__19grHitCircleObserverFP5Vec2fUciUl((FnOwner*)owner, a1, object[4], a2, a3);
}
}

