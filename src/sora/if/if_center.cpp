#include <cstdio>
#include <gf/gf_model.h>
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

// TODO: clean up, give variables real names
namespace jj_800DE9B4 {
class gfModelAnimation {
public:
    void setUpdateRate(float rate);
};

extern "C" void fn_800B753C(void* self, float rate);
extern const float lbl_805A2368;
extern const float lbl_805A2374;

class Obj {
public:
    virtual void s00();
    virtual void s01();
    virtual void s02();
    virtual void s03();
    virtual void s04();
    virtual void s05();
    virtual void s06();
    virtual void s07();
    virtual void s08();
    virtual void s09();
    virtual void s10();
    virtual void s11(void*, void*);
    virtual void s12();
    virtual void s13(void*);
};

extern "C" void fn_800DE9B4(void* arg0) {
    char* base = (char*)arg0;
    int i;
    void* anim = *(void**)(base + 0x4C);
    ((gfModelAnimation*)*(void**)((char*)anim + 0x14))->setUpdateRate(lbl_805A2368);
    fn_800B753C(anim, lbl_805A2374);
    char* p = base + 0x34;
    for (i = 0xD; i < 0x18; i++, p += 4) {
        void* e = *(void**)(p + 0x10);
        if (e != 0) {
            Obj* o = *(Obj**)(base + 0x8);
            o->s13(*(void**)((char*)e + 0x10));
        }
    }
    Obj* o = *(Obj**)(base + 0x8);
    o->s11(*(void**)((char*)o + 0xE4), *(void**)((char*)anim + 0x10));
}
}

namespace jj_800DEAE4 {
extern "C" void setUpdateRate__16gfModelAnimationFf(void*, float);
extern "C" void fn_800B753C(void*, float);
extern const float lbl_805A2368;
extern const float lbl_805A2374;

struct Obj2 {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10();
    virtual void v13(void*, void*);
    virtual void v14();
    virtual void v15(void*);
    char pad[0xE4 - 4];
    void* e4;
};
struct S { char pad[8]; Obj2* o; };
struct T { char pad[0x10]; void* p10; void* anim; };
struct E { char pad[0x10]; void* p10; };

extern "C" void fn_800DEAE4(S* self) {
    int i;
    T* t = *(T**)((char*)self + 0x50);
    setUpdateRate__16gfModelAnimationFf(t->anim, lbl_805A2368);
    fn_800B753C(t, lbl_805A2374);
    E** p = (E**)((char*)self + 0x34);
    for (i = 13; i < 24; i++) {
        E* e = p[4];
        if (e) {
            self->o->v15(e->p10);
        }
        p++;
    }
    Obj2* o = self->o;
    o->v13(o->e4, t->p10);
}
}

namespace jj_800DEE60 {


extern "C" void fn_800B753C(void*, float);
extern const float lbl_805A2368;
extern const float lbl_805A2374;

class Target {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11(void*, void*);
    virtual void s12();
    virtual void s13(void*);

    char pad[0xE0];
    void* field_E4;
};

extern "C" void fn_800DEE60(void* arg0) {
    char* base = (char*)arg0;
    int i;
    void* anim = *(void**)(base + 0x5C);

    ((gfModelAnimation*)*(void**)((char*)anim + 0x14))->setUpdateRate(lbl_805A2368);
    fn_800B753C(anim, lbl_805A2374);

    char* p = base + 0x34;
    for (i = 0xD; i < 0x18; i++, p += 4) {
        void* entry = *(void**)(p + 0x10);
        if (entry != 0) {
            Target* target = *(Target**)(base + 0x8);
            target->s13(*(void**)((char*)entry + 0x10));
        }
    }

    Target* target = *(Target**)(base + 0x8);
    target->s11(target->field_E4, *(void**)((char*)anim + 0x10));
}
}

namespace jj_800DEF4C {


class Controller {
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
    virtual void v11(void*, void*);
    virtual void v12();
    virtual void v13(void*);

    char pad[0xE4 - 4];
    void* fieldE4;
};

struct Self {
    char pad0[0x08];
    Controller* controller;
    char pad1[0x60 - 0x0C];
    void* model;
};

extern "C" void fn_800B753C(void*, float);
extern const float lbl_805A2368;
extern const float lbl_805A2374;

extern "C" void fn_800DEF4C(Self* self) {
    int i;
    void* model = self->model;
    char* entry;

    ((gfModelAnimation*)*(void**)((char*)model + 0x14))->setUpdateRate(lbl_805A2368);
    fn_800B753C(model, lbl_805A2374);

    entry = (char*)self + 0x34;
    i = 0xD;
    do {
        void* item = *(void**)(entry + 0x10);
        if (item != 0) {
            self->controller->v13(*(void**)((char*)item + 0x10));
        }
        ++i;
        entry += 4;
    } while (i < 0x18);

    self->controller->v11(self->controller->fieldE4, *(void**)((char*)model + 0x10));
}
}

