#pragma once

#include <so/so_array.h>
#include <so/templates/so_array_value_wnPikminPikminOwnerInfo.h>
#include <mt/mt_vector.h>

// Only pointers to the article are held, so the fighter unit (which cannot see the article builder) can use the manager.
class wnPikminPikmin;

// The group of Pikmin that follow one Olimar (the sub-object ftPikmin keeps at +0x1FD08). It holds three lists: the
// Pikmin in the line behind the fighter, the Pikmin held in hand and the Pikmin that were set free.
// HYPOTHESIS: list and member names; the roles follow the way the lists are filled by addPikmin, holdPikmin and
// libertyPikminAll.
class wnPikminPikminTroopsManager {
public:
    soArrayList<wnPikminPikmin*, 10> m_troops;  // 0x00
    soArrayList<wnPikminPikmin*, 10> m_hold;    // 0x80
    soArrayList<wnPikminPikmin*, 10> m_liberty; // 0x100
    s32 m_sortVariation;                        // 0x180: colour the line is sorted from

    wnPikminPikminTroopsManager();
    void addPikmin(wnPikminPikmin* pikmin);
    void erasePikmin(int taskId);
    void reset();
    int getPikminNum();
    int getHoldPikminNum();
    void holdPikmin(int num);
    wnPikminPikmin* getHoldPikminTop();
    soArrayList<wnPikminPikmin*, 10>* getHoldPikminList();
    void reducePikminAll(const soArrayFixed<wnPikminPikminOwnerInfo>* owners);
    void libertyPikminAll();
    void update(const soArrayFixed<wnPikminPikminOwnerInfo>* owners);
    void updateHoldPikmin(const soArrayFixed<wnPikminPikminOwnerInfo>* owners);
    const wnPikminPikminOwnerInfo& getAssignOwnerInfoSimple(wnPikminPikmin* pikmin, int index, const soArrayFixed<wnPikminPikminOwnerInfo>* owners);
    const wnPikminPikminOwnerInfo& getAssignOwnerInfoSub(int index, const soArrayFixed<wnPikminPikminOwnerInfo>* owners);
    void syncLr(wnPikminPikmin* pikmin, float lr);
    void syncPos(wnPikminPikmin* pikmin, Vec2f* pos);
    void setTargetPos(wnPikminPikmin* pikmin, Vec2f* pos);
    void attachChain(wnPikminPikmin* pikmin, int unk1, int unk2, int logGroup);
    void detachChain(wnPikminPikmin* pikmin, int status);
    bool isConstraint(wnPikminPikmin* pikmin, u32 taskId);
    void detachConstraint(wnPikminPikmin* pikmin);
    void setPowerMulStatus(wnPikminPikmin* pikmin, float power);
    void sortPikmin();
    void onStatusFlag(wnPikminPikmin* pikmin, u32 flag);
    void setStatusFloat(wnPikminPikmin* pikmin, float value, u32 index);
    void onInstanceFlag(wnPikminPikmin* pikmin, u32 flag);
    bool isMakeBloodAll();
    int getCatchKind(wnPikminPikmin* pikmin);
    bool attachGround(wnPikminPikmin* pikmin);
    void setOwnerCondition(wnPikminPikmin* pikmin, int condition);
    void updateNodeSRT(wnPikminPikmin* pikmin);
    void holdPikminForce(int taskId);
    void updateJostlePriority();
    void setVisibilityChain(u8 visible, int num, int rest);
    void setTeamAll(int team, int teamOwner, int skipFlagged);
};
static_assert(sizeof(wnPikminPikminTroopsManager) == 0x184, "Troops manager layout");
