#include <sora_adv_stage/gr_adventure_move4_ground.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>
#include <st/st_trigger.h>
#include <types.h>

class stActTriggerStepJump : public stTriggerCB {
public:
    u32 unk8;
    char _spacerC[8];
    bool unk14;
    char _spacer15[3];
    float unk18;

    virtual ~stActTriggerStepJump();
    virtual void userProc();
};

class stActTriggerScrollLockOn : public stTriggerCB {
public:
    virtual ~stActTriggerScrollLockOn();
};

class stActTriggerForceScrollOn : public stTriggerCB {
public:
    virtual ~stActTriggerForceScrollOn();
};

class stActTriggerCameraDemo : public stTriggerCB {
public:
    virtual ~stActTriggerCameraDemo();
};

class stActTriggerMotionPathMove4Start : public stTriggerCB {
public:
    u32 unk8;
    grAdventureMove4Ground* unkC;
    u8 unk10;

    virtual ~stActTriggerMotionPathMove4Start();
    virtual void userProc();
};

class stActTriggerMotionPathMove4Stop : public stTriggerCB {
public:
    u32 unk8;
    grAdventureMove4Ground* unkC;
    u8 unk10;

    virtual ~stActTriggerMotionPathMove4Stop();
    virtual void userProc();
};

class stActTriggerCreateItem : public stTriggerCB {
public:
    virtual ~stActTriggerCreateItem();
};

class stActTriggerCreateFixedItem : public stTriggerCB {
public:
    virtual ~stActTriggerCreateFixedItem();
};

class stActTriggerQuake : public stTriggerCB {
public:
    virtual ~stActTriggerQuake();
};

class stActTriggerEffect : public stTriggerCB {
public:
    u32 unk8;
    char _spacerC[4];
    bool unk10;
    char _spacer11[3];
    float unk14;

    virtual ~stActTriggerEffect();
    virtual void userProc();
};

class stActTriggerStockReset : public stTriggerCB {
public:
    virtual ~stActTriggerStockReset();
};

void stActTriggerStepJump::userProc() {
    unk14 = true;
    unk18 = 0.0f;
}

void stActTriggerMotionPathMove4Start::userProc() {
    unkC->changePath(unk10);
}

void stActTriggerMotionPathMove4Stop::userProc() {
    unkC->stopPath(unk10);
}

void stActTriggerEffect::userProc() {
    unk10 = true;
    unk14 = 0.0f;
}

stActTriggerGimmickStatus::stActTriggerGimmickStatus(grAdventureNotifyGimmickTriggerStatus* owner, int index, bool isValid, int flag) :
    unkC(1), m_index(index), m_isValid(isValid), m_flag(flag), m_owner(owner) { }

stActTriggerStockReset::~stActTriggerStockReset() { }

stActTriggerEffect::~stActTriggerEffect() { }

stActTriggerQuake::~stActTriggerQuake() { }

stActTriggerCreateFixedItem::~stActTriggerCreateFixedItem() { }

stActTriggerCreateItem::~stActTriggerCreateItem() { }

stActTriggerMotionPathMove4Stop::~stActTriggerMotionPathMove4Stop() { }

stActTriggerMotionPathMove4Start::~stActTriggerMotionPathMove4Start() { }

stActTriggerCameraDemo::~stActTriggerCameraDemo() { }

stActTriggerForceScrollOn::~stActTriggerForceScrollOn() { }

stActTriggerScrollLockOn::~stActTriggerScrollLockOn() { }

stActTriggerStepJump::~stActTriggerStepJump() { }

#include <ef/ef_screen.h>
#include <ef/ef_screen_handle.h>
#include <gr/gr_gimmick_motion_path.h>
#include <cm/cm_camera_controller.h>
#include <ft/ft_manager.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <sr/sr_common.h>
#include <gf/gf_3d_scene.h>
#include <snd/snd_system.h>
#include <string.h>
#include <so/so_array_vector_calculator.h>
// TODO: clean up, give variables real names
namespace jj_E824 {

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* fn_40_EA7C(void*, int, void*);
void setValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
}

extern int lbl_40_data_2280;

struct Obj {
    int unk0;
    void* unk4;
    int unk8;
    int unkC;
    void* unk10;
    u8 unk14;
    u8 unk15;
    void* unk18;
    void* unk1C;
};

extern "C" Obj* fn_40_E824(Obj* self, int arg1, void* arg2, s8 arg3) {
    u8 kind;
    void* p;

    self->unk0 = 0;
    self->unk8 = 0x15;
    self->unk4 = &lbl_40_data_2280;
    self->unkC = arg1;
    self->unk10 = arg2;
    self->unk14 = 0;
    self->unk15 = arg3;
    self->unk18 = 0;
    kind = ((u8*)arg2)[7];
    if (kind == 0) {
        self->unk18 = g_stTriggerMng->createTrigger((Gimmick::AreaKind)0x15, -1);
    } else if (kind == 1) {
        self->unk18 = g_stTriggerMng->createTrigger((Gimmick::AreaKind)0x15, -1);
    } else {
        self->unk18 = g_stTriggerMng->createTrigger((Gimmick::AreaKind)0x15, (stTriggerData*)((u8*)arg2 + 0x14));
    }
    p = __nw__FUlQ25Heaps8HeapType(0x48, 0xF);
    if (p != 0) {
        p = fn_40_EA7C(p, self->unkC, self->unk10);
    }
    self->unk1C = p;
    setValidEdgeCallback__9stTriggerFP10gfCallBack(self->unk18, p);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_E914 {
class Object_vptr_3c_base { public: char pad[0x3c]; };
class Object_vptr_3c : public Object_vptr_3c_base {
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
    virtual void slot80();
    virtual void slot81();
    virtual void slot82();
    virtual void slot83();
    virtual void slot84();
    virtual void slot85();
    virtual void slot86();
    virtual void slot87();
    virtual void slot88();
    virtual void slot89();
    virtual void slot90();
    virtual void slot91();
    virtual void slot92();
    virtual void slot93();
    virtual void slot94();
    virtual void slot95();
    virtual void slot96();
    virtual void slot97();
    virtual void slot98();
    virtual void slot99();
    virtual void slot100();
    virtual void slot101();
    virtual void slot102();
    virtual void slot103();
    virtual void slot104();
    virtual void slot105();
    virtual void slot106();
    virtual void slot107();
    virtual void slot108();
    virtual void slot109();
    virtual void slot110();
    virtual void slot111();
    virtual void slot112();
    virtual void slot113();
    virtual void slot114();
    virtual void slot115(s32);
};

struct Params {
    f32 f0;
    u8 b4;
    u8 b5;
    u8 b6;
    u8 b7;
};

struct Ctl {
    char pad[0x18];
    u8 b18;
};

struct StageMgr {
    char pad0[0xC];
    Object_vptr_3c* unkC;
    Params* unk10;
    s8 unk14;
    u8 unk15;
    char pad16[2];
    void* unk18;
    Ctl* unk1C;
};

extern "C" {
void* getInstance__9emManagerFv();
void removeEnemyAll__9emManagerFv(void*);
void fn_40_50DC(void* self, u8 id);
void fn_40_51F4(void* self, u8 id);
void fn_40_7CAC(void* self);
void setParamPattern__9ftManagerFi(void* self, s32 val);
void* fn_27_232338(void* self, void* key);
void setObserveFlag__9stTriggerFb(void* self, u8 val);
void fn_27_2323A0(void* self, StageMgr* p);
void setFrameTrigger__9stTriggerFf(void* self, f32 f);
void fn_27_234660(void* self, void* p);
void playSE__9sndSystemF5SndIDiiii(void* snd, s32 id, s32 a, s32 b, s32 c, s32 d);
extern u32 g_emManager;
extern void* g_ftManager;
extern void* g_stTriggerMng;
extern void* g_sndSystem;

void fn_40_E914(StageMgr* self) {
    void* r30;
    self->unk14 = 1;
    if (g_emManager != 0) {
        removeEnemyAll__9emManagerFv(getInstance__9emManagerFv());
    }
    if (self->unk15 == 0) {
        fn_40_50DC(self->unkC, self->unk10->b4);
    } else {
        fn_40_51F4(self->unkC, self->unk10->b4);
    }
    self->unkC->slot115(0);
    if (self->unk10->b6 != 0) {
        setParamPattern__9ftManagerFi(g_ftManager, 0);
    }
    r30 = fn_27_232338(g_stTriggerMng, (char*)self->unk10 + 8);
    if (r30 != 0) {
        setObserveFlag__9stTriggerFb(r30, 1);
    }
    fn_27_2323A0(g_stTriggerMng, self);
    if (self->unk10->b7 == 0) {
        setFrameTrigger__9stTriggerFf(self->unk18, self->unk10->f0);
    } else if (self->unk10->b7 == 1) {
        fn_27_234660(self->unk18, r30);
    }
    if (self->unk10->b5 != 0) {
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x227B, -1, 0, 0, -1);
    }
    self->unk1C->b18 = 0;
    fn_40_7CAC(self->unkC);
}
}
}

