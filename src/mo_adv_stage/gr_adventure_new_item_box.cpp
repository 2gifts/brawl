#include <gr/gr_calc_world_callback.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the record of the item box (sora_adv_stage map: grAdventureNewItemBox); only what the box reads is named.
struct grAdventureNewItemBoxData {
    u8 m_unk0[0x8];
    u32 m_attackFlags;              // 0x8 the attacks that break the box (isEnableAttackPersonAttacked)
    u8 m_unkC[0x38 - 0xC];
    u32 m_itemGenId;                // 0x38 the item table the box drops from (and the id adventureEventGetItem answers)
    u32 m_itemNum;                  // 0x3C how many items the box drops
    u8 m_unk40[4];
    stTriggerData m_gotTrigger;     // 0x44 set when the item was taken
    stTriggerData m_validTrigger;   // 0x48 the box is valid while this trigger is (not) set
    u8 m_unk4C[4];
    stTriggerData m_breakTrigger;   // 0x50 set when the box breaks
};

// The item box of the adventure mode: it shakes when hit and drops items from its item table when it breaks.
class grAdventureNewItemBox : public grYakumono {
public:
    enum Phase {
        Phase_Wait,
        Phase_ShakeUp,
        Phase_ShakeDown,
        Phase_BlinkVisible,
        Phase_BlinkInVisible,
        Phase_End,
    };

    int m_phase;                            // 0x150
    u8 m_unk154[0x17C - 0x154];
    u32 m_unk17C;                           // 0x17C
    u32 m_unk180;
    u32 m_unk184;
    u32 m_unk188;
    u32 m_unk18C;
    grCalcWorldCallBack m_calcWorldCallBack; // 0x190
    grAdventureNewItemBoxData* m_boxData;   // 0x1A0
    u32 m_itemGenParam;                     // 0x1A4
    u32 m_dropCount;                        // 0x1A8
    float m_unk1AC;                         // 0x1AC
    float m_shake;                          // 0x1B0
    u8 m_isHeart;                           // 0x1B4

    grAdventureNewItemBox(const char* taskName)
        : grYakumono(taskName), m_phase(Phase_Wait), m_unk17C(0), m_unk180(0), m_unk184(0), m_unk188(0), m_unk18C(0) {
        m_boxData = NULL;
        m_itemGenParam = 0;
        m_dropCount = 0;
        m_unk1AC = 0.0f;
        m_shake = 0.0f;
        m_isHeart = 0;
    }
    static grAdventureNewItemBox* create(int mdlIndex, const char* taskName);
    virtual ~grAdventureNewItemBox();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual bool adventureEventGetItem(int genParamId, stTriggerData* triggerData);
    virtual void setVisibilityByClipping(u32 isVisible);
    void requestBreak();
    void phaseWait(float deltaFrame);
    void phaseShakeUp(float deltaFrame);
    void phaseShakeDown(float deltaFrame);
    void phaseBlinkVisible(float deltaFrame);
    void phaseBlinkInVisible(float deltaFrame);
    void phaseEnd(float deltaFrame);
};
static_assert(sizeof(grAdventureNewItemBox) == 0x1B8, "Class is wrong size!");

grAdventureNewItemBox* grAdventureNewItemBox::create(int mdlIndex, const char* taskName) {
    grAdventureNewItemBox* box = new (Heaps::StageInstance) grAdventureNewItemBox(taskName);
    if (box != NULL) {
        box->setMdlIndex(mdlIndex);
    }
    return box;
}

grAdventureNewItemBox::~grAdventureNewItemBox() { }

void grAdventureNewItemBox::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_boxData->m_validTrigger, true)) {
        if (m_boxData->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_boxData->m_validTrigger.m_isValid) {
        setValid(0);
    }
    if (g_stTriggerMng->getTriggerFlag(&m_boxData->m_gotTrigger, false) == true) {
        setValid(0);
        m_phase = Phase_End;
    }
}

void grAdventureNewItemBox::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    void (grAdventureNewItemBox::*phases[])(float) = {
        &grAdventureNewItemBox::phaseWait,         &grAdventureNewItemBox::phaseShakeUp,
        &grAdventureNewItemBox::phaseShakeDown,    &grAdventureNewItemBox::phaseBlinkVisible,
        &grAdventureNewItemBox::phaseBlinkInVisible, &grAdventureNewItemBox::phaseEnd,
    };
    (this->*phases[m_phase])(deltaFrame);
    grGimmick::updateCallback(0);
}

void grAdventureNewItemBox::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (grYakumono::isEnableAttackPersonAttacked(reinterpret_cast<u32>(damage), reinterpret_cast<u32>(attackerInfo),
                                     reinterpret_cast<u32>(&m_boxData->m_attackFlags))) {
        requestBreak();
    }
}

void grAdventureNewItemBox::phaseWait(float deltaFrame) { }

void grAdventureNewItemBox::phaseShakeUp(float deltaFrame) {
    Vec3f pos = getPos();
    m_shake += 0.5f;
    if (m_shake >= 2.0f) {
        m_phase = Phase_ShakeDown;
        m_shake = 0.0f;
    }
    pos.m_y += 0.5f;
    setPos(&pos);
}

void grAdventureNewItemBox::phaseShakeDown(float deltaFrame) {
    Vec3f pos = getPos();
    m_shake += 0.5f;
    if (m_shake >= 2.0f) {
        m_phase = Phase_End;
        m_shake = 0.0f;
        setValid(0);
    }
    pos.m_y -= 0.5f;
    setPos(&pos);
}

void grAdventureNewItemBox::phaseBlinkVisible(float deltaFrame) { }

void grAdventureNewItemBox::phaseBlinkInVisible(float deltaFrame) { }

void grAdventureNewItemBox::phaseEnd(float deltaFrame) { }

bool grAdventureNewItemBox::adventureEventGetItem(int genParamId, stTriggerData* triggerData) {
    if (m_boxData->m_itemGenId == genParamId) {
        g_stTriggerMng->setTriggerFlag(&m_boxData->m_gotTrigger);
        // TODO: clean up - the trigger is copied by its parts (u16 id, two bytes).
        struct TriggerParts {
            u16 m_id;
            u8 m_flags;
            u8 m_keepKind;
        };
        *reinterpret_cast<TriggerParts*>(triggerData) = *reinterpret_cast<TriggerParts*>(&m_boxData->m_gotTrigger);
        return true;
    }
    return false;
}

void grAdventureNewItemBox::setVisibilityByClipping(u32 isVisible) {
    if (m_resFile.ptr() != NULL) {
        Ground::setVisibilityByClipping(isVisible, 0);
        m_isVisibleByClipping = isVisible;
    }
}
