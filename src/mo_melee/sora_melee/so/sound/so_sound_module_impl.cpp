#define SO_SOUND_3D_GENERATOR_ACCESSER_FULL
#include <mt/mt_vector.h>
#include <snd/snd_system.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_log.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/sound/so_sound_util.h>
#include <types.h>

// HYPOTHESIS: parameter lists of the unnamed main.dol sndSystem stopGroupId, setVol, setPitch, setSeqSpeed and getVol
// (sora map); the stage units still call them through their own stand-ins.

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soSoundStatusDataView {
    char _0[0xc];
    u32 : 27;
    u32 m_keepStatusSE : 1;
};

// HYPOTHESIS: one SE per damage sound attribute (rows) and sound level (columns).
static const SndID s_damageSEs[29][4] = {
    {(SndID)-1, (SndID)-1, (SndID)-1, (SndID)-1},
    {(SndID)0x3B, (SndID)0x3C, (SndID)0x3D, (SndID)-1},
    {(SndID)0x3E, (SndID)0x3F, (SndID)0x40, (SndID)-1},
    {(SndID)0x41, (SndID)0x42, (SndID)0x43, (SndID)-1},
    {(SndID)0xE8, (SndID)0xE8, (SndID)0xE8, (SndID)0xE8},
    {(SndID)0x1F84, (SndID)0x1F83, (SndID)0x1F82, (SndID)-1},
    {(SndID)0x1F8C, (SndID)0x1F8B, (SndID)0x1F89, (SndID)-1},
    {(SndID)0x79, (SndID)0x7A, (SndID)0x7B, (SndID)0x7C},
    {(SndID)0x4B, (SndID)0x4C, (SndID)0x4D, (SndID)-1},
    {(SndID)0xAA, (SndID)0xAB, (SndID)0xAC, (SndID)0xAD},
    {(SndID)-1, (SndID)-1, (SndID)-1, (SndID)-1},
    {(SndID)0xC6, (SndID)0xC7, (SndID)0xC8, (SndID)0xC9},
    {(SndID)-1, (SndID)-1, (SndID)-1, (SndID)-1},
    {(SndID)0x190A, (SndID)0x1909, (SndID)0x1907, (SndID)0x1908},
    {(SndID)0x16E8, (SndID)0x16E7, (SndID)0x16E5, (SndID)0x16E5},
    {(SndID)0x3B5, (SndID)0x3B4, (SndID)0x3B2, (SndID)0x3B3},
    {(SndID)0x75, (SndID)0x76, (SndID)0x77, (SndID)0x78},
    {(SndID)0x1FA6, (SndID)0x1FA6, (SndID)0x1FA6, (SndID)0x1FA6},
    {(SndID)0xF1B, (SndID)0xF1B, (SndID)0xF1B, (SndID)0xF1B},
    {(SndID)0xF1C, (SndID)0xF1C, (SndID)0xF1C, (SndID)0xF1C},
    {(SndID)0xF1D, (SndID)0xF1D, (SndID)0xF1D, (SndID)0xF1D},
    {(SndID)0xF1E, (SndID)0xF1E, (SndID)0xF1E, (SndID)0xF1E},
    {(SndID)0x168D, (SndID)0x168C, (SndID)0x168A, (SndID)0x168B},
    {(SndID)0x112A, (SndID)0x112A, (SndID)0x112A, (SndID)0x112A},
    {(SndID)0x136, (SndID)0x136, (SndID)0x136, (SndID)0x136},
    {(SndID)0x120, (SndID)0x120, (SndID)0x120, (SndID)0x120},
    {(SndID)0xD79, (SndID)0xD79, (SndID)0xD79, (SndID)0xD79},
    {(SndID)0xE75, (SndID)0xE75, (SndID)0xE75, (SndID)0xE75},
    {(SndID)0xCC, (SndID)0xCD, (SndID)0xCE, (SndID)0xCF},
};

soSoundModule::~soSoundModule() { }

soSoundModuleImpl::~soSoundModuleImpl() { }

void soSoundModuleImpl::activate(Vec3f* pos) {
    m_soundGeneratorAccesser->activate(pos);
}

void soSoundModuleImpl::deactivate() {
    g_sndSystem->stopGroup(m_seGroupId, 0);
    g_sndSystem->stopGroup(m_statusSEGroupId, 0);
    m_sePitch = 1.0f;
    m_soundGeneratorAccesser->deactivate();
    m_checkSoundGroup = -1;
}

