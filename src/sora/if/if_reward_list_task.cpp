#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <ut/ut_list.h>
#include <gf/gf_pad_status.h>
#include <mu/mu_object.h>
#include <snd/snd_system.h>
#include <nw4r/math/math_arithmetic.h>
#include <types.h>
#include <if/if_reward_list_task.h>
#include <if/if_utility.h>
#include <mu/mu_msg.h>
#include <ty/ty_seal_listmng.h>
#include <string.h>

// Function bodies below are taken from https://github.com/jj-link/brawl.

// TODO: clean up, give variables real names
extern "C" void fn_800F851C(int* p) {
    *p = 0;
}

IfRewardListTask::Panel::Panel() {
}

IfRewardListTask::Panel::~Panel() {
}

void IfRewardListTask::detachDialogMsg() {
    if (unk340 != NULL) {
        for (int message = 0; message < 2; ++message) {
            unk340->detachMuObject(unk268);
        }
    }
}

// muMenuController::main (the map name); include/mu/mu_menu_controller.h calls it by this stand-in name.
extern "C" void fn_800B9984(muMenuController* controller);

void IfRewardListTask::processDefault() {
    if (!unk101) {
        if (!utility::Loader::isLoadFinish()) {
            return;
        }
        createModel();
        initMsg();
        unk101 = true;
    }
    fn_800B9984(&unk48);
    controllProc();
    if (unk100) {
        updateScroll();
    }
}

char lbl_8045BFB8[15] = "Seal_Sammy_Mat";
char lbl_8059E468[6] = "pos00";
char lbl_8059E470[8] = "";
extern const float lbl_805A26A0;

// TODO: clean up, give variables real names
namespace jj_800F8680 {
extern "C" {
void fn_800B8A0C(void*, int);
void __dl__FPv(void*);
}

// TODO: clean up, give variables real names
extern "C" void* fn_800F8680(void** p, int n) {
    if (p) {
        fn_800B8A0C(*p, 1);
        if (n > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

namespace jj_800F7CCC {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_800F7D30(void*, int, int, int);

// TODO: clean up, give variables real names
extern "C" void* fn_800F7CCC(int a, int b, int c) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x27ac, 0x2a);
    if (p != 0) {
        p = fn_800F7D30(p, a, b, c);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_800F86DC {

extern "C" {

void fn_800B8A0C(void*, int);
void fn_800B8EDC(void*, void*);
void fn_800FABEC(void*, void*);
void fn_800F8548(void*);
void fn_800F8680(void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void __destroy_arr(void*, void*, int, int);
void release__14gfFileIOHandleFv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
void removeExist__6utListFP10utListNode(void*, void*);

extern char lbl_8045BFC8[];
extern char lbl_8045B4B8[];
extern char lbl_8049E550[];

}

class gfPad5CProbe {
public:
    char pad[0x5C];
};

class gfVCallProbe : public gfPad5CProbe {
public:
    virtual void f(int);
};

extern "C" {

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_800F86DC(void* self, int arg1) {
    char* s = (char*)self;

    if (self != 0) {
        unsigned int j;
        unsigned int i;
        char* p;
        char* q;

        *(void**)(s + 0x3C) = lbl_8045BFC8;
        p = s;
        i = 0;
        do {
            q = s;
            j = 0;
            do {
                fn_800B8EDC(*(void**)(p + 0x334), *(void**)(q + 0x274));
                j++;
                q += 0x18;
            } while (j < 8);
            i++;
            p += 4;
        } while (i < 3);

        if ((void*)(s + 0x378) != 0) {
            *(void**)(s + 0x384) = lbl_8045B4B8;
            fn_800FABEC(s + 0x378, lbl_8045B4B8);
            release__14gfFileIOHandleFv(s + 0x388);
            __dt__14gfFileIOHandleFv(s + 0x388, -1);
            if ((void*)(s + 0x378) != 0) {
                if ((void*)(s + 0x378) != 0) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x378);
                    *(void**)(s + 0x380) = 0;
                    if ((void*)(s + 0x378) != 0) {
                        if (*(void**)(s + 0x380) != 0) {
                            removeExist__6utListFP10utListNode(0, s + 0x378);
                            *(void**)(s + 0x380) = 0;
                        }
                    }
                }
            }
        }

        if ((void*)(s + 0x360) != 0) {
            *(void**)(s + 0x36C) = lbl_8045B4B8;
            fn_800FABEC(s + 0x360, lbl_8045B4B8);
            release__14gfFileIOHandleFv(s + 0x370);
            __dt__14gfFileIOHandleFv(s + 0x370, -1);
            if ((void*)(s + 0x360) != 0) {
                if ((void*)(s + 0x360) != 0) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x360);
                    *(void**)(s + 0x368) = 0;
                    if ((void*)(s + 0x360) != 0) {
                        if (*(void**)(s + 0x368) != 0) {
                            removeExist__6utListFP10utListNode(0, s + 0x360);
                            *(void**)(s + 0x368) = 0;
                        }
                    }
                }
            }
        }

        if ((void*)(s + 0x348) != 0) {
            *(void**)(s + 0x354) = lbl_8045B4B8;
            fn_800FABEC(s + 0x348, lbl_8045B4B8);
            release__14gfFileIOHandleFv(s + 0x358);
            __dt__14gfFileIOHandleFv(s + 0x358, -1);
            if ((void*)(s + 0x348) != 0) {
                if ((void*)(s + 0x348) != 0) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x348);
                    *(void**)(s + 0x350) = 0;
                    if ((void*)(s + 0x348) != 0) {
                        if (*(void**)(s + 0x350) != 0) {
                            removeExist__6utListFP10utListNode(0, s + 0x348);
                            *(void**)(s + 0x350) = 0;
                        }
                    }
                }
            }
        }

        if ((void*)(s + 0x340) != 0) {
            fn_800B8A0C(*(void**)(s + 0x340), 1);
        }

        __destroy_arr(s + 0x334, fn_800F8680, 4, 3);
        __destroy_arr(s + 0x274, fn_800F8548, 0x18, 8);

        if ((void*)(s + 0x270) != 0) {
            void* o = *(void**)(s + 0x270);
            if (o != 0) {
                Destroy__Q34nw4r3g3d6G3dObjFv(o);
            }
            *(void**)(s + 0x270) = 0;
        }

        if ((void*)(s + 0x26C) != 0) {
            void* o = *(void**)(s + 0x26C);
            if (o != 0) {
                ((gfVCallProbe*)o)->f(1);
            }
        }

        if ((void*)(s + 0x268) != 0) {
            void* o = *(void**)(s + 0x268);
            if (o != 0) {
                ((gfVCallProbe*)o)->f(1);
            }
        }

        if ((void*)(s + 0x264) != 0) {
            void* o = *(void**)(s + 0x264);
            if (o != 0) {
                Destroy__Q34nw4r3g3d6G3dObjFv(o);
            }
            *(void**)(s + 0x264) = 0;
        }

        if ((void*)(s + 0x24C) != 0) {
            *(void**)(s + 0x258) = lbl_8045B4B8;
            fn_800FABEC(s + 0x24C, lbl_8045B4B8);
            release__14gfFileIOHandleFv(s + 0x25C);
            __dt__14gfFileIOHandleFv(s + 0x25C, -1);
            if ((void*)(s + 0x24C) != 0) {
                if ((void*)(s + 0x24C) != 0) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x24C);
                    *(void**)(s + 0x254) = 0;
                    if ((void*)(s + 0x24C) != 0) {
                        if (*(void**)(s + 0x254) != 0) {
                            removeExist__6utListFP10utListNode(0, s + 0x24C);
                            *(void**)(s + 0x254) = 0;
                        }
                    }
                }
            }
        }

        __dt__6gfTaskFv(s, 0);
        if (arg1 > 0) {
            __dl__FPv(s);
        }
    }

