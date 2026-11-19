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

// TODO: clean up, give variables real names
namespace jj_32698 {
struct GrYakumonoBase {
    char pad0[0x3c];
};

class GrYakumono : public GrYakumonoBase {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27(int);
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual void v46();
    virtual void v47();
    virtual void v48();
    virtual void v49();
    virtual void v50();
    virtual void v51();
    virtual void v52();
    virtual void v53();
    virtual void v54();
    virtual void v55();
    virtual void v56();
    virtual void v57();
    virtual void v58();
    virtual void v59();
    virtual void v60();
    virtual void v61();
    virtual void v62();
    virtual void v63();
    virtual void v64();
    virtual void v65();
    virtual void v66();
    virtual void v67();
    virtual void v68();
    virtual void v69();
    virtual void v70();
    virtual void v71();
    virtual void v72();
    virtual void v73();
    virtual void v74();
    virtual void v75();
    virtual void v76();
    virtual void v77();
    virtual void v78();
    virtual void v79();
    virtual void v80();
    virtual void v81();
    virtual void v82();
    virtual void v83();
    virtual void v84();
    virtual void v85();
    virtual void v86();
    virtual void v87();
    virtual void v88();
    virtual void v89();
    virtual void v90();
    virtual void v91();
    virtual void v92();
    virtual void v93();
    virtual void v94();
    virtual void v95();
    virtual void v96();
    virtual void v97();
    virtual void v98();
    virtual void v99();
    virtual void v100();
    virtual void v101();
    virtual void v102();
    virtual void v103();
    virtual void v104();
    virtual void v105();
    virtual void v106();
    virtual void v107();
    virtual void v108();
    virtual void v109();
    virtual void v110();
    virtual void v111();
    virtual void v112();
    virtual void v113(int);

    char pad40[0x124];
    int unk164;
    char pad168[0x3c];
    char unk1A4;
};

extern "C" {
    extern const float lbl_40_rodata_628;
    void startGimmickSE__9grGimmickFUl(void*, unsigned long);
    void setEnableCollisionStatus__6GroundFb(void*, bool);
    int isEnableAttackPersonAttacked__10grYakumonoFUlUlUl(void*, unsigned int, unsigned int, unsigned int);

    void fn_40_32698(GrYakumono* self, unsigned int unused, const float* info, unsigned int arg3) {
        const float value = info[1];
        self->unk1A4 = 1;
        if (value > lbl_40_rodata_628 &&
            isEnableAttackPersonAttacked__10grYakumonoFUlUlUl(self, (unsigned int)info, arg3, self->unk164 + 0x70) != 0) {
            startGimmickSE__9grGimmickFUl(self, 0);
            self->v27(0);
            setEnableCollisionStatus__6GroundFb(self, false);
            self->v113(1);
        }
    }
}
}

