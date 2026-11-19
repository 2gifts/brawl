// wnemSimple and wnemSwHolder, the enemies' weapon articles (wnemSimple is a full Weapon built from the module builders,
// wnemSwHolder a small holder that activates a pooled wnemSimple). The unit starts with the holder.
//
// Status: the holder and the small wnemSimple members are reconstructed; the unit is NonMatching because the
// wnWeaponBuilder base (constructor 0x2D9D8 and its thirty module-builder destructors), the wnemSimple constructors and
// destructor, the vtable with its weak thunk tail and the larger members (activate, setupParam, onDeactivate,
// notifyEventLink, notifyEventCollisionAttack(Check), reflect, ShootItem, updateStatusProcFunc ...) are not drafted yet.
// The string literals of the unit's .data pool only line up once all of those are in, so the functions that print show
// string-offset differences in objdiff until then.
// The Enemy unit's header setup (see em/prim/em_prim.cpp): StageObject's long animation command slot, which Weapon overrides.
#define YK_STAGE_FULL
#define GF_GAMEFRAME_COUNTER_INLINE_CTOR // the counters' constructor is a weak copy of this unit
#define SO_ANIM_CMD_INTERPRET_CMD1_FRAME_LAST
#define SO_ANIM_CMD_RESET_INTERPRETER2_FRAME_LAST
#define WN_WEAPON_ANIMCMD_LONG
#define IP_NULL_INPUT_CONT_NO
#define SO_ARTICLE_DTOR_INLINE // the holder emits the article base destructor as a weak copy
#define WN_WEAPON_BUILDER
#define WN_EM_SIMPLE_UNIT
#define WN_EM_SIMPLE_PROPERTY_COPY
#define WN_EM_SIMPLE_HEAP_DTOR
#define SO_GLOW_MODULE_ATTACK_API
#define WN_EM_SIMPLE_GLOW_API
#define FT_REL_LINK_EXTERN
#define WN_SOUND_GENERATOR_HELPERS
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_DAMAGE_EVENT_OBSERVER_WEAK_NOTIFY
#include <wn/wn_module_accesser_builder.h>
#include <em/em_weapon_manager.h>
#include <em/wnem/wn_em_report.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <it/it_manager.h>
#include <so/ground/so_ground_util.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/collision/so_collision_log.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/anim/so_anim_cmd_arg_list.h>
#include <st/st_trigger.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_trig.h>
#include <math.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>
#include <so/article/so_article_mediator.h>
#include <it/it_gen_archive.h>
#include <wn/wn_module_accesser_build_data.h>
#include <em/wnem/wn_em_heap_module_impl.h>

// This shared constant is owned by the original unit's read-only pool.
extern const float lbl_41_rodata_E54;

template <>
inline soAttributeFlag soInstanceUnitFullProperty<soAnimCmdControlUnit>::getAttribute() const {
    return soAttributeFlag(m_attribute.m_mask);
}

template <>
inline soInstanceUnit<soAnimCmdControlUnit>::soInstanceUnit(soAnimCmdControlUnit& elm, s32 id) {
    m_element.m_animCmdInterpreter = elm.m_animCmdInterpreter;
    m_element.m_animCmdAddressPackArraySeparate = elm.m_animCmdAddressPackArraySeparate;
    m_id = id;
}

// stTriggerMng notification (sora_melee) of a hit by an enemy article; see damage/em_damage_module_impl.cpp.
extern "C" void fn_27_23313C(u32 createId, u32 attackerTaskId, u32 taskId, u8 attackerCategory);

// The base of the pooled weapons has an empty destructor; the unit carries its weak copy (map name
// utWeaponObjectBase::~utWeaponObjectBase).
inline utWeaponObjectBase::~utWeaponObjectBase() { }

// ---- wnemSwHolder ---------------------------------------------------------------------------------------------------

// HYPOTHESIS: the presenter ids of the article base (manage id -1, unit id 0xB) are constants of the holder; the second
// constructor argument is not used.
// TODO(nonmatching): 14%, the original keeps the manage id test of the presenter constructor (`extsh. r0,r4; ble` on the
// constant -1 it just stored) instead of folding it away, so its constructor inlines the whole event system lookup.
wnemSwHolder::wnemSwHolder(s32 articleId, s16 manageId, const soArticleConstructInfo& info, const wnemDummy_Param* param) :
    soArticle(-1, 0xB), m_isActive(false), m_weapon(NULL), m_articleId(articleId) { }

wnemSwHolder::~wnemSwHolder() { }

// Takes a wnemSimple from the weapon pool and starts it with the shot.
void wnemSwHolder::active(Vec3f* pos, float lr, int team, int p10, int founderTaskId, int resourceId, wnemSimple_Data* work,
                          float speed, bool flag, u32 life) {
    if (m_weapon == NULL) {
        wnemSimple* wnem = static_cast<wnemSimple*>(emWeaponManager::getInstance()->getObject(true));
        if (wnem != NULL) {
            emWeaponManager::getInstance()->activateObject(wnem, founderTaskId, resourceId, work, 0, pos, lr, team, speed,
                                                          p10, flag, life, -1);
            wnem->m_holder = this;
            wnem->m_isFromPool = true;
            m_isActive = true;
            m_weapon = wnem;
        }
    }
}

void wnemSwHolder::deactivate() {
    if (m_weapon != NULL) {
        m_weapon->m_holder = NULL;
        m_weapon = NULL;
    }
    m_isActive = false;
}

s16 wnemSwHolder::getArticleEventManageId() {
    if (m_weapon != NULL) {
        return m_weapon->getArticleEventManageId();
    }
    return -1;
}

void wnemSwHolder::linkOwner(int ownerId, int p2) {
    if (m_weapon != NULL) {
        m_weapon->linkOwner(ownerId, p2);
    }
}

void wnemSwHolder::unlinkOwner(int ownerId) {
    if (m_weapon != NULL) {
        m_weapon->unlinkOwner(ownerId);
    }
}

void wnemSwHolder::have(int, int, int) { }

bool wnemSwHolder::isConstraint(bool unk) {
    if (m_weapon != NULL) {
        return m_weapon->isConstraint(unk);
    }
    return false;
}

void wnemSwHolder::setAttackPowerMulPattern(float) { }

float wnemSwHolder::getActiveGlobalFrame() {
    return -1.0f;
}

void wnemSwHolder::intrudeLogAttackInfo() { }

void wnemSwHolder::updateLogAttackInfo() { }

soLogAttackInfo wnemSwHolder::getLogAttackInfo() {
    return soLogAttackInfo(0, 0);
}

void wnemSwHolder::setLogAttackInfo(soLogAttackInfo*) { }

u32 wnemSwHolder::getFounderTaskId() {
    return -1;
}

int wnemSwHolder::getOwnerDeactiveTreatType() {
    return 0;
}

void wnemSwHolder::deactivateDescendantForce() { }

void wnemSwHolder::setSituationKind(SituationKind) { }

void wnemSwHolder::action(int) { }

void wnemSwHolder::set2nd(u8) { }

void wnemSwHolder::setVisibility(int, u8) { }

void wnemSwHolder::setVisibilityWhole(u8) { }

void wnemSwHolder::changeStatus(int) { }

void wnemSwHolder::setRate(float) { }

void wnemSwHolder::setFrame(float) { }

bool wnemSwHolder::isActive() {
    return m_isActive;
}

void wnemSwHolder::deactivateArticle() {
    deactivate();
}

bool wnemSwHolder::isActiveArticle() {
    return isActive();
}

void wnemSwHolder::setSyncOwnerStatus(bool) { }

bool wnemSwHolder::isSyncOwnerStatus() {
    return false;
}

int wnemSwHolder::getTaskId() {
    return m_weapon->m_taskId;
}

int wnemSwHolder::getArticleId() {
    return m_articleId;
}

void wnemSwHolder::remove() {
    deactivate();
}

// ---- wnemSimple -----------------------------------------------------------------------------------------------------

// The animation command set of the shot description (its offset 0x70): a table of command lists.
// HYPOTHESIS: field names; the strings name _pAnimCmdSet, _pData and _size.
struct wnemAnimCmdEntry {
    void* m_cmd;
    s32 m_kind;
};
struct wnemAnimCmdSet {
    wnemAnimCmdEntry* m_data;
    u32 m_size;
};
// BYTES: emShotParam keeps the set pointer in its unnamed bytes; read it through a typed view of the description.
struct wnemShotParamView {
    u8 unk0[0x18];
    s32 m_startAnimCmd;   // 0x18
    u8 unk1C[0x4];
    s32 m_constraintNode; // 0x20, the model node of the owner the weapon is constrained to (-1: none)
    u8 m_flag24;          // 0x24, HYPOTHESIS: four flag bytes the weapon copies (setupParam)
    u8 m_flag25;
    u8 m_flag26;
    u8 m_flag27;
    u32 m_modelFlags;     // 0x28
    void* m_motionData;   // 0x2C
    s32 m_resetHitStatus; // 0x30, HYPOTHESIS: nonzero when the weapon keeps the hit status of its hit module
    void* m_groundData;   // 0x34, the ground shape data of the weapon (setupGroundData); null: no ground collision
    u8 unk38[0x38];
    wnemAnimCmdSet* m_animCmdSet; // 0x70
};

