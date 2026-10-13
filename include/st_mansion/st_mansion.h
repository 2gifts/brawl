#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_Color.h>
#include <revolution/GX.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Mansion, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Mansion, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Mansion, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stMansionData {
    float unk00;    // 0x00 frames the four areas stay broken (before the break is shown)
    float unk04;    // 0x04 frames the break takes
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;    // 0x20 hit points of a pillar of the first kind
    float unk24;    // 0x24 hit points of a pillar of the second kind
    float unk28;    // 0x28 frames until a floor moves for the first time
    float unk2C;    // 0x2C least frames until the next move of a floor
    float unk30;    // 0x30 most frames until the next move of a floor
    float unk34;    // 0x34 least frames until a floor falls
    float unk38;    // 0x38 most frames until a floor falls
    float unk3C;    // 0x3C how fast a floor speeds up
    float unk40;    // 0x40 the top speed of a floor
    float unk44;
    float unk48;    // 0x48 the chance that a floor falls when it is landed on
    float unk4C;
    float unk50;
    float unk54;
    float unk58;
    float unk5C;
    float unk60;    // 0x60 how the hit points of the pillars are scaled (the lowest level of the event)
    float unk64;    // 0x64 (the second level)
    float unk68;    // 0x68 (the highest levels)
};
static_assert(sizeof(stMansionData) == 0x6C, "Class is wrong size!");

// Luigi's Mansion: the stage is a house with four areas (up/down on each side) and the pillars between them. The pillars
// can be hit until they break, then the area they hold collapses; later the areas come back. The two ghost floors move
// by themselves. The stage keeps the state of all of it and the grounds read it from the work pointers.
class stMansion : public stMelee {
    Vec3f m_posLimit[2];                // 0x1D8 the left/top and right/bottom limit of the camera
    Vec3f m_posGimmick[2];              // 0x1F0 the two places that the grounds of the stage (and the front) read
    u8 m_phase;                         // 0x208 the step of the collapse (0 = start, 1 = wait, 2 = broken, 3 = repairs)
    char _209[3];
    float m_timer;                      // 0x20C frames until the next step
    float m_sceneFrame;                 // 0x210 the frame of the scene animation (the light) that plays
    float m_scaleRate;                  // 0x214 how far the collapse is (the pillars read it)
    u8 m_eventState;                    // 0x218 5 = nothing, 0 = the event goes on (the front reads it)
    char _219[3];
    Vec3f m_posArea[4];                 // 0x21C where the four areas are (up left, down left, up right, down right)
    u8 m_stateArea[4];                  // 0x24C the step of each area (2 = fell, 3 = repairs, 4 = back)
    Vec3f m_posFloor[2];                // 0x250 where the left and the right ghost floor are
    u8 m_stateFloor[2];                 // 0x268 the step of the floors
    u8 m_isEvent;                       // 0x26A 1 = the stage is run by the event mode

public:
    stMansion();
    static stMansion* create();

    virtual ~stMansion();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjGround(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjArea(int index);
    virtual void createObjPillar(int index);
    virtual void createObjFront(int index);
    virtual void updateLimit();
    virtual void updatePillar(float deltaFrame);
    virtual void updateArea(float deltaFrame);
    virtual void notifyEventInfoGo();
    virtual u32 getZoneLightSetIndex(Vec3f* position);
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x1400047d); }

    static stClassInfoImpl<Stages::Mansion, stMansion> bss_loc_14;
};
static_assert(sizeof(stMansion) == 0x26C, "Class is wrong size!");