void soSoundModuleImpl::update(Vec3f* pos) {
    if (!m_generatorStatus) {
        setPos(pos, 0);
    }
}

void soSoundModuleImpl::setPos(Vec3f* pos, int index) {
    soSoundUtil::setPos(m_soundGeneratorAccesser, pos, index);
}

int soSoundModuleImpl::playSE(SndID sndId, bool setPitch, bool noGroup, int index) {
    SndID id = m_soundIdExchanger->getId(sndId, m_moduleAccesser);
    if ((u32)id == 0xFFFFFFFF) {
        return -1;
    }
    int handle;
    if (noGroup == true) {
        handle = soSoundUtil::playSE(m_soundGeneratorAccesser, index, id, -1, m_checkSoundGroup);
    } else {
        handle = soSoundUtil::playSE(m_soundGeneratorAccesser, index, id, m_seGroupId, m_checkSoundGroup);
    }
    if (m_sePitch != 1.0f && setPitch == true && handle >= 0) {
        g_sndSystem->setPitch(handle, m_sePitch);
    }
    return handle;
}

int soSoundModuleImpl::playSENo3d(SndID sndId, bool noGroup) {
    SndID id = m_soundIdExchanger->getId(sndId, m_moduleAccesser);
    if ((u32)id == 0xFFFFFFFF) {
        return -1;
    }
    if (noGroup == true) {
        return g_sndSystem->playSE(id, -1, 0, 0, m_checkSoundGroup);
    }
    return g_sndSystem->playSE(id, m_seGroupId, 0, 0, m_checkSoundGroup);
}

int soSoundModuleImpl::playSEPos(SndID sndId, Vec3f* pos, bool setPitch, bool noGroup, int index) {
    setPos(pos, index);
    return playSE(sndId, setPitch, noGroup, index);
}

int soSoundModuleImpl::playStatusSE(SndID sndId, bool setPitch, int index) {
    SndID id = m_soundIdExchanger->getId(sndId, m_moduleAccesser);
    if ((u32)id == 0xFFFFFFFF) {
        return -1;
    }
    int handle = soSoundUtil::playSE(m_soundGeneratorAccesser, index, id, m_statusSEGroupId, m_checkSoundGroup);
    if (m_sePitch != 1.0f && setPitch == true && handle >= 0) {
        g_sndSystem->setPitch(handle, m_sePitch);
    }
    return handle;
}

void soSoundModuleImpl::setSEVol(int handle, float volume, int unk) {
    g_sndSystem->setVol(handle, volume, unk);
}

float soSoundModuleImpl::getSEVol(int handle) {
    return g_sndSystem->getVol(handle);
}

void soSoundModuleImpl::setSEPitch(u32 handle, double pitch) {
    g_sndSystem->setPitch(handle, pitch);
}

void soSoundModuleImpl::stopStatusSE() {
    g_sndSystem->stopGroup(m_statusSEGroupId, 0);
}

void soSoundModuleImpl::stopSE(int unk, int index) {
    soSoundUtil::stopSE(m_soundGeneratorAccesser, index, unk, m_seGroupId);
}

void soSoundModuleImpl::stopSEHandle(int handle, int unk) {
    g_sndSystem->stopSE(static_cast<s32>(handle), unk);
}

void soSoundModuleImpl::playDamageSE(u32 index1, u32 index2) {
    SndID id = s_damageSEs[index1][index2];
    if ((u32)id != 0xFFFFFFFF) {
        playSE(id, false, false, 0);
    }
}

void soSoundModuleImpl::setSESpeed(int handle, float speed) {
    g_sndSystem->setSeqSpeed(handle, speed);
}

bool soSoundModuleImpl::isPlay(int handle) {
    return g_sndSystem->isPlay(handle);
}

bool soSoundModuleImpl::isObserv(char unk1) {
    return unk1 == 10;
}

void soSoundModuleImpl::setPlayHitSEFlag(bool flag) {
    m_playHitSEFlag = flag;
}

void soSoundModuleImpl::setFixGeneratorStatus() {
    m_generatorStatus = 1;
}

void soSoundModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    m_generatorStatus = 0;
    if (!reinterpret_cast<soSoundStatusDataView*>(statusData)->m_keepStatusSE) {
        stopStatusSE();
    }
    m_sndID = (SndID)-1;
}