// HYPOTHESIS: the ground shape data of a weapon description (emShotParam 0x34): the shape kind (1: rhombus, 2: rhombus that
// follows a model node) and its measurements.
struct wnemGroundShapeData {
    u8 m_kind;
    union {
        struct {
            float m_up;
            float m_down;
            float m_width;
        } m_rhombus;     // 0x4, kind 1
        struct {
            s32 m_node;
            float m_up;
            float m_down;
            float m_width;
        } m_followNode;  // 0x4, kind 2
    };
};

// HYPOTHESIS: a plain view of the work block's int-sized fields at the offsets the unit's functions read (the block the
// header declares inside m_shotWork is shorter in names than in use).
static inline u8& wnemByteAt(wnemSimple* wnem, int offset) {
    return *(reinterpret_cast<u8*>(wnem) + offset);
}
static inline Vec2f& wnemVec2At(void* base, int offset) {
    return *reinterpret_cast<Vec2f*>(reinterpret_cast<u8*>(base) + offset);
}

// Copies the shot description and the work block into the weapon's own state.
// HYPOTHESIS: the second argument is the emWeaponParamWork of the shot (its launch speed and acceleration).
bool wnemSimple::setupParam(emShotParam* param, emWeaponParamWork* work, float lr) {
    unk21B9 = false;
    unk21B8 = false;
    unk2215 = false;
    unk2216 = false;
    unk2217 = false;
    unk2218 = false;
    m_requestDeactive = false;
    m_isManaged = false;
    m_isFromPool = false;
    m_linkId = param->m_shotKind;
    m_lifeLimit = param->m_life;
    unk21D4 = static_cast<u32>(param->m_constantValue);
    unk21D8 = static_cast<u32>(param->m_constantValue);
    m_weaponType = param->m_type;
    unk21E8 = -1;
    unk21EC = 0;
    unk21BA[0] = reinterpret_cast<wnemShotParamView*>(param)->m_flag24;
    wnemByteAt(this, 0x21BB) = reinterpret_cast<wnemShotParamView*>(param)->m_flag25;
    wnemByteAt(this, 0x21B6) = reinterpret_cast<wnemShotParamView*>(param)->m_flag26;
    wnemByteAt(this, 0x21B7) = reinterpret_cast<wnemShotParamView*>(param)->m_flag27;
    m_procFncHpZero = NULL;
    m_procFncLifeZero = NULL;
    m_procFncTouchGround = NULL;
    m_procFncHitShell = NULL;
    m_holder = NULL;
    unk2234 = NULL;
    wnemByteAt(this, 0x21B5) = work->unk2;
    m_baseSpeed = wnemVec2At(work, 0x14);
    m_speed = m_baseSpeed;
    m_accel = wnemVec2At(work, 0x1C);
    m_accel.m_y += param->m_flySpeedY;
    resetFrameCounter(0);
    m_frameCounters[0].stop();
    if (m_lifeLimit > 0) {
        unk21B9 = true;
        m_frameCounters[0].start();
    }
    if (unk21D8 > 0.0f) {
        unk21B8 = true;
    }
    if (work->unk24 != 0) {
        unk2234 = reinterpret_cast<s32*>(work->unk24);
    }
    return true;
}

// Map name wnemSimple::resetAnimCmd: clears the interpreters of the three sub groups.
extern "C" void fn_41_31708(wnemSimple* wnem);

void wnemSimple::RequestDeactive() {
    m_requestDeactive = true;
}

// The glow module of the accesser (the accesser has no accessor for it).
static inline soGlowModule& wnemGlowModule(soModuleAccesser* moduleAccesser) {
    return *static_cast<soGlowModule*>(moduleAccesser->m_enumerationStart->m_glowModule);
}

extern "C" void fn_41_31708(wnemSimple* wnem) {
    soAnimCmdModule& animCmd = wnem->m_moduleAccesser->getAnimCmdModule();
    animCmd.resetInterpreter3(4, NULL, wnem->m_moduleAccesser);
    animCmd.resetInterpreter3(2, NULL, wnem->m_moduleAccesser);
    animCmd.resetInterpreter3(3, NULL, wnem->m_moduleAccesser);
}

bool wnemSimple::executeAnimCmd(s32 cmd, s32 group, s32 isOneShot) {
    if (cmd != 0) {
        soAnimCmdModule& animCmd = m_moduleAccesser->getAnimCmdModule();
        if (isOneShot == 0) {
            if (group == 2) {
                animCmd.resetInterpreter3(group, reinterpret_cast<void*>(cmd), m_moduleAccesser);
            } else {
                animCmd.interpretCmd1(group, m_moduleAccesser, 1.0f);
            }
        } else {
            animCmd.resetInterpreter3(group, reinterpret_cast<void*>(cmd), m_moduleAccesser);
            animCmd.interpretCmd1(group, m_moduleAccesser, 1.0f);
            animCmd.resetInterpreter3(group, NULL, m_moduleAccesser);
        }
    }
}

bool wnemSimple::setProcFnc_HpZero(s32 id) {
    if (id == -1) {
        m_procFncHpZero = NULL;
    } else {
        m_procFncHpZero = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncHpZero != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::setProcFnc_LifeZero(s32 id) {
    if (id == -1) {
        m_procFncLifeZero = NULL;
    } else {
        m_procFncLifeZero = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncLifeZero != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::setProcFnc_TouchGround(s32 id) {
    if (id == -1) {
        m_procFncTouchGround = NULL;
    } else {
        m_procFncTouchGround = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncTouchGround != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::setProcFnc_HitShell(s32 id) {
    if (id == -1) {
        m_procFncHitShell = NULL;
    } else {
        m_procFncHitShell = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncHitShell != NULL) {
            return true;
        }
    }
    return false;
}

void wnemSimple::resetFrameCounter(s32 index) {
    if (index <= -1 || index >= 2) {
        return;
    }
    gfGameFrameCounter& counter = m_frameCounters[index];
    counter.reset();
    counter.start();
}

u32 wnemSimple::getFrameCounter(s32 index) {
    if (index <= -1 || index >= 2) {
        return 0;
    }
    return m_frameCounters[index].getCounter();
}

// Reduces the hit points; the weapon dies when they run out.
void wnemSimple::setDamage(float damage) {
    if (unk21D4 > lbl_41_rodata_E54) {
        unk21D4 -= damage;
        if (unk21D4 <= lbl_41_rodata_E54) {
            OnDead();
        }
    }
}

// HYPOTHESIS: the float work value the weapon's scaled values grow with (the Enemy's helpers use ValueFloat_LevelScale,
// 0x1100000c; the weapon's accesser reads the first float of the same group, Weapon's Float_Active_Global_Frame).
static const u32 WeaponValueFloat_Scale = 0x11000000;



// Map name wnemSimple::getValueInt / getValueFloat: the scaled value for the enemy of the accesser, as the Enemy's own
// helpers compute it (they keep address names because other units call them).
extern "C" s32 fn_41_33AD0(emScaledValue* value, soModuleAccesser* acc);
extern "C" s32 fn_41_33AD0(emScaledValue* value, soModuleAccesser* acc) {
    int result = value->m_base;
    float scale = soValueAccesser::getValueFloat(acc, WeaponValueFloat_Scale, 0);
    if (value->m_rate != 0.0f) {
        result = result + value->m_rate * scale;
    }
    return result;
}

extern "C" float fn_41_33B6C(emScaledValue* value, soModuleAccesser* acc);
extern "C" float fn_41_33B6C(emScaledValue* value, soModuleAccesser* acc) {
    float result = value->m_base;
    float scale = soValueAccesser::getValueFloat(acc, WeaponValueFloat_Scale, 0);
    if (value->m_rate != 0.0f) {
        result += value->m_rate * scale;
    }
    return result;
}

extern const wnemDummy_Param lbl_41_data_3E50;

struct wnemVec3Init {
    float x, y, z;
};

struct wnemVec2Init {
    float x, y;
};

struct wnemHitDataInit {
    wnemVec3Init startOffsetPos;
    wnemVec3Init endOffsetPos;
    float size;
    u32 flags;
};

struct wnemGroundDataInit {
    const wnemVec2Init* points;
    s32 count;
    bool unk8;
    bool unk9;
};

static u8 s_modelLoupeNo = 0xff;
static wnemHitDataInit s_defaultHitData[2] = {
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 8.0f, 0x00090000 },
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 8.0f, 0x00090000 }
};
static wnemHitDataInit s_nullHitData[2] = {
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0x000F0000 },
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0x000F0000 }
};
struct wnemHitDataSetInit {
    wnemHitDataInit* elements;
    s32 size;
};
static wnemHitDataSetInit s_defaultHitDataSet = { s_defaultHitData, 2 };
static wnemVec2Init s_groundPts[5] = {
    { 0.0f, 1.0f }, { -2.5f, -1.0f }, { -1.5f, 1.5f }, { 2.5f, -1.5f }, { 1.5f, 0.0f }
};
static wnemGroundDataInit s_groundData = { s_groundPts, 5, false, true };

// Same layout as em/em_article_mediator.h. That header cannot be included here: its pool templates clash with the
// fighter builder's soInstancePoolInfo.
struct soArticleKindInfo {
    s32 m_category;
    s32 m_kind;
    soArticleKindInfo(s32 category, s32 kind) : m_category(category), m_kind(kind) { }
};
struct soKindInfo {
    s32 m_category;
    s32 m_kind;
#pragma push
#pragma dont_inline on
    soKindInfo(s32 category, s32 kind) : m_category(category), m_kind(kind) { }
#pragma pop
};
struct soArticleConstructInfo {
    const soArticleKindInfo* m_kindInfo;
    void* m_heapModule;
    soArticleConstructInfo(const soArticleKindInfo& kindInfo, void* heapModule) : m_kindInfo(&kindInfo), m_heapModule(heapModule) { }
    wnemHeapModuleImpl* getHeapModule() const { return static_cast<wnemHeapModuleImpl*>(m_heapModule); }
    operator const wnArticleBuildInfo&() const { return reinterpret_cast<const wnArticleBuildInfo&>(*this); }
};

static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);
static u8 s_genSheetKindPad[0x18];
static soKindInfo s_kindInfo(2, 0);

