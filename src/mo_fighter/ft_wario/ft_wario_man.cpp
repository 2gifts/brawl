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