void soSoundModuleImpl::playHitSE(float power, void* data) {
    soCollisionAttackData* attackData = static_cast<soCollisionAttackData*>(data);
    if (attackData->m_soundAttribute != 0) {
        if (power > 0.0f) {
            playDamageSE(attackData->m_soundAttribute, attackData->m_soundLevel);
        } else {
            playSE((SndID)0xF, false, false, 0);
        }
    }
}

void soSoundModuleImpl::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk, soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    if (getPlayHitSEFlag() && attackData->m_soundAttribute != 0) {
        if (collisionLog->_41 == true) {
            playSE((SndID)0xBD, false, false, 0);
        } else {
            playHitSE(power, attackData);
        }
    }
}

bool soSoundModuleImpl::getPlayHitSEFlag() {
    return m_playHitSEFlag;
}

void soSoundModuleImpl::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    if (kind == Situation_Ground) {
        playLandingSE();
    }
}

void soSoundModuleImpl::playLandingSE() {
    if ((u32)m_sndID != 0xFFFFFFFF) {
        playSE(m_sndID, false, false, 0);
    }
}

int soSoundModuleImpl::getCheckSoundGroup() {
    return m_checkSoundGroup;
}

void soSoundModuleImpl::setCheckSoundGroup(int group) {
    m_checkSoundGroup = group;
}

soSoundModule::SoundIdData* soSoundModuleImpl::getSoundIdData() {
    return m_soundIdData;
}

void soSoundModuleImpl::setSoundIdData(SoundIdData* data) {
    m_soundIdData = data;
}

float soSoundModuleImpl::getSEPitch() {
    return m_sePitch;
}

void soSoundModuleImpl::setSEPitch(float pitch) {
    m_sePitch = pitch;
}