#pragma auto_inline off
wnemSimple::wnemSimple() :
    utWeaponObjectBase(),
    wnWeaponBuilder<wnemSimple_ModuleAccesserBuildConfig>(
        "wnemSimple", 0,
        wnArticleBuildInfo(reinterpret_cast<const wnArticleKindInfo*>(&s_kindInfo), &g_wnemHeapModule),
        wnModuleAccesserBuildData(0x1a, reinterpret_cast<const u8*>(&lbl_41_data_3E50) + 8,
            static_cast<soHeapModule&>(g_wnemHeapModule).getInstanceHeap(),
            static_cast<soHeapModule&>(g_wnemHeapModule).getNWModelInstanceHeap(),
            static_cast<soHeapModule&>(g_wnemHeapModule).getNWMotionInstanceHeap(),
            0xa, -1, &g_soParamAccesserNull, 0, 0, reinterpret_cast<s32>(&s_groundData), 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), -1),
    soCollisionSearchEventObserver(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), -1),
    m_shotWork(),
    unk21B4(false),
    unk21B5(true),
    unk21B6(false),
    unk21B7(false),
    unk21B8(false),
    unk21B9(false),
    unk21BA(),
    unk21BB(false),
    m_procFncHpZero(NULL),
    m_procFncLifeZero(NULL),
    m_procFncTouchGround(NULL),
    m_procFncHitShell(NULL),
    m_linkId(0),
    m_lifeLimit(0),
    unk21D4(0.0f),
    unk21D8(0.0f),
    m_weaponType(0),
    m_maxHitPower(0.0f),
    m_attackStopMax(0.0f),
    unk21E8(-1),
    unk21EC(0),
    m_baseSpeed(0.0f, 0.0f),
    m_speed(0.0f, 0.0f),
    m_accel(0.0f, 0.0f),
    m_holder(NULL),
    m_procFnc(NULL),
    m_weaponKind(-1),
    unk2214(false),
    unk2215(false),
    unk2216(0),
    unk2217(false),
    unk2218(false),
    m_requestDeactive(false),
    m_isManaged(false),
    m_isFromPool(false),
    m_intParams(2, 0),
    m_floatParams(5, 0),
    m_cmdInterpreter(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), &m_cmdStack, 6, 1, -1.0f, ftAddressOf((u8)0)),
    m_cmdStack(0) {
    SetDefault();
}

wnemSimple::wnemSimple(s32 articleId, s16 manageId, const soArticleConstructInfo& info, const wnemDummy_Param* param) :
    utWeaponObjectBase(),
    wnWeaponBuilder<wnemSimple_ModuleAccesserBuildConfig>(
        "wnemSimple", articleId, reinterpret_cast<const wnArticleBuildInfo&>(info),
        wnModuleAccesserBuildData(0x1a, reinterpret_cast<const u8*>(param) + 8,
            info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            0xa, -1, &g_soParamAccesserNull, 0, 0, reinterpret_cast<s32>(&s_groundData), 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), -1),
    soCollisionSearchEventObserver(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), -1),
    m_shotWork(),
    unk21B4(false),
    unk21B5(true),
    unk21B6(false),
    unk21B7(false),
    unk21B8(false),
    unk21B9(false),
    unk21BA(),
    unk21BB(false),
    m_procFncHpZero(NULL),
    m_procFncLifeZero(NULL),
    m_procFncTouchGround(NULL),
    m_procFncHitShell(NULL),
    m_linkId(0),
    m_lifeLimit(0),
    unk21D4(0.0f),
    unk21D8(0.0f),
    m_weaponType(0),
    m_maxHitPower(0.0f),
    m_attackStopMax(0.0f),
    unk21E8(-1),
    unk21EC(0),
#ifdef WN_EM_SIMPLE_UNIT
    m_baseSpeed(0.0f, 0.0f),
#else
    unk21F0(),
#endif
    m_speed(0.0f, 0.0f),
    m_accel(0.0f, 0.0f),
    m_holder(NULL),
    m_procFnc(NULL),
    m_weaponKind(-1),
    unk2214(false),
    unk2215(false),
    unk2216(0),
    unk2217(false),
    unk2218(false),
    m_requestDeactive(false),
    m_isManaged(false),
    m_isFromPool(false),
    m_intParams(2, 0),
    m_floatParams(5, 0),
    m_cmdInterpreter(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), &m_cmdStack, 6, 1, -1.0f, ftAddressOf((u8)0)),
    m_cmdStack(0) {
    (void)manageId;
    SetDefault();
}
#pragma auto_inline on

// Defined after wnemSimple's constructor so the manage-id constructor stays a call.
soDamageEventObserver::soDamageEventObserver(short param1, s8 param2)
    : soEventObserver<soDamageEventObserver>(0x6) {
    initialize(param1, param2);
}

void soDamageEventObserver::addObserver(short param1, s8 param2) {
    addObserverSub(param1, this, param2);
}

soDamageEventObserver::~soDamageEventObserver() { }

soCollisionSearchEventObserver::soCollisionSearchEventObserver(short manageId, s8 param2)
    : soEventObserver<soCollisionSearchEventObserver>(0x11) {
    initialize(manageId, param2);
}

void utWeaponObjectBase::setDeactivate(bool) { }

gfGameFrameCounter::gfGameFrameCounter() : m_startFrame(0xFFFFFFFF), m_latestStopFrame(0), m_stoppedFrames(0) { }

wnemSimple_Data::wnemSimple_Data() : m_param(NULL) { }

soArticleMediator::~soArticleMediator() { }

inline void wnemSimple::setDeactivate(bool x) {
    deactivate(x);
}

wnemSimple::~wnemSimple() { }

#pragma auto_inline off
void wnemSimple::SetDefault() {
    m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
    soSet<soCollisionHitData>* hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(&s_defaultHitDataSet);
    if (hitSet != NULL) {
        m_moduleAccesser->getCollisionHitModule().add(hitSet, 0);
    }
    m_moduleAccesser->getCollisionHitModule().setXluGlobal(0);
    *reinterpret_cast<s32*>(unk2368 + 0) = 0;
    *reinterpret_cast<void**>(unk2368 + 4) = unk2368;
    *reinterpret_cast<s32*>(unk2368 + 8) = 1;
    soEffectModuleImpl& eff = dynamic_cast<soEffectModuleImpl&>(m_moduleAccesser->getEffectModule());
    eff.unk50 = reinterpret_cast<s32>(unk2368 + 4);
    soAnimCmdAddressPackArraySeparate pack(
        reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(g_soAnimCmdAddrArrayNull),
        reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(g_soAnimCmdAddrArrayNull),
        reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(g_soAnimCmdAddrArrayNull));
    soAttributeFlag attr(0x10);
    soAnimCmdControlUnit unit;
    unit.m_animCmdInterpreter = &m_cmdInterpreter;
    unit.m_animCmdAddressPackArraySeparate = &pack;
    m_moduleAccesser->getAnimCmdModule().registInterpreter(unit, attr);
    Weapon::deactivate(false);
    wnemByteAt(this, 0x21B4) = 1;
}
#pragma auto_inline on