    return self;
}

}
}

namespace jj_800F9A14 {

extern "C" {
gfPadStatus fn_800B9F90(void* a, int flag);
f32 fn_800FAAC8(int stickY, f32 lo, f32 hi);
}

extern const f32 lbl_805A2678;
extern const f32 lbl_805A2680;
extern const f32 lbl_805A2684;
extern const f32 lbl_805A2688;
extern const f32 lbl_805A268C;
extern const f32 lbl_805A2690;

struct Self {
    char _0x0[0x48];
    char m_input[0x8];
    int m_50;
    char _0x54[0x4];
    int m_58;
    char _0x5c[0x9c];
    int m_state;
    f32 m_fc;
    u8 m_flags;
    char _0x101[0x167];
    MuObject* m_mu;
    char _0x26c[0x1138];
    u16 m_13a4;
    char _0x13a6[0x1404];
    u16 m_27aa;
};

static inline f32 lowerClamp(f32 value, f32 lo) {
    return nw4r::math::FSelect(value - lo, value, lo);
}

// TODO: clean up, give variables real names
extern "C" int fn_800F9A14(Self* self) {
    gfPadStatus in = fn_800B9F90(self->m_input, 0);

    if (self->m_state != 0) {
        if (in.m_buttonsPressedThisFrame.bits & 0x100) {
            g_sndSystem->playSE((SndID)(((self->m_flags >> 6) & 1) ? 0x13 : 1), -1, 0, 0, -1);
            return 1;
        }
        if (in.m_buttonsPressedThisFrame.bits & 0x200) {
            g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
            g_sndSystem->playSE((SndID)6, -1, 0, 0, -1);
            switch (self->m_state) {
            case 0:
                break;
            case 1:
                if ((self->m_flags >> 6) & 1) {
                    g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
                }
                /* fallthrough */
            case 2:
                self->m_flags = (u8)(self->m_flags & ~0x40);
                self->m_mu->setFrameMatCol(lbl_805A2680);
                break;
            }
            return 1;
        }
    }

    if (self->m_58 & 1) {
        switch (self->m_state) {
        case 0:
            break;
        case 1:
            if (((u32)((self->m_flags >> 6) & 1)) != 1) {
                g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            }
            /* fallthrough */
        case 2:
            self->m_flags = (u8)(self->m_flags | 0x40);
            self->m_mu->setFrameMatCol(lbl_805A2678);
            break;
        }
    } else if (self->m_58 & 2) {
        switch (self->m_state) {
        case 0:
            break;
        case 1:
            if ((self->m_flags >> 6) & 1) {
                g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            }
            /* fallthrough */
        case 2:
            self->m_flags = (u8)(self->m_flags & ~0x40);
            self->m_mu->setFrameMatCol(lbl_805A2680);
            break;
        }
    } else if (!(self->m_50 & 3)) {
        f32 speed;
        if (in.m_buttonsCurrentFrame2.bits & 8) {
            speed = lbl_805A2684;
        } else if (in.m_buttonsCurrentFrame2.bits & 4) {
            speed = lbl_805A2688;
        } else {
            speed = fn_800FAAC8(in.m_stickY, lbl_805A2678, lbl_805A2680);
        }
        f32 old = self->m_fc;
        f32 next = old + lbl_805A268C * speed;
        f32 limit = (f32)self->m_27aa - lbl_805A2690;
        self->m_fc = nw4r::math::FSelect(next - limit, limit, next);

        f32 lo = (self->m_13a4 != 0) ? lbl_805A2684 : lbl_805A2678;
        self->m_fc = lowerClamp(self->m_fc, lo);
        if ((s32)old != (s32)self->m_fc) {
            g_sndSystem->playSE((SndID)7, -1, 0, 0, -1);
        }
    }
    return 0;
}
}

