// The history array of the module has the sora_melee constructor / destructor (see em/em_ai_module_impl.h).
#define EM_AI_MODULE_IMPL_BUILDER
// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
// (ft/ft_manager.h specializes soInstanceUnit<soEventUnit*>, so it comes before the event system headers.)
#include <ft/ft_manager.h>
#include <em/enemy.h>
#include <em/em_ai_module_impl.h>
#include <em/em_area_module_impl.h>
#include <em/em_manager.h>
#include <ac/ac_anim_cmd_impl.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_prng.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// TODO(shared header): statics of emAreaModuleImpl (module map names) that em/em_area_module_impl.h does not declare
// and whose symbols are not named yet; called through their addresses until both are (see the report).
extern "C" bool fn_41_23314(u32 area);                              // emAreaModuleImpl::isFighterTargetKind
extern "C" bool fn_41_23164(u32 area);                              // emAreaModuleImpl::isInvalidAreaTarget
extern "C" void fn_41_26430(soModuleAccesser* acc, u32 area);       // emAreaModuleImpl::setTargetFoundInfo
extern "C" float fn_41_24570(soModuleAccesser* acc, u32 area);      // emAreaModuleImpl::getTargetRealDistance

// TODO(shared header): the target search module (soModuleAccesser::m_targetSearchModule) has no declared interface; only
// the slot this unit calls is known. HYPOTHESIS: it tells whether target search unit id sees area.
class soTargetSearchModule {
public:
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual bool isTargetArea(int targetSearchId, int area);
};

// The values and work variables the AI talks to its anim cmds through. HYPOTHESIS: names.
static const u32 ConstantIndefinite_AIAreaRates = 0xA818; // float per area: the weight of picking it
static const u32 ValueFloat_AIKeepAreaRateMul = 0xBFD;    // the weight multiplier of the area kept from the last update
static const u32 ValueFloat_AIRndScale = 0x1100000C;
static const u32 WorkFlag_AIDecided = 0x22000004;
static const u32 WorkFloat_AIRndRate = 0x21000000;
static const u32 WorkInt_AIRndIndex = 0x20000006;

// The AI info getPrevAIInfo returns past the end of the history.
static emAIInfo s_nullAIInfo;

// The argument at index (a virtual call through the array interface, as in the Enemy unit).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

// TODO(shared header): Enemy::m_createId (0xB0) is private and em/enemy.h has no getter yet (patch in the report); this
// reads the member at its offset until the getter exists.
static inline u32 getEnemyCreateId(Enemy& enemy) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&enemy) + 0xB0);
}

static inline s16 getEventManageId(soModuleAccesser* acc) {
    return static_cast<soEventManager&>(acc->getEventManageModule()).getManageId();
}

emAIModuleImpl::emAIModuleImpl(soModuleAccesser* acc, emAIEntry* entries, void* aiData, int entryNum, int p5, int p6)
    : soAnimCmdEventObserver(getEventManageId(acc), static_cast<s8>(-1)),
      soCollisionAttackEventObserver(getEventManageId(acc), static_cast<s8>(-1)), m_moduleAccesser(acc),
      m_units(reinterpret_cast<emAIUnit*>(entries)), m_unitParams(static_cast<emAIUnitParam**>(aiData)),
      m_unitNum(entryNum), m_animCmdGroup(p5), m_rndAnimCmdGroup(p6), m_attackAreaTarget(0), m_isInactive(false),
      m_keepArea(-1), m_history(2, emAIInfo()) { }

emAIModuleImpl::~emAIModuleImpl() { }

bool emAIModuleImpl::isInvalid(int unitIndex) {
    if (unitIndex == -1) {
        return false;
    }
    if (unitIndex < 0) {
        return true;
    }
    return unitIndex >= m_unitNum;
}

void emAIModuleImpl::createUnitAll() {
    for (int i = 0; i < m_unitNum; i++) {
        m_units[i].create(m_unitParams[i]);
    }
}