#include <so/event/so_event_system.h>
#include <so/so_instance_manager.h>
// TODO: clean up, give variables real names
namespace jj_56534 {
extern "C" char lbl_27_data_5B30[];
extern "C" void* getInstance__13soEventSystemFv();
extern "C" void* getManager__13soEventSystemFl(void*, long);
extern "C" void __dl__FPv(void* p);

struct SoEventSystem {
    virtual void fn0();
    virtual void fn1();
    virtual void fn2();
    virtual void fn3();
    virtual void fn4();
    virtual void fn5();
    virtual u32 fn6(s16);
};

struct EventManager {
    virtual void fn0();
    virtual void fn1();
    virtual void fn2();
    virtual void fn3(s16, s16);
};

struct Obj {
    void* vtbl;
    s16 m4;
    s16 m6;
    s16 m8;
};

extern "C" void* fn_27_56534(void* p, s32 flag) {
    if (p) {
        if (p) {
            s16 id = ((Obj*)p)->m4;
            *(void**)p = (void*)lbl_27_data_5B30;
            if (((SoEventSystem*)getInstance__13soEventSystemFv())->fn6(id) == 1) {
                ((EventManager*)getManager__13soEventSystemFl(
                    getInstance__13soEventSystemFv(), ((Obj*)p)->m4))
                    ->fn3(((Obj*)p)->m6, ((Obj*)p)->m8);
            }
            ((Obj*)p)->m8 = -1;
            ((Obj*)p)->m4 = -1;
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_56BA0 {
struct SoundSystem;

extern "C" SoundSystem *g_sndSystem;
extern "C" void setPitch__9sndSystemFlf(SoundSystem *);

extern "C" void fn_27_56BA0()
{
    setPitch__9sndSystemFlf(g_sndSystem);
}
}

// TODO: clean up, give variables real names
namespace jj_56C5C {
// Matches the original except 3 lines in the case 0/12 block: the temporary
// holding the second argument's value is allocated to r0 instead of r5.

struct ArgVal {
    s32 type;
    s32 value;
};

struct Arg {
    ArgVal* p;
    u8 null;
};

struct Ent {
    u8 pad0[0x0d];
    u8 f0d;
    u8 pad1[0x14 - 0x0e];
    s32 f14;
    u8 pad2[0x25 - 0x18];
    u8 f25;
    u8 pad3[0x2c - 0x26];
    s32 f2c;
    u8 pad4[0x3d - 0x30];
    u8 f3d;
    u8 pad5[0x44 - 0x3e];
    s32 f44;
};

extern u8* g_Stage;
extern "C" Ent* getCollisionAttrParamSet__20stCollisionAttrParamFi(u8* p, s32 n);
extern "C" int getValueInt__15soValueAccesserFP16soModuleAccesserUlUl(void* acc, u32 id, u32 arg);

class CmdA {
public:
    virtual void slot0();
    virtual s32 slot1();
    virtual s8 slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual u32 slot6(Arg* out, s32 which);
};

struct Pad8 {
    s32 pad_a;
    s32 pad_b;
};

class D : public Pad8 {
public:
    virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3();
    virtual void m4(); virtual void m5(); virtual void m6(); virtual void m7();
    virtual void m8(); virtual void m9(); virtual void m10(); virtual void m11();
    virtual void m12(); virtual void m13(); virtual void m14(); virtual void m15();
    virtual void m16(); virtual void m17(); virtual void m18(); virtual void m19();
    virtual void m20(); virtual void m21(); virtual void m22(); virtual void m23();
    virtual void m24(); virtual void m25(); virtual void m26(); virtual void m27();
    virtual void m28(); virtual void m29(); virtual void m30(); virtual void m31();
    virtual void m32(); virtual void m33(); virtual void m34(); virtual void m35();
    virtual void m36(); virtual void m37(); virtual void m38(); virtual void m39();
    virtual void m40(); virtual void m41(); virtual void m42(); virtual void m43();
    virtual void m44(); virtual void m45(); virtual void m46(); virtual void m47();
    virtual void m48(); virtual void m49(); virtual void m50(); virtual void m51();
    virtual void m52(); virtual void m53(); virtual void m54();
    virtual u32 m55(s32 a, s32 b);
    virtual void m56(); virtual void m57(); virtual void m58(); virtual void m59();
    virtual void m60(); virtual void m61();
    virtual s32 m62(s32 a, s32 b);
};

class E {
public:
    virtual void e0(); virtual void e1(); virtual void e2(); virtual void e3();
    virtual void e4();
    virtual void e5(s32 a, s32 b);
};

struct Mods {
    u8 pad0[0x10];
    D* f10;
    u8 pad1[0x64 - 0x14];
    E* f64;
};

struct Acc {
    u8 pad0[0xd8];
    Mods* mods;
};

class SubC {
public:
    virtual void g0();
    virtual void g1();
    virtual void g2();
    virtual s32 g3(s32 a, Acc* acc);
    virtual s32 g4(s32 a, Acc* acc);
};

class SelfB {
public:
    virtual void h08(); virtual void h0C(); virtual void h10(); virtual void h14();
    virtual void h18();
    virtual s32 h1C(s32 a, s32 b, s32 c, s32 d);
    virtual s32 h20(s32 a, s32 b);
    virtual void h24();
    virtual void h28(s32 a, s32 b, s32 c);
    virtual void h2C();
    virtual void h30(s32 a);
    virtual void h34(); virtual void h38(); virtual void h3C(); virtual void h40();
    virtual void h44(); virtual void h48(); virtual void h4C(); virtual void h50();
    virtual void h54(s32 a, s32 b);
    virtual void h58(s32 a, s32 b);
    virtual void h5C();
    virtual void h60();
    virtual void h64(); virtual void h68();
    virtual void h6C();
    virtual void h70(); virtual void h74(); virtual void h78(); virtual void h7C();
    virtual void h80(); virtual void h84(); virtual void h88(); virtual void h8C();
    virtual void h90(); virtual void h94(); virtual void h98(); virtual void h9C();
    virtual void hA0(); virtual void hA4(); virtual void hA8(); virtual void hAC();
    virtual void hB0(); virtual void hB4(); virtual void hB8(); virtual void hBC();
    virtual void hC0(); virtual void hC4();
    virtual s32 hC8(s32 a);
    u8 pad_4[0x40];
    SubC* m44;
    s32 m48;
};

extern "C" int fn_27_56C5C(SelfB* self, CmdA* cmd, Acc* acc) {
    u8 nul;
    s32 k = cmd->slot1();
    if (self->hC8((s8)k) == 0) {
        return 0;
    }
    if (cmd->slot2() <= -1 || cmd->slot2() >= 14) {
        return 0;
    }
    switch (cmd->slot2()) {
    case 0:
    case 12: {
        s32 r5;
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        s32 r28 = (a.null == 1) ? 0 : a.p->value;
        if (cmd->slot2() == 0) {
            r28 = self->h1C(r28, 1, 0, 0);
        } else {
            r28 = self->h20(r28, 1);
        }
        if (cmd->slot6(&a, 1) == 1) {
            nul = a.null;
            if (nul == 1) {
                r5 = 0;
            } else {
                r5 = a.p->value;
            }
            if (r5 >= 0) {
                acc->mods->f64->e5(r28, r5);
            }
        }
        return 1;
    }
    case 2: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        s32 v = (a.null == 1) ? 0 : a.p->value;
        self->h28(v, 1, 0);
        return 1;
    }
    case 3: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        s32 v = (a.null == 1) ? 0 : a.p->value;
        self->h54(v, 0);
        return 1;
    }
    case 4: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        s32 v = (a.null == 1) ? 0 : (a.p->value != 0);
        self->h30(v);
        return 1;
    }
    case 5: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        s32 v = (a.null == 1) ? 0 : a.p->value;
        self->h1C(v, 1, 1, 0);
        return 1;
    }
    case 6:
        self->h2C();
        return 1;
    case 7: {
        s32 v = 0;
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        if (a.null == 1) {
            v = 0;
        } else {
            v = a.p->value;
        }
        self->m48 = v;
        return 1;
    }
    case 8:
        self->h60();
        return 1;
    case 1: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        u8 flag = 1;
        if (acc->mods->f10->m55(8, 0) == 1) {
            s32 id = acc->mods->f10->m62(8, 0);
            if (id != 0) {
                Ent* e = getCollisionAttrParamSet__20stCollisionAttrParamFi(g_Stage + 0xF8, id);
                self->h1C(e->f14, 1, 0, 0);
                flag = e->f0d;
            }
        }
        if (flag == 1) {
            s32 v = (a.null == 1) ? 0 : a.p->value;
            s32 res = self->m44->g3(v, acc);
            self->h1C(res, 1, 0, 0);
        }
        return 1;
    }
    case 9: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        u8 flag = 1;
        if (acc->mods->f10->m55(8, 0) == 1) {
            s32 id = acc->mods->f10->m62(8, 0);
            if (id != 0) {
                Ent* e = getCollisionAttrParamSet__20stCollisionAttrParamFi(g_Stage + 0xF8, id);
                self->h1C(e->f2c, 1, 0, 0);
                flag = e->f25;
            }
        }
        if (flag == 1) {
            s32 v = (a.null == 1) ? 0 : a.p->value;
            s32 res = self->m44->g4(v, acc);
            self->h1C(res, 1, 0, 0);
        }
        return 1;
    }
    case 10: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        u8 flag = 1;
        if (acc->mods->f10->m55(8, 0) == 1) {
            s32 id = acc->mods->f10->m62(8, 0);
            if (id != 0) {
                Ent* e = getCollisionAttrParamSet__20stCollisionAttrParamFi(g_Stage + 0xF8, id);
                self->h1C(e->f44, 1, 0, 0);
                flag = e->f3d;
            }
        }
        if (flag == 1) {
            s32 v = (a.null == 1) ? 0 : a.p->value;
            s32 res = self->m44->g4(v, acc);
            self->h1C(res, 1, 0, 0);
        }
        return 1;
    }
    case 11:
        self->h6C();
        return 1;
    case 13: {
        Arg a;
        a.p = 0;
        a.null = 0;
        cmd->slot6(&a, 0);
        s32 type = (a.null == 1) ? -1 : a.p->type;
        s32 r28;
        if (type == 5) {
            r28 = getValueInt__15soValueAccesserFP16soModuleAccesserUlUl(acc, (a.null == 1) ? 0 : a.p->value, 0);
        } else {
            r28 = (a.null == 1) ? 0 : a.p->value;
        }
        s32 r30 = 0;
        if (cmd->slot6(&a, 1) == 1) {
            r30 = (a.null == 1) ? 0 : a.p->value;
        }
        self->h58(r28, r30);
        return 1;
    }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_5753C {
class Object {
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
    virtual void reset();

    unsigned char pad04[0x44];
    int state;
    unsigned char pad4C[0x0D];
    bool flag;
};

struct Params {
    unsigned char pad00[0x0C];
    unsigned int flags;
};

extern "C" void fn_27_5753C(Object* self, void*, void*, Params params) {
    self->flag = 0;
    if (!((params.flags >> 4) & 1)) {
        self->reset();
    }
    self->state = -1;
}
}

