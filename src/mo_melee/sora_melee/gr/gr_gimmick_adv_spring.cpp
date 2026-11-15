// The ykAreaData members are cleared inline by the constructor (as in gr_gimmick_area_common.cpp).
#define YK_DATA_INLINE_CTOR
#include <gf/gf_memory_util.h>
#include <gr/gr_gimmick_adv_spring.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <snd/snd_system.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <st/st_trigger.h>
#include <types.h>

// The spring of the Subspace stages: a fighter in its area starts the "on" node animation, the spring shoots at the end
// of it and plays the "off" animation back to rest.

struct grGimmickAdvSpringEventArgs : soGimmickSpringEventArgs {
    grGimmickAdvSpringEventArgs(Gimmick::EventKind kind, int sendID) : soGimmickSpringEventArgs(kind, sendID) { }
};

grGimmickAdvSpring* grGimmickAdvSpring::create(int mdlIndex, const char* taskName) {
    grGimmickAdvSpring* ground = new (Heaps::StageInstance) grGimmickAdvSpring(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grGimmickAdvSpring::grGimmickAdvSpring(const char* taskName) :
    grYakumono(taskName), unk154(0), m_state(State_Rest), m_animFrame(0.0f) { }

grGimmickAdvSpring::~grGimmickAdvSpring() { }

void grGimmickAdvSpring::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_state) {
    case State_On:
        // MATCH-ONLY: the lengths convert to float as signed ints.
        if (m_animFrame > (f32)(s32)m_animOnLength) {
            grGimmickAdvSpringEventArgs args(Gimmick::Spring_Event_Shoot, 0);
            getTopNode(&args.m_topPos);
            args.m_bounce = m_springData->m_bounce;
            args.m_rot = m_springData->m_rot;
            m_yakumono->presentEventGimmick(&args, -1);
            setMotionOff();
        } else {
            grGimmickAdvSpringEventArgs args(Gimmick::Spring_Event_Pos, 0);
            getTopNode(&args.m_topPos);
            m_yakumono->presentEventGimmick(&args, -1);
        }
        break;
    case State_Off:
        if (m_animFrame > (f32)(s32)m_animOffLength) {
            m_state = State_Rest;
            m_animFrame = 0.0f;
        }
        break;
    }
    m_animFrame += deltaFrame;
    grGimmick::updateCallback(0U);
}

void grGimmickAdvSpring::setMotionOn() {
    m_motionRatio = 1.0f;
    changeNodeAnim(1, 0);
    m_state = State_On;
    m_animFrame = 0.0f;
}

void grGimmickAdvSpring::setMotionOff() {
    changeNodeAnim(0, 0);
    g_sndSystem->playSE(static_cast<SndID>(0x226E), -1, 0, 0, -1);
    m_state = State_Off;
    m_animFrame = 0.0f;
}

void grGimmickAdvSpring::getTopNode(Vec3f* pos) {
    Ground::getNodePosition(pos, 0, "SpringTop");
}

void grGimmickAdvSpring::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_springData = static_cast<grGimmickSpringData*>(getGimmickData());
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    grGimmickMotionPathInfo motionPathInfo = { data, &m_springData->m_motionPathData, false, true, NULL, NULL, NULL, NULL, NULL, NULL };
    createAttachMotionPath(&motionPathInfo, NULL, "path_locator");
    Vec3f pos;
    Vec3f rot;
    pos = Vec3f(m_springData->m_pos.m_x, m_springData->m_pos.m_y, 0.0f);
    rot = Vec3f(0.0f, 0.0f, m_springData->m_rot);
    setPos(&pos);
    setRot(&rot);
    changeNodeAnim(1, 0);
    m_motionRatio = 0.0f;
    u32 nodeIndex = Ground::getNodeIndex(0, "SpringTop");
    gfMemFill(&m_ykData, 0, sizeof(m_ykData));
    m_areaData.m_shapeType = soAreaInstance::Shape_Rectangle;
    m_areaData.m_group = 0x15;
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = soAreaData::SHAPE_MASK_FOLLOW_NODE;
    m_areaData.m_nodeIndex = nodeIndex;
    m_areaData.m_offsetPos.m_x = m_springData->m_areaData.m_offsetPos.m_x;
    m_areaData.m_offsetPos.m_y = m_springData->m_areaData.m_offsetPos.m_y - 2.0f;
    m_areaData.m_range.m_x = m_springData->m_areaData.m_range.m_x;
    m_areaData.m_range.m_y = 2.0f + m_springData->m_areaData.m_range.m_y;
    grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(0x23), -1);
    trigger->setObserveYakumono(m_yakumono);
    m_animOffLength = m_resFile.GetResAnmChr(static_cast<u32>(0)).ptr()->m_animLength;
    m_animOnLength = m_resFile.GetResAnmChr(static_cast<u32>(1)).ptr()->m_animLength;
    createIsValidTrigger(&m_springData->m_triggerData);
}

void grGimmickAdvSpring::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_springData->m_triggerData, true)) {
        if (m_springData->m_triggerData.m_isValid) {
            setValid(0);
        }
    } else {
        if (!m_springData->m_triggerData.m_isValid) {
            setValid(0);
        }
    }
}

void grGimmickAdvSpring::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    // HYPOTHESIS: 0x17..0x19 are the spring events (on, shoot, position); 0x33 is skipped first.
    int kind = eventInfo->m_kind;
    if (kind != 0x33 && kind > 0x16 && kind < 0x1A) {
        switch (kind) {
        case 0x17:
            if (m_state != State_On) {
                setMotionOn();
            }
            getTopNode(&static_cast<soGimmickSpringEventArgs*>(eventInfo)->m_topPos);
            break;
        }
    }
}
