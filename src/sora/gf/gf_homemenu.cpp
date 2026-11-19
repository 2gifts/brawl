#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <mv/mv_movie_player.h>
#include <gf/gf_application.h>
#include <revolution/GX/GXTypes.h>
#include <gf/gf_shutdown_manager.h>
#include <gf/gf_camera.h>
#include <revolution/OS/OSTime.h>
#include <revolution/MTX/mtx.h>
#include <revolution/GX/GXHardware.h>
#include <gf/gf_homemenu.h>
#include <gf/gf_trigger_callback.h>
#include <mt/mt_vector.h>
#include <nw4r/snd/snd_RemoteSpeaker.h>
#include <nw4r/snd/snd_RemoteSpeakerManager.h>
#include <revolution/MEM/mem_allocator.h>
#include <types.h>

extern "C" void HBMStartBlackOut();

gfTriggerCallback::~gfTriggerCallback() { }

void gfHomeMenu::userProc() {
    unkB1_20 = 1;
    if (unkB1_10 && unkB0.unkB0Hi == 2) {
        HBMStartBlackOut();
    }
}

bool gfHomeMenu::returnStatus() {
    bool result = false;
    if (unkB0.unkB0Mode != 1 || !unkB1_10 || unkB0.unkB0Hi != 2) {
        result = true;
    }
    return result;
}

void SpeakerSetupCallback(long, long);
void SpeakerShutdownCallback(long, long);

void ConnectCallback(long channel, long status) {
    if (status == 0) {
        nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(channel).Setup(SpeakerSetupCallback);
    } else if (status == -1) {
        nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(channel).Shutdown(SpeakerShutdownCallback);
    }
}

class Allocator : public MEMAllocator {
public:
    // HYPOTHESIS: allocator cursor and end follow two unclassified words.
    u32 unk10[2];
    u8* unk18;
    u8* unk1C;
    static void* funcAlloc(MEMAllocator*, unsigned long);
};

void* Allocator::funcAlloc(MEMAllocator* allocator, unsigned long size) {
    Allocator* self = static_cast<Allocator*>(allocator);
    u8* result = self->unk18;
    self->unk18 += (size + 7) & ~7UL;
    if (self->unk18 > self->unk1C) {
        return 0;
    }
    return result;
}

// HYPOTHESIS: cursor coordinates use the existing two-component vector type.
bool calcDigitalCursorPos(u32 buttons, Vec2f* pos) {
    switch (buttons & 0xC003) {
    case 1: pos->m_y -= 0.03515625f; break;
    case 2: pos->m_x -= 0.03515625f; break;
    case 0x4000: pos->m_y += 0.03515625f; break;
    case 0x8000: pos->m_x += 0.03515625f; break;
    case 3:
        pos->m_y -= 0.024858983233571053f;
        pos->m_x -= 0.024858983233571053f;
        break;
    case 0x8001:
        pos->m_y -= 0.024858983233571053f;
        pos->m_x += 0.024858983233571053f;
        break;
    case 0x4002:
        pos->m_y += 0.024858983233571053f;
        pos->m_x -= 0.024858983233571053f;
        break;
    case 0xC000:
        pos->m_y += 0.024858983233571053f;
        pos->m_x += 0.024858983233571053f;
        break;
    default: return false;
    }
    pos->m_x = pos->m_x > 1.0f ? 1.0f : pos->m_x < -1.0f ? -1.0f : pos->m_x;
    pos->m_y = pos->m_y > 1.0f ? 1.0f : pos->m_y < -1.0f ? -1.0f : pos->m_y;
    return true;
}
extern "C" void fn_80228154(s32 p1);
extern "C" void fn_80228138(s32 p1);

struct gfHomeMenuFields {
    char unk0[0x9C];
    s32 unk9C;
    char unkA0[0x10];
    union {
        struct {
            u32 unkB0Hi : 4;
            u32 unkB0Mode : 4;
            u32 unkB0Lo : 24;
        } unkB0;
        u8 unkB0Bytes[4];
    };
};

