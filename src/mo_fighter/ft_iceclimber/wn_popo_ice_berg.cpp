#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_ice_berg.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_value_accesser.h>
#include <st/stage.h>
#include <so/so_slow.h>

// MATCH-ONLY: the unit's rodata starts with this word (HYPOTHESIS: the remains of a constant of an unreconstructed function).
#pragma force_active on
extern const int sIceBergRodataLead = 3;
#pragma force_active reset

wnPopoIceBerg::wnPopoIceBerg(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergData* data) :
    wnWeaponBuilder<wnPopoIceBergModuleAccesserBuildConfig>(
        "PopoIceBerg", articleId, info,
        wnModuleAccesserBuildData(0x82, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, data->unk40, 0, 0, 0, 0)),
    soDamageEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param), m_paramAccesser(data->m_param) {
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}

void wnPopoIceBerg::onDeactivate() {
    wnPopoIceBergLinkEvent event(0x45c);
    m_moduleAccesser->getLinkModule().sendEventParents(3, event);
}

void wnPopoIceBerg::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, int heap) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        Vec3f(0.0f, 0.0f, 0.0f), lr, team, 0, 2, 0x80, 0, 0x35f, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getModelModule().setScaleZ(1.0f);
    m_moduleAccesser->getEffectModule().ignoreScaleZ(true);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    float offset = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa2, 0);
    Stage* stage = g_Stage;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (stage && stage->m_stageParam) {
        scaleX = stage->m_stageParam->m_iceClimbersFinalScale.m_x;
        scaleY = stage->m_stageParam->m_iceClimbersFinalScale.m_y;
    }
    offset *= scaleY;
    Vec3f pos(*position);
    pos.m_y -= offset;
    m_moduleAccesser->getPostureModule().setPos(&pos);
    m_moduleAccesser->getWorkManageModule().setFloat(scaleX, 0x11000001);
    m_moduleAccesser->getWorkManageModule().setFloat(scaleY, 0x11000002);
    m_moduleAccesser->getPostureModule().setScale(scaleX);
    m_moduleAccesser->getWorkManageModule().setFloat(position->m_x, 0x11000003);
    m_moduleAccesser->getWorkManageModule().setFloat(position->m_y, 0x11000004);
    m_moduleAccesser->getGroundModule().constructGrCollision(resourceId, 0, static_cast<Heaps::HeapType>(heap), 0, 0, 0);
    m_moduleAccesser->getGroundModule().setGrCollisionFlagAll(1, 2);
    // HYPOTHESIS: default compact stage attack parameters.
    int attackData[6] = { 0, 0, 0, 0, 2, -1 };
    attackData[0] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc4, 0);
    attackData[1] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc5, 0);
    attackData[2] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc6, 0);
    attackData[3] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc7, 0);
    attackData[5] = team;
    g_Stage->setStageAttackData(attackData);
    m_moduleAccesser->getGroundModule().setGrCollisionMode(true);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    updateNodeSRT();
    Vec3f top = m_moduleAccesser->getModelModule().getNodeGlobalPosition(9, false);
    Vec3f bottom = m_moduleAccesser->getModelModule().getNodeGlobalPosition(7, false);
    m_moduleAccesser->getGenerateArticleManageModule().removeExist(4, 0);
    soArticle* article = m_moduleAccesser->getLinkModule().generateArticleLinkParents(3, 4, m_moduleAccesser);
    m_moduleAccesser->getGenerateArticleManageModule().entry(article);
}

void wnPopoIceBerg::processFixPosition() {
    Weapon::processFixPosition();
    if (soSlow::getInstance()->isAdjust() == true && !m_moduleAccesser->getStopModule().isStop()) {
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000003) ||
            m_moduleAccesser->getWorkManageModule().isFlag(0x12000004)) {
            // HYPOTHESIS: default compact stage attack parameters.
            int attackData[6] = { 0, 0, 0, 0, 2, -1 };
            attackData[5] = m_moduleAccesser->getTeamModule().getTeam()->getNo();
            if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000004)) {
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000004);
                attackData[0] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc8, 0);
                attackData[1] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc9, 0);
                attackData[2] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dca, 0);
                attackData[3] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dcb, 0);
            } else {
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000003);
                attackData[0] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc4, 0);
                attackData[1] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc5, 0);
                attackData[2] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc6, 0);
                attackData[3] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc7, 0);
            }
            g_Stage->setStageAttackData(attackData);
        } else if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000006) == true) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x12000006);
            float scale = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
            m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
            Vec3f origin = m_moduleAccesser->getModelModule().getNodeGlobalPosition(9, false);
            Vec3f upper = m_moduleAccesser->getModelModule().getNodeGlobalPosition(8, false) - origin;
            Vec3f lower = m_moduleAccesser->getModelModule().getNodeGlobalPosition(5, false) - origin;
            if (scale > 0.0f && scale != 1.0f) {
                upper.m_x /= scale;
                upper.m_y /= scale;
                lower.m_x /= scale;
                lower.m_y /= scale;
            }
            m_moduleAccesser->getCollisionAttackModule().setOffset(1, &upper);
            m_moduleAccesser->getCollisionAttackModule().setOffset(3, &upper);
            m_moduleAccesser->getCollisionAttackModule().setOffset(0, &lower);
            m_moduleAccesser->getCollisionAttackModule().setOffset(2, &lower);
        }
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000005) == true) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x12000005);
            syncCapsuleSRT(4);
            syncCapsuleSRT(5);
            syncCapsuleSRT(6);
        }
    }
}