void emAIModuleImpl::reset() {
    m_currentInfo.m_unitIndex = -2;
    m_currentInfo.unk4 = -1;
    m_history.getArray()->clear();
    m_attackAreaTarget = 0;
    m_isInactive = false;
    m_keepArea = -1;
    for (int i = 0; i < m_unitNum; i++) {
        m_units[i].reset();
    }
}

// Puts the units' flags back to their initial values (unitIndex -1: all of them).
void emAIModuleImpl::resetFlag(int unitIndex) {
    if (isInvalid(unitIndex) == true) {
        return;
    }
    if (unitIndex == -1) {
        for (int i = 0; i < m_unitNum; i++) {
            m_units[i].resetFlag();
        }
    } else {
        m_units[unitIndex].resetFlag();
    }
}

void emAIModuleImpl::setFlag(u8 flag, int unitIndex) {
    if (isInvalid(unitIndex) == true) {
        return;
    }
    if (unitIndex == -1) {
        for (int i = 0; i < m_unitNum; i++) {
            m_units[i].setFlag(flag);
        }
    } else {
        m_units[unitIndex].setFlag(flag);
    }
}

// The weight of area i for the random choice of update: its rate, scaled for the area kept from the last update.
static inline float getAreaRate(const float* rates, int keepArea, int i, float keepMul) {
    float rate = rates[i];
    if (keepArea == i) {
        rate *= keepMul;
        if (rate < 0.0f) {
            rate = 1.0f;
        }
    }
    return rate;
}

