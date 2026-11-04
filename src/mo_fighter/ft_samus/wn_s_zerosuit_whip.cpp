#include <types.h>
#include <wn/samus/wn_s_zerosuit_whip.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// soExternalValueAccesser::getMotionKind (the map name); wn_szerosuit_paralyzer.cpp calls it by this stand-in name.
extern "C" int fn_27_8CC10(StageObject* stageObject);

void wnSZerosuitWhip::deactivate(bool flag) {
    if (m_moduleAccesser->m_enumerationStart->m_workManageModule->getInt(0x10000007) != -1) {
        m_moduleAccesser->m_enumerationStart->m_collisionAttackModule->unRefLogGroup(0);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(-1, 0x10000007);
    }
    Weapon::deactivate(flag);
}

void wnSZerosuitWhip::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* accesser, StageObject* sender, int selector) {
    soLinkModule* link = m_moduleAccesser->m_enumerationStart->m_linkModule;
    if (sender->soGetKind() == 0 && sender->soGetSubKind() == 0x18 &&
        link->isLink(3)) {
        u32 taskId = sender->m_taskId;
        u32 parentTaskId = link->getParentTaskId(3);
        if (parentTaskId == taskId) {
            switch (event->m_eventKind) {
            case 0x455:
                int kind = -1;
                switch (fn_27_8CC10(sender)) {
                case 0x1D5: kind = 3; break;
                case 0x1D6: kind = 4; break;
                case 0x1DC: kind = 5; break;
                case 0x1DD: kind = 6; break;
                }
                m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(kind, 0x10000009);
                break;
            }
        }
    }
    Weapon::notifyEventLink(event, accesser, sender, selector);
}

void wnSZerosuitWhip::processFixPosition() {
    Weapon::processFixPosition();
    int kind = m_moduleAccesser->m_enumerationStart->m_workManageModule->getInt(0x10000009);
    if (kind != -1) {
        float frame = m_moduleAccesser->m_enumerationStart->m_motionModule->getFrame();
        soMotionChangeParam param;
        param.m_kind = kind;
        param.m_frame = frame;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 1;
        param._15 = 0;
        m_moduleAccesser->m_enumerationStart->m_motionModule->changeMotionRequest(&param);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(-1, 0x10000009);
    }
}

bool wnSZerosuitWhip::notifyEventCollisionAttackCheck(u32) {
    int frame = m_moduleAccesser->m_enumerationStart->m_workManageModule->getInt(0x10000008);
    if (frame > 0) {
        m_moduleAccesser->m_enumerationStart->m_stopModule->setHitStopFrame(frame, 0);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(0.0f, 0x11000001);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(0, 0x10000008);
    }
    return false;
}