// TODO: clean up, give variables real names
namespace jj_EC34 {
typedef unsigned char u8;

struct Base4 {
    unsigned int value;
};

struct CastObject : Base4 {
    virtual void f0();
    virtual ~CastObject();
};

extern "C" {
extern u8 lbl_40_data_11A0[];
extern u8 __RTTI__11stTriggerCB[];
extern u8 __RTTI__10gfCallBack[];

void __dl__FPv(void*);
void* __dynamic_cast(void*, long, const void*, const void*, short);

void* fn_40_EC34(u8* self, int deleting)
{
    if (self) {
        if (self) {
            *reinterpret_cast<void**>(self + 4) = lbl_40_data_11A0;
            if (__dynamic_cast(*reinterpret_cast<void**>(self), 4,
                               __RTTI__11stTriggerCB, __RTTI__10gfCallBack, 0)) {
                delete static_cast<CastObject*>(
                    __dynamic_cast(*reinterpret_cast<void**>(self), 4,
                                   __RTTI__11stTriggerCB, __RTTI__10gfCallBack, 0));
            }
        }

        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_ED00 {
extern "C" {
extern void* g_sndSystem;
extern void* lbl_40_bss_15C;
extern const float lbl_40_rodata_14C;
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
void fn_40_6FEC(void* self);

void fn_40_ED00(void* self) {
    char* s = (char*)self;
    char* p = *(char**)(s + 0x10);
    if ((unsigned char)p[5] != 0) {
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x227c, -1, 0, 0, -1);
        *(unsigned char*)(s + 0x18) = 3;
        *(unsigned char*)(s + 0x19) = 3;
        *(float*)(s + 0x14) = lbl_40_rodata_14C;
        *(unsigned char*)(s + 0x1a) = 0;
    } else {
        *(unsigned char*)(s + 0x18) = 9;
    }
    *(unsigned char*)(s + 0x1b) = 1;
    fn_40_6FEC(lbl_40_bss_15C);
}
}
}

// TODO: clean up, give variables real names
namespace jj_ED98 {

struct Handle {
    u8 value;
};

struct Owner {
    char pad[0x1b8];
    u8 flag;
};

struct Data {
    char pad[5];
    u8 flag;
    char pad2[6];
    char key[1];
};

struct Self {
    char pad[12];
    Owner* owner;
    Data* data;
    float timer;
    u8 state;
    u8 index;
    u8 started;
    u8 pending;
    s8 handle;
    GXColor colors[5];
    char padding[3];
    float times[1];
};

extern "C" {
extern const float lbl_40_rodata_120[];
extern float g_stGameFrame;
extern void* g_ftManager;
extern void* g_stTriggerMng;

void requestFill__8efScreenF14efScreenHandlefP8_GXColor(efScreen*, Handle, GXColor*, float);
void cancelFill__8efScreenFP14efScreenHandlef(efScreen*, Handle, float);
void fn_40_533C(void*);
void fn_40_7CF8(void*);
void setParamPattern__9ftManagerFi(void*, int);
void* fn_27_232338(void*, void*);
void setObserveFlag__9stTriggerFb(void*, int);
}

extern "C" void fn_40_ED98(Self* s) {
    const float* c = lbl_40_rodata_120;

    if (s->data->flag) {
        switch (s->state) {
        case 0:
            if (!s->started && !s->owner->flag) {
                u8 index = s->index;
                float time = s->times[index];
                GXColor color = s->colors[index];
                s->handle = (unsigned int)g_efScreen->requestFill(time, 7, 0, &color) >> 24;
                s->started = 1;
            }
            if (s->timer > c[26]) {
                s->index++;
                s->state++;
                s->timer = c[11];
                s->started = 0;
            }
            break;

        case 1:
        case 8:
            if (!s->started && !s->owner->flag) {
                if (((efScreenHandle*)&s->handle)->isValid()) {
                    u8 index = s->index;
                    float time = s->times[index];
                    Handle h = { s->handle };
                    GXColor color = s->colors[index];
                    requestFill__8efScreenF14efScreenHandlefP8_GXColor(g_efScreen, h, &color, time);
                } else {
                    u8 index = s->index;
                    float time = s->times[index];
                    GXColor color = s->colors[index];
                    s->handle = (unsigned int)g_efScreen->requestFill(time, 7, 0, &color) >> 24;
                }
                s->started = 1;
            }
            if (s->timer > s->times[s->index]) {
                s->index++;
                s->state++;
                s->timer = c[11];
                s->started = 0;
            }
            break;

        case 2:
            if (!s->started && !s->owner->flag) {
                u8 index = s->index;
                float time = s->times[index];
                Handle h = { s->handle };
                GXColor color = s->colors[index];
                requestFill__8efScreenF14efScreenHandlefP8_GXColor(g_efScreen, h, &color, time);
                s->started = 1;
            }
            break;

        case 4:
        case 6:
            if (!s->started && !s->owner->flag) {
                if (((efScreenHandle*)&s->handle)->isValid()) {
                    Handle h = { s->handle };
                    cancelFill__8efScreenFP14efScreenHandlef(g_efScreen, h, c[28]);
                    s->handle = -1;
                }
                s->started = 1;
            }
            if (s->timer > s->times[s->index]) {
                u8 state = s->state + 1;
                s->started = 0;
                s->state = state;
                s->timer = c[11];
            }
            break;

        case 3:
        case 5:
            if (!s->started && !s->owner->flag) {
                if (((efScreenHandle*)&s->handle)->isValid()) {
                    Handle h = { s->handle };
                    GXColor color = s->colors[s->index];
                    requestFill__8efScreenF14efScreenHandlefP8_GXColor(g_efScreen, h, &color, c[28]);
                } else {
                    GXColor color = s->colors[s->index];
                    s->handle = (unsigned int)g_efScreen->requestFill(c[28], 7, 0, &color) >> 24;
                }
                s->started = 1;
            }
            if (s->timer > s->times[s->index]) {
                u8 state = s->state + 1;
                s->started = 0;
                s->state = state;
                s->timer = c[11];
            }
            break;

        case 7: {
            u8 index = s->index + 1;
            s->state++;
            s->index = index;
            s->timer = c[11];
            s->started = 0;
            break;
        }

        case 9: {
            if (!s->owner->flag) {
                Handle h = { s->handle };
                cancelFill__8efScreenFP14efScreenHandlef(g_efScreen, h, c[14]);
                s->handle = -1;
            }
            u8 state = s->state + 1;
            s->started = 0;
            s->state = state;
            s->timer = c[11];
            break;
        }
        }
    }

    if (s->pending) {
        fn_40_533C(s->owner);
        setParamPattern__9ftManagerFi(g_ftManager, 1);
        void* p = fn_27_232338(g_stTriggerMng, s->data->key);
        if (p) {
            setObserveFlag__9stTriggerFb(p, 1);
        }
        fn_40_7CF8(s->owner);
        s->pending = 0;
    }

    float time = s->timer;
    float delta = g_stGameFrame;
    s->timer = time + delta;
}
}

// TODO: clean up, give variables real names
namespace jj_F2C8 {
struct Obj {
    char pad[0x0C];
    void* state;
    char pad2[0x0C];
    unsigned char flag;
    float value;
};

extern const float lbl_40_rodata_14C;

extern "C" void fn_40_F2C8(Obj* p) {
    if (*(int*)((char*)p->state + 0x52C) == 2)
        return;
    if (p->flag != 0)
        return;
    p->flag = 1;
    p->value = lbl_40_rodata_14C;
}
}

// TODO: clean up, give variables real names
namespace jj_F2FC {

struct Stage_base { char pad[0x3c]; };
#define V(name) virtual void name();
class Stage : public Stage_base {
public:
    V(processDefault) V(processBegin) V(fn_40_2024) V(processUpdate)
    V(processPreMapCorrection) V(processMapCorrection) V(processFixPosition)
    V(processPreCollision) V(processCollision) V(processCatch) V(processHit)
    V(processCamera) V(fn_40_21B8) V(processEffect) V(processGameProc)
    V(fn_40_222C) V(renderPre) V(fn_40_8A84) V(renderXlu) V(processDebug)
    V(renderDebug) V(init) V(fn_40_12F8) V(fn_40_D24) V(createObjPokeTrainer)
    V(fn_40_5098) V(fn_40_50A0) V(fn_40_50BC) V(getPokeTrainerDrawLayer__5StageFv) V(fn_40_8B28)
    V(fn_40_7D94) V(fn_40_8154) V(fn_40_80AC) V(fn_40_8160) V(fn_40_8478)
    V(initializeStage) V(closeStage) V(renderDebugPositions) V(fn_40_45C)
    V(process) V(fn_40_7D44) V(debugCollision) V(fn_40_6FF4) V(getDefaultLightSetIndex__5StageFv)
    V(getZoneLightSetIndex) V(getScrollDir__5StageFP5Vec3f) V(clearCameraParam) V(fn_40_6C6C)
    V(fn_40_464) V(fn_40_5EC) V(fn_40_990) V(fn_40_A7C) V(fn_40_B98)
    V(fn_40_5FD8) V(fn_40_6050) V(isReStartSamePoint) V(getPokeTrainerStartPos)
    V(getItemPosCount) V(getItemPos) V(getRandItemPos) V(getKirifudaPos)
    V(getKirifudaAngle) V(getKirifudaScale) V(getKirifudaModelType)
    V(getPokeTrainerPosCount) V(getPokeTrainerPos) V(fn_40_7034) V(fn_40_70B4)
    V(getEnableZ) V(getBgmID__5StageFv) V(fn_40_76F8) V(fn_40_76E0) V(getBgmOptionID__5StageFv)
    V(isBgmChange__5StageFv) V(getBgmChangeID__5StageFPUlPf) V(setBgmChange__5StageFfbUl) V(fn_40_774C) V(fn_40_77DC)
    V(fn_40_783C) V(setFrameRuleTime__5StageFf) V(getFrameRuleTime__5StageFv) V(fn_40_7A94) V(isEnd)
    V(isEventEnd) V(enableDevil) V(disableDevil) V(isDevil) V(setDevilScrool)
    V(getLucarioFinalTechniquePosition) V(startAppear) V(setAppearKind)
};
struct grYakumono_base { char pad[0x3c]; };
class grYakumono : public grYakumono_base {
public:
    V(slot0) V(slot1) V(slot2) V(slot3) V(slot4) V(slot5) V(slot6) V(slot7)
    V(slot8) V(slot9) V(slot10) V(slot11) V(slot12) V(slot13) V(slot14)
    V(slot15) V(slot16) V(slot17) V(slot18) V(slot19) V(slot20) V(slot21)
    V(slot22) V(slot23) V(slot24) V(slot25) V(slot26) V(slot27) V(slot28)
    V(slot29) V(slot30) V(slot31) V(slot32) V(slot33) V(slot34) V(slot35)
    V(slot36) V(slot37) V(slot38) V(slot39) V(slot40) V(slot41) V(slot42)
    V(slot43) V(slot44) V(slot45) V(slot46) V(slot47) V(slot48) V(slot49)
    V(slot50) V(slot51) V(slot52) V(slot53)
    V(setNodeVisibilityAll) V(isNodeVisible) V(setNodeCollision)
    V(setNodeCollision_2) V(getNodeScale) V(getNodeScale_2)
    V(slot60) V(slot61) V(slot62) V(slot63) V(slot64)
    V(setMotionRatio) V(setMotionFrame) V(getMotionFrame) V(setMotionLoop)
    V(setMatAlphaMul) V(setMatAlpha) V(updateG3dProcCalcWorld)
    V(slot72) V(slot73) V(slot74)
    V(setTransparencyFlag) V(updateCallback) V(fixedPosition)
    V(setTgtNode) V(getTgtNode) V(setGimmickData) V(getGimmickData)
    V(changeNodeAnim) V(createFadeVisibleProduction)
    V(createSoundEffectVisibleProductionForExcel)
    V(createEffectVisibleProductionForExcel)
    V(slot86) V(slot87) V(slot88) V(getTransparencyFlag) V(slot90)
};
struct grGimmickWarpZone_base { char pad[0x3c]; };
class grGimmickWarpZone : public grGimmickWarpZone_base {
public:
    V(processDefault) V(processBegin) V(processAnim) V(processUpdate)
    V(processPreMapCorrection) V(processMapCorrection) V(processFixPosition__19grAdventureWarpZoneFv)
    V(processPreCollision) V(processCollision) V(processCatch) V(processHit)
    V(processCamera) V(processFixCamera) V(processEffect) V(processGameProc)
    V(processEnd) V(renderPre) V(renderOpa) V(renderXlu) V(processDebug)
    V(renderDebug) V(init) V(__dt__19grAdventureWarpZoneFv) V(update) V(loadGroundData)
    V(bindData) V(setupMelee) V(setVisibility) V(setVisibilityByClipping)
    V(setVisibilityAttachedEffect) V(receiveCollMsg) V(receiveCollMsg_Landing)
    V(receiveCollMsg_Heading) V(receiveCollMsg_Wall) V(receiveCollMsg_Attack)
    V(unloadData) V(getModelCount) V(startup) V(setNode) V(setStageData)
    V(getStageData) V(initStageData) V(setMdlIndex) V(getMdlIndex)
    V(isEnableCalcCollision) V(enableCalcCollision) V(disableCalcCollision)
    V(getNodeIndex) V(getNodePosition) V(getNodePosition_2)
    V(getNodeMatrix) V(getNodeMatrix_2) V(setNodeVisibility)
    V(setNodeVisibility_2) V(setNodeVisibilityAll) V(isNodeVisible)
    V(setNodeCollision) V(setNodeCollision_2) V(getNodeScale) V(getNodeScale_2)
    V(setValid) V(setValidAttachedEffect) V(setInitializeFlag)
    V(setInitializeInfo) V(getInitializeInfo) V(setMotionRatio)
    V(setMotionFrame) V(getMotionFrame) V(setMotionLoop)
    V(setMatAlphaMul) V(setMatAlpha) V(updateG3dProcCalcWorld)
    V(preExit) V(adventureEventGetItem) V(invalidatedByCameraClipping)
    V(setTransparencyFlag) V(updateCallback) V(fixedPosition)
    V(setTgtNode) V(getTgtNode) V(setGimmickData) V(getGimmickData)
    V(changeNodeAnim) V(createFadeVisibleProduction)
    V(createSoundEffectVisibleProductionForExcel)
    V(createEffectVisibleProductionForExcel)
    V(setSimpleEffectVisibleProduction)
    V(dbDispInvalidatedByCameraClippingSphere) V(setTransparency)
    V(getTransparencyFlag) V(onDamage)
};
#undef V

extern float g_stGameFrame;
extern "C" void fn_40_54F0(void*, int);
extern "C" void fn_40_4FE0(void*, Vec3f*);
extern "C" void fn_40_55BC(void*, Vec3f*);
struct Params {
    char pad[0x10];
    float delay;
};
struct Self {
    char pad[0x0c];
    void* object;
    grGimmickMotionPath* path;
    grGimmickMotionPath* otherPath;
    Params* params;
    unsigned char state;
    float frame;
};
extern "C" void fn_40_F2FC(Self* self) {
    Vec3f pos;
    if (self->state == 1) {
        float delta = g_stGameFrame;
        self->frame += delta;
        if (self->frame > self->params->delay) {
            self->state = 2;
            fn_40_54F0(self->object, 0);
            self->path->startMove();
            if (self->otherPath) {
                self->otherPath->startMove();
                pos = self->otherPath->getPos();
                fn_40_4FE0(self->object, &pos);
            }
        }
    }
    if (self->state == 2) {
        pos = self->path->getPos();
        fn_40_55BC(self->object, &pos);
        if (self->otherPath) {
            pos = self->otherPath->getPos();
            fn_40_4FE0(self->object, &pos);
        }
        if (self->path->isMoveEnd()) {
            self->state = 3;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_F448 {
extern "C" {
extern void* g_stTriggerMng;
void setTriggerFlag__12stTriggerMngFP13stTriggerData(void*, void*);
void fn_40_533C(void*);
void fn_27_2323A0(void*, void*);
}

extern "C" void fn_40_F448(char* self) {
    if (*(unsigned char*)(self + 0x1c) != 3)
        return;

    setTriggerFlag__12stTriggerMngFP13stTriggerData(g_stTriggerMng,
                                                   *(char**)(self + 0x18) + 8);
    fn_40_533C(*(void**)(self + 0x0c));
    fn_27_2323A0(g_stTriggerMng, self);
    *(unsigned char*)(self + 0x1c) = 4;
}
}

// TODO: clean up, give variables real names
namespace jj_F4B4 {
struct State {
    char pad[0x52C];
    int step;
};

struct Obj {
    char pad[0x0C];
    State* state;
    char pad2[0x0C];
    unsigned char flag;
    char pad3[3];
    float value;
};

extern const float lbl_40_rodata_14C;

extern "C" void fn_40_F4B4(Obj* p) {
    if (p->state->step == 2)
        return;
    if (p->flag != 0)
        return;
    p->flag = 1;
    p->value = lbl_40_rodata_14C;
}
}

// TODO: clean up, give variables real names
namespace jj_F4E8 {

extern float g_stGameFrame;
extern char* g_Stage;
extern "C" void fn_40_54F0(void*, int);
extern "C" void fn_40_5628(void*, Vec3f*, Vec3f*);

struct FighterFlags {
    char pad[0x6c];
    unsigned char flag : 1;
};

struct Self {
    char pad[0xc];
    void* object;
    grGimmickMotionPath* path;
    grGimmickMotionPath* otherPath;
    void* params;
    unsigned char state;
    float frame;
};

extern "C" void fn_40_F4E8(Self* self) {
    if (self->state == 1) {
        float delta = g_stGameFrame;
        float frame = self->frame + delta;
        self->state = 2;
        self->frame = frame;
        CameraController::getInstance()->changeInput(CameraController::Adventure);
        fn_40_54F0(self->object, 1);
        self->path->startMove();
        self->otherPath->startMove();
    }
    if (self->state == 2) {
        Vec3f pos2;
        Vec3f pos;
        pos = self->path->getPos();
        pos2 = self->otherPath->getPos();
        fn_40_5628(self->object, &pos, &pos2);
        if (self->otherPath->isMoveEnd() || self->path->isMoveEnd()) {
            self->state = 3;
        }
        g_Stage[0xe9] = 0;
        ((FighterFlags*)g_ftManager)->flag = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_F628 {
extern "C" {
extern void* g_stTriggerMng;
extern void* g_ftManager;
extern char* g_Stage;
void setTriggerFlag__12stTriggerMngFP13stTriggerData(void*, void*);
void fn_40_50DC(void*, int);
void fn_27_2323A0(void*, void*);
void setFighterOperationStatusAll__9ftManagerFi(void*, int);
}

extern "C" void fn_40_F628(char* self) {
    if (*(unsigned char*)(self + 0x1c) != 3) return;
    
    setTriggerFlag__12stTriggerMngFP13stTriggerData(g_stTriggerMng, (char*)*(char**)(self + 0x18) + 0x10);
    fn_40_50DC(*(void**)(self + 0x0c), 0xff);
    fn_27_2323A0(g_stTriggerMng, self);
    
    *(unsigned char*)(g_Stage + 0xe9) = 1;
    *(unsigned char*)(g_Stage + 0x19e) = 1;
    
    setFighterOperationStatusAll__9ftManagerFi(g_ftManager, 1);
    
    *(unsigned char*)((char*)g_ftManager + 0x6c) |= 0x80;
}
}

// TODO: clean up, give variables real names
namespace jj_F6E8 {
extern "C" {
struct TriggerMng;
extern TriggerMng* g_stTriggerMng;
extern unsigned char lbl_40_data_2070[];
void fn_27_2322E4(TriggerMng*, void*);

void* fn_40_F6E8(void* self, void* item) {
    unsigned int* p = (unsigned int*)self;
    p[0] = 0;
    p[2] = 60;
    p[1] = (unsigned int)lbl_40_data_2070;
    p[3] = 0;
    p[4] = (unsigned int)item;
    unsigned char x = ((unsigned char*)item)[0x14];
    p[5] = (unsigned int)(x != 0);
    fn_27_2322E4(g_stTriggerMng, self);
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_F75C {
class stTriggerData;
class stTriggerMng {
public:
    bool getTriggerFlag(stTriggerData* data, bool b);
};

extern "C" {
    extern stTriggerMng* g_stTriggerMng;
    bool getTriggerFlag__12stTriggerMngFP13stTriggerDatab(stTriggerMng*, stTriggerData*, bool);

    void fn_40_F75C(void* self) {
        char* s = (char*)self;
        if (getTriggerFlag__12stTriggerMngFP13stTriggerDatab(
                g_stTriggerMng,
                (stTriggerData*)((char*)(*(void**)(s + 0x10)) + 0xc),
                false) == 1) {
            *(int*)(s + 0xc) = 3;
        } else {
            if (*(int*)(s + 0xc) == 0) {
                *(int*)(s + 0xc) = 1;
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_F7C4 {
extern "C" void* getInstance__9itManagerFv();
extern "C" void* getSafeItemFromIndex__9itManagerFUl(void* mgr, int idx);
extern "C" void fn_40_6240(void* self, int a, void* pos);

extern void* lbl_40_bss_15C;
extern const float lbl_40_rodata_14C;

struct Counter {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual int getCount();
};

struct Vec3f {
    float x;
    float y;
    float z;
};

struct Pos {
    float x;
    float y;
    int id;
};

struct Work {
    char pad0[0xC];
    /* 0x0C */ int state;
    /* 0x10 */ Pos* pos;
    /* 0x14 */ int unk14;
};

extern "C" void fn_40_F7C4(Work* work) {
    u32 count;
    int found = 0;
    int i;
    if ((u32)(work->state - 1) <= 1u) {
        count = ((Counter*)((char*)getInstance__9itManagerFv() + 0x6D8))->getCount();
        for (i = 0; i < count; i++) {
            void* item = getSafeItemFromIndex__9itManagerFUl(getInstance__9itManagerFv(), i);
            if (item != NULL) {
                if (*(u32*)&work->pos->id == *(u32*)((char*)item + 0x3D28)) {
                    found = 1;
                    break;
                }
            }
        }
    }
    if (work->state == 1 && found == 0) {
        float y = work->pos->y;
        Vec3f pos = {(work->pos->x), y, lbl_40_rodata_14C};
        fn_40_6240(lbl_40_bss_15C, work->pos->id, &pos);
        if (work->unk14 == 0) {
            work->state = 2;
        } else {
            work->state = 0;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_F8E0 {
extern "C" {
extern void* g_stTriggerMng;
void setTriggerFlag__12stTriggerMngFP13stTriggerData(void*, void*);
}

extern "C" int fn_40_F8E0(char* self, unsigned int id, unsigned char* out) {
    char* p = *(char**)(self + 0x10);
    if (id == *(unsigned int*)(p + 0x8)) {
        setTriggerFlag__12stTriggerMngFP13stTriggerData(g_stTriggerMng, p + 0xc);
        char* q = *(char**)(self + 0x10);
        *(unsigned short*)out = *(unsigned short*)(q + 0xc);
        out[2] = q[0xe];
        out[3] = q[0xf];
        return 1;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_F95C {
struct Src {
    int a;
    int b;
};

struct Obj {
    int m0;
    void* m4;
    int m8;
    int mC;
    int m10;
    int m14;
};

extern "C" char lbl_40_data_2018[];

extern "C" void fn_40_F95C(Obj* self, const Src* src, int x) {
    self->m0 = 0;
    self->m8 = 60;
    self->m4 = lbl_40_data_2018;
    self->mC = x;
    self->m10 = src->a;
    self->m14 = src->b;
}
}

// TODO: clean up, give variables real names
namespace jj_FAC4 {

extern "C" {
extern unsigned char lbl_40_data_1FC0[];
extern const float lbl_40_rodata_14C;
void __ct__16stScnObjCallBackFv(void*);
void initialize__16stScnObjCallBackFPQ34nw4r3g3d9ResAnmScn(void*, void*);
void GetResAnmScnNumEntries__Q34nw4r3g3d7ResFileCFv(nw4r::g3d::ResFile*);
int HasResAnmScn__Q34nw4r3g3d7ResFileCFv(nw4r::g3d::ResFile*);
void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void*, void*);
void fn_40_FC34();
void fn_40_FC38();
void* __construct_new_array(void*, void (*)(), void*, unsigned long, int);
}

struct Object {
    void* field0;
    void* vtable;
    int field8;
    void* scene;
    int field10;
    void* array14;
    void* array18;
    void* array1C;
    void* array20;
    unsigned char field24;
    char pad25[3];
    float field28;
    unsigned char field2C;
    char pad2D[3];
    int field30;
    int count34;
    int count38;
    int count3C;
    int count40;
};

extern "C" Object* fn_40_FAC4(Object* self, void* data, int arg2, signed char arg3) {
    self->field0 = 0;
    self->field8 = 0;
    self->vtable = lbl_40_data_1FC0;
    self->scene = 0;
    self->field10 = arg2;
    self->field28 = lbl_40_rodata_14C;
    self->field2C = 0;
    __ct__16stScnObjCallBackFv(&self->field30);
    nw4r::g3d::ResFile file(data);
    GetResAnmScnNumEntries__Q34nw4r3g3d7ResFileCFv(&file);
    if (file.IsValid() && HasResAnmScn__Q34nw4r3g3d7ResFileCFv(&file) != 0) {
        void* scene = file.GetResAnmScn(0).ptr();
        self->scene = scene;
        void* temp = scene;
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&self->scene, &temp);
    }
    initialize__16stScnObjCallBackFPQ34nw4r3g3d9ResAnmScn(&self->field30, &self->scene);
    self->array14 = operator new[](self->count34 * 0x14, Heaps::StageInstance);
    int count = self->count38;
    self->array18 = __construct_new_array(
        operator new[](count * 0x2C + 0x10, Heaps::StageInstance),
        fn_40_FC34, 0, 0x2C, count);
    count = self->count3C;
    self->array1C = __construct_new_array(
        operator new[](count * 0x20 + 0x10, Heaps::StageInstance),
        fn_40_FC38, 0, 0x20, count);
    self->array20 = operator new[](self->count40 * 0x2C, Heaps::StageInstance);
    void* scene = *(void**)((char*)g_gfSceneRoot + 0x40);
    *(int*)((char*)scene + 0xD4) = 0;
    self->field24 = arg3;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_FC34 {
extern "C" void fn_40_FC34(void) {}
}

// TODO: clean up, give variables real names
namespace jj_FC38 {
extern "C" void fn_40_FC38(void) {}
}

// TODO: clean up, give variables real names
namespace jj_FC3C {
typedef unsigned char u8;

struct Base4 {
    unsigned int value;
};

struct CastTarget : Base4 {
    virtual void f0();
    virtual void f3(int);
};

extern "C" {
extern u8 lbl_40_data_1FC0[];
extern u8 lbl_40_data_11A0[];
extern u8 __RTTI__11stTriggerCB[];
extern u8 __RTTI__10gfCallBack[];
extern void* g_gfSceneRoot;

void __dla__FPv(void*);
void __dl__FPv(void*);
void* __dynamic_cast(void*, long, const void*, const void*, short);
void __dt__16stScnObjCallBackFv(void*, int, void*);

void* fn_40_FC3C(u8* self, int deleting)
{
    if (self) {
        void* p14 = *reinterpret_cast<void**>(self + 0x14);
        *reinterpret_cast<void**>(self + 4) = lbl_40_data_1FC0;
        if (p14) {
            __dla__FPv(p14);
            *reinterpret_cast<void**>(self + 0x14) = 0;
        }

        void* p18 = *reinterpret_cast<void**>(self + 0x18);
        if (p18) {
            if (p18) {
                __dla__FPv((u8*)p18 - 0x10);
            }
            *reinterpret_cast<void**>(self + 0x18) = 0;
        }

        void* p1c = *reinterpret_cast<void**>(self + 0x1c);
        if (p1c) {
            if (p1c) {
                __dla__FPv((u8*)p1c - 0x10);
            }
            *reinterpret_cast<void**>(self + 0x1c) = 0;
        }

        void* p20 = *reinterpret_cast<void**>(self + 0x20);
        if (p20) {
            __dla__FPv(p20);
            *reinterpret_cast<void**>(self + 0x20) = 0;
        }

        void* scene = *reinterpret_cast<void**>((u8*)g_gfSceneRoot + 0x40);
        *reinterpret_cast<void**>((u8*)scene + 0xd4) = 0;
        __dt__16stScnObjCallBackFv(self + 0x30, -1, scene);

        if (self) {
            *reinterpret_cast<void**>(self + 4) = lbl_40_data_11A0;
            if (__dynamic_cast(*reinterpret_cast<void**>(self), 4,
                               __RTTI__11stTriggerCB, __RTTI__10gfCallBack, 0)) {
                CastTarget* target = static_cast<CastTarget*>(
                    __dynamic_cast(*reinterpret_cast<void**>(self), 4,
                                   __RTTI__11stTriggerCB, __RTTI__10gfCallBack, 0));
                if (target) {
                    target->f3(1);
                }
            }
        }

        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_FDAC {

typedef unsigned char u8;
struct Color { u8 r,g,b,a; };
struct Floats { float r,g,b,a; };
struct Anim { int value; float x,y; Color color; };
struct Entry { int flags, id, value; float dx,dy; char pad[8]; float dr,dg,db,da; };
struct Settings { int unk; float duration; int sound1,sound2; };
struct Item { char pad[16]; int id; };
struct Interface {
    virtual void a(); virtual void b(); virtual void c(); virtual void d();
    virtual void e(); virtual void f(); virtual void update();
    u32 count1,count2,count3,count4;
    void *p1,*p2,*p3;
    Entry *entries;
};
struct Object {
    char pad[12]; void *resource; Settings *settings;
    void *array1,*array2,*array3; Entry *array4;
    u8 mode; char pad25[3]; float time; u8 initialized; char pad2d[3];
    Interface iface;
};
struct Scene { char pad[0xd4]; Interface *iface; };
struct Root { char pad[0x40]; Scene *scene; };
extern Root *g_gfSceneRoot;
extern const float lbl_40_rodata_14C;
extern "C" {
unsigned int GetResAnmFogNumEntries__Q34nw4r3g3d9ResAnmScnCFv(void*);
Item *GetResAnmFog__Q34nw4r3g3d9ResAnmScnCFUl(void*, unsigned int);
void GetAnmResult__Q34nw4r3g3d9ResAnmFogCFPQ34nw4r3g3d12FogAnmResultf(Item**, Anim*, float);
char *GetFog__Q34nw4r3g3d7ScnRootFi(Scene*, int);
void GetFog__Q34nw4r3g3d3FogFP10_GXFogTypePfPfPfPfP8_GXColor(char**, int*, float*, float*, float*, float*, Color*);
void setFogType__16stScnObjCallBackFii(Interface*, unsigned int, int);
void setFogStartEnd__16stScnObjCallBackFiff(Interface*, unsigned int, float, float);
void setFogColor__16stScnObjCallBackFiPC10stFogColor(Interface*, unsigned int, Floats*);
}
extern "C" void fn_40_FDAC(Object *self) {
    if (!self->initialized) {
        self->time = lbl_40_rodata_14C;
        memset(self->array1, 0, self->iface.count1 * 0x14);
        memset(self->array2, 0, self->iface.count2 * 0x2c);
        memset(self->array3, 0, self->iface.count3 * 0x20);
        memset(self->array4, 0, self->iface.count4 * 0x2c);
        for (u32 i = 0; i < self->iface.count4; ++i) {
            if (i < GetResAnmFogNumEntries__Q34nw4r3g3d9ResAnmScnCFv(&self->resource)) {
                int value;
                float x,y,z,w;
                Color color;
                Item *item = 0;
                item = GetResAnmFog__Q34nw4r3g3d9ResAnmScnCFUl(&self->resource, i);
                Anim anim;
                *(int*)&anim.color = -1;
                GetAnmResult__Q34nw4r3g3d9ResAnmFogCFPQ34nw4r3g3d12FogAnmResultf(&item, &anim, lbl_40_rodata_14C);
                int id = item->id;
                self->array4[i].id = id;
                char *pane = GetFog__Q34nw4r3g3d7ScnRootFi(g_gfSceneRoot->scene, id);
                GetFog__Q34nw4r3g3d3FogFP10_GXFogTypePfPfPfPfP8_GXColor(&pane, &value, &x, &y, &z, &w, &color);
                self->iface.entries[i].id = id;
                setFogType__16stScnObjCallBackFii(&self->iface, i, anim.value);
                setFogStartEnd__16stScnObjCallBackFiff(&self->iface, i, x, y);
                Floats rgba = { (float)(int)color.r, (float)(int)color.g, (float)(int)color.b, (float)(int)color.a };
                setFogColor__16stScnObjCallBackFiPC10stFogColor(&self->iface, i, &rgba);
                self->array4[i].value = anim.value;
                self->array4[i].dx = (anim.x - x) / self->settings->duration;
                self->array4[i].dy = (anim.y - y) / self->settings->duration;
                self->array4[i].dr = (float)(anim.color.r - color.r) / self->settings->duration;
                self->array4[i].dg = (float)(anim.color.g - color.g) / self->settings->duration;
                self->array4[i].db = (float)(anim.color.b - color.b) / self->settings->duration;
                self->array4[i].da = (float)(anim.color.a - color.a) / self->settings->duration;
            }
        }
        g_gfSceneRoot->scene->iface = &self->iface;
        self->iface.update();
        if (self->mode == 1)
            g_sndSystem->playSE((SndID)self->settings->sound1, -1, 0, 0, -1);
        else
            g_sndSystem->playSE((SndID)self->settings->sound2, -1, 0, 0, -1);
        self->initialized = 1;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_10160 {
extern "C" void addFog__16stScnObjCallBackFPC13stFogParamSet(void* a, void* b);
extern float g_stGameFrame;

struct Sub {
    char p0[4];
    float f4;
};

struct Obj {
    char pad0[0x10];
    Sub* sub10;
    char pad14[0xC];
    void* ptr20;
    char pad24[4];
    float f28;
    unsigned char state2c;
    char pad2d[3];
    char pad30[4];
};

extern "C" {
void fn_40_10160(Obj* self) {
    if (self->state2c == 1) {
        addFog__16stScnObjCallBackFPC13stFogParamSet(self->pad30, self->ptr20);
        float a = self->f28;
        float b = g_stGameFrame;
        self->f28 = a + b;
        if (self->f28 >= self->sub10->f4) {
            self->state2c = 0;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_101D0 {
typedef int s32;
typedef unsigned char u8;

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Sub {
    u8 pad[0x14];
    s32 sndId;
};

struct Obj {
    u8 pad[0xc];
    Sub* sub;
};

extern "C" {
void* fn_8009D2DC();
void reqQuake__7cmQuakeFQ27cmQuake9AmplitudeP5Vec3f(void*, unsigned long, Vec3*);
void playSE__9sndSystemF5SndIDiiii(void*, s32, s32, s32, s32, s32);
extern void* g_sndSystem;

void fn_40_101D0(void* self) {
    Sub* sub = ((Obj*)self)->sub;
    u8 state = sub->pad[0xc];
    switch (state) {
    case 0:
        reqQuake__7cmQuakeFQ27cmQuake9AmplitudeP5Vec3f(fn_8009D2DC(), 3, (Vec3*)sub);
        break;
    case 1:
        reqQuake__7cmQuakeFQ27cmQuake9AmplitudeP5Vec3f(fn_8009D2DC(), 4, (Vec3*)sub);
        break;
    case 2:
        reqQuake__7cmQuakeFQ27cmQuake9AmplitudeP5Vec3f(fn_8009D2DC(), 5, (Vec3*)sub);
        break;
    default:
        break;
    }
    playSE__9sndSystemF5SndIDiiii(g_sndSystem, ((Obj*)self)->sub->sndId, 0, 0, 0, -1);
}
}
}

// TODO: clean up, give variables real names
namespace jj_10A6C {
typedef unsigned char u8;
typedef unsigned int u32;

struct Src {
    u32 val;
};

class Obj {
public:
    virtual void v0();
    virtual void v5(const Src* s);
    virtual void v6(const Src* s);
    u32 m_val;
};

struct Self {
    char pad0[0xc];
    u8 b0c;
    char pad1[3];
    int w10;
    u8 b14;
    char pad2[3];
    const Src* p18;
    Obj* p1c;
};

extern "C" {
extern u8 lbl_40_data_1E08[];
void setGimmickStatus__37grAdventureNotifyGimmickTriggerStatusFPCi(Obj* self, const Src* s);

void fn_40_10A6C(Self* self) {
    u8* base = lbl_40_data_1E08;
    if (self->b0c) {
        if (self->w10 == 1) {
            if (self->b14) {
                setGimmickStatus__37grAdventureNotifyGimmickTriggerStatusFPCi(self->p1c, (const Src*)(base + 0x30));
                self->p1c->v5(self->p18);
            } else {
                setGimmickStatus__37grAdventureNotifyGimmickTriggerStatusFPCi(self->p1c, (const Src*)(base + 0x34));
                self->p1c->v6(self->p18);
            }
        } else if (self->w10 == 0) {
            if (self->b14) {
                setGimmickStatus__37grAdventureNotifyGimmickTriggerStatusFPCi(self->p1c, (const Src*)(base + 0x38));
                self->p1c->v6(self->p18);
            } else {
                setGimmickStatus__37grAdventureNotifyGimmickTriggerStatusFPCi(self->p1c, (const Src*)(base + 0x3c));
                self->p1c->v5(self->p18);
            }
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_10B70 {
typedef unsigned char u8;
extern "C" void __dl__FPv(void*);
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" u8 lbl_40_data_11A0[];
extern "C" u8 __RTTI__11stTriggerCB[];
extern "C" u8 __RTTI__10gfCallBack[];

struct Pad {
    unsigned int unk0;
};

struct IFace : Pad {
    virtual void f0();
    virtual void f1(int);
    virtual void f2();
};

extern "C" void* __dt__25stActTriggerGimmickStatusFv(u8* self, int deleting)
{
    if (self) {
        if (self) {
            *reinterpret_cast<void**>(self + 4) = lbl_40_data_11A0;
            void* p = __dynamic_cast(*reinterpret_cast<void**>(self), 4, __RTTI__11stTriggerCB, __RTTI__10gfCallBack, 0);
            if (p) {
                void* q = __dynamic_cast(*reinterpret_cast<void**>(self), 4, __RTTI__11stTriggerCB, __RTTI__10gfCallBack, 0);
                if (q) {
                    reinterpret_cast<IFace*>(q)->f1(1);
                }
            }
        }
        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_11500 {
extern "C" void __dl__FPv(void* p);

extern "C" void* __dt__26soArrayVectorCalcInterfaceFv(void* p, int size) {
    if (p != 0) {
        if (size > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_11540 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_40_11540(void* ptr, int count) {
    if (ptr != 0 && count > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_11580 {
struct Data {
    unsigned int padding[2];
    unsigned int value;
};

extern "C" int fn_40_11580(const Data* data)
{
    return static_cast<int>((data->value << 10) & 0xFC000000u) >> 27;
}
}

// TODO: clean up, give variables real names
namespace jj_11590 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_40_11590(void* ptr, int size) {
    if (ptr != 0 && size > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_115D0 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_40_115D0(void* ptr, int size) {
    if (ptr && size > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_11610 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_40_11610(void* ptr, int size) {
    if (ptr != 0 && size > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_11650 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_40_11650(void* ptr, int size) {
    if (ptr != 0 && size > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_11690 {
extern "C" int fn_40_11690(const int *arg)
{
    return arg[2] >> 27;
}
}

// TODO: clean up, give variables real names
namespace jj_1169C {
// Placeholder type: the real class is not identified. Only the word at offset 0x8 is known from the assembly.
struct AdvStageFlagObject {
    u32 m_0x0;
    u32 m_0x4;
    u32 m_flags : 5;      // top 5 bits of the word at 0x8 (CodeWarrior allocates bitfields from the MSB)
    u32 m_0x8_rest : 27;  // remaining bits of the word at 0x8
};

// Writes the low 5 bits of `flags` into bits 31..27 of the word at offset 0x8.
// Note: sora_adv_stage_function_12258.cpp declares fn_40_1169C(void*) with one parameter,
// which conflicts with the second argument (r4) that this function reads.
extern "C" void fn_40_1169C(AdvStageFlagObject* self, u32 flags) {
    self->m_flags = flags;
}
}

// TODO: clean up, give variables real names
namespace jj_116AC {
extern "C" int fn_40_116AC(const unsigned int* arg0)
{
    unsigned int v = arg0[2];
    return (int)((v << 5) & 0xFC000000u) >> 27;
}
}

// TODO: clean up, give variables real names
namespace jj_116BC {
struct S {
    char pad[8];
    unsigned int a : 5;
    unsigned int b : 5;
    unsigned int c : 22;
};

extern "C" void fn_40_116BC(S* s, int v) {
    s->b = v;
}
}

// TODO: clean up, give variables real names
namespace jj_116CC {
extern "C" char* fn_40_116CC(char* base, int idx) {
    return base + idx * 4 + 0xc;
}
}

// TODO: clean up, give variables real names
namespace jj_116DC {
struct StageData {
    unsigned int padding[2];
    unsigned int flags;
};

extern "C" void fn_40_116DC(StageData* self) {
    self->flags |= 0x00010000u;
}
}

// TODO: clean up, give variables real names
namespace jj_116EC {
extern "C" void fn_40_116EC(unsigned int* object)
{
    object[2] &= ~0x00010000u;
}
}

// TODO: clean up, give variables real names
namespace jj_116FC {
extern "C" int fn_40_116FC(const unsigned int *p)
{
    return (p[2] >> 16) & 1;
}
}

// TODO: clean up, give variables real names
namespace jj_11708 {
extern "C" s32 fn_40_11708(void) {
    return 0xa;
}
}

// TODO: clean up, give variables real names
namespace jj_11710 {
struct Obj {
    int m_pad[2];
    int m_val;
    int m_arr[10];
};

extern "C" {
int* fn_40_11710(Obj* self, int idx) {
    int sh = self->m_val >> 27;
    int t = sh + idx;
    if (t >= 10) {
        t = t - 10;
    }
    return &self->m_arr[t];
}
}
}

// TODO: clean up, give variables real names
namespace jj_11738 {
struct S {
    int a;
    int b;
    unsigned pad : 10;
    unsigned f : 5;
    unsigned rest : 17;
};

extern "C" void fn_40_11738(S* s, unsigned v) {
    s->f = v;
}
}

// TODO: clean up, give variables real names
namespace jj_11748 {
struct A {
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
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
};

extern "C" void fn_40_11748(A* a) {
    a->v24();
}
}

// TODO: clean up, give variables real names
namespace jj_11758 {
struct A {
    virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
    virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
    virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11();
    virtual void f12(); virtual void f13(); virtual void f14(); virtual void f15();
    virtual void f16(); virtual void f17(); virtual void f18(); virtual void f19();
    virtual void f20(); virtual void f21(); virtual void f22(); virtual void f23();
    virtual void f24();
};

extern "C" void fn_40_11758(A* a) {
    a->f24();
}
}

// TODO: clean up, give variables real names
namespace jj_11768 {
struct Item {
    int value;
};

struct soArrayVectorCalcInterface {};

extern "C" int unshift__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfaceblll(
    soArrayVectorCalcInterface&, bool, long, long, long);

class Array {
public:
    virtual void v08();
    virtual void v0C();
    virtual void v10();
    virtual int size();
    virtual void v18();
    virtual void v1C();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2C();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual int v3C();
    virtual bool v40();
    virtual void v44();
    virtual void v48();
    virtual void v4C();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5C();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6C();
    virtual Item* v70(int);
    virtual int v74();
    virtual int v78();
    virtual void v7C(int);

    soArrayVectorCalcInterface calc;
};

extern "C" void fn_40_11768(Array* self, const Item* value) {
    *self->v70(unshift__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfaceblll(
        self->calc, self->v40(), self->v3C(), self->v74(), self->v78())) = *value;
    self->v7C(self->size() + 1);
}
}

// TODO: clean up, give variables real names
namespace jj_1185C {
typedef signed int s32;

class soArrayVectorCalcInterface;

extern "C" void shift__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacebll(
    soArrayVectorCalcInterface* a, s32 b, s32 c, s32 d);

class MyClass {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual s32 f3();
    virtual s32 f4();
    virtual s32 f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual s32 f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual void f21();
    virtual void f22();
    virtual void f23();
    virtual void f24();
    virtual void f25();
    virtual void f26();
    virtual s32 f27();
    virtual void f28();
    virtual void f29(s32);
};

extern "C" void fn_40_1185C(MyClass* self) {
    s32 b;
    s32 a = self->f27();
    b = self->f13();
    s32 c = self->f4();
    shift__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacebll(
        (soArrayVectorCalcInterface*)((char*)self + 4), c, b, a);
    s32 sz = self->f3();
    self->f29(sz - 1);
}
}

// TODO: clean up, give variables real names
namespace jj_11918 {
struct Item {
    int value;
};

struct Obj {
    virtual void v8();
    virtual void vc();
    virtual void v10();
    virtual int size();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual int v3c();
    virtual bool v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5c();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6c();
    virtual Item* get(int);
    virtual int v74();
    virtual int v78();
    virtual void v7c(int);

    int calc;
};

extern "C" int push__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfaceblll(
    void*, bool, int, int, int);

extern "C" void fn_40_11918(Obj* self, const Item* src) {
    int index = push__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfaceblll(
        &self->calc, self->v40(), self->v3c(), self->v74(), self->v78());
    *self->get(index) = *src;
    self->v7c(self->size() + 1);
}
}

// TODO: clean up, give variables real names
namespace jj_11A0C {
class soArrayVectorCalcInterface;

class soArrayVectorCalculator {
public:
    void pop(soArrayVectorCalcInterface& iface, bool b, long l1, long l2);
};

class Outer {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual long f3();
    virtual soArrayVectorCalcInterface* f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual bool f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual void f21();
    virtual void f22();
    virtual void f23();
    virtual void f24();
    virtual void f25();
    virtual void f26();
    virtual void f27();
    virtual long f28();
    virtual void f29(long n);
    soArrayVectorCalculator calc;
};

extern "C" void fn_40_11A0C(Outer* self) {
    bool r31;
    long r30;
    r30 = self->f28();
    r31 = self->f13();
    soArrayVectorCalcInterface* p = self->f4();
    long junk;
    self->calc.pop(*p, r31, r30, junk);
    self->f29(self->f3() - 1);
}
}

// TODO: clean up, give variables real names
namespace jj_11AC8 {

class Obj {
public:
    virtual void v08();
    virtual void v0C();
    virtual void v10();
    virtual s32 size();
    virtual void v18();
    virtual void v1C();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2C();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual s32 v3C();
    virtual bool v40();
    virtual void v44();
    virtual void v48();
    virtual void v4C();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5C();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6C();
    virtual s32* get(s32);
    virtual s32 v74();
    virtual s32 v78();
    virtual void v7C(s32);

    s32 calc;
};

extern "C" void fn_40_11AC8(Obj* self, s32 index, const s32* src) {
    s32 pos = soArrayVectorCalculator::insert(
        *reinterpret_cast<soArrayVectorCalcInterface*>(&self->calc), index, self->v40(), self->size(),
        self->v3C(), self->v74(), self->v78());
    s32* dst = self->get(pos);
    *dst = *src;
    self->v7C(self->size() + 1);
}
}

// TODO: clean up, give variables real names
namespace jj_11BE0 {

class Array {
public:
    virtual void v08();
    virtual void v0C();
    virtual void v10();
    virtual int size();
    virtual void v18();
    virtual void v1C();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2C();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual int v3C();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4C();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5C();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6C();
    virtual void v70(int);
    virtual int v74();
    virtual int v78();
    virtual void v7C(int);
    int calc;
};

extern "C" void fn_40_11BE0(Array* self, int index) {
    soArrayVectorCalculator::erase(
        *reinterpret_cast<soArrayVectorCalcInterface*>(&self->calc),
        index, self->size(), self->v3C(), self->v74(), self->v78());
    self->v7C(self->size() - 1);
}
}

// TODO: clean up, give variables real names
namespace jj_11CB8 {
// Classes this function calls through, from the game's vtables. Keep every method in this order so
// the vtable offsets come out the same; declare parameters and return types as the code needs.

// vtable at 0x5648: 16 slots; the object holds the vtable pointer at offset 0x0; shown up to slot 3
class Container {
public:
    virtual void reserved0();   // offset 0x8
    virtual int* get(int);      // offset 0xc
    virtual void reserved1();   // offset 0x10
    virtual int size();         // offset 0x14
};

extern "C" void fn_40_11CB8(Container* self, int start, const int* value,
                            unsigned int countArg)
{
    int count = static_cast<int>(countArg);

    if (count + start >= self->size())
        count = self->size() - start;

    for (int i = 0; i < count; ++i) {
        *self->get(start + i) = *value;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_11D5C {
extern "C" void clear__23soArrayVectorCalculatorFR26soArrayVectorCalcInterface(void* calc);

struct Obj {
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
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v31(int);
};

extern "C" void fn_40_11D5C(Obj* self) {
    clear__23soArrayVectorCalculatorFR26soArrayVectorCalcInterface((char*)self + 4);
    self->v31(0);
}
}

// TODO: clean up, give variables real names
namespace jj_11DA4 {
extern "C" s32 fn_40_11DA4(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_11DAC {
struct VA {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
    virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
    virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14();
    virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24();
    virtual void v25();
    virtual void* getA();
};

struct VB {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
    virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
    virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14();
    virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24();
    virtual void v25();
    virtual void* getB(void* arg);
};

extern "C" void fn_40_11DAC(void* self, void* unused, void* arg) {
    void* p = ((VA*)self)->getA();
    void* q = ((VB*)self)->getB(arg);
    *(unsigned int*)q = *(unsigned int*)p;
}
}

// TODO: clean up, give variables real names
namespace jj_11E1C {
struct SomeObj {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual int f3();
};

extern "C" int fn_40_11E1C(SomeObj* obj) {
    return !obj->f3();
}
}

// TODO: clean up, give variables real names
namespace jj_12240 {
extern "C" void fn_40_11590(void*);

extern "C" void fn_40_12240(void* self) {
    fn_40_11590(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_12248 {
extern "C" void fn_40_11540(void*);

extern "C" void fn_40_12248(void* self) {
    fn_40_11540(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_12250 {
extern "C" void fn_40_116BC(void*);

extern "C" void fn_40_12250(void* self) {
    fn_40_116BC(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_12258 {
extern "C" void fn_40_1169C(void*);

extern "C" void fn_40_12258(void* self) {
    fn_40_1169C(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_12260 {
extern "C" void fn_40_116EC(void*);

extern "C" void fn_40_12260(void* self) {
    fn_40_116EC(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_12268 {
extern "C" void fn_40_116DC(void*);

extern "C" void fn_40_12268(void* self) {
    fn_40_116DC(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_12270 {
extern "C" void fn_40_11DAC(void*);

extern "C" void fn_40_12270(void* self) {
    fn_40_11DAC(reinterpret_cast<u8*>(self) - 0x4);
}
}

