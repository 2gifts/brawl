#include <types.h>

// TODO: clean up, give variables real names
namespace jj_8001AA88 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern "C" void GetTexObjCIParam__Q34nw4r3g3d6ResTexCFPPvPUsPUsP11_GXCITexFmtPfPfPUc(const void *, u32 *, u16 *, u16 *, u32 *, float *, float *, u8 *);
extern "C" void GetTexObjParam__Q34nw4r3g3d6ResTexCFPPvPUsPUsP9_GXTexFmtPfPfPUc(const void *, u32 *, u16 *, u16 *, u32 *, float *, float *, u8 *);
extern "C" void GXInitTexObjCI(void *, u32, u16, u16, u32, u32, u32, u8, u32);
extern "C" void GXInitTexObj(void *, u32, u16, u16, u32, u32, u32, u8);
extern "C" void GXInitTexObjLOD(void *, s32, s32, s32, s32, s32, float, float, float);
extern "C" float lbl_805A1460;

struct Resource {
    u8 pad00[0x10];
    s32 relative;
    u8 pad14[4];
    u32 value;
    u16 size;
};

struct ParseLocals {
    u8 special;
    u8 pad11;
    u16 height;
    u16 width;
    u16 pad16;
    float y;
    float x;
    u32 formatB;
    u32 formatA;
    u32 file;
};