extern "C" void fn_41_3091C(wnemSimple* article, wnemSimple_Data* work, int founderTaskId, int resourceId,
                            int arg4, bool flag, Vec3f* pos, float lr, int team, float scale,
                            int, u32 life, s16 manageId) {
    struct wnemShotDataCopy {
        emShotParam* m_param;
        u8 unk4[2];
        bool m_unk6;
        u32 m_nodeId;
        u32 m_rot[3];
        u32 m_vel[2];
        u32 m_acc[2];
        void* m_constantParam;
        void* m_addressPack;
    };
    *reinterpret_cast<wnemShotDataCopy*>(&article->m_shotWork) = *reinterpret_cast<const wnemShotDataCopy*>(work);
    article->m_procFnc = NULL;
    article->m_weaponKind = -1;
    article->m_maxHitPower = 0.0f;
    article->m_attackStopMax = 0.0f;
    for (s32 i = 0; i < 2; i++) {
        article->resetFrameCounter(i);
    }
    article->setupParam(article->m_shotWork.m_param,
                        reinterpret_cast<emWeaponParamWork*>(article->m_shotWork.unk4), lr);
    article->StageObject::activate(pos, lr, 0.0f, true);
    wnemReport("-------------------------------------------------------------- \n");
    wnemReport("----- wnemSimple::activate ----- tskID( %u ), linkNo( %d ) === \n",
               article->getTaskId(), arg4);
    wnemReport("-------------------------------------------------------------- \n\n");
    if (*reinterpret_cast<void**>(article->m_shotWork.unk2C) != NULL) {
        dynamic_cast<soAnimCmdModuleImpl&>(article->m_moduleAccesser->getAnimCmdModule())
            .unk44(6, *reinterpret_cast<void**>(article->m_shotWork.unk2C));
    }
    article->m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
    if (article->m_weaponType == 1) {
        article->m_moduleAccesser->getSituationModule().setKind(Situation_Air, true);
    }
    if (reinterpret_cast<wnemShotParamView*>(work->m_param)->m_groundData != NULL) {
        article->m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
    wnemResourceModuleImpl& resMod =
        dynamic_cast<wnemResourceModuleImpl&>(article->m_moduleAccesser->getResourceModule());
    resMod.setGroupNo(static_cast<u8>(static_cast<u8>(article->m_shotWork.m_param->m_weaponId) + 10), 0xff);
    resMod.getResourceIdAccesser()->setResIdPacked(resourceId);

    u32 modelFlag = (reinterpret_cast<wnemShotParamView*>(article->m_shotWork.m_param)->m_modelFlags | 0x10) & ~0x7000;
    if (article->m_moduleAccesser->getColorBlendModule().isNull() == false) {
        modelFlag |= 0x108;
    }
    u32 resIdx = 2;
    if (static_cast<u32>(resourceId) != 0xffff) {
        resIdx = 1;
    }
    article->m_moduleAccesser->getModelModule().constructInstance(
        static_cast<gfSceneRoot::LayerType>(2),
        modelFlag, false, resIdx,
        static_cast<void*>(NULL), NULL, NULL, 0xffff, NULL, s_modelLoupeNo, 0);
    article->m_moduleAccesser->getModelModule().activate();

    void* motionData = reinterpret_cast<wnemShotParamView*>(work->m_param)->m_motionData;
    if (motionData != NULL) {
        article->m_moduleAccesser->getMotionModule().setData(motionData);
    } else {
        article->m_moduleAccesser->getMotionModule().setData(NULL);
    }

    if (reinterpret_cast<wnemShotParamView*>(work->m_param)->m_groundData != NULL) {
        article->setupGroundData();
    } else {
        wnemReport("wnemSimple::activate : _pGoundShapeData is null....clear data \n");
        article->m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
    }

    article->setupCollisionHitData(pos->m_x, article->m_moduleAccesser->getPostureModule().getScale(),
                                   article->m_moduleAccesser->getSituationModule().getKind(), lr);
    article->setupCollisionHitStatus();

    u32 nodeId = article->m_shotWork.m_nodeId;
    u32 constraintNode = reinterpret_cast<wnemShotParamView*>(article->m_shotWork.m_param)->m_constraintNode;
    if (constraintNode == 0xFFFFFFFFu || nodeId == 0xFFFFFFFFu) {
        *reinterpret_cast<s32*>(article->unk2368 + 0) = 0;
    } else {
        *reinterpret_cast<s32*>(article->unk2368 + 0) = static_cast<s32>(constraintNode);
    }
    soEffectModuleImpl& eff = dynamic_cast<soEffectModuleImpl&>(article->m_moduleAccesser->getEffectModule());
    eff.unk50 = reinterpret_cast<s32>(article->unk2368 + 4);

    article->m_moduleAccesser->getWorkManageModule().setInt(founderTaskId, 0x10000005);
    if (static_cast<u32>(founderTaskId) != 0xFFFFFFFF) {
        if (!article->m_moduleAccesser->getLinkModule().link(3, founderTaskId)) {
            return;
        }
        article->setupGlowModule();
        if (flag == true) {
            article->constraint();
        }
    }
    if (team != -1) {
        article->m_moduleAccesser->getTeamModule().setTeam(team, true);
        article->m_moduleAccesser->getTeamModule().setHitTeam(team);
        article->m_moduleAccesser->getTeamModule().setTeamOwnerId(founderTaskId);
    }
    if (manageId > -1) {
        article->setManageId(manageId);
    }
    article->m_moduleAccesser->getWorkManageModule().setFloat(static_cast<float>(life), WeaponValueFloat_Scale);
    article->m_moduleAccesser->getModelModule().setScale(scale);
    article->m_moduleAccesser->getPostureModule().setRot(&article->m_shotWork.m_rotation, 0);
    if (wnemByteAt(article, 0x21B7)) {
        Vec3f rot(0.0f, 0.0f, 0.0f);
        bool isLeft = false;
        float vx = article->m_baseSpeed.m_x;
        if (vx < 0.0f) {
            isLeft = true;
        }
        rot.m_z = static_cast<float>(atan2(article->m_baseSpeed.m_y, vx)) * 57.29578f;
        if (article->m_moduleAccesser->getPostureModule().getRotYLr() <= -90.0f) {
            rot.m_y = 180.0f;
        }
        if (isLeft == true) {
            rot.m_z += 180.0f;
            rot.m_y -= 180.0f;
        }
        article->m_moduleAccesser->getPostureModule().setRot(&rot, 0);
    }
    article->updateNodeSRT();
    s32 startCmd = reinterpret_cast<wnemShotParamView*>(article->m_shotWork.m_param)->m_startAnimCmd;
    if (startCmd != 0) {
        article->executeAnimCmd(startCmd, 4, 1);
    }
    wnemReport("wnemSimple::Activate : onExit \n");
}

// The weapon is switched off: the holder that started it lets go, the weapon leaves the manager's lists and goes back to
// the pool, and the proc function and effects are cleared.
void wnemSimple::onDeactivate() {
    // TODO: the flag at 0x21B4 lies inside the 0x34-byte emShot work block of the header (unk2C[4]); see updatePosture.
    if (unk21B4) {
        wnemReport("---- wnemSimple::onDeactivate : onEnter ----- \n");
        fn_41_31708(this);
        if (m_holder != NULL) {
            wnemReport("---- wnemSimple::onDeactivate : _pHolderArticle ----- \n");
            if (m_holder->isNull() == false) {
                m_holder->deactivateArticle();
            }
            m_holder = NULL;
        } else if (m_isManaged) {
            wnemReport("---- wnemSimple::onDeactivate : UnregisterManageWeapon ----------- \n");
            emWeaponManager::getInstance()->UnregisterManageWeapon(getTaskId());
        }
        if (m_isFromPool) {
            wnemReport("---- wnemSimple::onDeactivate : UnregisterManageWeaponPool ----------- \n");
            emWeaponManager::getInstance()->releaseObject(this);
            m_isFromPool = false;
        }
        m_procFnc = NULL;
        m_weaponKind = -1;
        m_moduleAccesser->getEffectModule().removeAll(0);
    }
}

// Shapes the weapon's ground module after the ground data of its shot description.
void wnemSimple::setupGroundData() {
    soGroundModuleImpl& ground = dynamic_cast<soGroundModuleImpl&>(m_moduleAccesser->getGroundModule());
    wnemGroundShapeData* data = static_cast<wnemGroundShapeData*>(reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_groundData);
    ground.setShapeKind(static_cast<soGroundShapeImpl::ShapeKind>(static_cast<u8>(data->m_kind)), 0);
    data = static_cast<wnemGroundShapeData*>(reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_groundData);
    s32 kind = data->m_kind;
    if (kind == 2) {
        ground.modifyRhombusFollowNode(data->m_followNode.m_up, data->m_followNode.m_down, data->m_followNode.m_width,
                                       data->m_followNode.m_node, 0);
        wnemReport("--- SO_GROUND_CORRECT_SHAPE_RHOMBUS_FOLLOW_NODE ----------- \n");
    } else if (kind == 1) {
        ground.modifyRhombus(data->m_rhombus.m_up, data->m_rhombus.m_down, data->m_rhombus.m_width, 0);
    }
}

// A weapon linked to a glowing owner (link connection 3) takes over the owner's glow values.
void wnemSimple::setupGlowModule() {
    if (m_moduleAccesser->getLinkModule().isParentGlow(3) == 1) {
        s32 attack = m_moduleAccesser->getLinkModule().getParentWeaponGlowAttack(3);
        wnemGlowModule(m_moduleAccesser).setGlowAttack(attack);
        s32 defense = m_moduleAccesser->getLinkModule().getParentWeaponGlowDefense(3);
        wnemGlowModule(m_moduleAccesser).setGlowDefense(defense);
        float eff = m_moduleAccesser->getLinkModule().getParentWeaponGlowEff(3);
        float damageMin = m_moduleAccesser->getLinkModule().getParentWeaponGlowDamageMin(3);
        float damageMax = m_moduleAccesser->getLinkModule().getParentWeaponGlowDamageMax(3);
        wnemGlowModule(m_moduleAccesser).setGlowEffect(eff, damageMin, damageMax);
        wnemReport("wnemSimple::setupGlowModule : atk( %d ), def( %d ), eff,min,max( %f, %f, %f ) \n", wnemGlowModule(m_moduleAccesser).getGlowAttack(),
                   wnemGlowModule(m_moduleAccesser).getGlowDefense(), eff, damageMin, damageMax);
    }
}

bool wnemSimple::executeAnimCmd(u32 index) {
    wnemAnimCmdSet* set = reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_animCmdSet;
    if (set == NULL) {
        wnemReport("[warning!!] wnemSimple::existAnimCmd : pParam->_pAnimCmdSet is null \n");
        return false;
    }
    if (index >= set->m_size) {
        wnemReport("wnemSimple::executeAnimCmd :[warning] index(%d) out og range _pAnimCmdSet._size(%d) \n ", index, set->m_size);
        return false;
    }
    void* cmd = set->m_data[index].m_cmd;
    if (cmd == NULL) {
        wnemReport("wnemSimple::executeAnimCmd :[warning] _pAnimCmdSet->_pData[ %d ]._pData is null \n", index);
        return false;
    }
    s32 kind = set->m_data[index].m_kind;
    s32 isOneShot = 0;
    if (kind == 4) {
        isOneShot = 1;
    }
    executeAnimCmd(reinterpret_cast<s32>(cmd), kind, isOneShot);
    return true;
}

// Map name wnemSimple::existAnimCmd: whether the command list of the index is set.
extern "C" bool fn_41_31960(wnemSimple* wnem, u32 index);
extern "C" bool fn_41_31960(wnemSimple* wnem, u32 index) {
    wnemAnimCmdSet* set = reinterpret_cast<wnemShotParamView*>(wnem->m_shotWork.m_param)->m_animCmdSet;
    if (set == NULL) {
        return false;
    }
    if (index >= set->m_size) {
        wnemReport("wnemSimple::existAnimCmd :[warning] index(%d) out og range _pAnimCmdSet._size(%d) \n ", index, set->m_size);
        return false;
    }
    return set->m_data[index].m_cmd != NULL;
}

// Binds the weapon's model to a node of its owner (link connection 3). The weapon type 2 (parts) also references the
// owner's facing.
void wnemSimple::constraint() {
    u32 ownerId = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    u32 nodeId = m_shotWork.m_nodeId;
    if (ownerId == 0xFFFFFFFFu || static_cast<u32>(reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_constraintNode) == 0xFFFFFFFFu || nodeId == 0xFFFFFFFFu) {
        wnemReport("wnemSimple::constraint : constraint failure \n");
        return;
    }
    if (m_weaponType == 2) {
        m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(9), true);
    }
    m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_constraintNode,
                                                  nodeId, 0xb, true);
    if (m_weaponType == 2) {
        m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(9), true);
        wnemReport("wnemSimple::constraint : setAttribute SO_LINK_ATTRIBUTE_REFERENCE_PARENT_LR \n");
    }
    m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
}

