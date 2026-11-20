#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_FIGHTER_ANIMCMD_LONG
#define MT_VEC3F_CTOR_NOINLINE
#define FT_NOINLINE_SKIP_SHARED_DEFS
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/wario/ft_wario_man_builder.h>
#include <so/so_kinetic_utility.h>
#include <ft/wario/ft_wario_common.h>
#include <ft/wario/ft_wario_man_extend_param_accesser.h>
#include <ft/wario/ft_wario_transactor.h>
#include <ft/wario/ft_wario_unk8.h>
#include <ft/ft_common_data_accesser.h>
#include <so/damage/so_damage.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <types.h>


#define FT_BC ftWarioManBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Wario-Man's data extends the common fighter data with an area set and the model node conversion data.
struct ftWarioManData : ftData {
    soSet<soAreaData> additionalAreas; // +0x94
    u32** costumeNodeConvertData; // +0x9C
};

extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNOpen;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialS;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialHiStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNBite;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNBiteEnd;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNEatStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNEat;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSDrive;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSWheelie;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSCommon;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSEscape;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSDown;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialHiJump;

ftWarioMan::ftWarioMan(s32 entryId,
                       Heaps::HeapType instHeap,
                       Heaps::HeapType nwModelInstHeap,
                       Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftWarioManBuildConfig>(entryId, Fighter_WarioMan, instHeap, instHeap, nwMotionInstHeap),
    m_extendParam(reinterpret_cast<ftWarioManDataView*>(g_ftCommonDataAccesser.getData(Fighter_WarioMan))) {
    // Wario-Man keeps Wario's special move processes (the form shares the Waft and bike states).
    soStatusUniqProcess* processes[30] = {0};
    processes[0] = &g_ftWarioStatusUniqProcessSpecialNOpen;
    processes[1] = &g_ftWarioStatusUniqProcessSpecialS;
    processes[2] = &g_ftWarioStatusUniqProcessSpecialHiStart;
    processes[3] = &g_ftWarioStatusUniqProcessSpecialLw;
    processes[5] = &g_ftWarioStatusUniqProcessSpecialNOpen;
    processes[7] = &g_ftWarioStatusUniqProcessSpecialNBite;
    processes[8] = &g_ftWarioStatusUniqProcessSpecialNBite;
    processes[9] = &g_ftWarioStatusUniqProcessSpecialNBiteEnd;
    processes[10] = &g_ftWarioStatusUniqProcessSpecialNEatStart;
    processes[11] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[12] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[13] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[14] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[15] = &g_ftWarioStatusUniqProcessSpecialSStart;
    processes[17] = &g_ftWarioStatusUniqProcessSpecialSStart;
    processes[18] = &g_ftWarioStatusUniqProcessSpecialSDrive;
    processes[19] = &g_ftWarioStatusUniqProcessSpecialSWheelie;
    processes[20] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[21] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[22] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[23] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[24] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[25] = &g_ftWarioStatusUniqProcessSpecialSEscape;
    processes[26] = &g_ftWarioStatusUniqProcessSpecialSDown;
    processes[27] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[28] = &g_ftWarioStatusUniqProcessSpecialHiJump;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 30);

    m_moduleAccesser->getAreaModule().addArea(&reinterpret_cast<ftWarioManData*>(m_extendParam)->additionalAreas);
    m_moduleAccesser->getModelModule().setNodeConvertData(reinterpret_cast<ftWarioManData*>(m_extendParam)->costumeNodeConvertData);
}

ftWarioManExtendParamAccesser g_ftWarioManExtendParamAccesser;
static ftWarioUnk8 s_unkTag0(0xff, 0);
static ftWarioUnk8 s_unkTag1(0xff, 1);
ftClassInfoImpl<Fighter_WarioMan, ftWarioMan> g_ftClassInfoWarioMan;

// MATCH-ONLY: the fighter manager header cannot coexist with the builder headers; only the rumble request is needed.
class ftManager {
public:
    void setControllerRumble(int entryId, int unk1, int unk2, u32 isOn, int unk3);
};
extern ftManager* g_ftManager;

// Wario-Man (the Final Smash form of Wario). The fighter overrides forward to the
// Waft gauge / bike code that Wario shares with it (ftWarioCommon, in the melee module).

// LA-Float 0x13 is the time left of the transformation, LA-Flag 0x3d tells that the form is active.
enum {
    Work_Float_FormTime = 0x11000013,
    Work_Flag_FormActive = 0x1200003D,
    Work_Flag_FormEnding = 0x1200003E,
};

