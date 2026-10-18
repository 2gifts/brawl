#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

// One parallax layer of an adventure stage: a model node ("enkeiN") that follows the camera with a per-layer scale and
// offset read from the stage (stAdventure2::m_backGroundParams).
class grAdventureBackGround {
public:
    grAdventureBackGround(int index, const char* nodeName);
    virtual ~grAdventureBackGround();
    virtual void update(float deltaFrame);

    bool setBackGround(nw4r::g3d::ScnMdl* sceneModel);

private:
    bool m_isFound;             // 0x4, the node exists in one of the models
    int m_index;                // 0x8, layer index (0..3)
    nw4r::g3d::ResNode m_node;  // 0xC
    u32 unk10;                  // 0x10
    Vec3f m_nodePos;            // 0x14, the node's original translation
    Vec3f m_offset;             // 0x20
    Vec2f m_scale;              // 0x2C, camera-follow ratio
    char m_nodeName[0x20];      // 0x34
};
static_assert(sizeof(grAdventureBackGround) == 0x54, "Class is wrong size!");

#include <sora_adv_stage/st_adventure2.h>

// A gimmick whose model carries locator nodes that the stage reads positions from (fighter start points and the like).
class grAdventureFighterPos : public grGimmick {
protected:
    u32 unk14C; // 0x14C

public:
    grAdventureFighterPos(const char* taskName);
    virtual ~grAdventureFighterPos();
    virtual void update(float deltaFrame);

    bool getLocaterNodePosition(Vec3f* pos, const char* nodeName);

    static grAdventureFighterPos* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureFighterPos) == 0x150, "Class is wrong size!");
// The base ground of an adventure stage model: a gimmick that also drives the four parallax background layers.
class grAdventure2 : public grGimmick {
protected:
    u32 unk14C;                                 // 0x14C
    grAdventureBackGround* m_backGrounds[4];    // 0x150

public:
    grAdventure2(const char* taskName) : grGimmick(taskName) {
        unk14C = 0;
        m_backGrounds[0] = NULL;
        m_backGrounds[1] = NULL;
        m_backGrounds[2] = NULL;
        m_backGrounds[3] = NULL;
        m_isUseMdlNameForStartAnim = true;
        m_transparencySettings = 0;
    }
    virtual ~grAdventure2();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void renderPre();

    static grAdventure2* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventure2) == 0x160, "Class is wrong size!");

