#include <sora_adv_stage/gr_adventure_fallingrock.h>
#include <types.h>

grAdventureFallingRock* grAdventureFallingRock::create(int mdlIndex, const char* taskName) {
    grAdventureFallingRock* ground = new (Heaps::StageInstance) grAdventureFallingRock(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureFallingRock::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureMoveAttackGround::startup(data, unk1, layerType);
    m_data = m_gimmickData;
}

grAdventureFallingRock::~grAdventureFallingRock() { }

// TODO: clean up, give variables real names
namespace jj_2A214 {
extern "C" {
extern const float lbl_40_rodata_438;
extern const double lbl_40_rodata_440;
extern void* g_GameGlobal;
void setVisibility__6GroundFUl(void*, unsigned long);
void unbindNodeAnim__16gfModelAnimationFPQ34nw4r3g3d6ScnMdl(void*, void*);
void setFrame__19grGimmickMotionPathFf(void*, float);
void startMove__19grGimmickMotionPathFv(void*);
void changeNodeAnim__9grGimmickFUlUl(void*, unsigned long, unsigned long);
void disableAttack__10grYakumonoFUl(void*, unsigned long);
void changeMotionPathRandomSE__21grAdventureMoveGroundFUl(void*, unsigned char);
void setAttackGimmick__10grYakumonoFiiUlPQ29grGimmick10AttackDataPQ29grGimmick13AttackDetails(void*, int, int, unsigned long, void*, void*);
void update__27grAdventureMoveAttackGroundFf(void*, float);
}

struct Meta {
    char pad[0x1c];
    unsigned short frame;
};

struct Owner {
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual float s6(void*);
    char pad[0x28];
    Meta* meta;
};

struct Anim {
    char pad[0xc];
    Owner* owner;
};

struct PathBase {
    char pad[0x3c];
};

struct Path : PathBase {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
    virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
    virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14();
    virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24();
    virtual void v25(); virtual void v26(); virtual void v27(); virtual void v28(); virtual void v29();
    virtual void v30(); virtual void v31(); virtual void v32(); virtual void v33(); virtual void v34();
    virtual void v35(); virtual void v36(); virtual void v37(); virtual void v38(); virtual void v39();
    virtual void v40(); virtual void v41(); virtual void v42(); virtual void v43(); virtual void v44();
    virtual void v45(); virtual void v46(); virtual void v47(); virtual void v48(); virtual void v49();
    virtual void v50(); virtual void v51(); virtual void v52(); virtual void v53(); virtual void v54();
    virtual void v55(); virtual void v56(); virtual void v57(); virtual void v58(); virtual void v59();
    virtual void v60(); virtual void v61(); virtual void v62(); virtual void v63(); virtual void v64();
    virtual void v65(); virtual void v66(); virtual void v67(); virtual void v68(); virtual void v69();
    virtual void v70(); virtual void v71(); virtual void v72(); virtual void v73(); virtual void v74();
    virtual void v75(); virtual void v76(); virtual void v77(); virtual void v78(); virtual void v79();
    virtual void v80(); virtual void v81(); virtual void v82(); virtual void v83(); virtual void v84();
    virtual void v85(); virtual void v86(); virtual void v87(); virtual void v88(); virtual void v89();
    virtual void v90(); virtual void v91(); virtual void v92();
};

struct Base {
    char pad[0x3c];
};

struct Self : Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
    virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
    virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14();
    virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24();
    virtual void v25(); virtual void v26(); virtual void v27(int);

    char pad40[4];
    void** sceneModel;
    Anim** model;
    char pad4c[0x21];
    unsigned char flag6d;
    char pad6e[0x4e];
    void* table;
    char padc0[0xc];
    Path* path;
    char padd0[0x98];
    float timer;
    unsigned char active;
    unsigned char armed;
    char pade[2];
    unsigned char* control;
};

extern "C" void fn_40_2A214(Self* self, float delta) {
    void* table = self->table;

    if ((self->flag6d >> 6) & 1) {
        if (!self->active && (int)*(unsigned char*)((char*)self->path + 0x14c) == 1)
            self->active = 1;

        if (self->active) {
            if (self->timer > lbl_40_rodata_438) {
                self->timer -= delta;

                Owner* owner = (*self->model)->owner;
                unsigned short frame = owner->meta->frame;
                if (owner->s6(owner->meta) >= (float)frame && !self->control[0x284])
                    setVisibility__6GroundFUl(self, 0);

                if (self->timer <= lbl_40_rodata_438) {
                    unbindNodeAnim__16gfModelAnimationFPQ34nw4r3g3d6ScnMdl(
                        *self->model, *self->sceneModel);
                    setFrame__19grGimmickMotionPathFf(self->path, lbl_40_rodata_438);
                    self->path->v92();
                    self->timer = lbl_40_rodata_438;
                    startMove__19grGimmickMotionPathFv(self->path);
                    self->armed = 0;
                }
            } else {
                if ((int)*(unsigned char*)((char*)self->path + 0x14c) == 0) {
                    changeNodeAnim__9grGimmickFUlUl(self, 0, 0);
                    if (!self->control[0x284])
                        disableAttack__10grYakumonoFUl(self, 0);

                    char* game = (char*)g_GameGlobal;
                    char* system = *(char**)(game + 0x30);
                    int duration = *(unsigned char*)(system + 0x5fd);
                    int minimum = duration - 5;
                    int value = duration - *(unsigned char*)(system + 0x4916);
                    if (value < minimum)
                        value = minimum;
                    if (value < 0)
                        value = 0;
                    if (value > 14)
                        value = 14;

                    self->timer =
                        *(float*)((char*)table + 0x248 + ((unsigned char)value * 4));
                }

                if ((int)*(unsigned char*)((char*)self->path + 0x14c) == 1 &&
                    !self->control[0x284] && !self->armed) {
                    changeMotionPathRandomSE__21grAdventureMoveGroundFUl(self, *(unsigned char*)((char*)table + 0x285));
                    setAttackGimmick__10grYakumonoFiiUlPQ29grGimmick10AttackDataPQ29grGimmick13AttackDetails(
                        self, 0, 0, 0, (char*)table + 0x1ec, 0);
                    self->v27(1);
                    self->armed = 1;
                }
            }
        }

        update__27grAdventureMoveAttackGroundFf(self, delta);
    }
}
}