bool emAIModuleImpl::update(int priMin, int priMax) {
    if (m_isInactive == true) {
        return false;
    }
    emAIInfo info;
    float* areaRates =
        static_cast<float*>(soValueAccesser::getConstantIndefinite(m_moduleAccesser, ConstantIndefinite_AIAreaRates, 0));
    bool requested = false;

    // The unit taken for each area.
    int areaUnits[8];
    for (int i = 0; i < 8; i++) {
        areaUnits[i] = -2;
    }

    // Pick one of the fighter areas whose target exists, by their weights.
    int pickedArea = -1;
    int total = 0;
    float keepMul = soValueAccesser::getValueFloat(m_moduleAccesser, ValueFloat_AIKeepAreaRateMul, 0);
    for (int i = 0; i < 8; i++) {
        if (fn_41_23314(i) && emAreaModuleImpl::isExistTarget(m_moduleAccesser, i)) {
            total = total + getAreaRate(areaRates, m_keepArea, i, keepMul);
        }
    }
    if (total > 0) {
        int rnd = randi(total);
        int sum = 0;
        for (int i = 0; i < 8; i++) {
            if (fn_41_23314(i) && emAreaModuleImpl::isExistTarget(m_moduleAccesser, i)) {
                sum = sum + getAreaRate(areaRates, m_keepArea, i, keepMul);
                if (rnd < sum) {
                    pickedArea = i;
                    break;
                }
            }
        }
    }
    m_keepArea = -1;

    // Let every enabled unit of the priority window decide for each area it reacts to.
    bool pickedTried = false;
    bool pickedDecided = false;
    bool decided = false;
    for (int u = 0; u < m_unitNum; u++) {
        emAIUnit* unit = &m_units[u];
        if (unit->getFlag() == 0) {
            continue;
        }
        int priority = unit->getPriorty();
        if (priMin > priority || priMax < priority) {
            continue;
        }
        for (int area = 0; area < 8; area++) {
            bool isPicked = false;
            if (!unit->isTargetKind1(8) && pickedArea != -1 && fn_41_23314(area) == true) {
                if (pickedArea != area) {
                    continue;
                }
                isPicked = true;
            }
            int other = areaUnits[area];
            if (!isInvalid(other) && priority >= m_units[other].getPriorty()) {
                continue;
            }
            if (!unit->isTargetKind1(area)) {
                if (!unit->isTargetKind1(8) || area != m_currentInfo.unk4) {
                    continue;
                }
            } else if (!emAreaModuleImpl::isExistTarget(m_moduleAccesser, area)) {
                continue;
            }
            if (isPicked == true) {
                pickedTried = true;
            }
            if (unit->execAnimCmdDecide(m_moduleAccesser, m_animCmdGroup, area)) {
                areaUnits[area] = u;
                decided = true;
                if (isPicked == true) {
                    pickedDecided = true;
                }
            }
        }
    }
    // The picked area was tried but nothing was decided for it: keep it for the next update.
    if (pickedArea != -1 && pickedTried == true && !pickedDecided) {
        m_keepArea = pickedArea;
    }
    if (!decided) {
        return false;
    }

    // Among the areas with a unit, keep those whose unit has the lowest priority value.
    float minPriority = 3.4028235e38f;
    for (int area = 0; area < 8; area++) {
        int u = areaUnits[area];
        if (isInvalid(u) == true) {
            continue;
        }
        float priority = m_units[u].getPriorty();
        if (priority < minPriority) {
            minPriority = priority;
        }
    }
    int candidates[8];
    int candidateNum = 0;
    for (int i = 0; i < 8; i++) {
        candidates[i] = -1;
    }
    for (int area = 0; area < 8; area++) {
        int u = areaUnits[area];
        if (isInvalid(u) == true) {
            continue;
        }
        float priority = m_units[u].getPriorty();
        if (priority <= minPriority) {
            candidates[candidateNum++] = area;
        }
    }
    if (candidateNum == 0) {
        return false;
    }
    if (candidateNum == 1) {
        info.unk4 = candidates[0];
    } else {
        int total = 0;
        for (int i = 0; i < candidateNum; i++) {
            int area = candidates[i];
            if (fn_41_23164(area) == true) {
                continue;
            }
            total = total + areaRates[area];
        }
        int rnd = randi(total);
        int sum = 0;
        for (int i = 0; i < candidateNum; i++) {
            int area = candidates[i];
            if (fn_41_23164(area) == true) {
                continue;
            }
            sum = sum + areaRates[area];
            if (rnd < sum) {
                info.unk4 = area;
                break;
            }
        }
    }
    if (fn_41_23164(info.unk4) == true) {
        return false;
    }
    info.m_unitIndex = areaUnits[info.unk4];
    if (isInvalid(info.m_unitIndex) == true || info.m_unitIndex == -1) {
        return false;
    }

    // Remember the previous info, unless it is empty, gone or the same as the last remembered one.
    bool noPrev;
    if (m_currentInfo.m_unitIndex == -2) {
        noPrev = true;
    } else if (m_currentInfo.unk4 == -1) {
        noPrev = true;
    } else {
        noPrev = emAreaModuleImpl::isExistTarget(m_moduleAccesser, m_currentInfo.unk4) == false;
    }
    if (!noPrev) {
        soArray<emAIInfo>* history = m_history.getArray();
        if (history->size() <= 0) {
            if (history->isFull() == true) {
                history->pop();
            }
            history->unshift(m_currentInfo);
        } else {
            emAIInfo& last = history->at(0);
            bool same;
            if (m_currentInfo.m_unitIndex != last.m_unitIndex) {
                same = false;
            } else {
                same = last.unk4 == m_currentInfo.unk4;
            }
            if (!same) {
                if (history->isFull() == true) {
                    history->pop();
                }
                history->unshift(m_currentInfo);
            }
        }
    }

    emAIUnit& unit = m_units[info.m_unitIndex];
    if (unit.isSetInfo() == true) {
        m_currentInfo.m_unitIndex = info.m_unitIndex;
        m_currentInfo.unk4 = info.unk4;
        if (emAreaModuleImpl::isExistTarget(m_moduleAccesser, info.unk4) == true) {
            fn_41_26430(m_moduleAccesser, info.unk4);
        }
    }
    m_units[info.m_unitIndex].execAnimCmdExec(m_moduleAccesser, m_animCmdGroup);
    if (m_units[info.m_unitIndex].getStatusKind() != -1) {
        soModuleAccesser* acc = m_moduleAccesser;
        acc->getStatusModule().changeStatusRequest(m_units[info.m_unitIndex].getStatusKind(), acc);
        requested = true;
    }
    return requested;
}

emAIInfo* emAIModuleImpl::getCurrentAIInfo() {
    return &m_currentInfo;
}