extern "C" void fn_41_31B34(wnemSimple* wnem);
// Map name wnemSimple::removeConstraint.
extern "C" void fn_41_31B34(wnemSimple* wnem) {
    wnemReport("wnemSimple::removeConstraint \n");
    if (!wnem->m_moduleAccesser->getLinkModule().isModelConstraint()) {
        wnemReport("[warning]wnemSimple::removeConstraint : isModelConstraint false \n");
        return;
    }
    wnem->m_moduleAccesser->getLinkModule().adjustModelConstraintPosture(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    wnem->m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    wnem->m_moduleAccesser->getLinkModule().unlink(3);
    wnem->updateNodeSRT();
    if (reinterpret_cast<wnemShotParamView*>(wnem->m_shotWork.m_param)->m_groundData != NULL) {
        if (wnem->m_moduleAccesser->getSituationModule().getKind() == Situation_Air) {
            if (static_cast<u8>(wnem->m_moduleAccesser->getGroundModule().getCorrect(0)) != soGroundShapeImpl::Correct_Air_Passable) {
                wnem->m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
            }
        } else {
            wnem->m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
        }
    }
}

bool wnemSimple::changeProcFnc(s32 id, s32 isForce) {
    if (isForce != 0 && static_cast<s32>(m_weaponKind) == id) {
        return true;
    }
    if (id == -1) {
        m_procFnc = NULL;
    } else {
        void* proc = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (proc != NULL) {
            m_weaponKind = id;
            return changeProcFnc1(proc, isForce);
        }
    }
    return false;
}

bool wnemSimple::changeProcFnc1(void* proc, s32 isForce) {
    if (isForce != 0 && m_procFnc == proc) {
        wnemReport("wnemSimple::changeProcFnc : already procFnc currentProcFnc \n");
        return true;
    }
    if (proc != NULL) {
        m_procFnc = proc;
        unk2214 = true;
        resetFrameCounter(1);
        fn_41_31708(this);
        m_moduleAccesser->getColorBlendModule().offAll(false);
        m_procFncHpZero = NULL;
        m_procFncLifeZero = NULL;
        m_procFncTouchGround = NULL;
        m_procFncHitShell = NULL;
        wnemReport("wnemSimple::changeProcFnc( in newProcFuncPtr ) : change new procFnc \n");
        return true;
    }
    return false;
}

// HYPOTHESIS: the item shot description (the Botron ball's item block, wnemSimple::unk2234 + 4): the model node of the
// item's start and its offset from that node.
struct wnemItemShootParam {
    u8 unk0[0x14];
    u32 m_nodeId;   // 0x14 of the block (0x18 of the ball's parameters)
    Vec3f m_offset;
};

// Map name wnemSimple::ShootItem: creates the item at the node of the weapon's model (the first call only preloads its
// archive), as the Enemy's emShootItem does.
extern "C" bool fn_41_320F8(wnemSimple* wnem, int kind, int variation, s32* param) {
    if (kind == -1) {
        return false;
    }
    if (itManager::getInstance()->isCompItemKindArchive(static_cast<itKind>(kind), 0, true) == false) {
        itManager::getInstance()->preloadItemKindArchive(static_cast<itKind>(kind), 0, itArchive::Temp, true);
        return true;
    }
    itManager* items;
    int ownerTaskId = wnem->m_moduleAccesser->getStageObject().m_taskId;
    items = itManager::getInstance();
    BaseItem* item = items->createItem(static_cast<itKind>(kind), variation, ownerTaskId);
    if (item != NULL) {
        wnemItemShootParam* shootParam = reinterpret_cast<wnemItemShootParam*>(reinterpret_cast<u8*>(param) + 4);
        soModelModule& model = wnem->m_moduleAccesser->getModelModule();
        Vec3f pos;
        Vec3f offset;
        offset.m_x = shootParam->m_offset.m_x;
        offset.m_y = shootParam->m_offset.m_y;
        offset.m_z = shootParam->m_offset.m_z;
        pos = model.getNodeGlobalPosition(shootParam->m_nodeId, &offset, false, false);
        item->warp(&pos);
        item->updateNodeSRT();
    } else {
        wnemReport("wnemSimple::ShootItem : --- field!! -------------- \n");
        return false;
    }
    wnemReport("wnemSimple::ShootItem : --- success!! -------------- \n");
    return true;
}

void wnemSimple::SetLifeLimit(s32 frames) {
    s32 now = static_cast<s32>(getFrameCounter(0));
    m_lifeLimit = now + frames;
    if (m_lifeLimit > 0) {
        if (!unk21B9) {
            unk21B9 = true;
            m_frameCounters[0].start();
        }
        wnemReport("wnemSimple::SetLifeLimit now(%d), aim(%d) \n", getFrameCounter(0), m_lifeLimit);
    }
}

// Checks the weapon's ground contacts, runs its proc function and ends it when its life or a wall touch says so.
// HYPOTHESIS: the touch masks 1/8 (floor and ceiling), 0xA4 (right wall) and 0x52 (left wall) are the ground module's.
void wnemSimple::updateStatusProcFunc() {
    bool touched = false;
    soGroundModuleImpl& ground = dynamic_cast<soGroundModuleImpl&>(m_moduleAccesser->getGroundModule());
    if (ground.isTouch(1, 0) == true || ground.isTouch(8, 0) == true) {
        touched = true;
        wnemReport("wnemSimple::updateStatusProcFunc : check Up/Down Wall \n");
    } else if (m_speed.m_x >= 0.025f) {
        if (ground.isTouch(0xA4, 0) == true) {
            wnemReport("wnemSimple::updateStatusProcFunc : check Right Wall \n");
            touched = true;
        }
    } else if (m_speed.m_x <= -0.025f) {
        if (ground.isTouch(0x52, 0) == true) {
            wnemReport("wnemSimple::updateStatusProcFunc : check Left Wall \n");
            touched = true;
        }
    }
    if (touched) {
        if (m_procFncTouchGround == NULL) {
            if (unk21BB) {
                wnemReport("wnemSimple::processFixPositionPreAnimCmd TouchGround > deactive \n");
                m_speed.m_y = 0.0f;
                m_speed.m_x = 0.0f;
                deactivate(false);
                return;
            }
        } else {
            wnemReport("wnemSimple::processFixPositionPreAnimCmd TouchGround > change pProcFunc_GroundTouch \n");
            changeProcFnc1(m_procFncTouchGround, 1);
            if (m_weaponType == 1) {
                wnemReport("wnemSimple::processFixPositionPreAnimCmd TouchGround > _delta.setZero() \n");
                m_speed.m_y = 0.0f;
                m_speed.m_x = 0.0f;
            }
        }
    }
    if (m_procFnc != NULL) {
        reinterpret_cast<void (*)(wnemSimple*, soModuleAccesser*)>(m_procFnc)(this, m_moduleAccesser);
        if (m_requestDeactive) {
            wnemReport("wnemSimple::updateStatusProcFunc Request Deactive in ProcFunc \n");
            deactivate(false);
            return;
        }
    }
    if (unk21B9) {
        u32 now = getFrameCounter(0);
        if (now >= static_cast<u32>(m_lifeLimit)) {
            if (m_procFncLifeZero == NULL) {
                wnemReport("wnemSimple::updateStatusProcFunc deactive LifeCnt \n");
                deactivate(false);
            } else {
                wnemReport("wnemSimple::updateStatusProcFunc *** call changeProcFnc[ LifeZero ] *** now(%d), aim(%d) \n", now,
                           m_lifeLimit);
                changeProcFnc1(m_procFncLifeZero, 1);
            }
        }
    }
}

// The weapon falls out of the stage: it is removed, and deactivated when it is still active.
void wnemSimple::processHit() {
    StageObject::processHit();
    m_maxHitPower = 0.0f;
    m_attackStopMax = 0.0f;
    if (unk21BA[0]) {
        if (!isConstraint(false)) {
            Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
            if (soGroundUtil::checkDeadArea(&pos, 0.0f, 0.0f) != -1) {
                setNotifyArticleEventRemove(m_moduleAccesser);
                if (isActive() == 1) {
                    wnemReport("wnemSimple::processHit : Dead OutScreen -- call deactivate \n");
                    deactivate(false);
                }
            }
        }
    }
}

// A part weapon (type 2) remembers the strongest hit of the frame and the hit stop it asked for.
void wnemSimple::notifyEventCollisionAttack(float power, const soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(collisionLog->m_taskCategory),
                                                                  collisionLog->m_taskId);
        switch (m_weaponType) {
        case 0:
        case 1:
            break;
        case 2:
            if (power > m_maxHitPower) {
                m_maxHitPower = power;
            }
            if (m_attackStopMax < power) {
                wnemReport(" wnemSimple::notifyEventCollisionAttack(TypeParts) : getCollisionAttackModule().getData( (int)pLog->_noSelf, false )->_noStop = %d \n",
                           m_moduleAccesser->getCollisionAttackModule().getData(collisionLog->m_damageIndex, false)->m_noHitStop);
                if (m_moduleAccesser->getCollisionAttackModule().getData(collisionLog->m_damageIndex, false)->m_noHitStop == 0) {
                    wnemReport(" wnemSimple::notifyEventCollisionAttack(TypeParts) : set _attackStopMax (%d) \n", m_attackStopMax);
                    m_attackStopMax = power;
                }
            }
            break;
        }
    }
}