// TODO: clean up, give variables real names
extern "C" void fn_8001AA88(void *arg0, const Resource **source) {
    const Resource *resource = *source;
    ParseLocals locals;

    if ((resource->value & 1) != 0) {
        GetTexObjCIParam__Q34nw4r3g3d6ResTexCFPPvPUsPUsP11_GXCITexFmtPfPfPUc(
            source,
            &locals.file,
            &locals.width,
            &locals.height,
            &locals.formatA,
            &locals.x,
            &locals.y,
            &locals.special
        );
        s32 unused = 0;
        GXInitTexObjCI(
            arg0,
            locals.file,
            locals.width,
            locals.height,
            locals.formatA,
            1,
            1,
            locals.special,
            unused
        );
    } else {
        GetTexObjParam__Q34nw4r3g3d6ResTexCFPPvPUsPUsP9_GXTexFmtPfPfPUc(
            source,
            &locals.file,
            &locals.width,
            &locals.height,
            &locals.formatB,
            &locals.x,
            &locals.y,
            &locals.special
        );
        GXInitTexObj(
            arg0,
            locals.file,
            locals.width,
            locals.height,
            locals.formatB,
            1,
            1,
            locals.special
        );
    }
    if (locals.special) {
        GXInitTexObjLOD(
            arg0, 3, 1, 0, 0, 0,
            locals.x, locals.y, lbl_805A1460
        );
    } else {
        GXInitTexObjLOD(
            arg0, 1, 1, 0, 0, 0,
            locals.x, locals.y, lbl_805A1460
        );
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8001ABE4 {
extern "C" void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
extern "C" void gfMemFill__FPvUcUl(void*, unsigned char, unsigned long);

extern "C" void* lbl_8059FFC8;

struct Header {
    u16 count;
    u16 unk2;
    char* entries;
    u32 unk8;

    void init(u16 n, char* base) {
        count = n;
        unk2 = 0;
        entries = base;
        unk8 = 0;

        s32 off;
        s32 i = 0;
        off = 0;

        while (i < count) {
            char* e = entries + off;
            gfMemFill__FPvUcUl(e, 0, 0x20);
            *(u32*)(e + 0x20) = 0;
            off += 0x30;
            i++;
            *(u32*)(e + 0x24) = 0;
            *(u32*)(e + 0x28) = 0;
            *(u32*)(e + 0x2C) = 0;
        }
    }
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_8001ABE4(u16 count) {
    Header* h = (Header*)alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(
        3, count * 0x30 + 0x1C);
    char* entries = (char*)h + 0xC;

    if (h != 0) {
        h->init(count, entries);
    }

    lbl_8059FFC8 = h;
    return h;
}
}

namespace jj_8001AC98 {
class gfHeapManager {
public:
    void free(void* p);
};

extern "C" {
extern "C" gfHeapManager* lbl_8059FFC8;

// TODO: clean up, give variables real names
void fn_8001AC98(int unused, void* arg1) {
    lbl_8059FFC8->free(arg1);
    lbl_8059FFC8 = 0;
}
}
}

namespace jj_8001ACC4 {
extern "C" {

typedef struct Entry {
    char name[0x20];
    int f20;
    int f24;
    int f28;
    int f2C;
} Entry;

typedef struct Manager {
    unsigned short unk0;
    unsigned short unk2;
    Entry *unk4;
    Entry *unk8;
} Manager;

unsigned char *GetInstance__Q34nw4r2ef8ResourceFv(void);
void *Add__Q34nw4r2ef8ResourceFPUc(void *, void *);
void *AddTexture__Q34nw4r2ef8ResourceFPUc(void *, void *);
void fn_8016EDA8(void *, void *, void *, void *, void *);
int strncmp(const char *, const char *, unsigned long);
char *strncpy(char *, const char *, unsigned long);

// TODO: clean up, give variables real names
void fn_8001ACC4(Manager *mgr, const char *name, int a2, int a3, int a4) {
    int count;
    Entry *p1;
    Entry *p2;
    int i;
    p1 = mgr->unk4;
    p2 = p1;
    if (a2 != 0) {
        count = mgr->unk0;
        i = 0;
        while (i < count) {
            if (p2->f20 != 0 && strncmp(name, p2->name, 0x1f) == 0) {
                return;
            }
            i++;
            p2++;
        }
        for (; count > 0; count--) {
            if (p1->f20 == 0) {
                break;
            }
            p1++;
        }
        strncpy(p1->name, name, 0x1f);
        p1->f20 = a2;
        p1->f24 = a3;
        mgr->unk2 = mgr->unk2 + 1;
        if (a4 != 0) {
            mgr->unk8 = p1;
        }
        unsigned char *base = GetInstance__Q34nw4r2ef8ResourceFv();
        if (p1->f20 != 0) {
            p1->f28 = (int)Add__Q34nw4r2ef8ResourceFPUc(base, (void *)p1->f20);
        }
        if (p1->f24 != 0) {
            p1->f2C = (int)AddTexture__Q34nw4r2ef8ResourceFPUc(base, (void *)p1->f24);
        }
        if (a4 != 0) {
            fn_8016EDA8(base, 0, 0, (void *)mgr->unk8->f28,
                        (void *)mgr->unk8->f2C);
        } else {
            fn_8016EDA8(base, (void *)p1->f28, (void *)p1->f2C, 0,
                        (void *)mgr->unk8->f2C);
        }
    }
}

}
}

// TODO: clean up, give variables real names
namespace jj_8001AE1C {
extern "C" {
unsigned char* GetInstance__Q34nw4r2ef8ResourceFv(void);
int RemoveEffectProject__Q34nw4r2ef8ResourceFPQ34nw4r2ef13EffectProject(void* a, void* b);
int RemoveTextureProject__Q34nw4r2ef8ResourceFPQ34nw4r2ef14TextureProject(void* a, void* b);
void gfMemFill__FPvUcUl(void* p, unsigned char c, unsigned long n);
int strncmp(const char* a, const char* b, unsigned long n);
}

struct Entry {
    char name[0x20];
    void* p20;
    void* p24;
    void* p28;
    void* p2c;
};

struct Header {
    unsigned short count;
    unsigned short active;
    Entry* base;
    Entry* last;
};

extern "C" void fn_8001AE1C(Header* h, const char* name) {
    unsigned short n;
    Header* hp = h;
    Entry* e = hp->base;
    n = hp->count;
    int i = 0;
    while (i < (int)n) {
        if (e->p20 != 0 && strncmp(name, e->name, 0x1f) == 0) {
            break;
        }
        i++;
        e++;
    }
    if (i < (int)n) {
        unsigned char* base = GetInstance__Q34nw4r2ef8ResourceFv();
        if (e->p28 != 0) {
            RemoveEffectProject__Q34nw4r2ef8ResourceFPQ34nw4r2ef13EffectProject(base, e->p28);
        }
        if (e->p2c != 0) {
            RemoveTextureProject__Q34nw4r2ef8ResourceFPQ34nw4r2ef14TextureProject(base, e->p2c);
        }
        gfMemFill__FPvUcUl(e, 0, 0x20);
        e->p20 = 0;
        e->p24 = 0;
        e->p28 = 0;
        e->p2c = 0;
        if (hp->last == e) {
            hp->last = 0;
        }
        hp->active = hp->active - 1;
    }
}
}

#include <gf/gf_camera.h>

void gfDrawSetupCoord2D() {
    gfCameraManager::getManager()->getCamera(4)->setGX();
}

#include <mt/mt_vector.h>
#include <revolution/GX.h>

// Resets the eight hardware lights to one grey light far up-right of the origin, pointing down along (-1, -1, -1).
void gfDrawResetLight() {
    GXLightObj light;
    GXColor color;
    color.a = 0x80;
    color.b = 0x80;
    color.g = 0x80;
    color.r = 0x80;
    GXInitLightColor(&light, color);
    GXInitLightPos(&light, 1.0f, 100.0f, 1.0f);
    Vec3f dir(-1.0f, -1.0f, -1.0f);
    dir.normalize();
    GXInitLightDir(&light, dir.m_x, dir.m_y, dir.m_z);
    GXInitLightSpot(&light, 44.9f, GX_SP_OFF);
    GXInitLightDistAttn(&light, 100.0f, 0.5f, GX_DA_OFF);
    GXLoadLightObjImm(&light, GX_LIGHT0);
    GXLoadLightObjImm(&light, GX_LIGHT1);
    GXLoadLightObjImm(&light, GX_LIGHT2);
    GXLoadLightObjImm(&light, GX_LIGHT3);
    GXLoadLightObjImm(&light, GX_LIGHT4);
    GXLoadLightObjImm(&light, GX_LIGHT5);
    GXLoadLightObjImm(&light, GX_LIGHT6);
    GXLoadLightObjImm(&light, GX_LIGHT7);
}