emAIInfo* emAIModuleImpl::getPrevAIInfo(u32 index) {
    if (index >= m_history.getArray()->size()) {
        return &s_nullAIInfo;
    }
    return &m_history.getArray()->at(index);
}

// Picks one of num choices at random by their weights, among those whose anim cmds allow it; -1: none.
int emAIModuleImpl::switchRnd(emAIRndEntry* entries, u32 num) {
    int picked = -1;
    int allowedNum = 0;
    float total = 0.0f;
    if (entries == NULL) {
        return -1;
    }
    if (num > 32) {
        return -1;
    }
    if (num <= 1) {
        return -1;
    }
    int allowed[32];
    float rates[32];
    for (int i = 0; i < 32; i++) {
        allowed[i] = -1;
        rates[i] = 0.0f;
    }
    for (u32 i = 0; i < num; i++) {
        emAIRndEntry* entry = &entries[i];
        float scaleMul = soValueAccesser::getValueFloat(m_moduleAccesser, ValueFloat_AIRndScale, 0);
        float rate = entry->m_weight;
        if (0.0f != entry->m_scale) {
            rate += entry->m_scale * scaleMul;
        }
        if (rate < 0.0f) {
            continue;
        }
        m_moduleAccesser->getWorkManageModule().setFloat(rate, WorkFloat_AIRndRate);
        m_moduleAccesser->getWorkManageModule().setInt(i, WorkInt_AIRndIndex);
        bool isAllowed = false;
        if (entry->m_animCmd == NULL) {
            isAllowed = true;
        } else {
            soAnimCmdModule& animCmd = m_moduleAccesser->getAnimCmdModule();
            m_moduleAccesser->getWorkManageModule().offFlag(WorkFlag_AIDecided);
            animCmd.resetInterpreter3(m_rndAnimCmdGroup, entry->m_animCmd, m_moduleAccesser);
            animCmd.interpretCmd1(1.0f, m_rndAnimCmdGroup, m_moduleAccesser);
            if (m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_AIDecided) == true) {
                isAllowed = true;
            }
            animCmd.resetInterpreter3(m_rndAnimCmdGroup, NULL, m_moduleAccesser);
        }
        // The anim cmds may change the weight.
        rates[i] = m_moduleAccesser->getWorkManageModule().getFloat(WorkFloat_AIRndRate);
        if (isAllowed && !(rates[i] <= 0.0f)) {
            allowed[allowedNum++] = i;
            total += rates[i];
        }
    }
    if (allowedNum == 0) {
        return -1;
    }
    if (allowedNum == 1) {
        picked = allowed[0];
    } else {
        float rnd = total * randf();
        float sum = 0.0f;
        for (int i = 0; i < allowedNum; i++) {
            int index = allowed[i];
            if (index == -1) {
                continue;
            }
            sum += rates[index];
            if (rnd < sum) {
                picked = index;
                break;
            }
        }
    }
    return picked;
}

// The nearest area of areaMask (bit 8: the current AI area) whose target exists and target search unit
// targetSearchId sees; -1: none.
int emAIModuleImpl::getNearestAreaTargetInTargetSearch(int targetSearchId, u32 areaMask) {
    if (targetSearchId == -1) {
        return -1;
    }
    if (areaMask == 0) {
        return -1;
    }
    soTargetSearchModule* targetSearch =
        static_cast<soTargetSearchModule*>(m_moduleAccesser->m_enumerationStart->m_targetSearchModule);
    int nearest = -1;
    float nearestDist = 3.4028235e38f;
    for (int area = 0; area < 8; area++) {
        if (!emAIUnit::isTargetKind(areaMask, area)) {
            if (!emAIUnit::isTargetKind(areaMask, 8) || area != m_currentInfo.unk4) {
                continue;
            }
        }
        if (!emAreaModuleImpl::isExistTarget(m_moduleAccesser, area)) {
            continue;
        }
        if (!targetSearch->isTargetArea(targetSearchId, area)) {
            continue;
        }
        float dist = fn_41_24570(m_moduleAccesser, area);
        if (nearestDist > dist) {
            nearest = area;
            nearestDist = dist;
        }
    }
    return nearest;
}