void wnemSimple::OnDead() {
    wnemReport("wnemSimple::notifyEventOnDamage OnDead \n");
    m_moduleAccesser->getCollisionAttackModule().clearAll();
    m_moduleAccesser->getCollisionHitModule().setXluGlobal(0);
    if (unk21D4 <= lbl_41_rodata_E54) {
        unk21D4 = 0.0f;
    }
    if (m_procFncHpZero == NULL) {
        m_requestDeactive = true;
    } else {
        wnemReport("wnemSimple::OnDead changeProcFnc( _pProcFunc_HpZero ) \n");
        changeProcFnc1(m_procFncHpZero, 1);
    }
}

// Requests the common effect of the id: group 6 of the animation commands runs the effect table.
// HYPOTHESIS: the id names a common effect entry (the string says "reqCommonEffect : id").
void wnemSimple::reqCommonEffect(s32 id) {
    wnemReport("wnemSimple::reqCommonEffect : id = %d \n", id);
    m_moduleAccesser->getAnimCmdModule().resetInterpreter2(6, m_moduleAccesser, id, 0.0f);
    m_moduleAccesser->getAnimCmdModule().interpretCmd1(6, m_moduleAccesser, 0.0f);
}

void wnemSimple::initialize() {
    Weapon::initialize();
}

soKind wnemSimple::soGetKind() {
    return StageObject_Weapon;
}

bool wnemSimple::notifyEventCollisionSearchCheck() {
    return false;
}

void wnemSimple::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int p4,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    u32 targetTaskId = collisionLog->m_taskId;
    if (targetTaskId != 0xFFFFFFFF) {
        if (itManager::getInstance()->isSmashPlate(targetTaskId) == 1) {
            unk2217 = true;
        }
    }
    Weapon::notifyEventCollisionHit(power, attackData, index, p4, moduleAccesser, collisionLog);
}

void wnemSimple::notifyEventCollisionHit2nd(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex,
                                            float posX, float collisionLr, soModuleAccesser* moduleAccesser, bool flag) {
    Weapon::notifyEventCollisionHit2nd(attackModule, collisionLog, groupIndex, posX, collisionLr, moduleAccesser, flag);
    if (!flag && g_stTriggerMng != NULL) {
        const int& taskId = m_taskId;
        fn_27_23313C(m_moduleAccesser->getWorkManageModule().getInt(0x10000005), collisionLog->m_taskId, taskId, 0x28);
    }
}

bool wnemSimple::notifyEventAnimCmd(acAnimCmd* animCmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (Weapon::notifyEventAnimCmd(animCmd, moduleAccesser, unk3)) {
        return true;
    }
    if (!isObserv(animCmd->getGroup())) {
        return false;
    }
    if (animCmd->getType() <= -1 || animCmd->getType() >= 0xb) {
        return false;
    }
    return true;
}

