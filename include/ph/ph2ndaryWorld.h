#pragma once

#include <havok/hkArray.h>
#include <types.h>

class Vec2f;
class gfArea;
class grCollCollection;

// Secondary-motion world (map class ph2ndaryWorld). Offsets come from the asm of
// src/sora/ph/ph_2ndary_world.cpp. Field names are HYPOTHESIS; unkNN marks fields not yet seen in the asm.
class ph2ndaryWorld {
public:
    ph2ndaryWorld();
    ~ph2ndaryWorld();
    // One entry of the constraint list: kind byte, owning pointer and an id.
    struct ConstraintEntry {
        u8 kind;   // 0x00
        void* ptr; // 0x04
        s32 id;    // 0x08
    };

    // HYPOTHESIS: body of one wind area record (the kind halfword is compared against 12 and 13).
    struct WindEntry {
        u16 m_kind;     // 0x00
        u8 _02[0x1E];   // 0x02
    };
    // HYPOTHESIS: one record of the stage wind table (stride 0x24).
    struct WindElement {
        u32 m_id;          // 0x00
        WindEntry m_entry; // 0x04
    };

    // Returns the id of the new entry, or -1 when ptr is already in the list.
    s32 addConstraintArray(void* ptr, const u8* kind);
    void removeConstraintArray(void* ptr);
    f32 getSamusRandomAccel(f32 a, f32 b, f32 c);
    // HYPOTHESIS: parameter types from the register use.
    void getWindArray();
    u32 isCollisionBetweenPoints(const Vec2f* from, const Vec2f* to);

    hkArray<ConstraintEntry> m_constraints; // 0x00: data, size and capacity word
    union {
        u8 unk0C[0x48 - 0x0C];
        struct {
            s32 unkC;
            grCollCollection* unk10;
            u8 unk14;
            WindEntry* unk18;
            gfArea* unk1C;
            // HYPOTHESIS: four-byte array elements; only base storage is exposed.
            hkArrayBase unk20;
            hkArrayBase unk2C;
            u8 unk38;
            u8 unk39;
            u8 unk3A;
            u8 unk3B;
            u8 unk3C;
            u8 unk3D;
            u32 unk40;
            f32 unk44;
        };
    };
    f32 m_unk48;                          // 0x48, scale used by getSamusRandomAccel (HYPOTHESIS: a random-accel factor)
    union {
        u8 unk4C[0x6C - 0x4C];
        struct {
            u32 unk4CValue;
            u8 unk50;
            u32 unk54;
            f32 unk58;
            u32 unk5C;
            u32 unk60;
            u32 unk64;
            u8 unk68;
        };
    };
    s32 m_nextId;                         // 0x6C, id given to the next constraint
};