bool wnPopoIceBerg::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPopoIceBerg::syncCapsuleSRT(u32 index) {
    if (m_moduleAccesser->getCollisionAttackModule().isAttack(index, false) &&
        m_moduleAccesser->getCollisionAttackModule().getData(index, false) != 0) {
        m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    }
}

void wnPopoIceBergKineticTransactor::changeKinetic(int kineticType, wnPopoIceBergKineticPools* pools, soModuleAccesser* acc) {
    if (kineticType <= 0x1f) {
        wnKineticTransactor::changeKinetic(kineticType, pools, acc);
        return;
    }
    switch (kineticType) {
    case 32:
        changeKineticSub(soIntToType<32>(), pools, acc);
        break;
    case 33:
        changeKineticSub(soIntToType<33>(), pools, acc);
        break;
    }
}

void wnPopoIceBergKineticTransactor::changeKineticSub(const soIntToType<32>&, wnPopoIceBergKineticPools* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    float targetY = acc->getWorkManageModule().getFloat(0x11000004);
    float distance = targetY - acc->getPostureModule().getPos().m_y;
    float frames = soValueAccesser::getConstantInt(acc, 0x5dc0, 0);
    soKineticEnergyNormal* energy = pools->getEnergy(soIntToType<0>());
    *energy = soKineticEnergyNormal(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                                  Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false);
    energy->setSpeed(&Vec2f(0.0f, distance / frames));
    energy->enable();
}

void wnPopoIceBergKineticTransactor::changeKineticSub(const soIntToType<33>&, wnPopoIceBergKineticPools* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    float targetY = acc->getWorkManageModule().getFloat(0x11000004);
    float offset = soValueAccesser::getConstantFloat(acc, 0xfa2, 0);
    offset *= acc->getWorkManageModule().getFloat(0x11000002);
    float distance = targetY - offset - acc->getPostureModule().getPos().m_y;
    float frames = soValueAccesser::getConstantInt(acc, 0x5dc1, 0);
    soKineticEnergyNormal* energy = pools->getEnergy(soIntToType<0>());
    *energy = soKineticEnergyNormal(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                                  Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false);
    energy->setSpeed(&Vec2f(0.0f, distance / frames));
    energy->enable();
}

