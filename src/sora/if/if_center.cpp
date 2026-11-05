#include <if/if_center.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_bgm.h>
#include <types.h>
#include <gf/gf_pad_system.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>

// HYPOTHESIS: IfCenter::updateTimeFast (0x800DDCF8, still fn_) takes the time to show.
// IfCenter::updateTimeFast (the map name); src/mo_melee/sora_melee/st/st_operator_rule_time.cpp calls it by this stand-in name.
extern "C" void fn_800DDCF8(IfCenter* center, s32 time);

struct IfCenterStateFunc {
    void (*m_init)(IfCenter* center);
    s32 (*m_main)(IfCenter* center);
};

static const IfCenterStateFunc s_stateFuncTbl[12] = {
    { NULL, NULL },
    { IfCenter::centerReadyInit, IfCenter::centerReadyMain },
    { IfCenter::centerCountReadyInit, IfCenter::centerCountReadyMain },
    { IfCenter::centerGoInit, IfCenter::centerGoMain },
    { IfCenter::centerTimeUpInit, IfCenter::centerTimeUpMain },
    { IfCenter::centerCompleteInit, IfCenter::centerCompleteMain },
    { IfCenter::centerSuddenDeathInit, IfCenter::centerSuddenDeathMain },
    { IfCenter::centerGameSetInit, IfCenter::centerGameSetMain },
    { IfCenter::centerFailureInit, IfCenter::centerFailureMain },
    { IfCenter::centerSuccessInit, IfCenter::centerSuccessMain },
    { IfCenter::centerGameStartInit, IfCenter::centerGameStartMain },
    { IfCenter::centerGoalInit, IfCenter::centerGoalMain },
};

struct IfCenterTimePos {
    float x;
    float y;
};
static const IfCenterTimePos s_timePosTbl[3] = { { 0.0f, 0.0f }, { 0.0f, -1.0f }, { -10.0f, 0.0f } };

static const u8 s_centerObjTbl[11] = { 13, 14, 15, 16, 17, 18, 19, 20, 12, 23, 0 };

IfCenter::IfCenter() {
    init();
}

void IfCenter::init() {
    m_group = NULL;
    unkC = NULL;
    for (int i = 0; i < 81; i++) {
        m_models[i] = NULL;
    }
    for (int i = 0; i < 4; i++) {
        m_expands[i] = NULL;
    }
    m_timeDisp = false;
    unk0_40 = false;
    unk0_20 = false;
    m_state = 0;
    m_msg = NULL;
    unk168 = -1;
    unk16C = 1;
    m_updateRate = 1.0f;
    unk174 = 0;
    unk178 = -1;
}

IfCenter::~IfCenter() {
    destroyModel();
}

void IfCenter::setTimePos(int index) {
    MuObject* obj = m_models[0];
    Vec3f pos;
    pos.m_x = obj->m_trans.m_x;
    pos.m_y = obj->m_trans.m_y;
    pos.m_z = obj->m_trans.m_z;
    pos.m_x += s_timePosTbl[index].x;
    pos.m_y += s_timePosTbl[index].y;
    obj->setTrans(&pos);
}

void IfCenter::removeCenterTime() {
    unk0_40 = true;
    fn_800DDCF8(this, unk17C);
}

extern const u8 lbl_805A2398[4][2];
extern const char lbl_8059E228[8];