// MATCH-ONLY: the kinetic dispatcher calls the getter out of line.
#pragma dont_inline on
ftWarioExtendParam* ftWarioMan::getExtendParam() {
    return &m_extendParam->m_param;
}
#pragma dont_inline reset

void ftWarioMan::onActivate() {
    g_ftWarioCommon.onActivate(m_moduleAccesser, &m_animObj, m_extendParam->m_param.bikeRider->remountCooldownFrames);
}

void ftWarioMan::onDeactivate() {
    g_ftWarioCommon.onDeactivate(m_moduleAccesser, &m_animObj);
}

void ftWarioMan::onStart(int startKind) {
    Fighter::onStart(startKind);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(0.0f, Work_Float_FormTime);
    work.offFlag(Work_Flag_FormActive);
    m_moduleAccesser->getCollisionHitModule().setInvincibleGlobal(0);
    g_ftWarioCommon.onStart(m_moduleAccesser, startKind);
}

void ftWarioMan::toDead(Dead::Reason deadReason) {
    g_ftWarioCommon.toDead(m_moduleAccesser, deadReason);
    Fighter::toDead(deadReason);
}

void ftWarioMan::onDeadEnd() {
    g_ftWarioCommon.toChange(m_moduleAccesser, m_entryId, 0, 0);
    int entryId = m_entryId;
    ftManager* manager = g_ftManager;
    manager->setControllerRumble(entryId, 6, soValueAccesser::getConstantInt(m_moduleAccesser, 0x5A38, 0), 0, m_taskId);
    Fighter::onDeadEnd();
}

void ftWarioMan::notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    // The bike and the rider events are handled by the common code, except while Wario is on the Waft (0x116).
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        Fighter::notifyEventLink(args, moduleAccesser, sender, unk4);
    } else if (!g_ftWarioCommon.notifyEventLink(static_cast<ftWarioLinkEventArgs*>(args), moduleAccesser, sender, unk4)) {
        Fighter::notifyEventLink(args, moduleAccesser, sender, unk4);
    }
}

void ftWarioMan::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    g_ftWarioCommon.notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

void ftWarioMan::notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(Work_Flag_FormActive)) {
        // A hit takes time off the transformation.
        float amount = damage->m_damageAdd * m_extendParam->m_manParam->damageToGauge;
        work.subFloat(amount, Work_Float_FormTime);
    }
    Fighter::notifyEventOnDamage(damage, unk1, moduleAccesser);
}

bool ftWarioMan::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    // Animation command type 5 changes Wario-Man back into Wario (or the next fighter of the team).
    switch (acmd->getType()) {
    case 5:
        g_ftWarioCommon.toChange(moduleAccesser, m_entryId, 1, 0);
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

void ftWarioMan::setupChangeSucceedWork(void* changeSucceedWork) {
    Fighter::setupChangeSucceedWork(changeSucceedWork);
    g_ftWarioCommon.setupChangeSucceedWork(m_moduleAccesser, changeSucceedWork);
}

void ftWarioMan::changeSucceedCore(void* changeSucceedWork, int unk2) {
    Fighter::changeSucceedCore(changeSucceedWork, unk2);
    g_ftWarioCommon.changeSucceedCore(m_moduleAccesser, changeSucceedWork, unk2);
}

void ftWarioMan::change(void* unk1, int startKind) {
    onStart(startKind);
    ftWarioManParam* param = m_extendParam->m_manParam;
    // How long the transformation lasts in frames: HYPOTHESIS: the second term shortens it with a fighter constant.
    float time = 60.0f * param->durationSeconds;
    float constant = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xDB2, 0);
    time -= constant * param->unk4;
    m_moduleAccesser->getWorkManageModule().setFloat(time, Work_Float_FormTime);
    m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_FormActive);
    changeSucceedCore(unk1, 0x130);
}

void ftWarioMan::processUpdate() {
    Fighter::processUpdate();
    g_ftWarioCommon.processUpdate(m_extendParam->m_param.specialLw, m_moduleAccesser, &m_animObj);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    if (work.isFlag(Work_Flag_FormActive)) {
        // The form runs out one frame at a time; the end warning starts at a gauge level.
        work.subFloat(1.0f, Work_Float_FormTime);
        if (!work.isFlag(Work_Flag_FormEnding)) {
            ftWarioManDataView* params = m_extendParam;
            float time = work.getFloat(Work_Float_FormTime);
            if (time < params->m_manParam->unkC) {
                work.onFlag(Work_Flag_FormEnding);
                m_moduleAccesser->getEffectModule().reqCommon(0x25, 0.0f);
            }
        }
    }
    if (m_moduleAccesser->getEffectModule().isExistCommon(1)) {
        m_moduleAccesser->getEffectModule().removeCommon(1);
    }
}