// TODO: clean up, give variables real names
namespace jj_1B800 {
typedef unsigned int u32;
struct WorkModule {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void set(int, unsigned int);
};
struct Pad8 { char p[8]; };
struct MotionModule : Pad8 {
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
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16();
    virtual void s17();
    virtual void s18();
    virtual void s19();
    virtual void s20();
    virtual void s21();
    virtual void s22();
    virtual void s23();
    virtual void s24();
    virtual void s25();
    virtual void s26();
    virtual void s27();
    virtual void s28();
    virtual void s29();
    virtual void s30();
    virtual void s31();
    virtual void s32();
    virtual void s33();
    virtual void s34();
    virtual void s35();
    virtual void s36();
    virtual void s37();
    virtual void s38();
    virtual void s39();
    virtual void s40();
    virtual void s41();
    virtual void s42();
    virtual void s43();
    virtual void s44();
    virtual void s45();
    virtual void s46();
    virtual void s47();
    virtual void s48();
    virtual void s49();
    virtual void s50();
    virtual void s51();
    virtual void s52();
    virtual void s53();
    virtual void s54();
    virtual void s55();
    virtual void s56();
    virtual void s57();
    virtual void s58();
    virtual void s59();
    virtual void s60();
    virtual void s61();
    virtual void s62();
    virtual void s63();
    virtual void s64();
    virtual void s65();
    virtual void s66();
    virtual void s67();
    virtual void s68();
    virtual void s69();
    virtual void s70();
    virtual void s71();
    virtual void s72();
    virtual void s73();
    virtual void s74();
    virtual void s75();
    virtual void s76();
    virtual void s77();
    virtual void s78();
    virtual void s79();
    virtual void s80();
    virtual void s81();
    virtual void s82();
    virtual void s83();
    virtual void s84();
    virtual void s85();
    virtual void s86();
    virtual void s87();
    virtual void s88();
    virtual void s89();
    virtual void s90();
    virtual void s91();
    virtual void s92();
    virtual void s93();
    virtual void s94();
    virtual void s95();
    virtual void s96();
    virtual void s97();
    virtual void s98();
    virtual void s99();
    virtual void s100();
    virtual void s101();
    virtual void s102();
    virtual void s103();
    virtual void s104();
    virtual void s105();
    virtual void s106();
    virtual void s107();
    virtual void s108();
    virtual void s109();
    virtual void s110();
    virtual void s111();
    virtual void s112();
    virtual void s113();
    virtual void s114();
    virtual void s115();
    virtual void s116();
    virtual void s117();
    virtual void s118();
    virtual void s119();
    virtual void s120();
    virtual void s121();
    virtual void s122();
    virtual void s123();
    virtual void s124();
    virtual void s125();
    virtual void s126();
    virtual void s127();
    virtual void s128();
    virtual void s129();
    virtual void s130();
    virtual void s131();
    virtual void call();
};
struct Modules {
    char pad0[0x10];
    MotionModule* proxy;
    char pad1[0x50];
    WorkModule* work;
};
struct Accesser { char pad[0xd8]; Modules* modules; };
struct Fighter { char pad[0x60]; Accesser* accesser; };
extern "C" {
extern void* g_ftManager;
void notifyEventChangeStatus__6WeaponFiiP12soStatusDataP16soModuleAccesser(void*);
void setFinalStatus__9ftManagerFi(void*, int);
void setControllerRumbleAll__9ftManagerFiiUli(void*, int, int, bool, int);
void stopControllerRumbleAll__9ftManagerFii(void*, int, int);
void cancelFinalStatus__9ftManagerFv(void*);
int getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(Accesser*, unsigned long, unsigned long);
float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(Accesser*, unsigned long, unsigned long);
}
extern "C" void fn_105_1B800(Fighter* self, int state, int previous) {
    notifyEventChangeStatus__6WeaponFiiP12soStatusDataP16soModuleAccesser(self);
    if (state == 0) {
        setFinalStatus__9ftManagerFi(g_ftManager, 1);
        setControllerRumbleAll__9ftManagerFiiUli(g_ftManager, 2, 0, 0, 5);
    } else if (state == 1) {
        int value = getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(self->accesser, 0x5dc2, 0);
        float amount = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(self->accesser, 0xfa1, 0);
        int frames = (int)((float)value - amount);
        if (frames < 0) frames = 0;
        WorkModule* work = self->accesser->modules->work;
        work->set(frames, 0x20000000);
        self->accesser->modules->proxy->call();
        stopControllerRumbleAll__9ftManagerFii(g_ftManager, 2, 5);
    } else if (state == 2) {
        setControllerRumbleAll__9ftManagerFiiUli(g_ftManager, 2, 0, 0, 5);
    } else {
        stopControllerRumbleAll__9ftManagerFii(g_ftManager, 2, 5);
    }
    if (previous == 2) cancelFinalStatus__9ftManagerFv(g_ftManager);
}
}

// TODO: clean up, give variables real names
namespace jj_1BD8C {
extern "C" void notifyEventAddDamage__21soDamageEventObserverFP8soDamageP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_1BEA0 {
extern "C" void notifyEventOnDamage__21soDamageEventObserverFP8soDamagebP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_1C398 {
extern "C" void* getInstanceAt__21ftPurinArticleSubPoolFl(void* object, int index) {
    if (index == 0) {
        return static_cast<char*>(object) + 0x0c;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_1C474 {
extern "C" void fn_105_1B800(void*);

extern "C" void fn_105_1C474(void* self) {
    fn_105_1B800(reinterpret_cast<u8*>(self) - 0x64);
}
}

// TODO: clean up, give variables real names
namespace jj_1C49C {
extern "C" void fn_105_1B97C(void*);

extern "C" void fn_105_1C49C(void* self) {
    fn_105_1B97C(reinterpret_cast<u8*>(self) - 0x2708);
}
}

// TODO: clean up, give variables real names
namespace jj_1C698 {
union Range {
    struct {
        float minimum, maximum;
    } values;
    struct {
        unsigned long minimum, maximum;
    } bits;
};

struct Config {
    const void* vtable;
    unsigned char state;
    unsigned char enabled : 1;
    unsigned char flags : 7;
    unsigned char state2;
    unsigned char pad;
    Range ranges[5];
    bool firstFlag, secondFlag, initialized;
};

extern "C" {
extern const unsigned char __vt__21soKineticEnergyNormal[];

void __ct__21soKineticEnergyNormalFRC5Vec2fRC5Vec2fRC5Vec2fRC5Vec2fbb(
    Config* self,
    const Range* a,
    const Range* b,
    const Range* c,
    const Range* d,
    const Range* e,
    bool f,
    bool g
) {
    self->state = 0;
    self->enabled = true;
    self->state2 = 0;
    self->vtable = __vt__21soKineticEnergyNormal;
    self->ranges[0] = *a;
    self->ranges[1] = *b;
    self->ranges[2] = *c;
    self->ranges[3] = *d;
    self->ranges[4] = *e;
    self->firstFlag = f;
    self->secondFlag = g;
    self->initialized = true;
}
}
}

// TODO: clean up, give variables real names
namespace jj_1C720 {
struct Vec2Pair {
    float x;
    float y;
};

struct Holder {
    unsigned int header[2];
    float x;
    float y;
};

extern "C" void setSpeed__21soKineticEnergyNormalFP5Vec2f(Holder* obj, const Vec2Pair* src) {
    obj->x = src->x;
    obj->y = src->y;
}
}

