#include <math.h>
#include <revolution/GX/GXTypes.h>
#include <mt/mt_matrix.h>
#include <gf/gf_camera.h>
#include <revolution/GX/GXHardware.h>
#include <types.h>
#include <ef/ef_screen.h>
#include <ef/ef_screen_handle.h>
#include <sr/sr_common.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
efScreen* g_efScreen = 0;

void efScreen::create() {
    if (!g_efScreen) {
        g_efScreen = new (Heaps::System) efScreen();
    }
}

void efScreen::destroy() {
    if (g_efScreen) {
        delete g_efScreen;
    }
}

void efScreen::cancelFill(efScreenHandle* handle, float frames) {
    efScreenFillSlot& slot = m_fills[handle->unk0];
    slot.m_active = 1;
    slot.m_progress = 0.0f;
    slot.m_frames = frames;
    slot.m_current[0] = slot.m_to[0];
    slot.m_current[1] = slot.m_to[1];
    slot.m_current[2] = slot.m_to[2];
    slot.m_current[3] = slot.m_to[3];
    slot.m_from[0] = slot.m_to[0];
    slot.m_from[1] = slot.m_to[1];
    slot.m_from[2] = slot.m_to[2];
    slot.m_from[3] = 0;
}

// TODO: clean up, give variables real names
namespace jj_8005DC24 {
struct Entry { unsigned char pad0, active, pad2[2], priority, pad5[27]; };
struct Group { signed char pad, ids[16]; };
struct Object { Group groups[9]; char pad[3]; Entry entries[1]; };
extern "C" void fn_8005DCF8(Object*, Entry*);
// TODO: clean up, give variables real names
extern "C" void fn_8005DC24(Object* obj, int group) {
    Group* g = &obj->groups[group];
    int max = -1;
    int best = -1;
    for (int i = 0; i < 16; i++) {
        Entry* entry;
        signed char* p = &g->ids[i];
        int id = *p;
        if (id != -1) {
            entry = &obj->entries[id];
            if (entry->active) {
                int priority = entry->priority;
                if (priority >= max) {
                    max = priority;
                    best = id;
                }
            } else {
                *p = -1;
            }
        }
    }
    if (best >= 0) fn_8005DCF8(obj, &obj->entries[best]);
}
}

namespace jj_8005DCF8 {

struct Input {
    u8 pad[5];
    u8 mode;
    u8 pad2[10];
    float time;
    float period;
    GXColor color;
    GXColor other;
};

extern "C" void fn_8005E438(int, GXColor*);

// TODO: clean up, give variables real names
extern "C" void fn_8005DCF8(int arg0, Input* arg1) {
    switch (arg1->mode) {
    case 0: {
        float alpha;
        if (arg1->color.a == 0)
            alpha = 1.0f;
        else
            alpha = (float)arg1->color.a / 255.0f;
        float t = arg1->time;
        float s = (float)sin((6.2831854820251465f / arg1->period) * t - 1.5707963705062866f);
        GXColor color;
        color.r = arg1->color.r;
        color.g = arg1->color.g;
        color.b = arg1->color.b;
        color.a = (u8)(int)(alpha * (128.0f + 127.0f * s));
        GXColor copy = color;
        fn_8005E438(arg0, &copy);
        break;
    }
    case 2: {
        GXColor color = arg1->other;
        fn_8005E438(arg0, &color);
        break;
    }
    }
}
}

namespace jj_8005DE38 {
inline void shift(s32 i, u8* base) {
    for (; i < 15; ++i) base[i + 1] = base[i + 2];
}
inline void remove(u8* base, s32 value) {
    s32 i = 0;
    u32 count = *base;
    for (; i < count; ++i) {
        if ((s8)base[i + 1] == value) {
            shift(i, base);
            ((s8*)base)[16] = -1;
            --base[0];
            return;
        }
    }
}
// TODO: clean up, give variables real names
extern "C" void fn_8005DE38(u8* base, s32 index, s32 value) {
    base += index * 17;
    remove(base, value);
}
}