bool emAIModuleImpl::isObserv(char group) {
    return group == 'j';
}

#define READ_INT_ARG(args, index, acc, out)                                                                            \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getValueInt(acc, getCmdArg(args, index).getValueIndexData(), 0);                        \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getIntData();                                                                     \
    }

static const s32 CmdArgType_ValueIndex = 5;

// The anim cmds of the AI (group 'j').
bool emAIModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (acmd->getGroup() != 'j') {
        return false;
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 5) {
        return false;
    }
    switch (type) {
    case 0:
    case 1: {
        // Enable (0) or disable (1) the listed units.
        if (argNum < 1 || argNum > 5) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u8 flag;
        if (type == 0) {
            flag = true;
        } else if (type == 1) {
            flag = false;
        } else {
            return true;
        }
        for (int i = 0; i < argNum; i++) {
            setFlag(flag, getCmdArg(args, i).getIntData());
        }
        return true;
    }
    case 2: {
        // Put the listed units' flags back (no argument: all of them).
        if (argNum < 0 || argNum > 4) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        if (argNum == 0) {
            resetFlag(-1);
        } else {
            for (int i = 0; i < argNum; i++) {
                resetFlag(getCmdArg(args, i).getIntData());
            }
        }
        return true;
    }
    case 3:
        // Forget the current AI info.
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        m_currentInfo.m_unitIndex = -2;
        m_currentInfo.unk4 = -1;
        return true;
    case 4: {
        // Write the nearest area seen by a target search unit to a work int: (search id, area mask, work variable).
        if (argNum < 3 || argNum > 3) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int targetSearchId;
        READ_INT_ARG(args, 0, moduleAccesser, targetSearchId)
        int areaMask;
        READ_INT_ARG(args, 1, moduleAccesser, areaMask)
        if (getCmdArg(args, 2).getArgType() != CmdArgType_ValueIndex) {
            return true;
        }
        u32 workId = getCmdArg(args, 2).getValueIndexData();
        moduleAccesser->getWorkManageModule().setInt(getNearestAreaTargetInTargetSearch(targetSearchId, areaMask), workId);
        return true;
    }
    }
    return false;
}

// Records what the enemy's attack hit (see m_attackAreaTarget).
void emAIModuleImpl::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog == NULL) {
        return;
    }
    u32 taskId = collisionLog->m_taskId;
    if (taskId == -1) {
        return;
    }
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(collisionLog->m_taskCategory, taskId);
    if (task == NULL) {
        return;
    }
    switch (task->m_taskCategory) {
    case gfTask::Category_Ground:
        break;
    case gfTask::Category_Fighter: {
        Fighter& fighter = dynamic_cast<Fighter&>(*task);
        if (fighter.m_entryId == g_ftManager->getEntryId(0)) {
            if (g_ftManager->getFighter(fighter.m_entryId, -1) == task) {
                m_attackAreaTarget |= 1;
            } else {
                m_attackAreaTarget |= 2;
            }
        } else if (fighter.m_entryId == g_ftManager->getEntryId(1)) {
            if (g_ftManager->getFighter(fighter.m_entryId, -1) == task) {
                m_attackAreaTarget |= 4;
            } else {
                m_attackAreaTarget |= 8;
            }
        }
        break;
    }
    case gfTask::Category_Enemy: {
        u32 createId = getEnemyCreateId(dynamic_cast<Enemy&>(moduleAccesser->getStageObject()));
        // HYPOTHESIS: the parent create id is looked up and not used (the original discards the result).
        emManager::getInstance()->getEnemyParentCreateId(createId);
        if (createId == getEnemyCreateId(dynamic_cast<Enemy&>(*task))) {
            m_attackAreaTarget |= 0x10;
        } else {
            m_attackAreaTarget |= 0x20;
        }
        break;
    }
    }
}

bool emAIModuleImpl::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

bool emAIModuleImpl::isAttackAreaTarget(int area) {
    return emAIUnit::isTargetKind(m_attackAreaTarget, area);
}

void emAIModuleImpl::initAttackAreaTarget() {
    m_attackAreaTarget = 0;
}
