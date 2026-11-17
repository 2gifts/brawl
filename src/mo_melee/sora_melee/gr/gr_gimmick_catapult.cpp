// The ykAreaData members are cleared inline by the constructor (as in gr_gimmick_area_common.cpp).
#define YK_DATA_INLINE_CTOR
#include <types.h>
#include <gr/gr_gimmick_catapult.h>
#include <gr/gr_gimmick_motion_path.h>
#include <so/event/so_gimmick_event_presenter.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

grGimmickCatapult::~grGimmickCatapult() {
}

grGimmickCatapult* grGimmickCatapult::create(short mdlIndex, const char* taskName) {
    grGimmickCatapult* ground = new (Heaps::StageInstance) grGimmickCatapult(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grGimmickCatapult::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_state != 0) {
        Vec3f pos;
        Vec3f rot;
        pos = m_motionPath->getPos();
        rot = m_motionPath->getRot();
        setPos(&pos);
        setRot(&rot);
    }
    m_frameCount += deltaFrame;
    grGimmick::updateCallback(0U);
}

struct grGimmickCatapultPosArgs : soGimmickCatapultEventArgs {
    inline grGimmickCatapultPosArgs() : soGimmickCatapultEventArgs(Gimmick::Catapult_Event_Pos, 0) { }
};

// The positions of the fighters on the catapult follow their nodes.
void grGimmickCatapult::presentFighterPosEvent() {
    grGimmickCatapultPosArgs args;
    nw4r::math::MTX34 mtxStorage; // MATCH-ONLY: Matrix's own constructor would run (and be called) here
    for (int i = 0; i < 2; i++) {
        if ((&m_360)[i] != -1) {
            getNodeMatrix(static_cast<Matrix*>(&mtxStorage), 0, (&m_360)[i - 2]);
            Vec3f nodePos = static_cast<Matrix*>(&mtxStorage)->getTranslation();
            args.m_pos.m_x = nodePos.m_x;
            args.m_pos.m_y = nodePos.m_y;
            if (m_catapultData->m_isFaceLeft == 0) {
                args.m_lr = 1.0f;
            }
            if (m_catapultData->m_isFaceLeft == 1) {
                args.m_lr = -1.0f;
            }
            m_yakumono->presentEventGimmick(&args, (&m_360)[i]);
        }
    }
}