namespace jj_8005E438 {


extern "C" {

void fn_801F52E4(int);
void fn_801F51DC(Matrix*, unsigned int);
void fn_801EFB10(void);
void fn_801EF280(int, int);
void fn_801EFB44(int, int, int, int, int);
void GXSetZMode(int, int, int);
void GXSetBlendMode(int, int, int, int);
void GXBegin(int, int, int);
void gfDrawSetVtxPosColorPrimEnvironment__Fv(void);

extern const float lbl_805A1948;
extern const float lbl_805A194C;
extern const float lbl_805A1950;

}

// TODO: clean up, give variables real names
extern "C" void fn_8005E438(int unused, u32* arg1) {
    Matrix sp8(true);
    sp8.setIdentity();

    float hx = lbl_805A1948;
    float hy = lbl_805A194C;
    float hz = hx;

    gfDrawSetVtxPosColorPrimEnvironment__Fv();
    gfCameraManager* mgr = gfCameraManager::getManager();
    mgr->getCamera(0)->setGX();
    float dist = mgr->getCamera(0)->m_near;
    if (dist >= hx) {
        hz = lbl_805A1950 + dist;
        float k = hz / hx;
        hx = hx * k;
        hy = hy * k;
    }

    fn_801F52E4(0);
    fn_801F51DC(&sp8, 0);
    fn_801EFB10();
    fn_801EF280(9, 1);
    fn_801EF280(0xB, 1);
    fn_801EFB44(1, 9, 1, 4, 0);
    fn_801EFB44(1, 0xB, 1, 5, 0);
    GXSetZMode(0, 7, 0);
    u32 color = *arg1;
    GXSetBlendMode(1, 4, 5, 0);
    GXBegin(0x98, 1, 4);

    WGPIPE.f = -hx;
    WGPIPE.f = hy;
    WGPIPE.f = -hz;
    WGPIPE.ui = color;
    WGPIPE.f = hx;
    WGPIPE.f = hy;
    WGPIPE.f = -hz;
    WGPIPE.ui = color;
    WGPIPE.f = -hx;
    WGPIPE.f = -hy;
    WGPIPE.f = -hz;
    WGPIPE.ui = color;
    WGPIPE.f = hx;
    WGPIPE.f = -hy;
    WGPIPE.f = -hz;
    WGPIPE.ui = color;
}
}

// TODO: clean up, give variables real names
namespace jj_8005D7D8 {
struct Color {
    unsigned char r, g, b, a;
    Color() {}
    Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}
};
struct gfCameraController_base { unsigned char id; char pad[3]; };
class gfCameraController : public gfCameraController_base {
public:
    virtual void slot0(int, Color);
};
struct Group { unsigned char count; signed char slots[16]; };
struct Data {
    unsigned char flag;
    char pad[3];
    float duration;
    float time;
    char pad2[8];
    Color color;
};
struct Entry {
    unsigned char unk0; unsigned char active : 8; unsigned char unk2, group, unk4, type;
    char pad[2];
    Data data;
};
struct Self {
    Group groups[9];
    char pad[3];
    Entry entries[10];
    gfCameraController* controller;
    unsigned char counts[256];
};
extern "C" void fn_8005DA68(Self*, Data*);
extern "C" void fn_8005DE38(Self*, unsigned char, int);
extern float lbl_805A1918;

// TODO: clean up, give variables real names
extern "C" void fn_8005D7D8(Self* self, float delta) {
    if (!(delta <= lbl_805A1918)) {
        for (int i = 0; i < 10; i++) {
            Entry* entry = &self->entries[i];
            if (entry->active) {
                int keep = 1;
                switch (entry->type) {
                case 0:
                    entry->data.time += delta;
                    if (entry->data.time > entry->data.duration)
                        keep = 0;
                case 1:
                    entry->data.time += delta;
                    if (entry->data.time > entry->data.duration)
                        keep = 0;
                    break;
                case 2:
                    Data* data = &entry->data;
                    if (data->time > data->duration) {
                        if (data->flag)
                            keep = 0;
                        else
                            data->time = data->duration;
                    }
                    fn_8005DA68(self, data);
                    if (!keep) {
                        if (--self->counts[entry->group] == 0) {
                            if (self->controller && entry->group == self->controller->id) {
                                self->controller->slot0(2, data->color);
                            }
                        }
                    }
                    data->time += delta;
                    break;
                }
                if (!keep) {
                    entry->active = 0;
                    fn_8005DE38(self, entry->group, i);
                }
            }
        }
        if (self->controller) {
            unsigned char group = self->controller->id;
            if (self->counts[group]) {
                Group* g = &self->groups[group];
                Color value;
                value.r = 0; value.g = 0; value.b = 0; value.a = 0;
                for (int i = 0; i < g->count; i++) {
                    int index = g->slots[i];
                    if (index >= 0 && self->entries[index].active && self->entries[index].type == 2) {
                        *(unsigned int*)&value = *(unsigned int*)&self->entries[index].data.color;
                    }
                }
                self->controller->slot0(1, value);
            }
        }
    }
}
}