void IfCenter::updateTimeInit() {
    if (m_timeDisp) {
        nw4r::g3d::ResMdl model = static_cast<nw4r::g3d::ScnMdlExpand*>(m_expands[0])->m_scnMdl->m_resMdl;
        char nodeName[8];
        for (int digit = 0; digit < 4; digit++) {
            for (int part = 0; part < 2; part++) {
                sprintf(nodeName, lbl_8059E228, lbl_805A2398[digit][part]);
                unk180Nodes[digit][part] = model.GetResNode(nodeName).GetID();
            }
        }
        m_models[0]->m_modelAnim->setUpdateRate(0.0f);
        m_models[0]->m_sceneModel->SetScnObjOption(8, 1);
        for (int index = 1; index < 9; index++) {
            m_models[index]->m_modelAnim->setUpdateRate(0.0f);
            m_models[index]->m_sceneModel->SetScnObjOption(8, 1);
        }
        m_models[9]->m_modelAnim->setUpdateRate(0.0f);
        for (int digit = 0; digit < 4; digit++) {
            for (int part = 0; part < 2; part++) {
                unk1A0[digit][part] = -1;
            }
        }
        for (int digit = 0; digit < 4; digit++) {
            for (int part = 0; part < 2; part++) {
                unk1C0[digit][part] = -1;
            }
        }
    }
}

void IfCenter::destroyModel() {
    for (int i = 0; i < 4; i++) {
        if (m_expands[i] != NULL) {
            m_expands[i]->Destroy();
            m_expands[i] = NULL;
        }
    }
    for (int i = 0; i < 81; i++) {
        if (m_models[i] != NULL) {
            delete m_models[i];
            m_models[i] = NULL;
        }
    }
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
}

void IfCenter::setCenterState(int state) {
    for (int i = 0; i <= 10; i++) {
        MuObject* obj = m_models[s_centerObjTbl[i]];
        if (obj != NULL) {
            m_group->Remove(obj->m_sceneModel);
        }
    }
    m_state = state;
    if (s_stateFuncTbl[state].m_init != NULL) {
        s_stateFuncTbl[state].m_init(this);
    }
}

void IfCenter::removeAllCenterObj() {
    for (int i = 13; i < 24; i++) {
        if (m_models[i] != NULL) {
            m_group->Remove(m_models[i]->m_sceneModel);
        }
    }
}

void IfCenter::centerReadyInit(IfCenter* center) {
    MuObject* obj = center->m_models[13];
    obj->m_modelAnim->setUpdateRate(center->m_updateRate);
    obj->setFrame(0.0f);
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
    u32 bgmId = center->unk168;
    if (bgmId != -1) {
        center->m_group->Insert(center->m_group->sceneItemsCount, center->m_models[12]->m_sceneModel);
        sndBgmTitleData* data = lbl_805A01D8->getBgmTitleDataPlayId(bgmId);
        if (data != NULL) {
            center->m_msg->printIndex(0, data->m_msgIndex);
        }
    }
}

s32 IfCenter::centerReadyMain(IfCenter* center) {
    MuObject* obj = center->m_models[13];
    if (!obj->isNodeAnimFinished()) {
        return center->m_state;
    }
    center->m_group->Remove(obj->m_sceneModel);
    center->m_group->Remove(center->m_models[12]->m_sceneModel);
    s32 next = 0;
    if (center->unk16C) {
        next = 3;
    }
    return next;
}

void IfCenter::centerCountReadyInit(IfCenter* center) {
    MuObject* obj = center->m_models[23];
    obj->m_modelAnim->setUpdateRate(center->m_updateRate);
    obj->setFrame(0.0f);
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
    u32 bgmId = center->unk168;
    if (bgmId != -1) {
        center->m_group->Insert(center->m_group->sceneItemsCount, center->m_models[12]->m_sceneModel);
        sndBgmTitleData* data = lbl_805A01D8->getBgmTitleDataPlayId(bgmId);
        if (data != NULL) {
            center->m_msg->printIndex(0, data->m_msgIndex);
        }
    }
}

s32 IfCenter::centerCountReadyMain(IfCenter* center) {
    MuObject* obj = center->m_models[23];
    if (!obj->isNodeAnimFinished()) {
        return center->m_state;
    }
    center->m_group->Remove(obj->m_sceneModel);
    center->m_group->Remove(center->m_models[12]->m_sceneModel);
    s32 next = 0;
    if (center->unk16C) {
        next = 3;
    }
    return next;
}

