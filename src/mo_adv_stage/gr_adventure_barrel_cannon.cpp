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

#include <gf/gf_model.h>
// TODO: clean up, give variables real names
namespace jj_1A7B8 {

extern "C" const char lbl_40_data_46B0[];
extern "C" const char lbl_40_data_46C0[];

extern "C" grGimmickMotionPath*
create__19grGimmickMotionPathFiPCcPCc(short, const char*, const char*);

struct VBase {
    char pad[0x3c];
};

class MotionObject : public VBase {
public:
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
    virtual void setGimmickData(void*);
};

extern "C" void fn_40_1A7B8(char* self) {
    unsigned char state = *(unsigned char*)(self + 0x164);
    if (state == 0 || state == 2) {
        return;
    }

    *(grGimmickMotionPath**)(self + 0x15c) =
        create__19grGimmickMotionPathFiPCcPCc(
            (short)*(unsigned char*)(*(char**)(self + 0x158) + 0x146),
            lbl_40_data_46B0, lbl_40_data_46C0);

    (*(MotionObject**)(self + 0x15c))
        ->setGimmickData(*(char**)(self + 0x158) + 0x140);

    char* path = *(char**)(self + 0x15c);
    *(char**)(path + 0x1c) = self;

    char* head = *(char**)(self + 0x20);
    if (head == 0) {
        *(char**)(self + 0x20) = path;
        return;
    }

    char* cur = head;
    while (*(char**)(cur + 0x24) != 0) {
        cur = *(char**)(cur + 0x24);
    }
    *(char**)(cur + 0x24) = path;
}
}

// TODO: clean up, give variables real names
namespace jj_1B804 {
extern "C" void stopCameraAdvCameraOffset__12stAdventure2Fv(void*);
extern "C" void* lbl_40_bss_15C;
extern "C" const float lbl_40_rodata_280;
struct Entry { unsigned char active; char pad[3]; int id; float value; };
struct Work { char pad0[0x44]; nw4r::g3d::ScnMdl** model; gfModelAnimation** anim; char pad4c[0x121]; unsigned char flag; char pad16e[6]; Entry entries[4]; unsigned char state; char pad1a5[3]; float value; };
extern "C" void fn_40_1B804(Work* self, int id) {
 for (int i = 0; i < 4; ++i) {
  if (id == self->entries[i].id) self->entries[i].active = 0;
 }
 if (self->flag) { stopCameraAdvCameraOffset__12stAdventure2Fv(lbl_40_bss_15C); self->flag = 0; }
 int active = 0;
 for (int i = 0; i < 4; ++i) {
  if (self->entries[i].active == 1) active = 1;
 }
 if (!active) {
  gfModelAnimation** anim = self->anim;
  if ((*anim)->m_anmObjChrRes) {
   self->state = 2;
   self->value = lbl_40_rodata_280;
   (*anim)->unbindNodeAnim(*self->model);
  }
 }
}
}