// Moves the weapon by its speed (the speed grows by the acceleration) and, for a weapon that turns to its flight
// direction, rotates it around z by the angle of the speed (flipped around y while it flies to the left).
void wnemSimple::updatePosture(bool isUpdate) {
    if (isUpdate) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        pos.m_x += m_speed.m_x;
        pos.m_y += m_speed.m_y;
        m_speed.m_x += m_accel.m_x;
        m_speed.m_y += m_accel.m_y;
        m_moduleAccesser->getPostureModule().setPos(&pos);
        // TODO: the flag at 0x21B7 lies inside the 0x34-byte emShot work block of the header (unk2C[7]); the block is
        // really 0x30 bytes (wnemSimple's constructor writes the flags 0x21B4-0x21B7 itself).
        if (unk21B7) {
            Vec3f rot(0.0f, 0.0f, 0.0f);
            bool isLeft = false;
            float speedX = m_speed.m_x;
            if (speedX < 0.0f) {
                isLeft = true;
            }
            rot.m_z = static_cast<float>(atan2(m_speed.m_y, speedX)) * 57.29578f;
            if (m_moduleAccesser->getPostureModule().getRotYLr() <= -90.0f) {
                rot.m_y = 180.0f;
            }
            if (isLeft == true) {
                rot.m_z += 180.0f;
                rot.m_y -= 180.0f;
            }
            m_moduleAccesser->getPostureModule().setRot(&rot, 0);
        }
    }
}

// Sets the invincibility of the weapon's hit module: the weapon is hittable only when it asked for hit status (the
// byte at 0x21B8) and its description keeps it.
void wnemSimple::setupCollisionHitStatus() {
    soCollisionHitModule& hitModule = m_moduleAccesser->getCollisionHitModule();
    if (unk21B8) {
        if (reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_resetHitStatus != 0) {
            hitModule.resetStatusGlobal(0);
            hitModule.resetStatusAll(0);
            hitModule.cancelXluGlobal(0);
        } else {
            hitModule.setXluGlobal(0);
        }
    } else {
        hitModule.setXluGlobal(0);
    }
}

// Runs the animation commands of the weapon (the sub groups 0x1d while it moves, 0x10 while it is stopped), keeps the
// ground correction of its shape in line with its situation and then runs the proc function.
void wnemSimple::processFixPositionPreAnimCmd() {
    // TODO(nonmatching): the original loads 1.0f before the address of the second branch's attribute (scheduling).
    if (m_moduleAccesser->getStopModule().isStop() == false) {
        soAttributeFlag attr(0x1d);
        soModuleAccesser* acc = m_moduleAccesser;
        soSlowModule* slow = &acc->getSlowModule();
        acc->getAnimCmdModule().interpretCmd(acc, attr, slow->getRate());
    } else {
        m_moduleAccesser->getAnimCmdModule().interpretCmd(m_moduleAccesser, soAttributeFlag(0x10), 1.0f);
    }
    if (reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_groundData != NULL) {
        if (static_cast<u8>(m_moduleAccesser->getGroundModule().getCorrect(0)) != soGroundShapeImpl::Correct_None) {
            if (m_moduleAccesser->getSituationModule().getKind() == Situation_Air) {
                if (static_cast<u8>(m_moduleAccesser->getGroundModule().getCorrect(0)) != soGroundShapeImpl::Correct_Air_Passable) {
                    m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
                }
            } else {
                m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
            }
        }
    }
    if (m_moduleAccesser->getStopModule().isStop() == false) {
        updateStatusProcFunc();
    }
}

// A search hit by a reflecting reflector (kind 3) turns the weapon to its team; a weapon that did not reflect flags itself.
void wnemSimple::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        bool reflected = false;
        if (collisionLog->m_kind == 3 && moduleAccesser->getReflectModule().isReflect() == 1) {
            reflectSettingTeam();
            reflected = reflect();
        }
        if (reflected == false) {
            unk2218 = true;
        }
    }
}

// The weapon's model root node follows its posture (scale, rotation and position), unless the flag at 0x21B5 hands the
// node update to the stage object.
void wnemSimple::updateNodeSRT() {
    // TODO: the flag at 0x21B5 lies inside the emShot work block of the header (unk2C[5]); see updatePosture.
    if (unk21B5) {
        StageObject::updateNodeSRT();
    } else {
        soModuleAccesser* moduleAccesser = m_moduleAccesser;
        float scaleValue = moduleAccesser->getPostureModule().getScale();
        Vec3f scale;
        scale.m_x = scale.m_y = scale.m_z = scaleValue;
        Vec3f rotation;
        Vec3f position;
        position = moduleAccesser->getPostureModule().getPos();
        rotation = moduleAccesser->getPostureModule().getRot(0);
        moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
    }
}