namespace jj_800DF038 {
class Target {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11(void*, void*);
    virtual void s12();
    virtual void s13(void*);
};

extern "C" void setUpdateRate__16gfModelAnimationFf(void*, float);
extern "C" void fn_800B753C(void*, float);
extern const float lbl_805A2368;
extern const float lbl_805A2374;

extern "C" void fn_800DF038(void* arg0) {
    float rate = lbl_805A2368;
    int i;
    char* model = (char*)*(void**)((char*)arg0 + 0x64);
    setUpdateRate__16gfModelAnimationFf(*(void**)(model + 0x14), rate);
    fn_800B753C(model, lbl_805A2374);

    void** p = (void**)((char*)arg0 + 0x34);
    for (i = 0xD; i < 0x18; i++, p++) {
        if (p[4] != 0) {
            Target* t = *(Target**)((char*)arg0 + 0x8);
            t->s13(((void**)p[4])[4]);
        }
    }

    Target* t = *(Target**)((char*)arg0 + 0x8);
    t->s11(*(void**)((char*)t + 0xE4), *(void**)(model + 0x10));
}
}

namespace jj_800DF168 {
extern "C" void fn_800B753C(void*, float);
extern "C" void setUpdateRate__16gfModelAnimationFf(void*, float);

extern const float lbl_805A2368;
extern const float lbl_805A2374;

class Target {
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
    virtual void v11(int, void*);
    virtual void v12();
    virtual void v13(void*);
    virtual void v14();
    virtual void v15();

    char pad[0xE4 - 4];
    int f_e4;
};

struct Item {
    char pad[0x10];
    void* data;
};

struct Model {
    char pad[0x10];
    void* f10;
    void* animation;
};

struct Ctx {
    char pad0[8];
    Target* target;
    char pad1[0x68 - 0x0C];
    Model* model;
};

extern "C" void fn_800DF168(Ctx* self) {
    int i;
    Model* m = self->model;
    setUpdateRate__16gfModelAnimationFf(m->animation, lbl_805A2368);
    fn_800B753C(m, lbl_805A2374);

    char* p = (char*)self + 0x34;
    for (i = 13; i < 24; i++, p += 4) {
        Item* it = *(Item**)(p + 0x10);
        if (it != 0) {
            self->target->v13(it->data);
        }
    }

    self->target->v11(self->target->f_e4, m->f10);
}
}

namespace jj_800DED0C {
extern "C" void setUpdateRate__16gfModelAnimationFf(void*, float);
extern "C" void fn_800B753C(void*, float);
extern const float lbl_805A2368;
extern const float lbl_805A2374;

struct V {
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
    virtual void v11(int, void*);
    virtual void v12();
    virtual void v13(void*);
    char pad[0xe0];
    int e4;
};

struct Ent { char p[0x10]; void* v10; };
struct Obj { char p[0x10]; void* v10; void* v14; };

extern "C" void fn_800DED0C(void* s) {
    char* self = (char*)s;
    int i;
    Obj* o = *(Obj**)(self + 0x58);
    setUpdateRate__16gfModelAnimationFf(o->v14, lbl_805A2368);
    fn_800B753C(o, lbl_805A2374);
    Ent** p = (Ent**)(self + 0x34);
    for (i = 13; i < 24; i++, p++) {
        Ent* e = p[4];
        if (e) {
            (*(V**)(self + 8))->v13(e->v10);
        }
    }
    V* b = *(V**)(self + 8);
    b->v11(b->e4, o->v10);
    *(unsigned char*)(self + 0x174) = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_800DD960 {
struct Entry { const char* name; unsigned char begin, end, kind; };
struct Owner { char pad[0x10]; MuObject* objects[1]; };
extern "C" {
extern char lbl_8059E220[6];
void fn_800B20B0(MuObject*, int);
void fn_800B21C4(MuObject*, int);
}
extern "C" void fn_800DD960(Owner* self, Entry* entry, int count, nw4r::g3d::ResFile* res) {
    char buf[0x40];
    int n;
    Owner* slot;
    Entry* p = entry;
    int i;
    int j;
    for (i = 0; i < count; i++, p++) {
        n = 1;
        if (p->begin < p->end)
            n = p->end - p->begin;
        sprintf(buf, lbl_8059E220, p->name);
        for (j = 0; j < n; j++) {
            slot = (Owner*)((char*)self + (p->begin + j) * 4);
            slot->objects[0] = MuObject::create(res, p->name, p->kind, 0, (HeapType)0x28);
            fn_800B20B0(slot->objects[0], 1);
            fn_800B21C4(slot->objects[0], 1);
            slot->objects[0]->changeAnimN(buf);
            *(int*)((char*)slot->objects[0]->m_sceneModel + 0x118) = 8;
        }
    }
}
}

