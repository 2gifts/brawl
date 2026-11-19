#include <types.h>

// TODO: clean up, give variables real names
namespace jj_268BCC {
class grYakumonoPunchSlider;

extern "C" {
void __dt__10grYakumonoFv(void* self, int flags);
void __dl__FPv(void* p);

void* __dt__21grYakumonoPunchSliderFv(grYakumonoPunchSlider* self, int flags)
{
    if (self != 0) {
        __dt__10grYakumonoFv(self, 0);
        if (flags > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_269830 {
struct gfArchive;

struct Callback {
    void* object;
    unsigned long type;
    unsigned char padding[0x78];
};

struct Slider {
    unsigned char padding0[0xD4];
    Callback* callbacks;
    unsigned char padding1[0x88];
    void* object;
    unsigned long index;
};

extern "C" void makeCalcuCallback__9grGimmickFUlQ25Heaps8HeapType(
    void*, unsigned long, int);
extern "C" void setCalcuCallbackRoot__9grGimmickFUl(void*, unsigned long);

extern "C" void setCulcCallback__21grYakumonoPunchSliderFP9gfArchive(
    Slider* self, gfArchive*) {
    self->index = 1;
    if (self->object) {
        makeCalcuCallback__9grGimmickFUlQ25Heaps8HeapType(self, 2, 15);
        self->callbacks[self->index].object = self->object;
        self->callbacks[self->index].type = 6;
    } else {
        makeCalcuCallback__9grGimmickFUlQ25Heaps8HeapType(self, 1, 15);
    }
    setCalcuCallbackRoot__9grGimmickFUl(self, 3);
}
}

// TODO: clean up, give variables real names
namespace jj_269A6C {
extern "C" {
void setFrameUpdate__19grGimmickMotionPathFf(void*, float);
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
extern void* g_sndSystem;

void onInflict__21grYakumonoPunchSliderFP14soCollisionLogUlf(
    void* self, void*, unsigned long, float)
{
    switch (*(unsigned char*)((char*)self + 0x158)) {
    case 2:
    case 3:
        void* motion = *(void**)((char*)self + 0x150);
        void* data = *(void**)((char*)self + 0x154);
        float frame = *(float*)((char*)motion + 0x158);
        setFrameUpdate__19grGimmickMotionPathFf(
            motion, *(float*)((char*)data + 0x94) * frame);
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x2269, -1, 0, 0, -1);
        break;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_269ADC {
extern "C" {
void* __dt__18ykDamageModuleImplFv(void*, int);
void __dt__8soDamageFv(void*, int);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__24soCollisionHitModuleImplFv(void*, short);
void __dt__19soCollisionHitGroupFv(void*, int);
void __dt__16soCollisionGroupFv(void*, int);
void __dt__18soCollisionHitPartFv(void*, int);
void __dt__27soCollisionAttackModuleImplFv(void*, short);
void __dt__21soCollisionAttackPartFv(void*, int);
void __dt__8YakumonoFv(void*, int);
void __dl__FPv(void*);
}

extern "C" void* fn_27_269ADC(char* self, int flag) {
    if (self) {
        if (self + 0x6C0) {
            __dt__18ykDamageModuleImplFv(self + 0x76C, -1);
            if (self + 0x6C0 && self + 0x6C0) {
                __destroy_arr(self + 0x6CC, __dt__8soDamageFv, 0xA0, 1);
            }
        }
        if (self + 0x51C) {
            __dt__24soCollisionHitModuleImplFv(self + 0x658, -1);
            if (self + 0x614) {
                __destroy_arr(self + 0x620, __dt__19soCollisionHitGroupFv, 0x38, 1);
            }
            if (self + 0x590) {
                __destroy_arr(self + 0x59C, __dt__16soCollisionGroupFv, 0x78, 1);
            }
            if (self + 0x51C) {
                __destroy_arr(self + 0x528, __dt__18soCollisionHitPartFv, 0x68, 1);
            }
        }
        if (self + 0x358) {
            __dt__27soCollisionAttackModuleImplFv(self + 0x47C, -1);
            if (self + 0x3F4) {
                __destroy_arr(self + 0x400, __dt__16soCollisionGroupFv, 0x78, 1);
            }
            if (self + 0x358) {
                __destroy_arr(self + 0x364, __dt__21soCollisionAttackPartFv, 0x90, 1);
            }
        }
        __dt__8YakumonoFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_269C38 {
extern "C" void fn_27_269ADC(void*);

extern "C" void fn_27_269C38(void* self) {
    fn_27_269ADC(reinterpret_cast<u8*>(self) - 0x40);
}
}