// TODO: clean up, give variables real names
namespace jj_576F8 {

extern "C" {
extern char lbl_27_data_3CB8[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};

class Class_3CB8 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual s16 slot6(Self*, s8);
    virtual void slot7();
    virtual void slot8();
};

extern "C" void addObserver__27soCollisionHitEventObserverFsSc(Self* self, s32 arg1, s8 arg2) {
    soEventManager* mgr;
    u32 f31 = 0;
    s32 f30 = 0;
    s32 f4 = 0;
    s32 f5 = 0;
    s16 ret;
    s16 id;
    s16 a = self->a;

    if (a >= 0 && self->b >= 0) {
        f5 = 1;
    }
    if (f5 != 0 && self->c > -1) {
        f4 = 1;
    }
    if (f4 != 0) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(a) == 1) {
            f30 = 1;
        }
    }
    if (f30 != 0) {
        if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 1) {
            f31 = 1;
        }
    }
    if (f31 == 1) {
        id = self->a;
        if (soEventSystem::getInstance()->getInstanceManager().isContain(id) == 1) {
            soEventSystem::getInstance()->getManager(self->a)->eraseObserver(self->b, self->c);
        }
        self->c = -1;
        self->a = -1;
    }
    if (arg1 > -1 && self->b > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(arg1) != 0) {
            if (soEventSystem::getInstance()->getManager(arg1)->isNullUnit(self->b) != 1) {
                mgr = soEventSystem::getInstance()->getManager(arg1);
                if (mgr->getObserverCapacity(self->b) == 0) {
                    ret = -1;
                } else {
                    soEventUnit* unit = mgr->getEventUnit(self->b);
                    Class_3CB8* cast = (Class_3CB8*)__dynamic_cast(unit, 0, lbl_27_data_3CB8, __RTTI__11soEventUnit, 0);
                    if (cast == 0) {
                        ret = -1;
                    } else {
                        ret = cast->slot6(self, arg2);
                    }
                }
                self->c = ret;
                if (ret > -1 && ret > -1) {
                    self->a = (s16)arg1;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_57934 {


extern "C" {
extern char lbl_27_data_32A8[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual s16 slot6(Self*, s8);
    virtual void slot7();
    virtual void slot8();
};

extern "C" void addObserver__24soSituationEventObserverFsSc(Self* self, s32 arg1, s8 arg2) {
    soEventManager* mgr;
    u32 f31 = 0;
    s32 f30 = 0;
    s32 f4 = 0;
    s32 f5 = 0;
    s16 ret;
    s16 id;
    s16 a = self->a;

    if (a >= 0 && self->b >= 0) {
        f5 = 1;
    }
    if (f5 != 0 && self->c > -1) {
        f4 = 1;
    }
    if (f4 != 0) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(a) == 1) {
            f30 = 1;
        }
    }
    if (f30 != 0) {
        if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 1) {
            f31 = 1;
        }
    }
    if (f31 == 1) {
        id = self->a;
        if (soEventSystem::getInstance()->getInstanceManager().isContain(id) == 1) {
            soEventSystem::getInstance()->getManager(self->a)->eraseObserver(self->b, self->c);
        }
        self->c = -1;
        self->a = -1;
    }
    if (arg1 > -1 && self->b > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(arg1) != 0) {
            if (soEventSystem::getInstance()->getManager(arg1)->isNullUnit(self->b) != 1) {
                mgr = soEventSystem::getInstance()->getManager(arg1);
                if (mgr->getObserverCapacity(self->b) == 0) {
                    ret = -1;
                } else {
                    soEventUnit* unit = mgr->getEventUnit(self->b);
                    Object_vptr_0* cast = (Object_vptr_0*)__dynamic_cast(
                        unit, 0, lbl_27_data_32A8, __RTTI__11soEventUnit, 0);
                    if (cast == 0) {
                        ret = -1;
                    } else {
                        ret = cast->slot6(self, arg2);
                    }
                }
                self->c = ret;
                if (ret > -1 && ret > -1) {
                    self->a = (s16)arg1;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_57C58 {
extern "C" void fn_27_57C58(void) {}
}

// TODO: clean up, give variables real names
namespace jj_57C68 {
extern "C" void fn_27_56C5C(void*);

extern "C" void fn_27_57C68(void* self) {
    fn_27_56C5C(reinterpret_cast<u8*>(self) - 0x10);
}
}

