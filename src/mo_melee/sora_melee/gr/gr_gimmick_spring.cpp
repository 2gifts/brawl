// The ykAreaData members are cleared inline by startup (as in gr_gimmick_area_common.cpp).
#define YK_DATA_INLINE_CTOR
#include <gf/gf_memory_util.h>
#include <gr/gr_gimmick_spring.h>
#include <nw4r/g3d/g3d_anmshp.h>
#include <snd/snd_system.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <st/st_trigger.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

struct grGimmickSpringEventArgs : soGimmickSpringEventArgs {
    grGimmickSpringEventArgs(Gimmick::EventKind kind, int sendID) : soGimmickSpringEventArgs(kind, sendID) {}
};

grGimmickSpring::grGimmickSpring(const char* taskName) :
    grYakumono(taskName), m_276(0), m_state(State_Rest), m_animFrame(0.0f) { }

grGimmickSpring::~grGimmickSpring() {
}

extern f32 lbl_27_rodata_4410;

void grGimmickSpring::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_state) {
    case State_On: {
        m_modelAnims[0]->m_anmObjShpRes->SetFrame(m_animFrame);
        // MATCH-ONLY: the lengths convert to float as signed ints.
        if (m_animFrame > (f32)(s32)m_animOnLength) {
            grGimmickSpringEventArgs args(Gimmick::Spring_Event_Shoot, 0);
            getTopNode(&args.m_topPos);
            args.m_bounce = m_springData->m_bounce;
            args.m_rot = m_springData->m_rot;
            m_yakumono->presentEventGimmick(&args, -1);
            setMotionOff();
        } else {
            grGimmickSpringEventArgs args(Gimmick::Spring_Event_Pos, 0);
            getTopNode(&args.m_topPos);
            m_yakumono->presentEventGimmick(&args, -1);
        }
        break;
    }
    case State_Off: {
        m_modelAnims[0]->m_anmObjShpRes->SetFrame(m_animFrame);
        if (m_animFrame > (f32)(s32)m_animOffLength) {
            m_modelAnims[0]->unbindShapeAnim(m_sceneModels[0]);
            // MATCH-ONLY: the reset value is loaded before the state byte is stored.
            f32 rest = lbl_27_rodata_4410;
            m_state = State_Rest;
            m_animFrame = rest;
        }
        break;
    }
    }
    m_animFrame += deltaFrame;
    grGimmick::updateCallback(0U);
}


extern f32 lbl_27_rodata_4410;

void grGimmickSpring::setMotionOff() {
    m_modelAnims[0]->unbindShapeAnim(m_sceneModels[0]);
    changeNodeAnim(0, 0);
    changeShapeAnim(0, 0);
    g_sndSystem->playSE(static_cast<SndID>(0x226E), -1, 0, 0, -1);
    // MATCH-ONLY: the frame constant is loaded before the state store to match the original schedule.
    f32 animFrame = lbl_27_rodata_4410;
    m_state = State_Off;
    m_animFrame = animFrame;
}


bool grGimmickSpring::setSleepArea(bool unk1) {
    bool result = grYakumono::setSleepArea(unk1);
    if (unk1 == 1) {
        soGimmickEventArgs_Disable args;
        m_yakumono->presentEventGimmick(&args, -1);
    }
    return result;
}


bool grGimmickSpring::disableArea() {
    bool result = grYakumono::disableArea();
    soGimmickEventArgs_Disable args;
    m_yakumono->presentEventGimmick(&args, -1);
    return result;
}


struct grGimmickSpringPosArgs : soGimmickSpringEventArgs {
    inline grGimmickSpringPosArgs() : soGimmickSpringEventArgs(Gimmick::Spring_Event_Pos, 0) {}
};

void grGimmickSpring::presentPosEvent() {
    grGimmickSpringPosArgs args;
    getTopNode(&args.m_topPos);
    m_yakumono->presentEventGimmick(&args, -1);
}

void grGimmickSpring::presentShootEvent() {
    grGimmickSpringEventArgs args(Gimmick::Spring_Event_Shoot, 0);
    getTopNode(&args.m_topPos);
    args.m_bounce = m_springData->m_bounce;
    args.m_rot = m_springData->m_rot;
    m_yakumono->presentEventGimmick(&args, -1);
}

void grGimmickSpring::getTopNode(Vec3f* pos) {
    Ground::getNodePosition(pos, 0, "SpringTop");
}

void grGimmickSpring::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_springData = static_cast<grGimmickSpringData*>(getGimmickData());
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
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
    m_animOffLength = m_resFile.GetResAnmShp(0).ptr()->m_animLength;
    m_animOnLength = m_resFile.GetResAnmShp(1).ptr()->m_animLength;
    createIsValidTrigger(&m_springData->m_triggerData);
}

void grGimmickSpring::setInitializeFlag() {
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

void grGimmickSpring::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    // HYPOTHESIS: 0x17..0x19 are the spring events (on, shoot, position); 0x33 is skipped first.
    int kind = eventInfo->m_kind;
    if (kind != 0x33 && kind > 0x16 && kind < 0x1A) {
        switch (kind) {
        case 0x17:
            if (m_state != State_On) {
                if (m_state != State_Rest) {
                    m_modelAnims[0]->unbindShapeAnim(m_sceneModels[0]);
                }
                changeNodeAnim(1, 0);
                changeShapeAnim(1, 0);
                // MATCH-ONLY: the frame constant is loaded before the state byte is stored.
                f32 animFrame = lbl_27_rodata_4410;
                m_state = State_On;
                m_animFrame = animFrame;
            }
            getTopNode(&static_cast<soGimmickSpringEventArgs*>(eventInfo)->m_topPos);
            break;
        }
    }
}
