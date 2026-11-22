#define ST_OBS_TRIGGER_INLINE
#include <it/it_manager.h>
#include <it/item.h>
#include <snd/snd_system.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

// sora_melee functions without names in our symbols (map: stTrigger::checkEnemyAddDead,
// stTriggerMng::unValidItemCreate, stTriggerMng::checkGimmickInfofromAreaID).
// TODO: clean up - declare them as members once their sora_melee units are decompiled.
extern "C" bool fn_27_2340A0(stTrigger* trigger);
extern "C" void fn_27_233354(stTriggerMng* triggerMng, int genId);
extern "C" void fn_27_2323EC(stTriggerMng* triggerMng, int areaId, void* info);

// The gimmick information stTriggerMng::checkGimmickInfofromAreaID fills (0x14 bytes, the kind is the u16 at 0x2).
// HYPOTHESIS: the names.
struct stGimmickInfo {
    u16 m_unk0;
    u16 m_kind;
    u8 m_unk4[0x10];
};

// The kind of an area as the receivers read it (a signed bit field in the last byte of the word at 0x1C).
// TODO: clean up - gfArea declares that byte as padding.
struct gfAreaGroupView {
    u8 m_unk0[0x1C];
    int m_flags : 24;
    int m_kind : 8;
};

// The fields of an item the key receiver reads.
// TODO: clean up - name them in BaseItem.
struct BaseItemKeyView {
    u8 m_unk0[0x8C0];
    int m_kind;           // 0x8C0
    u8 m_unk8C4[0x3D28 - 0x8C4];
    int m_genId;          // 0x3D28
};

// The callback that sets its trigger when the enemies of a trigger are dead.
class stObsTriggerEnemyDeadCheck : public stObsTriggerCB {
public:
    stTrigger* m_enemyTrigger; // 0x10

    stObsTriggerEnemyDeadCheck(bool* flag, stTrigger* enemyTrigger);
    virtual void userProc();
    virtual ~stObsTriggerEnemyDeadCheck() { }
    virtual gfArea* getAreaPointer() { return NULL; }
    virtual bool getAreaID(int* areaID) { return false; }
    virtual void setAreaSleep(bool isSleep) { }
};

stObsTriggerEnemyDeadCheck::stObsTriggerEnemyDeadCheck(bool* flag, stTrigger* enemyTrigger) : stObsTriggerCB(flag, false) {
    m_enemyTrigger = enemyTrigger;
}

void stObsTriggerEnemyDeadCheck::userProc() {
    if (m_enemyTrigger == NULL) {
        *unk8 = true;
    } else if (fn_27_2340A0(m_enemyTrigger)) {
        *unk8 = true;
    }
}

void stAreaEventReciverKeyTrigger::reciveEvent(gfArea* area, gfArea* other) {
    if (reinterpret_cast<gfAreaGroupView*>(other)->m_kind == 3) {
        BaseItem* item = itManager::getInstance()->getItemFromAreaId(other->m_id);
        if (item != NULL && reinterpret_cast<BaseItemKeyView*>(item)->m_kind == 0x47) {
            fn_27_233354(g_stTriggerMng, reinterpret_cast<BaseItemKeyView*>(item)->m_genId);
            item->remove();
            m_owner->setAreaSleep(true);
            *m_flag = true;
            g_sndSystem->playSE(static_cast<SndID>(0x2277), -1, 0, 0, -1);
        }
    }
}

stObsTriggerKeyArea::stObsTriggerKeyArea(bool* flag, bool unkC) : stObsTriggerArea(flag, unkC) {
    m_reciver.m_flag = flag;
    m_reciver.m_owner = this;
}

void stObsTriggerKeyArea::createArea(stGimmickAreaData* areaData, Vec2f pos) {
    stObsTriggerArea::createArea(areaData, pos);
    if (m_gimmickAreaData.m_checkTargetGroup != 0) {
        getAreaPointer()->setEventReciver(&m_reciver);
    }
}

void stAreaEventReciverTruckTrigger::reciveEvent(gfArea* area, gfArea* other) {
    if (reinterpret_cast<gfAreaGroupView*>(other)->m_kind == 2) {
        stGimmickInfo info;
        memset(&info, 0, sizeof(info));
        fn_27_2323EC(g_stTriggerMng, other->m_id, &info);
        if (info.m_kind == 0x3B) {
            *m_flag = true;
        }
    }
}

stObsTriggerTruckArea::stObsTriggerTruckArea(bool* flag, bool unkC) : stObsTriggerArea(flag, unkC) {
    m_reciver.m_flag = flag;
    m_reciver.m_owner = this;
}

void stObsTriggerTruckArea::createArea(stGimmickAreaData* areaData, Vec2f pos) {
    stObsTriggerArea::createArea(areaData, pos);
    if (m_gimmickAreaData.m_checkTargetGroup != 0) {
        getAreaPointer()->setEventReciver(&m_reciver);
    }
}
