#pragma once

// The module classes and module storage the Enemy class (em/enemy.h) embeds, other than those with a header of their own.
// - The soArrayVector<T, C> instances are classes of sora_melee: the Enemy unit only calls their constructors and
//   destructors (FT_DOL_ARRAY_VECTOR declares them without a definition).
// - The em*ModuleImpl classes are the enemy versions of the so* modules; their code is in later units of sora_enemy that
//   are not reconstructed yet, so only their size (the distance to the next member of Enemy), their base class (from the
//   module map names) and the constructor the Enemy constructor calls are declared here.

#include <StaticAssert.h>
#ifdef EM_ENEMY_BUILDER
// The Enemy unit (which defines EM_ENEMY_BUILDER before any include) calls the sora_melee constructors of these arrays.
// The other units that include em/enemy.h only need the layout, which the plain template gives: an explicit
// specialization there would come after the template was already instantiated by the headers they include first.
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_animcmd.h> // the anim cmd and motion tables of sora_melee
#include <ft/builder/ft_builder_motion.h>
#include <ft/builder/ft_builder_status.h> // soArrayQueueImpl
#else
#include <so/so_array_queue_impl.h>
#endif
#include <ft/builder/ft_dol_types.h>
#include <ac/ac_cmd_interpreter.h>
#include <so/article/so_article.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/templates/so_array_value_soTargetSearchRayCheckCache.h>
#include <so/transition/so_transition_module_impl.h>
#include <types.h>
#include <wn/wn_em_simple.h> // soArrayVector<s32, 2>

class emTargetSearchUnit;

#ifdef EM_ENEMY_BUILDER
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 11);
FT_DOL_ARRAY_VECTOR(soOtherAnim, 3);
FT_DOL_ARRAY_VECTOR(soLinkConnection, 5);
FT_DOL_ARRAY_VECTOR(soCollisionAttackAbsolute, 1);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 10);
FT_DOL_ARRAY_VECTOR(soCollisionCatchPart, 2);
FT_DOL_ARRAY_VECTOR(soShakeTerm, 2);
FT_DOL_ARRAY_VECTOR(soTargetSearchRayCheckCache, 8);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 6);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 7);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 12);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 16);
FT_DOL_ARRAY_VECTOR(soArticle*, 10);
#endif

// HYPOTHESIS: the elements of this array are 0x10 bytes in the original (the array is 0xAC bytes and the mangled name
// says soArticleEventObserver), while BrawlHeaders' soArticleEventObserver is 0xC; the element layout is not used here.
template <>
class soArrayVector<soArticleEventObserver, 10> : public soArrayVectorAbstract<soArticleEventObserver> {
    s32 m_topIndex : sizeof(bit_width<10>) + 1;
    s32 m_lastIndex : sizeof(bit_width<10>) + 1;
    s32 m_size : sizeof(bit_width<10>) + 1;
    u32 m_isFull : 1;
    u8 m_elements[10][0x10];
public:
    soArrayVector(s32 size, s32);
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soArticleEventObserver& atFastAbstractSub(s32 index) const;
    virtual soArticleEventObserver& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};

// The anim cmd control units of the enemy (one per interpreter). The constructor is expanded in the Enemy constructor
// (its list is a class of sora_melee), the destructor is a weak copy of the Enemy unit.
template <>
class soInstanceManagerFullPropertyUniqImpl<soAnimCmdControlUnit, 11> : public soInstanceManagerFullProperty<soAnimCmdControlUnit> {
    soArrayList<soInstanceUnitFullProperty<soAnimCmdControlUnit>, 11> m_arrayList; // 0x10
public:
#ifdef SO_ARRAY_LIST_CTOR_FLAG
    // The Enemy unit (which defines the flag) expands it; the list constructor is the one of sora_melee.
    soInstanceManagerFullPropertyUniqImpl() : m_arrayList(0) { }
#else
    soInstanceManagerFullPropertyUniqImpl();
#endif
    virtual soAnimCmdControlUnit& at(s32 index);
    virtual soAnimCmdControlUnit& atIndex(s32 index);
    virtual s32 getId(s32 index);
    virtual u32 size() const;
    virtual bool isContain(s32) const;
    virtual void erase(s32);
    virtual void clear();
    virtual void set(const soAnimCmdControlUnit&, s32 index);
    virtual s32 add(soAnimCmdControlUnit&, s32, soAttributeFlag, s16);
    virtual u32 capacity();
    virtual soInstanceUnitFullProperty<soAnimCmdControlUnit>& atUnitIndexFast(s32 index);
    virtual s32 getIndex(s32 index) const;
    virtual void getAttributeArray(soAttributeFlag mask, soArray<soAnimCmdControlUnit*>& arr);
    virtual soAttributeFlag getAttribute(s32) const;
    virtual void getPriorityArray(soArray<soAnimCmdControlUnit*>& arr);
};
static_assert(sizeof(soInstanceManagerFullPropertyUniqImpl<soAnimCmdControlUnit, 11>) == 0x120, "Class is wrong size!");

