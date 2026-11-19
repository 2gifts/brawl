#include <sora_adv_stage/gr_adventure_enemy_creator.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

grAdventureEnemyCreator* grAdventureEnemyCreator::create(int mdlIndex, const char* taskName) {
    grAdventureEnemyCreator* ground = new (Heaps::StageInstance) grAdventureEnemyCreator(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureEnemyCreator::~grAdventureEnemyCreator() { }

void grAdventureEnemyCreator::setValid(u32 unk1) {
    grYakumono::setValid(unk1);
    if (unk1 == 0) {
        m_state = 5;
    } else {
        m_state = 0;
    }
}

void grAdventureEnemyCreator::setValidAttachedEffect(u32 unk1) {
    grYakumono::setValidAttachedEffect(unk1);
    if (unk1 == 0) {
        m_state = 5;
    } else {
        m_state = 0;
    }
}

#include <gr/gr_gimmick.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_model.h>
#include <st/st_trigger.h>
// TODO: clean up, give variables real names
namespace jj_36630 {
class stTriggerMng;
class stTriggerData;

struct ObjHead {
    char pad[0x3C];
};

class Obj : public ObjHead {
public:
#define S(n) virtual void s##n();
    S(0)S(1)S(2)S(3)S(4)S(5)S(6)S(7)S(8)S(9)
    S(10)S(11)S(12)S(13)S(14)S(15)S(16)S(17)S(18)S(19)
    S(20)S(21)S(22)S(23)S(24)S(25)S(26)S(27)S(28)S(29)
    S(30)S(31)S(32)S(33)S(34)S(35)S(36)S(37)S(38)S(39)
    S(40)S(41)S(42)S(43)S(44)S(45)S(46)S(47)S(48)S(49)
    S(50)S(51)S(52)S(53)S(54)S(55)S(56)S(57)S(58)S(59)
    virtual void call(int);
    S(61)S(62)S(63)S(64)S(65)S(66)S(67)S(68)S(69)
    S(70)S(71)S(72)S(73)S(74)S(75)S(76)S(77)S(78)S(79)
    S(80)S(81)S(82)S(83)S(84)S(85)S(86)S(87)S(88)S(89)
    S(90)S(91)
    virtual void restart();
};

extern "C" {
extern stTriggerMng* g_stTriggerMng;
bool getTriggerFlag__12stTriggerMngFP13stTriggerDatab(stTriggerMng*, stTriggerData*, bool);
void setFrame__19grGimmickMotionPathFf(void*, float);
void* fn_27_2332C0(stTriggerMng*, void*);

void fn_40_36630(void* self) {
    char* s = (char*)self;
    Obj* o = (Obj*)self;
    if (!getTriggerFlag__12stTriggerMngFP13stTriggerDatab(g_stTriggerMng, (stTriggerData*)(*(char**)(s + 0x160) + 0xA4), true)) {
        if ((*(unsigned char*)(*(char**)(s + 0x160) + 0xA6) & 1U) == 1U) {
            o->call(0);
        }
    } else {
        if (!(*(unsigned char*)(*(char**)(s + 0x160) + 0xA6) & 1)) {
            o->call(0);
        }
    }
    if (getTriggerFlag__12stTriggerMngFP13stTriggerDatab(g_stTriggerMng, (stTriggerData*)(*(char**)(s + 0x160) + 0xA8), false) == 1U) {
        setFrame__19grGimmickMotionPathFf(*(void**)(s + 0xCC), (float)*(unsigned int*)(*(char**)(s + 0xCC) + 0x160));
        ((Obj*)*(void**)(s + 0xCC))->restart();
    }
    *(void**)(s + 0x174) = fn_27_2332C0(g_stTriggerMng, *(char**)(s + 0x160) + 0xA0);
}
}
}

// TODO: clean up, give variables real names
namespace jj_36744 {

struct AreaPosition { char pad[0x28]; float x; float y; };
struct AreaObject { char pad[0x90]; AreaPosition* position; };
struct GimmickData {
    char pad[0x70];
    float x;
    float y;
    char pad78[0x28];
    stTriggerData trigger;
};
struct grYakumono_base { char pad[0x3c]; };
class grYakumono : public grYakumono_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
    virtual void __dt__23grAdventureEnemyCreatorFv();
    virtual void fn_40_36744();
    virtual void loadGroundData();
    virtual void bindData();
    virtual void setupMelee();
    virtual void setVisibility(u32 flag);
    virtual void setVisibilityByClipping();
    virtual void setVisibilityAttachedEffect();
    virtual void receiveCollMsg();
    virtual void receiveCollMsg_Landing();
    virtual void receiveCollMsg_Heading();
    virtual void receiveCollMsg_Wall();
    virtual void receiveCollMsg_Attack();
    virtual void unloadData();
    virtual void getModelCount();
    virtual void fn_40_35BE8();
    virtual void setNode();
    virtual void setStageData();
    virtual void getStageData();
    virtual void initStageData();
    virtual void setMdlIndex();
    virtual void getMdlIndex();
    virtual void isEnableCalcCollision();
    virtual void enableCalcCollision();
    virtual void disableCalcCollision();
    virtual void getNodeIndex();
    virtual void getNodePosition();
    virtual void getNodePosition_2();
    virtual void getNodeMatrix();
    virtual void getNodeMatrix_2();
    virtual void setNodeVisibility();
    virtual void setNodeVisibility_2();
    virtual void setNodeVisibilityAll();
    virtual void isNodeVisible();
    virtual void setNodeCollision();
    virtual void setNodeCollision_2();
    virtual void getNodeScale();
    virtual void getNodeScale_2();
    virtual void setValid__23grAdventureEnemyCreatorFUl();
    virtual void setValidAttachedEffect__23grAdventureEnemyCreatorFUl();
    virtual void fn_40_36630();
    virtual void setInitializeInfo();
    virtual void getInitializeInfo();
    virtual void setMotionRatio();
    virtual void setMotionFrame();
    virtual void getMotionFrame();
    virtual void setMotionLoop();
    virtual void setMatAlphaMul();
    virtual void setMatAlpha();
    virtual void updateG3dProcCalcWorld();
    virtual void preExit();
    virtual void adventureEventGetItem();
    virtual void invalidatedByCameraClipping();
    virtual void setTransparencyFlag();
    virtual void updateCallback();
    virtual void fixedPosition();
    virtual void setTgtNode();
    virtual void getTgtNode();
    virtual void setGimmickData();
    virtual void getGimmickData();
    virtual void changeNodeAnim(u32 a, u32 b);

    char pad40[4];
    nw4r::g3d::ScnMdl** models;
    gfModelAnimation** animations;
    char pad4C[0x104];
    u8 state;
    char pad151[3];
    float timer;
    float hideTimer;
    char pad15C[4];
    GimmickData* data;
    char bounds[0x10];
    AreaObject* area;
};

extern "C" {
u32 isIntersect__16CameraControllerFPC6Rect2D(CameraController*, void*);
int fn_40_139CC(AreaObject*);
void setPosGimmickEffect__9grGimmickFUlP5Vec3f(grYakumono*, int, const Vec3f*);
extern const float lbl_40_rodata_6C0;
}

extern "C" void fn_40_36744(grYakumono* self, float delta) {
    ((grGimmick*)self)->grGimmick::update(delta);
    if (CameraController::getInstance()) {
        AreaPosition* position = self->area->position;
        position->x = self->data->x + ((grGimmick*)self)->getPos().m_x;
        position = self->area->position;
        position->y = self->data->y + ((grGimmick*)self)->getPos().m_y;
        if (isIntersect__16CameraControllerFPC6Rect2D(CameraController::getInstance(), self->bounds) == 1U) {
            if ((int)self->state == 0)
                self->state = 1;
        } else if (self->state == 2) {
            self->state = 3;
        }
        switch (self->state) {
        case 0:
        case 2:
            if (self->timer > lbl_40_rodata_6C0) {
                self->timer -= delta;
                if (self->timer <= lbl_40_rodata_6C0) {
                    self->timer = lbl_40_rodata_6C0;
                    self->changeNodeAnim(0, 0);
                }
            }
            if (fn_40_139CC(self->area)) {
                Vec3f pos = ((grGimmick*)self)->getPos();
                pos.m_x += self->data->x;
                ((grGimmick*)self)->startGimmickSE(0);
                ((grGimmick*)self)->startGimmickEffect(0);
                setPosGimmickEffect__9grGimmickFUlP5Vec3f(self, 0, &pos);
            }
            break;
        case 1:
            self->data->trigger.m_isValid = true;
            g_stTriggerMng->setTriggerFlag(&self->data->trigger);
            self->state = 2;
            break;
        case 3:
            ((u8*)&self->data->trigger)[2] &= 0xFE;
            g_stTriggerMng->setTriggerFlag(&self->data->trigger);
            self->state = 0;
            break;
        case 4:
            if (self->hideTimer > lbl_40_rodata_6C0) {
                self->hideTimer -= delta;
                if (self->hideTimer <= lbl_40_rodata_6C0) {
                    self->hideTimer = lbl_40_rodata_6C0;
                    (*self->animations)->unbindNodeAnim(*self->models);
                    self->setVisibility(0);
                }
            }
            break;
        }
    }
    ((grGimmick*)self)->grGimmick::updateCallback(0);
}
}

// TODO: clean up, give variables real names
namespace jj_36DC0 {
extern "C" {
void* __dt__18ykDamageModuleImplFv(void*, int);
void __dt__8soDamageFv(void*, int);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__24soCollisionHitModuleImplFv(void*, short);
void __dt__19soCollisionHitGroupFv(void*, int);
void __dt__16soCollisionGroupFv(void*, int);
void __dt__18soCollisionHitPartFv(void*, int);
void __dt__8YakumonoFv(void*, int);
void __dl__FPv(void*);
}

extern "C" void* fn_40_36DC0(char* self, int flag) {
    if (self) {
        if (self + 0x500) {
            __dt__18ykDamageModuleImplFv(self + 0x5AC, -1);
            if (self + 0x500 && self + 0x500) {
                __destroy_arr(self + 0x50C, __dt__8soDamageFv, 0xA0, 1);
            }
        }
        if (self + 0x35C) {
            __dt__24soCollisionHitModuleImplFv(self + 0x498, -1);
            if (self + 0x454) {
                __destroy_arr(self + 0x460, __dt__19soCollisionHitGroupFv, 0x38, 1);
            }
            if (self + 0x3D0) {
                __destroy_arr(self + 0x3DC, __dt__16soCollisionGroupFv, 0x78, 1);
            }
            if (self + 0x35C) {
                __destroy_arr(self + 0x368, __dt__18soCollisionHitPartFv, 0x68, 1);
            }
        }
        __dt__8YakumonoFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_371B4 {
extern "C" void fn_40_36DC0(void*);

extern "C" void fn_40_371B4(void* self) {
    fn_40_36DC0(reinterpret_cast<u8*>(self) - 0x40);
}
}

