#include <memory.h>
#include <sora_adv_stage/gr_adventure_attack_switch_once.h>

grAdventureAttackSwitchOnce* grAdventureAttackSwitchOnce::create(int mdlIndex, bool* isOn, const char* taskName) {
    grAdventureAttackSwitchOnce* ground = new (Heaps::StageInstance) grAdventureAttackSwitchOnce(isOn, taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

void grAdventureAttackSwitchOnce::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureAttackSwitch::startup(data, unk1, layerType);
    setSoundEffectSlot(0, 0x2274, Vec2f(0.0f, 0.0f));
    int effectId = static_cast<grGimmickAttackSwitchOnceData*>(m_switchData)->m_effectId;
    if (effectId > 0) {
        setupPositiveEffect(effectId);
    } else {
        setupDefaultEffect();
    }
}

void grAdventureAttackSwitchOnce::update(float deltaFrame) {
    grAdventureAttackSwitch::update(deltaFrame);
    if (m_damaged != 0) {
        m_damaged = 0;
        clearAttackerInfo();
    }
}

void grAdventureAttackSwitchOnce::setSimpleEffectVisibleProduction() {
    static u32 visibleProductionIndex = 4;
    SimpleEffectData simpleEffectData;
    createSimpleEffectData(&simpleEffectData, 0x103001F, "effect_locator");
    createEffectVisibleProductionForExcel(&simpleEffectData, &visibleProductionIndex, m_visibleProductions);
}

void grAdventureAttackSwitchOnce::setInitializeFlag() {
    grYakumonoSwitch::setInitializeFlag();
    grGimmickAttackSwitchOnceData* data = static_cast<grGimmickAttackSwitchOnceData*>(m_switchData);
    if (g_stTriggerMng->getTriggerFlag(&data->m_trigger, false) == true) {
        setVisibility(0);
        setEnableCollisionStatus(false);
        disableHit(0, 0);
        m_switchState = 1;
        m_onTimer = 0.0f;
    }
}

void grAdventureAttackSwitchOnce::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float damageAdd = damage->m_damageAdd;
    m_damaged = 1;
    if (damageAdd > 0.0f) {
        if (grYakumono::isEnableAttackPersonAttacked((u32)damage, (u32)attackerInfo, (u32)&m_data->m_hit)) {
            startGimmickSE(0);
            setVisibility(0);
            setEnableCollisionStatus(false);
            setOn(true);
        }
    }
}

grAdventureAttackSwitchOnce::~grAdventureAttackSwitchOnce() { }
