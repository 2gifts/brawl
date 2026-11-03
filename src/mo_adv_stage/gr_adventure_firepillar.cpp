#include <gf/gf_model.h>
#include <mt/mt_common.h>
#include <mt/mt_matrix.h>
#include <sora_adv_stage/gr_adventure_firepillar.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

extern "C" int lbl_40_data_9550;
extern "C" int lbl_40_data_9554;
extern "C" int lbl_40_data_9558;
extern "C" int lbl_40_data_955C;

grAdventureFirePillar* grAdventureFirePillar::create(int mdlIndex, const char* taskName) {
    grAdventureFirePillar* ground = new (Heaps::StageInstance) grAdventureFirePillar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureFirePillar::gimmickStop() {
    unk180 = lbl_40_data_9558 + unk184 * 2;
    m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    unk180 = lbl_40_data_955C + unk184 * 2;
    changeNodeAnim(unk180, 0);
    changeTexSrtAnim(0, 0);
    m_modelAnims[0]->setFrame(0.0f);
    for (u32 i = 0; i < 5; i++) {
        stopGimmickSE(i + 3);
    }
    for (u32 i = 0; i < 5; i++) {
        stopGimmickEffect(i + 3);
    }
    endMoveSE();
    endMoveEffect();
    m_state = 2;
    unk17C = 0.0f;
}

void grAdventureFirePillar::update(float deltaFrame) {
    void (grAdventureFirePillar::*frows[4])(float) = {
        &grAdventureFirePillar::frowStarting,
        &grAdventureFirePillar::frowStarted,
        &grAdventureFirePillar::frowStopping,
        &grAdventureFirePillar::frowStopped,
    };
    (this->*frows[m_state])(deltaFrame);
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grAdventureFirePillar::frowStarting(float deltaFrame) {
    u16 animLength = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
    if (m_modelAnims[0]->m_anmObjChrRes->GetFrame() >= animLength) {
        m_state = 1;
        unk17C = 0.0f;
    }
    float scale;
    Matrix mtx(true);
    if (getNodeMatrix(&mtx, 0, (u32)0)) {
        Quat res[2];
        Quat q[2];
        res[0].m_x = mtx.m[0][0];
        res[0].m_y = mtx.m[0][1];
        res[0].m_z = mtx.m[0][2];
        res[0].m_w = mtx.m[0][3];
        q[0] = res[0];
        res[1].m_x = mtx.m[1][0];
        res[1].m_y = mtx.m[1][1];
        res[1].m_z = mtx.m[1][2];
        res[1].m_w = mtx.m[1][3];
        q[1] = res[1];
        float sx = mtSqrtf(q[0].m_z * q[0].m_z + (q[0].m_x * q[0].m_x + q[0].m_y * q[0].m_y));
        float sy = mtSqrtf(q[1].m_z * q[1].m_z + (q[1].m_x * q[1].m_x + q[1].m_y * q[1].m_y));
        if (sx < sy) {
            scale = sx;
        } else {
            scale = sy;
        }
    } else {
        scale = 1.0f;
    }
    if (unk194 < scale) {
        Ground::setVisibility(1);
        grGimmickFirePillarData* data = static_cast<grGimmickFirePillarData*>(m_gimmickData);
        if (data->unk24D == 0) {
            grYakumono::setAttackGimmick(0, 0, 0, &data->m_attackData, NULL);
        }
    }
}

void grAdventureFirePillar::frowStarted(float deltaFrame) {
    if (unk178 > 0.0f) {
        if (unk178 > unk17C) {
            unk17C += deltaFrame;
        } else {
            gimmickStop();
        }
    }
}

void grAdventureFirePillar::frowStopping(float deltaFrame) {
    u16 animLength = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
    if (m_modelAnims[0]->m_anmObjChrRes->GetFrame() >= animLength) {
        m_state = 3;
        unk17C = 0.0f;
    }
    float scale;
    Matrix mtx(true);
    if (getNodeMatrix(&mtx, 0, (u32)0)) {
        Quat res[2];
        Quat q[2];
        res[0].m_x = mtx.m[0][0];
        res[0].m_y = mtx.m[0][1];
        res[0].m_z = mtx.m[0][2];
        res[0].m_w = mtx.m[0][3];
        q[0] = res[0];
        res[1].m_x = mtx.m[1][0];
        res[1].m_y = mtx.m[1][1];
        res[1].m_z = mtx.m[1][2];
        res[1].m_w = mtx.m[1][3];
        q[1] = res[1];
        float sx = mtSqrtf(q[0].m_z * q[0].m_z + (q[0].m_x * q[0].m_x + q[0].m_y * q[0].m_y));
        float sy = mtSqrtf(q[1].m_z * q[1].m_z + (q[1].m_x * q[1].m_x + q[1].m_y * q[1].m_y));
        if (sx < sy) {
            scale = sx;
        } else {
            scale = sy;
        }
    } else {
        scale = 1.0f;
    }
    if (unk198 > scale) {
        Ground::setVisibility(0);
        grYakumono::disableAttack(0);
    }
}

void grAdventureFirePillar::frowStopped(float deltaFrame) {
    if (unk174 > unk17C) {
        unk17C += deltaFrame;
    } else if (unk188 == 0) {
        unk180 = lbl_40_data_9550 + unk184 * 2;
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
        unk180 = lbl_40_data_9554 + unk184 * 2;
        changeNodeAnim(unk180, 0);
        changeTexSrtAnim(0, 0);
        m_modelAnims[0]->setFrame(0.0f);
        startMoveSE();
        startMoveEffect();
        m_state = 0;
        unk17C = 0.0f;
    }
}

void grAdventureFirePillar::updateStatus() { }

void grAdventureFirePillar::notifyEventOnTrigger(int flag) {
    unk188 = 1;
    if (m_state != 2) {
        gimmickStop();
    }
    unk17C = 0.0f;
}

void grAdventureFirePillar::notifyEventOffTrigger(int flag) {
    unk188 = 0;
    unk180 = lbl_40_data_9550 + unk184 * 2;
    m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    unk180 = lbl_40_data_9554 + unk184 * 2;
    changeNodeAnim(unk180, 0);
    changeTexSrtAnim(0, 0);
    m_modelAnims[0]->setFrame(0.0f);
    startMoveSE();
    startMoveEffect();
    m_state = 0;
    unk17C = 0.0f;
}

void grAdventureFirePillar::dbDispInvalidatedByCameraClippingSphere() {
    grGimmick::dbDispInvalidatedByCameraClippingSphere();
}

grAdventureFirePillar::~grAdventureFirePillar() { }