void gfHomeMenu::start() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);
    s32 var_r4 = 0;

    if ((((u8) self->unkB0Bytes[1] >> 4U) & 1) || (self->unk9C != 0)) {
        var_r4 = 1;
    }
    if (var_r4 != 0) {
        fn_80228154(0);
        fn_80228154(1);
        fn_80228154(2);
        fn_80228154(3);
    }
    self->unkB0.unkB0Mode = 1;
}

void gfHomeMenu::kill() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);

    fn_80228138(0);
    fn_80228138(1);
    fn_80228138(2);
    fn_80228138(3);
    self->unkB0.unkB0Mode = 2;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804953D0[0x48] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80495418[0xc0] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804954D8[0x8] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804954E0[0xf4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804955D4[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_804955D5[0x1] = { 0 };
unsigned char lbl_804955D8[0x2100] __attribute__((aligned(8))) = { 0 };

// TODO: clean up, give variables real names
extern "C" void fn_8003626C(void) {}

// TODO: clean up, give variables real names
namespace jj_8003682C {
extern "C" void __dt__14gfFileIOHandleFv(void* p, short flag);
extern "C" void __dl__FPv(void* p);

extern "C" void* fn_8003682C(void* a, int b) {
    if (a) {
        __dt__14gfFileIOHandleFv((char*)a + 0xAC, -1);
        if (b > 0)
            __dl__FPv(a);
    }
    return a;
}
}

namespace jj_800367C8 {

extern "C" {
extern char lbl_804953D0[];
void fn_800368C8(void*);
void* fn_8003682C(void*, int);
void* __register_global_object(void*, void* (*)(void*, int), void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
gfHomeMenu* fn_800367C8() {
    char* base = lbl_804953D0;
    if ((signed char)base[0x130] == 0) {
        fn_800368C8(base + 0x140);
        __register_global_object(base + 0x140, fn_8003682C, base + 0x134);
        base[0x130] = 1;
    }
    return g_gfHomeMenu = (gfHomeMenu*)(base + 0x140);
}
}
}

// TODO: clean up, give variables real names
namespace jj_8003718C {
extern "C" void fn_80227728(int, void*, int);
extern "C" void fn_80037800(void*, unsigned int);
extern "C" unsigned int OSGetTick();

struct Fn8003718CFlagBits {
    unsigned int type : 4;
    unsigned int reserved : 9;
    unsigned int status : 7;
    unsigned int rest : 12;
};

union Fn8003718CFlags {
    unsigned int word;
    Fn8003718CFlagBits bits;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8003718C(void* self) {
    char buffer[0x840];
    unsigned int status;
    int value;

    if (((*(unsigned int*)((char*)self + 0xB0) >> 12) & 0x7F) == 0) {
        for (int i = 0; i < 4; i++) {
            fn_80227728(i, buffer, 0x10);
        }

        status = (*(const unsigned int*)((const char*)self + 0xB0) >> 12) & 0x7F;
        if (status != 0) {
        } else {
            value = ((Fn8003718CFlags*)((char*)self + 0xB0))->word;
            if ((value >> 28) == 0) {
                ((Fn8003718CFlags*)((char*)self + 0xB0))->bits.type = 1;
                fn_80037800(self, ((Fn8003718CFlags*)((char*)self + 0xB0))->word);
            }
        }

        *(unsigned char*)((char*)self + 0xB1) |= 0x10;
    } else {
        *(unsigned char*)((char*)self + 0xB1) |= 8;
        *(unsigned char*)((char*)self + 0xB8) = 0;
        *(unsigned int*)((char*)self + 0xBC) = OSGetTick();
        *(unsigned char*)((char*)self + 0xC0) = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8003798C {

extern "C" void fn_80228154(int arg0);
extern "C" void fn_8026F4EC(void* p, void* s);
extern "C" void fn_8026F698();
extern "C" int fn_8026F854(int arg0);
extern "C" void fn_8026F868(void* a, int b, int c);
extern "C" void* fn_80023AC8();

extern "C" void fn_8003623C();
extern "C" void fn_8003626C();
extern u8 lbl_804954D8[];
extern u8 lbl_804954E0[];

struct Self {
    u8 pad[0x10];
    void* m_10;
    void* m_14;
    void* m_18;
    void* m_1C;
    u8 pad2[0x38 - 0x20];
    int m_38;
    u8 pad3[0x98 - 0x3C];
    int m_98;
    int m_9C;
    u8 pad4[0xAC - 0xA0];
    gfFileIOHandle m_AC;
    union {
        u32 raw;
        struct {
            u32 state : 4;
            u32 rest : 28;
        } f;
    } m_B0;
};

struct B1Bits {
    u8 f0 : 1;
    u8 f1 : 1;
    u8 rest : 6;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_8003798C(Self* self) {
    u8* b1 = (u8*)self + 0xB1;
    int state = (int)self->m_B0.raw >> 28;
    if (state == 2)
        return 1;
    if (state == 0)
        return 0;
    if ((*b1 >> 5) & 1)
        return 0;
    if (!self->m_AC.isReady())
        return 0;
    self->m_B0.f.state = 2;

    int sz;
    void* data4;
    {
        gfArchive arc;
        int size = self->m_AC.getSize();
        arc.setFileImage(self->m_AC.getBuffer(), size, (HeapType)6);

        self->m_10 = arc.getData((ARCNodeType)1, 0, &sz, 0xFFFE);
        self->m_14 = arc.getData((ARCNodeType)1, 1, &sz, 0xFFFE);
        self->m_18 = arc.getData((ARCNodeType)1, 2, &sz, 0xFFFE);
        self->m_1C = arc.getData((ARCNodeType)1, 3, &sz, 0xFFFE);
        self->m_38 = sz;
        data4 = arc.getData((ARCNodeType)1, 4, &sz, 0xFFFE);
    }

    fn_80228154(0);
    fn_80228154(1);
    fn_80228154(2);
    fn_80228154(3);

    if ((s8)lbl_804954D8[0] == 0) {
        *(void**)(lbl_804954E0 + 0x10) = (void*)fn_8003623C;
        *(void**)(lbl_804954E0 + 0x14) = (void*)fn_8003626C;
        *(void**)(lbl_804954E0 + 0x00) = (void*)(lbl_804954E0 + 0x10);
        *(u32*)(lbl_804954E0 + 0x08) = 0;
        *(u32*)(lbl_804954E0 + 0x0C) = 0;
        *(u32*)(lbl_804954E0 + 0x1C) = 0;
        *(u32*)(lbl_804954E0 + 0x04) = 0;
        *(u32*)(lbl_804954E0 + 0x18) = 0;
        lbl_804954D8[0] = 1;
    }
    *(u32*)(lbl_804954E0 + 0x18) = *(u32*)(lbl_804954E0 + 0x04);

    fn_8026F4EC(&self->m_10, lbl_804954E0);
    fn_8026F698();
    fn_8026F854(1);
    self->m_9C = 1;
    fn_8026F868(data4, self->m_98, 0x1D000);

    u8* t = (u8*)fn_80023AC8();
    ((B1Bits*)b1)->f1 = (t[0x74] >> 3) & 1;
    t[0x74] = t[0x74] | 8;
    return 1;
}
}

namespace jj_80036C5C {

extern "C" {
void fn_80036ED0(void* self);
void fn_8003718C(void* self);
void fn_80037244(void* self);
void fn_80076860(void* snd, int arg);
void fn_80076A6C(void* snd, int arg);
void fn_8026F614(void);
}

extern "C" void* g_gfPadSystem;
extern "C" void* g_sndSystem;
extern "C" u8 lbl_804955D4[];
extern "C" u8 lbl_804955D5[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80036C5C(void* arg0) {
    u8* self = (u8*)arg0;
    s32 var_r4;
    s32 var_r3;
    s32 var_r4_3;
    s32 temp_r4_3;
    u32 var_r5_2;
    s32 var_r3_2;
    u8 var_r4_4;
    u32 x = *(u32*)(self + 0xB0);

    if (((s32)((x << 4) & 0xF8000000) >> 28) == 1) {
        if ((x >> 12) & 0x7F) {
            var_r4 = 0;
            if (((self[0xB1] >> 4) & 1) || *(s32*)(self + 0x9C) != 0) {
                var_r4 = 1;
            }
            if (var_r4 == 0) {
                goto dec;
            }
        }
        *(s32*)(self + 0xB4) = 2;
        goto cd4;
dec:
        if (*(s32*)(self + 0xB4) != 0) {
            *(s32*)(self + 0xB4) -= 1;
        }
cd4:

        if (lbl_804955D4[0] != 0) {
            var_r4 = 0;
            if (((self[0xB1] >> 4) & 1) || *(s32*)(self + 0x9C) != 0) {
                var_r4 = 1;
            }
            if (var_r4 == 0) {
                fn_8026F614();
            }
        }

        var_r4 = 0;
        if (((self[0xB1] >> 4) & 1) || *(s32*)(self + 0x9C) != 0) {
            var_r4 = 1;
        }
        lbl_804955D4[0] = (u8)var_r4;
        fn_80036ED0(arg0);

        var_r3 = 0;
        if (((self[0xB1] >> 4) & 1) || *(s32*)(self + 0x9C) != 0) {
            var_r3 = 1;
        }
        if (var_r3 == 0 && !((((u8*)arg0)[0xB1] >> 3) & 1) && ((*(u8*)((u8*)g_gfPadSystem + 0x34) >> 1) & 1)) {
            fn_8003718C(arg0);
        }
        if ((self[0xB1] >> 4) & 1) {
            fn_80037244(arg0);
        }

        var_r4_3 = 0;
        if (((self[0xB1] >> 4) & 1) || *(s32*)(self + 0x9C) != 0) {
            var_r4_3 = 1;
        }
        g_gfApplication->unkE8[5] = (u8)__rlwimi(g_gfApplication->unkE8[5], var_r4_3, 5, 26, 26);

        temp_r4_3 = (self[0xB1] >> 4) & 1;
        var_r5_2 = 1;
        if (temp_r4_3 == 0 && *(s32*)(self + 0x9C) == 0) {
            var_r5_2 = 0;
        }
        if (lbl_804955D5[0] != var_r5_2) {
            var_r3_2 = 0;
            if (temp_r4_3 != 0 || *(s32*)(self + 0x9C) != 0) {
                var_r3_2 = 1;
            }
            if (var_r3_2 != 0) {
                fn_80076860(g_sndSystem, 0);
                if (mvMoviePlayer::getInstance() != 0) {
                    if (!mvMoviePlayer::getInstance()->isPause()) {
                        mvMoviePlayer::getInstance()->setPause();
                        self[0xC1] = 1;
                    }
                }
            } else {
                fn_80076A6C(g_sndSystem, 0);
                if (mvMoviePlayer::getInstance() != 0 && self[0xC1] != 0) {
                    mvMoviePlayer::getInstance()->resetPause();
                    self[0xC1] = 0;
                }
            }
        }

        var_r4_4 = 0;
        if (((self[0xB1] >> 4) & 1) || *(s32*)(self + 0x9C) != 0) {
            var_r4_4 = 1;
        }
        lbl_804955D5[0] = var_r4_4;
    }
}
}

namespace jj_80037244 {

struct FnSub {
    char pad0[0x20];
    f32 unk20;
    f32 unk24;
    char pad1[0x36];
    s8 unk5E;
    char pad2[0x1];
    void* unk60;
    char pad3[0x8];
    f32 unk6C;
    f32 unk70;
};

struct FnSlot {
    FnSub* unk0;
    f32 unk4;
    f32 unk8;
    s32 unkC;
};

struct FnObj {
    char pad0[0x50];
    FnSlot slots[4];
    char pad1[0xC];
    s32 unk9C;
    char pad2[0x10];
};

struct FnB0 {
    u32 top : 4;
    u32 rest : 28;
};

struct FnB1 {
    u8 p0 : 1;
    u8 p1 : 1;
    u8 p2 : 1;
    u8 p3 : 1;
    u8 p4 : 4;
};

struct FnMgrHdr {
    char pad0[0x74];
    u8 hi : 4;
    u8 flag : 1;
    u8 lo : 3;
};

extern "C" {
int fn_8003798C(void);
int fn_8021A024(int, s32*);
void fn_80227728(int, void*, int);
void fn_80228138(int);
int fn_800365B0(void*, void*);
int fn_8026F6C4(void*);
int fn_8026F834(void);
void fn_8026F924(void);
void fn_8026F958(void);
u8* fn_80023AC8(void);
extern char lbl_804955D8[];
extern f32 lbl_805A1678;
extern f32 lbl_805A16A0;
extern f32 lbl_805A16A4;
extern f32 lbl_805A16C0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80037244(FnObj* self) {
    s32 r29b;
    if (fn_8003798C() != 0) {
        f32 f31;
        f32 f30;
        f32 f29;
        f32 f28;
        char* cur;
        FnSlot* slot;
        s32 i;
        f30 = lbl_805A16A0;
        f31 = lbl_805A16A4;
        cur = lbl_804955D8;
        f28 = lbl_805A16C0;
        slot = self->slots;
        f29 = lbl_805A1678;
        for (i = 0; i < 4; i++) {
            s32 sp8;
            s32 r29 = fn_8021A024(i, &sp8);
            fn_80227728(i, cur, 0x10);
            slot->unkC = sp8;
            switch (r29) {
            case 0:
            case -2:
            case -3:
            case -4:
            case -7:
                slot->unk0 = (FnSub*)cur;
                break;
            default:
                slot->unk0 = 0;
                break;
            }
            FnSub* sub = slot->unk0;
            if (sub != 0) {
                s32 r3_2 = fn_800365B0(sub->unk60, &slot->unk4);
                FnSub* s4 = slot->unk0;
                f32 b = s4->unk70 / f28;
                f32 a = s4->unk6C / f28;
                s32 var_r0;
                if (f29 == a && f29 == b) {
                    var_r0 = 0;
                } else {
                    f32 x = slot->unk4 + a;
                    slot->unk4 = x;
                    slot->unk8 = slot->unk8 - b;
                    if (x > f30) {
                        x = f30;
                    } else if (x < f31) {
                        x = f31;
                    }
                    slot->unk4 = x;
                    f32 y = slot->unk8;
                    if (y > f30) {
                        y = f30;
                    } else if (y < f31) {
                        y = f31;
                    }
                    slot->unk8 = y;
                    var_r0 = 1;
                }
                if ((r3_2 | var_r0) == 0) {
                    FnSub* s5 = slot->unk0;
                    if (s5->unk5E > 0) {
                        slot->unk4 = s5->unk20;
                        slot->unk8 = slot->unk0->unk24;
                    }
                }
            }
            cur += 0x840;
            slot++;
        }
        if (fn_8026F6C4(self->slots) >= 0) {
            s32 three = 3;
            ((FnB1*)((char*)self + 0xB1))->p3 = 0;
            ((FnB0*)((char*)self + 0xB0))->top = three;
            fn_8026F924();
            self->unk9C = 4;
            r29b = ((FnB1*)((char*)self + 0xB1))->p1;
            FnMgrHdr* mh = (FnMgrHdr*)fn_80023AC8();
            mh->flag = r29b;
            fn_80228138(0);
            fn_80228138(1);
            fn_80228138(2);
            fn_80228138(3);
            gfShutdownManager* mgr = &g_gfShutdownManager;
            int st = fn_8026F834();
            switch (st) {
            case 0:
                break;
            case 1:
                mgr->enterReturnToWiiMenuProcess();
                break;
            case 2:
                mgr->enterRestartProcess();
                break;
            case 3:
                break;
            }
        }
        fn_8026F958();
    }
}
}

namespace jj_800374DC {
struct Obj {
 char pad0[0x44]; float x; float y; char pad4c[0x48]; void* ptr; char pad98[0x18];
 union {
  struct { signed int mode : 4; signed int type : 4; unsigned int rest : 24; };
  struct { unsigned char padb0; unsigned char wide : 1; unsigned char padbits : 2; unsigned char active : 1; unsigned char fade : 1; unsigned char remaining : 3; unsigned char padb2[2]; };
 };
 char padb4[4]; signed char state; char padb9[3]; unsigned int tick; unsigned char alpha;
};
struct Render { char pad[0x24]; int a; int b; char pad2[8]; unsigned short w; unsigned short h; };
extern "C" {
void fn_801F47D8(int, int);
void fn_801F5500(int, int, int, int);
void fn_801F5484(float, float, float, float, float, float);
void fn_801F4880(int);
void fn_801ECF4C(void*, float, float, float, float, float, float);
void fn_801F50EC(void*, int);
void fn_8026F708();
void fn_80036270(void*, unsigned char, int);
void fn_801F1D10(int);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800374DC(Obj* self, Render* r) {
 float matrix[16];
 if (self->type == 1) {
  unsigned int wide = *(float*)((char*)gfCameraManager::getManager() + 0x738) > 1.3333334f;
  if (wide != self->wide) {
   self->wide = wide;
   if (!self->wide) { self->x = 1.0f; self->y = 1.0f; }
   else { self->x = 1.3684211f; self->y = 1.0f; }
  }
  if ((self->active && self->mode == 2) || self->fade) {
   fn_801F47D8(r->a, r->b);
   fn_801F5500(0, 0, r->w, r->h);
   fn_801F5484(0.0f, 0.0f, (float)r->w, (float)r->h, 0.0f, 1.0f);
   fn_801F4880(0);
   if (self->active && self->mode == 2) {
    if (!self->wide) fn_801ECF4C(matrix, 228.0f, -228.0f, -304.0f, 304.0f, 0.0f, 500.0f);
    else fn_801ECF4C(matrix, 228.0f, -228.0f, -416.0f, 416.0f, 0.0f, 500.0f);
    fn_801F50EC(matrix, 1); fn_8026F708();
   } else if (self->fade) {
    float elapsed = (float)((OSGetTick() - self->tick) / ((*(unsigned int*)0x800000F8 >> 2) / 1000));
    switch (self->state) {
    case 0:
     self->alpha = (unsigned char)(255.9f * (elapsed / 250.0f));
     if (elapsed >= 250.0f) { self->tick = OSGetTick(); self->state = 1; self->alpha = 255; }
     break;
    case 1:
     if (elapsed >= 1000.0f) { self->tick = OSGetTick(); self->state = 2; }
     break;
    case 2:
     self->alpha = (unsigned char)(255.9f * ((250.0f - elapsed) / 250.0f));
     if (elapsed >= 250.0f) { self->alpha = 0; self->fade = 0; }
     break;
    }
    fn_80036270(self->ptr, self->alpha, self->wide);
   }
   fn_801F1D10(0);
  }
 }
}
}

namespace jj_80036270 {

typedef struct {
    u8 r, g, b, a;
} Color4;

extern "C" {

void GXSetNumChans(int);
void GXSetChanCtrl(int, int, int, int, int, int, int);
void GXSetNumTexGens(int);
void GXSetTexCoordGen2(int, int, int, int, int, int);
void GXSetNumTevStages(int);
void GXSetTevOrder(int, int, int, int);
void GXSetBlendMode(int, int, int, int);
void GXSetZMode(int, int, int);
void GXBegin(int, int, int);

void fn_801EFB10(void);
void fn_801EFB44(int, int, int, int, int);
void fn_801EF280(int, int);
void fn_801F3D60(int, const void*);
void fn_801F3C30(int, int, int, int, int);
void fn_801F3CB0(int, int, int, int, int, int);
void fn_801F3C70(int, int, int, int, int);
void fn_801F3D08(int, int, int, int, int, int);
void fn_801F51DC(const void*, int);
void fn_801F52E4(int);
void fn_8025C058(s32, void*, int);
void fn_801F2FB0(void*, int);
void* fn_80023AC8(void);
void fn_801ECF4C(void*, f32, f32, f32, f32, f32, f32);
void fn_801F50EC(void*, int);
void setIdentity__6MatrixFv(void*);

extern void* lbl_805A0098;
extern u8 lbl_805A1670;
extern u8 lbl_805A1671;
extern u8 lbl_805A1672;
extern f32 lbl_805A1674;
extern f32 lbl_805A1678;
extern f32 lbl_805A167C;
extern f32 lbl_805A1680;
extern f32 lbl_805A1684;
extern f32 lbl_805A1688;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_80036270(s32 arg0, s8 arg1, s32 arg2) {
    Mtx sp70;
    Mtx44 sp30;
    f32 sp10[8];
    s32 spC;
    Color4 c;

    fn_801EFB10();
    fn_801EFB44(5, 9, 0, 4, 0);
    fn_801EFB44(5, 0xD, 1, 3, 0);
    fn_801EF280(9, 1);
    fn_801EF280(0xD, 1);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    c.r = lbl_805A1670;
    c.g = lbl_805A1671;
    c.b = lbl_805A1672;
    c.a = arg1;
    spC = *(s32*)&c;
    fn_801F3D60(1, &spC);
    GXSetTevOrder(0, 0, 0, 0xFF);
    fn_801F3C30(0, 0xF, 0xF, 0xF, 8);
    fn_801F3CB0(0, 0, 0, 0, 1, 0);
    fn_801F3C70(0, 7, 1, 4, 7);
    fn_801F3D08(0, 0, 0, 0, 1, 0);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZMode(0, 3, 0);
    setIdentity__6MatrixFv(sp70);
    fn_801F51DC(sp70, 3);
    fn_801F52E4(3);
    fn_8025C058(arg0, sp10, 0);
    fn_801F2FB0(sp10, 0);

    u8* p = (u8*)fn_80023AC8();
    f32 f31 = (f32)(s32)*(u16*)(p + 0x34);
    f32 f30 = (f32)(s32)*(u16*)(p + 0x36);
    fn_801ECF4C(sp30, f30 * lbl_805A1674, -f30 * lbl_805A1674, -f31 * lbl_805A1674, f31 * lbl_805A1674, lbl_805A1678, lbl_805A167C);
    fn_801F50EC(sp30, 1);

    f32 b = lbl_805A1680;
    f32 var_f4 = lbl_805A1684;
    f32 k = lbl_805A1684;
    var_f4 *= *(f32*)((u8*)lbl_805A0098 + 0x44);
    if (arg2 != 0) {
        var_f4 *= lbl_805A1688;
    }
    f32 temp_f1 = -f31 * lbl_805A1674;
    f32 temp_f30_2 = temp_f1 + b;
    f32 temp_f31_2 = temp_f30_2 + var_f4;
    f32 temp_f29 = (f30 * lbl_805A1674) - b;
    f32 temp_f28 = temp_f29 - k;

    GXBegin(0x80, 5, 4);
    WGPIPE.f = temp_f30_2;
    WGPIPE.f = temp_f28;
    WGPIPE.s = 0;
    WGPIPE.s = 1;
    WGPIPE.f = temp_f30_2;
    WGPIPE.f = temp_f29;
    WGPIPE.s = 0;
    WGPIPE.s = 0;
    WGPIPE.f = temp_f31_2;
    WGPIPE.f = temp_f29;
    WGPIPE.s = 1;
    WGPIPE.s = 0;
    WGPIPE.f = temp_f31_2;
    WGPIPE.f = temp_f28;
    WGPIPE.s = 1;
    WGPIPE.s = 1;
}

}
}

// TODO: clean up, give variables real names
namespace jj_80037DC8 {
extern "C" void fn_80036C20(void*);

extern "C" void fn_80037DC8(void* self) {
    fn_80036C20(reinterpret_cast<u8*>(self) - 0x8);
}
}

