#include <gf/gf_heap_manager.h>
#include <revolution/GX/GXInit.h>
#include <gf/gf_application.h>
#include <types.h>

extern "C" {
void fn_801DF434(s32 arg);
void fn_801F47D8(s32 a, s32 b);
void fn_801F4880(s32 a);
void fn_80016F8C(gfApplication* self);
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
void GXSetViewport(f32 x, f32 y, f32 wd, f32 ht, f32 nearZ, f32 farZ);
void GXSetClipMode(u32 mode);
void GXSetCullMode(u32 mode);
void VISetBlack(BOOL black);
void VIFlush();
void VIWaitForRetrace();
void fn_801FF1AC();
}

extern f32 lbl_805A13C8;
extern f32 lbl_805A13DC;

struct gfApplicationFields {
    u8 unk0[0x7C];
    s32 unk7C;
    s32 unk80;
    u8 unk84[0x8];
    u16 unk8C;
    u16 unk8E;
};

gfApplication::~gfApplication() { }

void gfApplication::reset() {
    VISetBlack(TRUE);
    VIFlush();
    fn_801FF1AC();
    VIWaitForRetrace();
    VIWaitForRetrace();
}

void gfApplication::restart() {
    VISetBlack(TRUE);
    VIFlush();
    fn_801FF1AC();
    VIWaitForRetrace();
    VIWaitForRetrace();
    fn_801DF434(2);
}