// The transition module of the enemy status module: the module followed by its GroupNum term groups of TermCap terms each.
// Built inline by the Enemy constructor; its destructor is a weak copy of the Enemy unit (map: soTransitionModuleEntity<3,1>).
template <int TermCap, int GroupNum>
class soTransitionModuleEntity : public soTransitionModuleImpl {
    soArrayVector<soTransitionTermGroup, GroupNum> m_groups;                       // +0x18
    soArrayVector<soInstanceUnitFullProperty<soTransitionTerm>, TermCap> m_terms[GroupNum]; // +0x38
public:
    soTransitionModuleEntity() : soTransitionModuleImpl(&m_groups), m_groups(0) {
        for (u32 i = 0; i < GroupNum; i++) {
            soTransitionTermGroup group(&m_terms[i]);
            soArray<soTransitionTermGroup>* groups = &m_groups;
            groups->push(group);
        }
    }
    virtual ~soTransitionModuleEntity() { }
};

// ---- modules of sora_melee without a header ----------------------------------------------------------------------------

// HYPOTHESIS: class name. The target search module (soModuleAccesser slot targetSearchModule), built from the enemy's
// target search units and parameters (emInfo::getTargetSearchNum / getTargetSearchParamAryPtrGetId) and the ray-check cache.
class soTargetSearchModuleImpl {
    u8 unk4[0x34 - 4];
public:
    soTargetSearchModuleImpl(soModuleAccesser* acc, emTargetSearchUnit* units, void* params, int unitNum,
                             soArray<soTargetSearchRayCheckCache>* rayCheckCache);
    virtual ~soTargetSearchModuleImpl();
};
static_assert(sizeof(soTargetSearchModuleImpl) == 0x34, "Class is wrong size!");

// HYPOTHESIS: class name. The glow module (soModuleAccesser slot glowModule) of the enemy; Enemy::resetGlowParam drives it.
class soGlowModuleImpl {
    u8 unk4[0x24 - 4];
public:
    soGlowModuleImpl(soModuleAccesser* acc);
    virtual ~soGlowModuleImpl();
};
static_assert(sizeof(soGlowModuleImpl) == 0x24, "Class is wrong size!");

// ---- enemy modules (code in later sora_enemy units) ---------------------------------------------------------------------

class emGroundModuleImpl : public soGroundModuleImpl {
    u8 unk58[0x8C - 0x58];
public:
    // HYPOTHESIS: groundData is the enemy's ground parameter block (a soValueAccesser constant).
    emGroundModuleImpl(soModuleAccesser* acc, soArray<soGroundShapeImpl>* shapes, void* groundData);
    virtual ~emGroundModuleImpl();
    // HYPOTHESIS: on lets the ground shape follow the model's nodes (anim cmd 39, enemies with a ground parameter block).
    void updateVtxFromNode(bool on);
};
static_assert(sizeof(emGroundModuleImpl) == 0x8C, "Class is wrong size!");

// soLinkModuleImpl has its full layout only under FT_MODULE_BUILDER (see em/enemy.h).
#ifdef FT_MODULE_BUILDER
class emLinkModuleImpl : public soLinkModuleImpl {
    u8 unk50[0x84 - 0x50];
public:
    emLinkModuleImpl(soModuleAccesser* acc, soArray<soLinkConnection>* connections);
    virtual ~emLinkModuleImpl();
    void initConstraintOffsetMtx();
    // HYPOTHESIS: the node the constraint offset is taken from (Enemy::notifyEventCaptureStatus passes the capture node).
    void setConstraintOffsetMtx(int node);
};
static_assert(sizeof(emLinkModuleImpl) == 0x84, "Class is wrong size!");
#else
class emLinkModuleImpl;
#endif