void IfCenter::centerGoInit(IfCenter* center) {
    MuObject* obj = center->m_models[14];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerGoMain(IfCenter* center) {
    MuObject* obj = center->m_models[14];
    if (!obj->isAnimFinished()) {
        return 3;
    }
    center->m_group->Remove(obj->m_sceneModel);
    return 0;
}

void IfCenter::centerTimeUpInit(IfCenter* center) {
    MuObject* obj = center->m_models[15];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerTimeUpMain(IfCenter* center) {
    MuObject* obj = center->m_models[15];
    bool finished;
    if (!obj->isAnimFinished()) {
        finished = false;
    } else {
        center->m_group->Remove(obj->m_sceneModel);
        finished = true;
    }
    if (!finished) {
        return 4;
    }
    return 0;
}

void IfCenter::centerCompleteInit(IfCenter* center) {
    MuObject* obj = center->m_models[16];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerCompleteMain(IfCenter* center) {
    bool finished;
    if (center->m_models[16]->isAnimFinished()) {
        finished = true;
    } else {
        finished = false;
    }
    if (!finished) {
        return 5;
    }
    return 0;
}

void IfCenter::centerSuddenDeathInit(IfCenter* center) {
    MuObject* obj = center->m_models[17];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
    float rate = center->m_updateRate;
    obj->m_modelAnim->setUpdateRate(rate);
}

s32 IfCenter::centerSuddenDeathMain(IfCenter* center) {
    MuObject* obj = center->m_models[17];
    bool finished;
    if (!obj->isAnimFinished()) {
        finished = false;
    } else {
        center->m_group->Remove(obj->m_sceneModel);
        finished = true;
    }
    if (!finished) {
        return 6;
    }
    return 3;
}

void IfCenter::centerGameSetInit(IfCenter* center) {
    MuObject* obj = center->m_models[18];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
    center->unk174 = 0;
}

s32 IfCenter::centerGameSetMain(IfCenter* center) {
    MuObject* obj = center->m_models[18];
    bool finished;
    if (!obj->isAnimFinished()) {
        finished = false;
    } else {
        center->m_group->Remove(obj->m_sceneModel);
        finished = true;
    }
    if (!finished) {
        return 7;
    }
    s32 next = 0;
    if (++center->unk174 < 2) {
        next = 7;
    }
    return next;
}

void IfCenter::centerFailureInit(IfCenter* center) {
    MuObject* obj = center->m_models[19];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerFailureMain(IfCenter* center) {
    switch (center->m_models[19]->isAnimFinished()) {
    case 0:
        return 8;
    default:
        return 0;
    }
}

void IfCenter::centerSuccessInit(IfCenter* center) {
    MuObject* obj = center->m_models[20];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerSuccessMain(IfCenter* center) {
    s32 next = 9;
    if (center->m_models[20]->isAnimFinished()) {
        next = 0;
    }
    return next;
}

void IfCenter::centerGameStartInit(IfCenter* center) {
    MuObject* obj = center->m_models[21];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerGameStartMain(IfCenter* center) {
    MuObject* obj = center->m_models[21];
    bool finished;
    if (!obj->isAnimFinished()) {
        finished = false;
    } else {
        center->m_group->Remove(obj->m_sceneModel);
        finished = true;
    }
    if (!finished) {
        return 10;
    }
    return 0;
}

void IfCenter::centerGoalInit(IfCenter* center) {
    MuObject* obj = center->m_models[22];
    obj->m_modelAnim->setUpdateRate(1.0f);
    obj->setFrame(0.0f);
    center->removeAllCenterObj();
    center->m_group->Insert(center->m_group->sceneItemsCount, obj->m_sceneModel);
}

s32 IfCenter::centerGoalMain(IfCenter* center) {
    MuObject* obj = center->m_models[22];
    bool finished;
    if (!obj->isAnimFinished()) {
        finished = false;
    } else {
        center->m_group->Remove(obj->m_sceneModel);
        finished = true;
    }
    if (!finished) {
        return 11;
    }
    return 0;
}

void IfCenter::eraseCorps() {
    nw4r::g3d::ScnGroup* corps = m_expands[2];
    while (corps->sceneItemsCount != 0) {
        // HYPOTHESIS: an inlined ScnGroup::PopBack.
        if (corps->sceneItemsCount != 0) {
            corps->Remove(corps->sceneItemsCount - 1);
        }
    }
    m_group->Remove(corps);
}

extern const u8 lbl_805A23A0[8];

void IfCenter::setVisibleReplaySave(bool visible, int controller, bool enabled, bool alternate) {
    MuObject* obj = m_models[80];
    m_group->Remove(obj->m_sceneModel);
    if (visible) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(controller, &status);
        float frame;
        if (status.m_error == gfPadError::NONE) {
            frame = lbl_805A23A0[status.m_controllerType];
        } else if (controller < 4) {
            frame = 0.0f;
        } else {
            frame = 2.0f;
        }
        obj->setFrameTex(frame);
        obj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        int visibleFrame = !enabled;
        if (alternate) {
            visibleFrame += 2;
        }
        obj->setFrameVisible(visibleFrame);
        obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        m_group->Insert(m_group->sceneItemsCount, obj->m_sceneModel);
    }
}

void IfCenter::startMelee(nw4r::g3d::ScnGroup* group, int mode, bool flag, int time, nw4r::g3d::ScnGroup* timeGroup) {
    m_group = group;
    unkC = timeGroup;
    m_timeDisp = false;
    unk0_40 = false;
    unk0_20 = false;
    if (mode != 0) {
        m_timeDisp = true;
        unk0_20 = flag;
        if (mode == 2) {
            unk0_40 = true;
        }
    }
    if (m_timeDisp) {
        updateTimeInit();
        fn_800DDCF8(this, time);
    }
    m_state = 0;
}

void IfCenter::main() {
    int previousState = m_state;
    if (s_stateFuncTbl[m_state].m_main != NULL) {
        m_state = s_stateFuncTbl[m_state].m_main(this);
    }
    if (m_state != previousState) {
        if (s_stateFuncTbl[m_state].m_init != NULL) {
            s_stateFuncTbl[m_state].m_init(this);
        }
    }
    if (m_timeDisp && unk178 >= 0) {
        if (unk178 >= 0) {
            if (m_expands[0]->m_parent != NULL) {
                if (unkC != NULL) {
                    unkC->Remove(m_expands[0]);
                } else {
                    m_group->Remove(m_expands[0]);
                }
            }
            bool hidden;
            if (--unk178 < 0 || (unk178 % 15) / 7 > 0) {
                hidden = false;
            } else {
                hidden = true;
            }
            if (!hidden) {
                if (m_expands[0]->m_parent == NULL) {
                    if (unkC != NULL) {
                        unkC->Insert(unkC->sceneItemsCount, m_expands[0]);
                    } else {
                        m_group->Insert(m_group->sceneItemsCount, m_expands[0]);
                    }
                }
            }
            m_models[0]->calcMatrix();
        }
    }
}

void IfCenter::dispKumiteKills(int kills) {
    nw4r::g3d::ScnMdlExpand* group = static_cast<nw4r::g3d::ScnMdlExpand*>(m_expands[3]);
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove(group->sceneItemsCount - 1);
        }
    }
    m_group->Remove(group);
    kills = kills > 0 ? kills : 0;
    kills = kills < 9999 ? kills : 9999;
    int digit = 0;
    for (; digit < 4 && (kills > 0 || digit == 0); digit++) {
        MuObject* obj = m_models[76 + digit];
        obj->setFrameTex(kills % 10);
        obj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        obj->calcMatrix();
        char nodeName[8];
        sprintf(nodeName, lbl_8059E228, digit);
        group->PushBack(obj->m_sceneModel, nodeName);
        kills /= 10;
    }
    MuObject* obj = m_models[75];
    obj->setFrameNode(digit);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    m_group->Insert(m_group->sceneItemsCount, group);
}