// TODO: clean up, give variables real names
namespace jj_2F1AC {
extern "C" void fn_27_C9D44(void* self, short arg);
extern "C" void __dl__FPv(void* p);

extern "C" void* fn_41_2F1AC(void* self, short del) {
    if (self != 0) {
        fn_27_C9D44(self, -1);
        if (del > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_2F810 {
extern "C" void* __dt__17soSoundModuleImplFv(void*, short);
extern "C" void* __dt__30soSound3dGeneratorAccesserImplFv(void* p, short size);

extern "C" void* fn_41_2F810(void* p, short flag) {
    if (p) {
        __dt__17soSoundModuleImplFv(static_cast<unsigned char*>(p) + 0xC, -1);
        __dt__30soSound3dGeneratorAccesserImplFv(p, -1);
        if (flag > 0) {
            operator delete(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_31308 {
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4(float, float, int, float, int);
 virtual void slot5();
 virtual void slot6();
 virtual void slot7();
 virtual void slot8();
 virtual void slot9();
 virtual void slot10();
 virtual void slot11();
 virtual void slot12(int, void*, int);
};
struct Entry { char bytes[0x20]; };
struct List { Entry* entries; int count; };
struct Data { char pad[0x30]; List* list; };
extern int lbl_41_data_405C;
extern "C" void fn_41_31308(char* self, float a, float b, int arg, float c) {
 Object_vptr_0* obj = *(Object_vptr_0**)(*(char**)(*(char**)(self + 0x6c) + 0xd8) + 0x20);
 if ((*(Data**)(self + 0x2184))->list) {
  obj->slot12(0, (*(Data**)(self + 0x2184))->list->entries, 0);
  if ((*(Data**)(self + 0x2184))->list->count >= 2)
   obj->slot12(1, (*(Data**)(self + 0x2184))->list->entries + 1, 0);
  else
   obj->slot12(1, &lbl_41_data_405C, 0);
 } else {
  obj->slot12(0, &lbl_41_data_405C, 0);
  obj->slot12(1, &lbl_41_data_405C, 0);
 }
 obj->slot4(a, b, arg, c, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_32D1C {
class soAnimCmdEventObserver {public:
virtual void addObserver();
virtual void slot1();
virtual void slot2();
virtual void slot3();
virtual void slot4();
virtual void onDamage__18emDamageModuleImplFUl();
virtual void isCheckGroundDamage__18soDamageModuleImplFv();
virtual void onGroundDamage__18soDamageModuleImplFv();
virtual void getReactionSub__18soDamageModuleImplFP21soCollisionAttackDataUli();
virtual void getReactionMul__18emDamageModuleImplFP21soCollisionAttackDataUli();
virtual void getWeightReactionMul__18soDamageModuleImplFP21soCollisionAttackData();
virtual void getDamageMul__18soDamageModuleImplFv();
virtual void reqDamageEffect__18soDamageModuleImplFv();
virtual void isObserv__18emDamageModuleImplFc();
virtual void notifyEventAnimCmd__18emDamageModuleImplFP9acAnimCmdP16soModuleAccesseri();
virtual void notifyEventCollisionHit__18emDamageModuleImplFfP21soCollisionAttackDataUliP16soModuleAccesserP14soCollisionLog();
};
class soNullable {public:
virtual void isNull();
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
virtual int slot15();
};
struct Message { int id; unsigned char result; int type; };
class Class_6E20 {public:
virtual void __dt__24wnemDamageTransactorImplFv();
virtual void getDamageValueParam__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void onDamageChangeStatusRequest__24wnemDamageTransactorImplFiP16soModuleAccesserP11soDamageLog();
virtual void getDamageStatusKind__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void isUseTurnDamage__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void isUseTurn__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void isApplyTurnDamage__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void getDamageHeight__24wnemDamageTransactorImplFP16soModuleAccesserUc();
virtual void getHitStopMul__23soDamageTransactorActorFP16soModuleAccesser();
virtual void isSlip__24wnemDamageTransactorImplFP16soModuleAccesserf();
virtual void isSleepStatus__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void isParalyzeDamage__24wnemDamageTransactorImplFP16soModuleAccesser();
virtual void addSleepTime__24wnemDamageTransactorImplFP16soModuleAccesserP8soDamageP11soDamageLog();
virtual void onFlowerDamage__24wnemDamageTransactorImplFP16soModuleAccesserP8soDamage();
virtual void onParalyzeDamage__24wnemDamageTransactorImplFP16soModuleAccesserP8soDamageP11soDamageLog();
virtual void setFlagDownDamage3__24wnemDamageTransactorImplFP16soModuleAccesserb(int, Message*);
};
struct Prefix8 { char padding[8]; };
class ReflectModule : public Prefix8 {public:
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
virtual unsigned int test();
};
struct Modules {
char pad0[0x1c];
soAnimCmdEventObserver* m1c;
char pad20[0x44-0x20];
soNullable* m44;
char pad48[0x54-0x48];
Class_6E20* m54;
char pad58[0xbc-0x58];
ReflectModule* mbc;
};
struct Accessor { char pad[0xd8]; Modules* modules; };
struct Prefix3C { char padding[0x3c]; };
class Inner : public Prefix3C {public:
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
virtual void slot115();
virtual void slot116();
virtual void slot117();
virtual void slot118();
virtual void slot119();
virtual void slot120();
virtual void slot121();
virtual void slot122();
virtual void slot123();
virtual void slot124();
virtual void slot125();
virtual void slot126();
virtual void slot127();
virtual void slot128();
virtual void slot129();
virtual void slot130();
virtual void slot131();
virtual void slot132();
virtual void slot133();
virtual void slot134();
virtual void slot135();
virtual void slot136();
virtual void reverse();
const float& getValue() const { return *(const float*)((const char*)this + 0x21d8); }
};
class Object : public Prefix8 {public:
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
virtual void slot115();
virtual void slot116();
virtual void slot117();
virtual void slot118();
virtual void slot119();
virtual void slot120();
virtual void slot121();
virtual void slot122();
virtual void slot123();
virtual void slot124();
virtual void slot125();
virtual void slot126();
virtual void slot127();
virtual void slot128();
virtual void slot129();
virtual void slot130();
virtual void slot131();
virtual void slot132();
virtual void slot133();
virtual void slot134();
virtual void slot135();
virtual void slot136();
virtual void slot137();
virtual void slot138();
virtual void slot139();
virtual void slot140();
virtual void slot141();
virtual void slot142();
virtual void slot143();
virtual void slot144();
virtual void slot145();
virtual void slot146();
virtual void slot147();
virtual void slot148();
virtual void slot149();
virtual void slot150();
virtual void slot151();
virtual void slot152();
virtual void slot153();
virtual void slot154();
virtual void slot155();
virtual void slot156();
virtual void slot157();
virtual void slot158();
virtual void slot159();
virtual void slot160();
virtual void slot161();
virtual void slot162();
virtual void slot163();
virtual void slot164();
virtual void slot165();
virtual void slot166();
virtual void slot167();
virtual void slot168();
virtual void slot169();
virtual void slot170();
virtual void slot171();
virtual int finish();
Inner inner;
char pad4c[0x6c-0x4c];
Accessor* accesser;
char pad70[0x21b6-0x70];
unsigned char flag21b6;
char pad21b7[0x21c8-0x21b7];
unsigned int proc;
char pad21cc[0x21dc-0x21cc];
int state;
char pad21e0[4];
float value;
char pad21e8[0x2215-0x21e8];
unsigned char flag2215;
char pad2216[3];
unsigned char flag2219;
};
extern "C" {
extern char lbl_41_data_4018[];
extern const float lbl_41_rodata_E28;
extern const float lbl_41_rodata_E30;
void reqCommonEffect__10wnemSimpleFl(Object*, int);
int changeProcFnc1__10wnemSimpleFPvl(Object*, unsigned int, int);
void setHitStop__6WeaponFfif(Inner*, float, int, float);
}
extern "C" int fn_41_32D1C(Object* self, unsigned int flags) {
char* base = lbl_41_data_4018;
switch (self->state) {
case 0: break;
case 1:
if (flags & 0x17) {
wnemReport(base + 0xc1a);
self->flag2215 = 1;
if (flags & 1) {
wnemReport(base + 0xc6a);
reqCommonEffect__10wnemSimpleFl(self, 0x17);
self->flag2219 = 1;
return 1;
}
if (flags & 0x10) {
wnemReport(base + 0xce3);
self->flag2219 = 1;
return 1;
}
if (self->proc == 0) {
if (self->flag21b6 == 0) {
self->flag2219 = 1;
wnemReport(base + 0xd55);
}
} else {
self->accesser->modules->m1c->slot4();
changeProcFnc1__10wnemSimpleFPvl(self, self->proc, 1);
wnemReport(base + 0xdb0);
}
return 1;
}
if (flags & 8) {
if (self->accesser->modules->mbc->test() == 1) {
self->inner.reverse();
return self->finish();
}
}
break;
case 2:
if (flags & 0x17) {
wnemReport(base + 0xe09);
self->flag2215 = 1;
if (flags & 1) {
wnemReport(base + 0xe59);
self->accesser->modules->m1c->slot4();
return 1;
}
if (self->value > lbl_41_rodata_E28) {
setHitStop__6WeaponFfif(&self->inner, self->inner.getValue(), 0, lbl_41_rodata_E30);
if (self->accesser->modules->m44->slot15() > 0 && self->state == 2) {
Message msg;
msg.id = 0x13f7;
msg.result = 0;
msg.type = 3;
self->accesser->modules->m54->setFlagDownDamage3__24wnemDamageTransactorImplFP16soModuleAccesserb(3, &msg);
if (msg.result == 1) {
wnemReport(base + 0xed2);
}
}
}
}
break;
}
return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_337E8 {
typedef unsigned int u32;
typedef unsigned char u8;

extern "C" {
extern float lbl_41_rodata_E28;
extern char lbl_41_data_4018[];
void wnemReport__FPCce(const char*, ...);
float getValueFloat__15soValueAccesserFP16soModuleAccesserUlUl(void*, u32, u32);
float mtCosf__Ff(float);
float mtSinf__Ff(float);
float fn_27_60AF4(void*, float, int, int, int*, float, float, float, float, float, int);
void setDamage__10wnemSimpleFf(void*, float);
}

struct Pair { int a; int b; };

struct SubObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual int vf3();
};

struct Vec4Obj {
    virtual void v0();
    virtual float* get(int);
    virtual void v2();
    virtual void v3();
};

struct WnemSelf {
    char pad[0x21b8];
    u8 f21b8;
    char pad2[0x21d4 - 0x21b9];
    float f21d4;
    float f21d8;
    char pad3[0x2216 - 0x21dc];
    u8 f2216;
    char pad4[0x224c - 0x2217];
    Vec4Obj obj224c;
};

extern "C" void fn_41_337E8(WnemSelf* self, char* arg1, void* dummy, void* accp) {
    char* base;
    float* rodata;

    rodata = &lbl_41_rodata_E28;
    base = lbl_41_data_4018;

    if (self->f21b8 == 0) return;

    wnemReport__FPCce(base + 0x1259);

    float f31 = *(float*)(arg1 + 0xc);
    float f30 = rodata[1] * *(float*)(arg1 + 0x7c);

    float f29 = getValueFloat__15soValueAccesserFP16soModuleAccesserUlUl(accp, 0x7d6, 0);
    float f28 = getValueFloat__15soValueAccesserFP16soModuleAccesserUlUl(accp, 0x7d8, 0);
    float f27 = getValueFloat__15soValueAccesserFP16soModuleAccesserUlUl(accp, 0x7d9, 0);
    float f26 = getValueFloat__15soValueAccesserFP16soModuleAccesserUlUl(accp, 0x7d7, 0);

    Pair pair = *(Pair*)(arg1 + 0x8c);

    SubObj* sub = *(SubObj**)((char*)*(void**)((char*)accp + 0xd8) + 0x14);
    int subval = sub->vf3();

    f26 = fn_27_60AF4(accp, f31, *(int*)(arg1 + 0x50), subval,
                      &pair.a, f30, f29, f28, f27, f26, 0);

    f31 = f30 * mtCosf__Ff(f26);
    self->obj224c.get(0)[0] = f31;

    f31 = mtSinf__Ff(f26);
    self->obj224c.get(1)[0] = f31;

    float* p2 = self->obj224c.get(2);
    *p2 = rodata[12];
    float* p3 = self->obj224c.get(3);
    *p3 = rodata[0];

    wnemReport__FPCce(base + 0x1289, f26);
    wnemReport__FPCce(base + 0x12be, f30);

    self->f2216 = 1;
    setDamage__10wnemSimpleFf(self, *(float*)(arg1 + 0x4));
    wnemReport__FPCce(base + 0x12f3, self->f21d4, self->f21d8,
                      *(float*)(arg1 + 0x4));
}
}

