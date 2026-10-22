#pragma once
// Local BrawlHeaders shadow: per-player HUD storage verified by the recovered stock face, share stock and
// dragoon part operations; the remaining storage stays unknown.

#include <StaticAssert.h>
#include <gm/gm_lib.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <nw4r/math/math_types.h>
#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ScnGroup; class ScnObj; class ScnMdlExpand; } }

// HYPOTHESIS: places a HUD model at a stage position; the world matrix is set on node 1 of the model it is attached to.
class IfPlayerObjCallback : public nw4r::g3d::IScnObjCallback {
public:
    Matrix m_matrix;         // 0x4
    nw4r::math::VEC3 m_pos;  // 0x34
    float unk40;
    int m_mode;              // 0x44

    virtual ~IfPlayerObjCallback();
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
};
static_assert(sizeof(IfPlayerObjCallback) == 0x48, "Class is wrong size!");

class IfPlayer {
public:
    struct ApealComment {
        u8 m_texObj[0x44]; // 0x0 HYPOTHESIS: msTexObj (freed by disappear)
        u8 unk44;
        u8 unk45[0x3];
    };

    struct DragoonPart {
        u8 m_part;          // 0x0
        int m_state;        // 0x4 HYPOTHESIS: 0 empty, 1 placed, 2 added
        MuObject* m_object; // 0x8
    };

    bool m_0x0_0 : 1;
    bool m_scoreDisplay : 1;
    bool unk0_2 : 1; // HYPOTHESIS: the HUD of this player is shown (cleared by disappear)
    u8 unk1;
    u8 unk2;
    u8 unk3;
    int unk4;
    u8 unk8[0x8];
    int unk10;
    int m_damageState;                   // 0x14
    int unk18;
    u8 unk1C[0x4];
    u32 m_stockCount;                    // 0x20
    u8 unk24[0x4];
    // HYPOTHESIS: the HUD models by kind; 24 holds the "pos%02d" nodes of the stock faces, 25 to 29 the stock faces.
    MuObject* m_objects[50];             // 0x28
    // HYPOTHESIS: 0 holds the stock faces, 1 the dragoon parts.
    nw4r::g3d::ScnMdlExpand* m_groups[3];    // 0xF0
    u8 unkFC[0x30];
    float unk12C;
    int unk130;
    int unk134;
    u8 unk138[0xDC];
    int m_stockFaceStates[5];            // 0x214
    Vec3f m_stockFacePos[5];             // 0x228 HYPOTHESIS: start position of a shared stock face
    u8 unk264[0x4];
    DragoonPart m_dragoonParts[3];       // 0x268
    u8 m_dragoonComplete;                // 0x28C
    u8 unk28D[0x23];
    int unk2B0;
    u32 m_loupeId;                           // 0x2B4 HYPOTHESIS: the gfLoupe slot of this player
    void* m_loupeTexture;                    // 0x2B8
    IfPlayerObjCallback m_loupeCallback;     // 0x2BC
    IfPlayerObjCallback m_loupeCallback2;    // 0x304
    u8 unk34C[0x4];
    bool m_isLoupeVisible;                   // 0x350
    u8 unk351[0x7];
    IfPlayerObjCallback m_headCursorCallback; // 0x358
    u8 unk3A0;
    u8 unk3A1[0x7];
    u8 m_texObj[0x44];                       // 0x3A8 HYPOTHESIS: msTexObj (freed by disappear)
    ApealComment m_apealComments[4];         // 0x3EC
    IfPlayerObjCallback m_balloonCallback;   // 0x50C
    int unk554;
    u8 unk558[0x50];
    // HYPOTHESIS: the node ids of "pos%02d" in the stock model; 2 to 6 the stock faces.
    u32 m_posNodeIds[7];                 // 0x5A8
    u32 m_dragoonNodeIds[3];             // 0x5C4
    int unk5D0;

    // HYPOTHESIS: parameter types below come from register use and the IfMngr callers;
    // the map has the names only.
    static nw4r::g3d::ScnMdlExpand* getPlayerLocator();
    static void destroyPlayerLocator();
    static void startMelee(nw4r::g3d::ScnGroup* group);
    void updateStockCoin(u8 amount);
    bool giveShareStock(Vec3f* pos);
    void updateCoinScoreSub(u8 amount);
    void disappear();
    static void loupeRenderPreInit();
    void loupeRenderPre();
    static void stockFaceAbsentInit(int index, IfPlayer* player);
    static void stockFaceDropInit(int index, IfPlayer* player);
    static int stockFaceDropMain(int index, IfPlayer* player);
    static void stockFaceExistInit(int index, IfPlayer* player);
    static void stockFaceShareInit(int index, IfPlayer* player);
    static int stockFaceShareMain(int index, IfPlayer* player);
    Vec3f getPlayerLocatorGlobalPos();
    void setLoupeVisible(bool visible);
    void setLoupePos(const nw4r::math::VEC3* pos);
    void setHeadCursorPos(const nw4r::math::VEC3* pos);
    void setApealCommentBalloonPos(const nw4r::math::VEC3* pos);
    int deleteDragoonParts(u32 part);
    void setDragoonPartsComplete(int complete);
    void resetDragoonParts();
    virtual ~IfPlayer();
};
static_assert(sizeof(IfPlayer) == 0x5D8, "Class is wrong size!");
