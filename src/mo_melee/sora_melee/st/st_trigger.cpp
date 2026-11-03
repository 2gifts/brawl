#include <st/st_trigger.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

    void fn_27_235514(stObsTriggerSquareWindCB* windCB, grGimmickWindData2nd* wnd, s32 p2);


    extern char lbl_27_data_4BCF8[];
    void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);

} // extern "C"

stTrigger::~stTrigger() {
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    if (unk20 != nullptr) {
        delete unk20;
    }
    if (unk24 != nullptr) {
        delete unk24;
    }
    if (unk28 != nullptr) {
        delete unk28;
    }
    m_obsTriggerCB = nullptr;
    unk20 = nullptr;
    unk24 = nullptr;
    unk28 = nullptr;
}

void stTrigger::setWindParam(grGimmickWindData2nd* wnd, s32 p2) {
    if (dynamic_cast<stObsTriggerSquareWindCB*>(m_obsTriggerCB) != nullptr) {
        fn_27_235514(dynamic_cast<stObsTriggerSquareWindCB*>(m_obsTriggerCB), wnd, p2);
    }
}


void stTrigger::setValidCallback(gfCallBack* cb) {
    // HYPOTHESIS: slot 0x20 is a gfCallBack list head; the header names it m_yakumono, but setObserveYakumono stores at 0x1C
    gfCallBack* last = *(gfCallBack**)((u8*)this + 0x20);
    if (last != NULL) {
        while (*(gfCallBack**)last != NULL) {
            last = *(gfCallBack**)last;
        }
        *(gfCallBack**)last = cb;
        return;
    }
    *(gfCallBack**)((u8*)this + 0x20) = (gfCallBack*)__dynamic_cast(cb, 4, lbl_27_data_4BCF8, &__RTTI__10gfCallBack, 0);
}

void stTrigger::setKeyAreaTrigger(stGimmickAreaData* areaData, Vec2f pos) {
    stObsTriggerKeyArea* cb = new (Heaps::StageInstance) stObsTriggerKeyArea(&m_isValid, true);
    cb->createArea(areaData, pos);
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
}

void stTrigger::setTruckAreaTrigger(stSimpleAreaData* areaData) {
    stObsTriggerTruckArea* cb = new (Heaps::StageInstance) stObsTriggerTruckArea(&m_isValid, true);
    stGimmickAreaData gimmickAreaData;
    cb->setDefaultGimmickAreaData(areaData, &gimmickAreaData);
    gimmickAreaData.m_group = gfArea::Stage_Group_Space_Normal;
    gimmickAreaData.m_checkTargetGroup = 1 << gfArea::Stage_Group_Gimmick_Normal;
    cb->createArea(&gimmickAreaData, Vec2f(0.0f, 0.0f));
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
}