class emCollisionHitModuleImpl : public soCollisionHitModuleImpl {
    u8 unk68[0x70 - 0x68];
public:
    // HYPOTHESIS: hitData and hitGroupData are the enemy's hit parameter blocks (soValueAccesser constants).
    emCollisionHitModuleImpl(soModuleAccesser* acc, int taskId, u8 category, soArray<soCollisionHitPart>* parts,
                             soArray<soCollisionGroup>* groups, soArray<soCollisionHitGroup>* hitGroups, void* hitGroupData,
                             void* hitData);
    virtual ~emCollisionHitModuleImpl();
};
static_assert(sizeof(emCollisionHitModuleImpl) == 0x70, "Class is wrong size!");

class emStatusModuleImpl : public soStatusModuleImpl {
    u8 unkAC[0xB4 - 0xAC];
public:
    // HYPOTHESIS: the three trailing values mirror soStatusModuleImpl's preCheck data, status kind count and flag.
    emStatusModuleImpl(soModuleAccesser* acc, soArrayContractibleTable<const soStatusData>* statusTable, void* uniqProcs,
                       soGeneralWorkSimple* statusWork, soTransitionModule* transition, void* changeRequests, void* preCheckData,
                       int unk1, int unk2);
    virtual ~emStatusModuleImpl();
    // HYPOTHESIS: the update priority of the AI against the status (Enemy::processGameProc raises it to (-350, -250)
    // around the status transition check of a dead area hit, then puts it back).
    void setAIUpdatePri(int, int);
    void initAIUpdatePri();
};
static_assert(sizeof(emStatusModuleImpl) == 0xB4, "Class is wrong size!");

// The enemy territory module (soModuleAccesser slot territoryModule).
class emTerritoryModuleImpl {
    u8 unk4[0x50 - 4];
public:
    // HYPOTHESIS: territoryData is a soValueAccesser constant, createTerritory points into the emCreate (+0x28); the five ints
    // are the indices of the territory kinds.
    emTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5, int p6, int p7,
                          int p8);
    virtual ~emTerritoryModuleImpl();
};
static_assert(sizeof(emTerritoryModuleImpl) == 0x50, "Class is wrong size!");

class emDamageModuleImpl {
    u8 unk4[0xE0 - 4];
public:
    emDamageModuleImpl(soModuleAccesser* acc, soArray<soDamage>* damages);
    virtual ~emDamageModuleImpl();
    // The position the damage of the enemy is taken at (Enemy::getDamageOffsetFromNode).
    Vec3f getDamagePos();
};
static_assert(sizeof(emDamageModuleImpl) == 0xE0, "Class is wrong size!");

class emTurnModuleImpl {
    u8 unk4[0x4C - 4];
public:
    emTurnModuleImpl(soModuleAccesser* acc);
    virtual ~emTurnModuleImpl();
};
static_assert(sizeof(emTurnModuleImpl) == 0x4C, "Class is wrong size!");

// The kinetic energy parameters of an enemy kind (emInfo::getKineticEnergyParamAryPtrGetId / getKineticEnergyNum).
struct emKineticEnergyParamDesc {
    void* m_params;
    int m_num;
    emKineticEnergyParamDesc(void* params, int num) : m_params(params), m_num(num) { }
};

class emKineticModuleImpl : public soKineticModule {
    u8 unk[0xBD4 - sizeof(soKineticModule)];
public:
    emKineticModuleImpl(soModuleAccesser* acc, const emKineticEnergyParamDesc& energies);
    virtual ~emKineticModuleImpl();
    // HYPOTHESIS: the argument is an energy index (-1: all energies), as in soKineticModule::updateEnergy.
    void updateEnergyPre(int index);
    // HYPOTHESIS: the counterpart run after the posture update (Enemy::updatePosture passes -1).
    void updateEnergyPost(int index);
};
static_assert(sizeof(emKineticModuleImpl) == 0xBD4, "Class is wrong size!");
