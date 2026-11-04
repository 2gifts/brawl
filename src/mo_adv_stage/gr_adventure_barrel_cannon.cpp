#include <gr/gr_gimmick_motion_path.h>
#include <math.h>
#include <sora_adv_stage/gr_adventure_barrel_cannon.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

grAdventureBarrelCannon::grAdventureBarrelCannon(const char* taskName) : grYakumono(taskName) {
    unk15C = 0;
    unk160 = 0;
    unk16C = 0;
    unk16D = 0;
    unk16E = 0;
    unk170 = 0.0f;
    unk1A4 = 2;
    unk1A8 = 0.0f;
    unk1AC = 60;
    unk1B0 = 60;
    unk1E0 = 0;
    unk1E4 = 0;
    unk174 = 0;
    unk175 = 0;
    unk178 = -1;
    unk17C = 0.0f;
    unk180 = 0;
    unk181 = 0;
    unk184 = -1;
    unk188 = 0.0f;
    unk18C = 0;
    unk18D = 0;
    unk190 = -1;
    unk194 = 0.0f;
    unk198 = 0;
    unk199 = 0;
    unk19C = -1;
    unk1A0 = 0.0f;
}

grAdventureBarrelCannon::~grAdventureBarrelCannon() { }

grAdventureBarrelCannon* grAdventureBarrelCannon::create(int mdlIndex, u8 kind, const char* taskName) {
    grAdventureBarrelCannon* ground = new (Heaps::StageInstance) grAdventureBarrelCannon(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->unk164 = kind;
    return ground;
}

void grAdventureBarrelCannon::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&unk150Data->unkE0, true)) {
        if (unk150Data->unkE0.m_isValid) {
            setValid(0);
        }
    } else {
        if (!unk150Data->unkE0.m_isValid) {
            setValid(0);
        }
    }
    if (g_stTriggerMng->getTriggerFlag(&unk150Data->unkDC, false) == true) {
        m_gimmickMotionPath->setFrame(m_gimmickMotionPath->m_frameCount);
        m_gimmickMotionPath->setFrameModelAnim();
    }
}

void grAdventureBarrelCannon::updateMove(float deltaFrame) {
    if (unk16C) {
        Vec3f rot;
        rot = getRot();
        rot.m_z += unk168;
        setRot(&rot);
        if (unk150Data->unkCA == 0) {
            if ((float)fabs(rot.m_z - unk150Data->unk3C) < unk1B4Float ||
                (float)fabs(rot.m_z - unk150Data->unk38) < unk1B4Float) {
                unk168 = -unk168;
            }
        }
        if (rot.m_z > 360.0f) {
            rot.m_z -= 360.0f;
            setRot(&rot);
        }
        if (rot.m_z < 0.0f) {
            rot.m_z += 360.0f;
            setRot(&rot);
        }
    }
}

void grAdventureBarrelCannon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch ((s8)unk1A4) {
    case 0:
        m_modelAnims[0]->m_anmObjChrRes->SetFrame(unk1A8);
        unk1A8 += deltaFrame;
        if (unk1A8 >= unk1AC) {
            unk1A4 = 2;
            unk1A8 = 0.0f;
        }
        break;
    case 1:
        m_modelAnims[0]->m_anmObjChrRes->SetFrame(unk1A8);
        unk1A8 += deltaFrame;
        if (unk1A8 >= unk1B0) {
            unk1A4 = 2;
            unk1A8 = 0.0f;
            m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
        }
        break;
    case 2:
        break;
    }
    updateMove(deltaFrame);
    if (unk16E) {
        unk170 -= deltaFrame;
        if (unk170 <= 0.0f) {
            enableArea();
            unk16E = 0;
            unk170 = 0.0f;
        }
    }
    grGimmick::updateCallback(0);
}
