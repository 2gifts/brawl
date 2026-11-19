#include <gf/gf_model.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_ringlift.h>

void grAdventureRingLift::update(float deltaFrame) {
    void (grAdventureRingLift::*frows[6])(float) = {
        &grAdventureRingLift::frowPreparation, &grAdventureRingLift::frowEntry, &grAdventureRingLift::frowStandby,
        &grAdventureRingLift::frowStart, &grAdventureRingLift::frowMoving, &grAdventureRingLift::frowStop,
    };
    (this->*frows[m_state])(deltaFrame);
    grGimmick::update(deltaFrame);
    updateCallback(0);
}

void grAdventureRingLift::frowPreparation(float deltaFrame) { }

void grAdventureRingLift::frowEntry(float deltaFrame) {
    unk150->m_motionPathData.m_motionRatio = static_cast<grAdventureRingLiftData*>(m_gimmickData)->m_motionSpeeds[advGetDifficultyLevel()];
    if (m_gimmickMotionPath->isMoveEnd()) {
        m_state = 2;
        m_gimmickMotionPath->changeNodeAnim(1, 0);
        m_gimmickMotionPath->applyEndFrame();
        m_gimmickMotionPath->m_frame = 0.0f;
    }
}

void grAdventureRingLift::frowStandby(float deltaFrame) { }

void grAdventureRingLift::frowStart(float deltaFrame) {
    m_gimmickMotionPath->startMove();
    changeNodeAnim(0, 0);
    float updateRate = m_modelAnims[0]->m_anmObjChrRes->GetUpdateRate();
    m_state = 4;
    m_motionRatio = m_animSpeed * updateRate;
    if (m_hasNodeAnim) {
        m_animFrame = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
        m_modelAnims[0]->setFrame(m_animFrame);
    }
    startGimmickSE(1);
    startGimmickEffect(1);
}

void grAdventureRingLift::frowMoving(float deltaFrame) {
    if (m_gimmickMotionPath->isMoveEnd()) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
        m_state = 5;
    }
    if (m_hasNodeAnim) {
        m_animFrame -= deltaFrame * m_animSpeed;
        if (m_animFrame <= 0.0f) {
            m_animFrame = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
        }
        m_modelAnims[0]->setFrame(m_animFrame);
    }
}

void grAdventureRingLift::updateCallback(u32 index) {
    if (m_gimmickMotionPath != NULL && (m_motionPathFlags & 1) && (m_motionPathFlags & 2)) {
        grNodeCallbackData* nodeData = m_calcWorldCallBack.m_nodeCallbackDatas;
        nodeData[1].m_rot = m_gimmickMotionPath->getRot();
    }
    grGimmick::updateCallback(0);
}

void grAdventureRingLift::notifyEventOnTrigger(int flag) {
    if (m_state == 0) {
        m_state = 1;
        m_gimmickMotionPath->setFrame(0.0f);
        m_gimmickMotionPath->startMove();
        startGimmickSE(0);
        startGimmickEffect(0);
    }
}

void grAdventureRingLift::notifyEventOffTrigger(int flag) { }

grAdventureRingLift* grAdventureRingLift::create(int mdlIndex, const char* taskName) {
    grAdventureRingLift* ground = new (Heaps::StageInstance) grAdventureRingLift(taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

grAdventureRingLift::~grAdventureRingLift() { }

// TODO: clean up, give variables real names
namespace jj_32F14 {
struct Base {
    unsigned char padding[0x3C];

    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void slot60();
    virtual void slot61();
    virtual void slot62();
    virtual void slot63();
    virtual void slot64();
    virtual void slot65();
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void slot69();
    virtual void slot70();
    virtual void slot71();
    virtual void slot72();
    virtual void slot73();
    virtual void slot74();
    virtual void slot75();
    virtual void slot76();
    virtual void slot77();
    virtual void slot78();
    virtual void slot79();
    virtual void slot80();
    virtual void slot81();
    virtual void slot82(int, int, float*);
    virtual void slot83();
    virtual void slot84();
    virtual void slot85();
    virtual void slot86();
    virtual void slot87();
    virtual void slot88();
    virtual void slot89();
    virtual void slot90();
    virtual void slot91();
    virtual void slot92();
};

extern "C" {
extern const float lbl_40_rodata_630;
extern const float lbl_40_rodata_688;
extern int g_stTriggerMng;

void applyEndFrame__19grGimmickMotionPathFv(void* self);
void* fn_27_232338(int a, void* b);
void setObserveFlag__9stTriggerFb(void* self, unsigned char val);
void stopGimmickSE__9grGimmickFUl(void* self, unsigned long v);
void stopGimmickEffect__9grGimmickFUl(void* self, unsigned long v);

void fn_40_32F14(void* self) {
    char* s = (char*)self;
    float* r6 = *(float**)(s + 0x150);
    *r6 = lbl_40_rodata_688;
    Base* o = *(Base**)(s + 0xCC);
    o->slot82(0, 0, r6);
    applyEndFrame__19grGimmickMotionPathFv(*(void**)(s + 0xCC));
    *(float*)(*(char**)(s + 0xCC) + 0x150) = lbl_40_rodata_630;
    o = *(Base**)(s + 0xCC);
    o->slot92();
    *(int*)(s + 0x164) = 0;
    void* ret = fn_27_232338(g_stTriggerMng, *(char**)(s + 0x150) + 0x1E4);
    setObserveFlag__9stTriggerFb(ret, 0);
    stopGimmickSE__9grGimmickFUl(self, 1);
    stopGimmickEffect__9grGimmickFUl(self, 1);
}
}
}

// TODO: clean up, give variables real names
namespace jj_33050 {
extern "C" {

extern char lbl_40_data_F19C[];
extern void* lbl_40_bss_15C;
bool isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(void*, void*, int);
char getPlayerNumber__10grYakumonoFP12grCollStatus(void*, void*);

class Pad {
public:
    char pad[0x3C];
};

class Obj : public Pad {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
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
    virtual void v27();
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
    virtual void v113();
    virtual void v114();
    virtual void trigger(int);
};

void fn_40_33050(void* ths, void* collStatus) {
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(ths, collStatus, (int)lbl_40_data_F19C)) {
        if (getPlayerNumber__10grYakumonoFP12grCollStatus(ths, collStatus) == 0) {
            int* step = (int*)((char*)ths + 0x164);
            if (*step == 2) {
                *step = 3;
                Obj* obj = (Obj*)lbl_40_bss_15C;
                obj->trigger(1);
            }
        }
    }
}

}
}