void gfApplication::mainLoop() {
    gfApplicationFields* self = (gfApplicationFields*)this;

    fn_801F47D8(self->unk7C, self->unk80);
    GXSetScissor(0, 0, self->unk8C, self->unk8E);
    GXSetViewport(lbl_805A13C8, lbl_805A13C8, (f32)self->unk8C, (f32)self->unk8E, lbl_805A13C8, lbl_805A13DC);
    fn_801F4880(0);
    GXSetClipMode(0);
    GXSetCullMode(0);
    fn_80016F8C(this);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80494890[0x10] __attribute__((aligned(8))) = { 0 };

void gfApplication::setPause(int value, int index) {
    m_pauseFlags = (m_pauseFlags & ~(1 << index)) | (value << index);
}

void gfApplication::clearPause() {
    m_pauseFlags = 0;
}

void gfApplication::setSkipLowEffect(u8 level) {
    if (m_lowEffectEnabled) {
        if (m_lowEffectActive) {
            if (level < 3) {
                m_skipCounter += 1;
                if (m_skipCounter > 30u) {
                    m_lowEffectActive = 0;
                }
            } else {
                m_skipCounter = 0;
            }
        } else if (level >= 3) {
            m_skipCounter += 1;
            if (m_skipCounter > 10u) {
                m_skipCounter = 0;
                m_lowEffectActive = 1;
            }
        } else if (m_skipCounter != 0) {
            m_skipCounter -= 1;
        }
    } else {
        m_skipLowEffect = 0;
        m_skipCounter = 0;
        m_lowEffectActive = 0;
    }
    m_skipLowEffect = m_lowEffectActive;
}

// TODO: clean up, give variables real names
namespace jj_8001789C {
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned long long u64;

extern "C" {
u64 OSGetTime(void);
void fn_80023AE4(void*);
void fn_800374DC(int, void*);
void fn_801B5AC8(void);
void fn_801F0248(void);
void fn_801F0A30(void);
void fn_801F0AF0(void);
void fn_801F1564(int, int, int, int);
void fn_801F15C8(int, int, int, int);
void fn_801F1E60(int, int);
void fn_801F3194(void);
void process__14gfCallBackListFv(void*);
void renderPre__15gfTaskSchedulerFv(void*);
void render__15gfTaskSchedulerFv(void*);
void render__17gfKeepFrameBufferFv(void*);
void update__17gfKeepFrameBufferFv(void*);
extern int g_gfHomeMenu;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8001789C(u8* self) {
    update__17gfKeepFrameBufferFv(self + 0xD0);
    u8* p = *(u8**)(self + 0xE0);
    if (p != 0) {
        p[0x350] = p[0x350] | 0x40;
        u64 t = OSGetTime();
        u8* q = *(u8**)(self + 0xE0);
        *(u32*)(q + 0x34C) = (u32)t;
        *(u32*)(q + 0x348) = (u32)(t >> 32);
        fn_801F0A30();
        p = *(u8**)(self + 0xE0);
        p[0x350] = p[0x350] & ~0x40;
    } else {
        fn_801F0A30();
    }
    renderPre__15gfTaskSchedulerFv(*(void**)(self + 0xD8));
    fn_801B5AC8();
    fn_801F0248();
    fn_801F3194();
    fn_80023AE4(self + 0x58);
    process__14gfCallBackListFv(self + 0x114);
    render__15gfTaskSchedulerFv(*(void**)(self + 0xD8));
    process__14gfCallBackListFv(self + 0x118);
    if ((self[0xED] >> 1) & 1) {
        if (*(int*)(self + 0x10C) != 0) {
            fn_801F0A30();
            int handle = *(int*)(self + 0x10C);
            fn_801F1564(0, 0, 0x280, 0x1E0);
            fn_801F15C8(0x280, 0x1E0, 6, 0);
            fn_801F1E60(handle, 1);
            fn_801F0AF0();
            fn_801F3194();
            *(int*)(self + 0x10C) = 0;
        }
        self[0xED] = self[0xED] & ~2;
    }
    render__17gfKeepFrameBufferFv(self + 0xD0);
    fn_800374DC(g_gfHomeMenu, self + 0x58);
}
}

namespace jj_80016938 {

struct Light {
    unsigned int data[16];
};

struct Color {
    unsigned char r, g, b, a;
};

struct Vec3f {
    float x, y, z;
};

struct Screen {
    unsigned char pad0[0x7C];
    unsigned int a;
    unsigned int b;
    unsigned char pad1[8];
    unsigned short c;
    unsigned short d;
};

extern "C" void fn_801F2400(Light*, Color);
extern "C" void fn_801F2284(Light*, float, float, float);
extern "C" void fn_8003DE70(Vec3f*);
extern "C" void fn_801F22B0(Light*, float, float, float);
extern "C" void fn_801F202C(Light*, int, float);
extern "C" void fn_801F21B4(Light*, int, float, float);
extern "C" void fn_801F2418(Light*, unsigned int);
extern "C" void fn_801F47D8(int, int);
extern "C" void fn_801F5500(int, int, int, int);
extern "C" void fn_801F5484(float, float, float, float, float, float);
extern "C" void fn_801F4880(int);
extern "C" void fn_80023E7C(Screen*);
extern "C" void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
extern "C" void* memset(void*, int, unsigned long);

extern const float lbl_805A13C8;
extern const float lbl_805A13CC;
extern const float lbl_805A13D0;
extern const float lbl_805A13D4;
extern const float lbl_805A13D8;
extern const float lbl_805A13DC;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80016938(Screen* screen) {
    Light light;
    Vec3f dir;
    Color color;

    void* buf = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(4, 0x40000);
    if (buf) {
        memset(buf, 0, 0x40000);
    }
    GXInit(buf, 0x00040000);

    color.r = color.g = color.b = color.a = 0x80;
    fn_801F2284(&light, lbl_805A13C8, lbl_805A13CC, lbl_805A13C8);
    fn_801F2400(&light, color);

    dir.x = lbl_805A13D0;
    dir.y = lbl_805A13D0;
    dir.z = lbl_805A13D0;
    fn_8003DE70(&dir);
    fn_801F22B0(&light, dir.x, dir.y, dir.z);

    fn_801F202C(&light, 0, lbl_805A13D4);
    fn_801F21B4(&light, 0, lbl_805A13CC, lbl_805A13D8);
    fn_801F2418(&light, 1);
    fn_801F2418(&light, 2);
    fn_801F2418(&light, 4);
    fn_801F2418(&light, 0x80);

    fn_801F47D8(screen->a, screen->b);
    fn_801F5500(0, 0, screen->c, screen->d);
    fn_801F5484(lbl_805A13C8, lbl_805A13C8, (float)screen->c, (float)screen->d, lbl_805A13C8, lbl_805A13DC);
    fn_801F4880(0);
    fn_80023E7C((Screen*)((char*)screen + 0x58));
}
}