void ftWarioMan::processFixPosition() {
    Fighter::processFixPosition();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soStatusModule& status = m_moduleAccesser->getStatusModule();
    // The form is over: leave it with status 0x116.
    if (soSlow::getInstance()->isAdjust() && work.isFlag(Work_Flag_FormActive) &&
        work.getFloat(Work_Float_FormTime) < 0.0f && status.getStatusKind() != 0x116) {
        status.changeStatusRequest(0x116, m_moduleAccesser);
    }
}


// BYTES: as in ftWario, the local static is addressed separately from the pooled BSS base.
#pragma pool_data off
void ftWarioMan::notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* acc) {
    static ftWarioTransactor transactor;
    static_cast<ftKirbyCopyAbilityProcess&>(transactor).notifyEventCollisionSearch(log, acc);
    Fighter::notifyEventCollisionSearch(log, acc);
}
#pragma pool_data reset

#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_array.h>
// TODO: clean up, give variables real names
namespace jj_1E7D0 {
extern "C" {
void fn_110_1E82C(void*, short);
void __dl__FPv(void*);

void* fn_110_1E7D0(void* self, int flag) {
    if (self != 0) {
        fn_110_1E82C(self, -1);
        if ((short)flag > 0)
            __dl__FPv(self);
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_1E89C {
extern "C" void fn_110_1E960(void*, short);

static inline void destroyMotionPoolBase(void* object) {
    if (object != 0) {
        fn_110_1E960(static_cast<unsigned char*>(object) + 4,
                     static_cast<short>(-1));
    }
}

extern "C" void* fn_110_1E89C(void* object, short deleting) {
    if (object != 0) {
        destroyMotionPoolBase(object);
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_1E900 {
extern "C" void fn_110_1E960(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_110_1E900(void* object, short deleting) {
    if (object != 0) {
        fn_110_1E960(static_cast<unsigned char*>(object) + 4, -1);
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_1E960 {
extern "C" void* fn_110_1E9C0(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_110_1E960(void* object, short deleting) {
    if (object != 0) {
        fn_110_1E9C0(static_cast<unsigned char*>(object) + 8, -1);
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_1E9C0 {
extern "C" void __dt__11wnWarioBikeFv(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_110_1E9C0(void* object, short deleting) {
    if (object != 0) {
        __dt__11wnWarioBikeFv(static_cast<unsigned char*>(object) + 4, -1);
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_20348 {
extern "C" int fn_110_20348(void* arg0, unsigned int articleId) {
    switch (articleId) {
    case 0: { char s[2] = {0, 0}; (void)s; return 1; }
    case 1: { char s[2] = {0, 0}; (void)s; return 0; }
    case 2: { char s[2] = {0, 0}; (void)s; return 0; }
    case 3: { char s[2] = {0, 0}; (void)s; return 0; }
    case 4: { char s[2] = {0, 0}; (void)s; return 0; }
    case 5: { char s[2] = {0, 0}; (void)s; return 0; }
    case 6: { char s[2] = {0, 0}; (void)s; return 0; }
    case 7: { char s[2] = {0, 0}; (void)s; return 0; }
    case 8: { char s[2] = {0, 0}; (void)s; return 0; }
    case 9: { char s[2] = {0, 0}; (void)s; return 0; }
    case 10: { char s[2] = {0, 0}; (void)s; return 0; }
    case 11: { char s[2] = {0, 0}; (void)s; return 0; }
    case 12: { char s[2] = {0, 0}; (void)s; return 0; }
    case 13: { char s[2] = {0, 0}; (void)s; return 0; }
    case 14: { char s[2] = {0, 0}; (void)s; return 0; }
    case 15: { char s[2] = {0, 0}; (void)s; return 0; }
    case 16: { char s[2] = {0, 0}; (void)s; return 0; }
    default: return 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_204CC {
extern "C" {
extern char __RTTI__11wnWarioBike[];
extern char __RTTI__9soArticle[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

class Obj {
public:
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5();
    virtual unsigned int getType();
};

extern "C" int fn_110_204CC(void*, void*, void* self) {
    unsigned char buf[40];
    unsigned char* p = buf;
    Obj* obj = reinterpret_cast<Obj*>(self);
    switch (obj->getType()) {
        case 0: p[32] = 0; p[33] = 0; __dynamic_cast(self, 0, __RTTI__11wnWarioBike, __RTTI__9soArticle, 1); return 1;
        case 1: p[30] = 0; p[31] = 0; return 1;
        case 2: p[28] = 0; p[29] = 0; return 1;
        case 3: p[26] = 0; p[27] = 0; return 1;
        case 4: p[24] = 0; p[25] = 0; return 1;
        case 5: p[22] = 0; p[23] = 0; return 1;
        case 6: p[20] = 0; p[21] = 0; return 1;
        case 7: p[18] = 0; p[19] = 0; return 1;
        case 8: p[16] = 0; p[17] = 0; return 1;
        case 9: p[14] = 0; p[15] = 0; return 1;
        case 10: p[12] = 0; p[13] = 0; return 1;
        case 11: p[10] = 0; p[11] = 0; return 1;
        case 12: p[8] = 0; p[9] = 0; return 1;
        case 13: p[6] = 0; p[7] = 0; return 1;
        case 14: p[4] = 0; p[5] = 0; return 1;
        case 15: p[2] = 0; p[3] = 0; return 1;
        case 16: p[0] = 0; p[1] = 0; return 1;
        default: return 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_20704 {
extern "C" s32 fn_110_20704(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_2070C {
extern "C" void fn_110_2070C(void* self, u8 value) {
    *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x2a60) = value;
}
}

// ftKineticMediatorImplT::updateEnergy2 of Wario Man's mediator (include/ft/builder/ft_builder_kinetic.h): every enabled,
// not suspended energy of the list is updated (the enable / suspend test is repeated by the inlined checks).
namespace jj_20C7C {
extern "C" void fn_110_20C7C(void*, soArray<soKineticEnergy**>* energies, soModuleAccesser* acc) {
    for (int i = 0; i < energies->size(); i++) {
        soKineticEnergy* energy = **&energies->at(i);
        if (energy->isEnable() != false && energy->isSuspend() != true) {
            if (energy->isEnable() == true && energy->isSuspend() == false) {
                if (energy->isEnable() == true && energy->isSuspend() == false) {
                    energy->updateEnergy(acc);
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_20D88 {
extern "C" s32 fn_110_20D88(void) {
    return 0x8;
}
}

// TODO: clean up, give variables real names
namespace jj_20FA0 {
extern "C" void fn_110_204CC(void*);

extern "C" void fn_110_20FA0(void* self) {
    fn_110_204CC(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_20FE8 {

struct WarioDispatchSpeed { float x, y; };
struct WarioResetContext {
    unsigned char initialized;
    WarioResetContext() { __memclr(&initialized, sizeof(initialized)); }
    void resetIdle(int, const Vec2f*, soModuleAccesser*) {}
};
extern "C" {
void changeKinetic__19ftKineticTransactorFiPvP16soModuleAccesser(int, int, soModuleAccesser*);
WarioDispatchSpeed preHelpProcess__23ftKineticTransactHelperFP16soModuleAccesserbb(soModuleAccesser*, bool, bool);
void enableOutsideEnergy__19ftKineticTransactorFP16soModuleAccesser(soModuleAccesser*);
void fn_110_210D8(void*, int, const Vec2f*, soModuleAccesser*);
}
extern "C" void fn_110_20FE8(int kind, int resetKind, soModuleAccesser* accesser) {
    if (kind <= 0x63) {
        changeKinetic__19ftKineticTransactorFiPvP16soModuleAccesser(kind, resetKind, accesser);
        return;
    }
    Vec2f speed;
    WarioDispatchSpeed returnedSpeed = preHelpProcess__23ftKineticTransactHelperFP16soModuleAccesserbb(accesser, 1, 1);
    __memcpy(&speed, &returnedSpeed, sizeof(speed));
    switch (kind) {
    case 0x64: {
        WarioResetContext context;
        fn_110_210D8(&context, resetKind, &speed, accesser);
        break;
    }
    case 0x65: {
        WarioResetContext context;
        context.resetIdle(resetKind, &speed, accesser);
        break;
    }
    case 0x66: {
        WarioResetContext context;
        context.resetIdle(resetKind, &speed, accesser);
        break;
    }
    case 0x67: {
        WarioResetContext context;
        context.resetIdle(resetKind, &speed, accesser);
        break;
    }
    case 0x68: {
        WarioResetContext context;
        context.resetIdle(resetKind, &speed, accesser);
        break;
    }
    default:
        return;
    }
    enableOutsideEnergy__19ftKineticTransactorFP16soModuleAccesser(accesser);
}
}

void operator delete(void*);
// TODO: clean up, give variables real names
namespace jj_1E82C {
extern "C" void* fn_110_1E89C(void*, short);
extern "C" void* fn_110_4FF4(void*, short);

extern "C" void* fn_110_1E82C(void* object, short deleting) {
    if (object != 0) {
        fn_110_1E89C(static_cast<unsigned char*>(object) + 0x8, -1);
        fn_110_4FF4(object, 0);
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

