// Local BrawlHeaders shadow: stShadow shadow-object pool (sizes from the stShadow pool loops)
#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>
#include <ut/ut_list.h>

class Vec3f;
namespace nw4r { namespace g3d { class ScnProc; } }

class stShadow : public gfTask {
public:
    // HYPOTHESIS: the pool entries are list nodes (stShadow links them into its two lists).
    struct ShadowObject : public utListNode {
        union {
            char _0x8[0x14];
            struct {
                u32 unk8;
                float unkC;
                float unk10;
                float unk14;
                float unk18;
            };
        };
        u32 unk1c;
        union {
            char _0x20[0x14];
            struct {
                void* unk20;
                u16 unk24;
                u16 unk26;
                u32 unk28;
                float unk2C;
                u32 unk30;
            };
        };

        ShadowObject();
    };

    union {
        char _0x40[4];
        struct {
            bool unk40;
            bool unk41;
            u8 unk42;
            u8 unk43;
        };
        struct {
            signed int : 24;
            signed int unk43_0 : 8;
        };
    };
    u16 unk44;
    u16 unk46;
    char _0x48[0x34];
    u32 unk7c;
    ShadowObject* unk80;
    utList unk84;
    utList unk90;
    char _0x9c[0x1f0];

    virtual void renderPre();
    virtual ~stShadow();

    void clearShadowObject();
    // HYPOTHESIS: the integer payloads identify and configure the shadow object.
    void addShadowObject(u32, const Vec3f*, float, u32, float, u32);
    static void advDrawShadowProc(nw4r::g3d::ScnProc*, bool);
    static void advDrawShadowProc_gr2(nw4r::g3d::ScnProc*, bool);
    void drawADVShadow(u32);
    static stShadow* create(int numObjects, int unk2); // HYPOTHESIS: parameter roles (Stage passes 0x10, 0x18)
};
static_assert(sizeof(stShadow::ShadowObject) == 0x34, "Class is wrong size!");
static_assert(sizeof(stShadow) == 0x28c, "Class is wrong size!");