#include <gf/gf_3d_scene.h>
#include <stdio.h>
// TODO: clean up, give variables real names
namespace jj_800F96AC {

extern "C" {
void fn_800B9F90(void* out, void* arg, int flag);
int isNodeAnimFinished__8MuObjectFv(void* obj);
int fn_800F9A14(void* self);
extern char lbl_8059E480[3];
extern char lbl_8059E484[3];
extern const f32 lbl_805A2680;
}

struct Name {
    char s[65];
};

struct Self {
    char pad0[0x48];
    char pad48[0xA8];
    int state;
    char padF4[4];
    int sub;
    char padFC[4];
    union {
        u8 flags;
        struct {
            u8 bit7 : 1;
            u8 bit6 : 1;
            u8 rest : 6;
        } bits;
    };
    char pad101[0x264 - 0x101];
    nw4r::g3d::ScnMdl* scn;
    MuObject* obj;
};

extern "C" void controllProc__16IfRewardListTaskFv(Self* self) {
    char padBuf[0x40];
    fn_800B9F90(padBuf, (char*)self + 0x48, 0);
    switch (self->state) {
    case 0x0:
        self->state = 0x12E;
        /* fallthrough */
    case 0x12E:
        self->state = 0x12F;
        break;
    case 0x12F:
        if ((self->flags >> 7) & 1) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Info, self->scn);
            MuObject* obj = self->obj;
            char* mdl = *(char**)((char*)obj + 8);
            int off = *(int*)(mdl + 0x3c);
            const char* src = off ? mdl + off : 0;
            Name n4;
            Name n3;
            Name n2;
            Name n1;
            n1.s[0] = 0;
            strcat(n1.s, src);
            n2 = n1;
            strcat(n2.s, lbl_8059E484);
            n3 = n2;
            sprintf(n3.s + strlen(n3.s), lbl_8059E480, 0);
            n4 = n3;
            obj->changeNodeAnimNIf(n4.s);
            switch (self->sub) {
            case 0:
                break;
            case 1:
                if ((self->flags >> 6) & 1) {
                    g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
                }
                /* fallthrough */
            case 2:
                self->flags &= ~0x40;
                self->obj->setFrameMatCol(lbl_805A2680);
                break;
            }
            self->state = 0x137;
        }
        break;
    case 0x137:
        if (isNodeAnimFinished__8MuObjectFv(self->obj)) {
            self->state = 0x13D;
        }
        break;
    case 0x13D:
        if (fn_800F9A14(self)) {
            MuObject* obj = self->obj;
            char* mdl = *(char**)((char*)obj + 8);
            int off = *(int*)(mdl + 0x3c);
            const char* src = off ? mdl + off : 0;
            Name n4;
            Name n3;
            Name n2;
            Name n1;
            n1.s[0] = 0;
            strcat(n1.s, src);
            n2 = n1;
            strcat(n2.s, lbl_8059E484);
            n3 = n2;
            sprintf(n3.s + strlen(n3.s), lbl_8059E480, 1);
            n4 = n3;
            obj->changeNodeAnimNIf(n4.s);
            self->state = 0x144;
        }
        break;
    case 0x144:
        if (isNodeAnimFinished__8MuObjectFv(self->obj)) {
            self->bits.bit7 = 0;
            g_gfSceneRoot->remove(self->scn);
            self->state = 0x12F;
        }
        break;
    }
}
}

