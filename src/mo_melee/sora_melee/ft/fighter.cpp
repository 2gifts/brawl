#define FT_FIGHTER_REFLECTOR_DTOR // BYTES: fighter.o holds the out-of-line soCollisionReflectorEventObserver destructor
#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides the StageObject slot
#define FT_FIGHTER_ONINHALED_BOOL // MATCH-ONLY: Fighter::onInhaled returns true after the flag write
#define FT_FIGHTER_FIELDS
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_log_transactor.h>
#include <ft/ft_owner.h>
#include <so/so_external_value_accesser.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <it/item.h>
#include <it/it_manager.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_hyp.h>
#include <so/turn/so_turn_module.h>
#include <ft/ft_kinetic_energy.h>
#include <so/event/so_log_event_presenter.h>
#include <ft/ft_util.h>
#include <ft/robot/ft_robot_unk8.h>
#include <st/stage.h>
#include <nw4r/ut/ut_Color.h>
#include <mt/mt_prng.h>
#include <gf/gf_task_scheduler.h>
#include <so/damage/so_damage_util_actor.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_loupe.h>
#include <so/ground/so_ground_util.h>
#include <ft/ft_info.h>
#include <ft/ft_audience_manager.h>

extern ftInfo g_ftInfo;

void Fighter::setAvailableFinal(bool isDiscretion) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getWorkManageModule().onFlag(0x12000006);
    moduleAccesser->getWorkManageModule().onFlag(0x12000008);
    moduleAccesser->getWorkManageModule().setFlag(isDiscretion, 0x1200001e);
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200000a) || getOwner()->getCurry() == true) {
        moduleAccesser->getEffectModule().removeAll(8);
        moduleAccesser->getEffectModule().reqEmit(static_cast<EfID>(0x48), 8);
    }
    moduleAccesser->getVisibilityModule().setStatusDefault(0, 0, true);
    Stage* stage = g_Stage;
    if (stage != 0) {
        u32 color = stage->getFinalTechniqColor(getFtKind());
        int frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a19, 0);
        moduleAccesser->getEffectModule().fillScreen(0, frames, &nw4r::ut::Color(color), 0, 0x80);
    }
    moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1f71), false, false, 0);
    if (!isDiscretion) {
        getOwner()->addLogInt(1, 0x1e, 0);
    }
}

// HYPOTHESIS: the same shared item-header tags emitted in the fighter article units.
extern ftRobotUnk8 lbl_27_bss_3138;
extern ftRobotUnk8 lbl_27_bss_3140;

extern "C" void fn_27_13F1D8() {
    lbl_27_bss_3138.unk0 = 0xff;
    lbl_27_bss_3138.unk4 = 0;
    lbl_27_bss_3140.unk0 = 0xff;
    lbl_27_bss_3140.unk4 = 1;
}

void Fighter::activate() {
    StageObject::activate(&Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0.0f, false);
    m_outsideEventPresenter.setManageId(g_ftManager->m_eventManageModule.getManageId());
    m_moduleAccesser->getWorkManageModule().setInt(m_entryId, 0x10000000);
    bool spycloak = getOwner()->getSpycloak();
    if (spycloak == true) {
        m_moduleAccesser->getMotionModule().setOnlyChr(true);
    }
    if (!m_moduleAccesser->getModelModule().getScnMdl()) {
        attachInstance();
    }
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x20);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x3f);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x32);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x31);
    onActivate();
    int motion = -1;
    if (getOwner()->getFighterColor() == 0xc) {
        motion = 0x1cc;
    } else if (spycloak == true) {
        motion = 0x1cd;
        m_moduleAccesser->getShadowModule().setActive(false);
    }
    if (motion != -1) {
        m_moduleAccesser->getMotionModule().addOtherAnim(0, motion, true, 0.0f, 1.0f);
    }
}

void Fighter::notifyEjectItem(BaseItem* item, u32 index, bool silent) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    setItemHoldAnim(item, index, false);
    switch (item->soGetSubKind()) {
    case 0x45:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0xdc) {
            moduleAccesser->getLinkModule().adjustModelConstraintPosture(1,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
            moduleAccesser->getLinkModule().unlink(3);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_z = 0.0f;
            moduleAccesser->getPostureModule().setPos(&pos);
            moduleAccesser->getStatusModule().changeStatusRequest(0xdd, moduleAccesser);
            moduleAccesser->getControllerModule().setRumble(0xf, 0, false, -1);
        }
        break;
    case 0x15:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0xde) {
            moduleAccesser->getLinkModule().unlink(3);
            moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
        }
        g_ftManager->removeDragoonAll(m_entryId);
        break;
    case 0x1b:
    case 0x1d:
        setHammer(false, 0);
        break;
    case 0:
        moduleAccesser->getWorkManageModule().offFlag(0x1200002b);
        break;
    case 0xd:
        moduleAccesser->getWorkManageModule().offFlag(0x1200002d);
        break;
    }
}

// HYPOTHESIS: the outside-event damage hook forwards the damage, reaction flag, owner and accesser.
extern "C" void fn_27_156C00(ftOutsideEventPresenter*, soDamage*, bool, ftOwner*, soModuleAccesser*);

void Fighter::notifyEventOnDamage(soDamage* damage, bool reaction, soModuleAccesser* moduleAccesser) {
    fn_27_156C00(&m_outsideEventPresenter, damage, reaction, getOwner(), moduleAccesser);
    dropItemCheck(damage, reaction);
    if (reaction == true && damage->m_reaction > 0.0f) {
        int count = moduleAccesser->getWorkManageModule().getInt(0x1000002c);
        moduleAccesser->getWorkManageModule().setInt(count + 1, 0x1000002c);
        float totalDamage = moduleAccesser->getWorkManageModule().getFloat(0x1100000d);
        totalDamage += damage->m_damageAdd;
        moduleAccesser->getWorkManageModule().setFloat(totalDamage, 0x1100000d);
        m_outsideEventPresenter.notifyOutsideEventSucceedHit(count + 1, totalDamage);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
        moduleAccesser->getWorkManageModule().subFloat(damage->m_damageAdd, 0x1100000c);
    }
    if (damage->m_damageAdd > 0.0f && unk139_0 == true) {
        ftUtil::setDamageRumble(moduleAccesser, damage->m_damageAdd);
    }
    moduleAccesser->getCameraModule().setDamage(static_cast<unsigned short>(damage->m_damage), 0);
    bool knockedOut;
    if (static_cast<bool>(g_ftManager->m_isStamina) == true &&
        getOwner()->getHitPoint() < 1.0f &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == false) {
        knockedOut = true;
    } else {
        knockedOut = false;
    }
    if (knockedOut == true) {
        toKnockOut();
    }
}

bool Fighter::notifyEventCollisionShieldCheck() {
    if (unk120 > 0.0f) {
        float shield = m_moduleAccesser->getWorkManageModule().getFloat(0x11000003);
        if (unk11C > 0.0f) {
            soModuleAccesser* moduleAccesser = m_moduleAccesser;
            shield -= soValueAccesser::getConstantFloat(moduleAccesser, 0xca9, 0) +
                unk11C * (1.0f - soValueAccesser::getConstantFloat(moduleAccesser, 0xcb0, 0)) *
                soValueAccesser::getConstantFloat(moduleAccesser, 0xca8, 0);
            float minimum = m_moduleAccesser->getWorkManageModule().getFloat(0x11000004);
            if (shield < minimum) {
                shield = minimum;
            }
        }
        if (shield < 0.0f) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x56, m_moduleAccesser);
        } else {
            if (m_moduleAccesser->getSituationModule().getKind() != 0) {
                return false;
            }
            m_moduleAccesser->getWorkManageModule().setFloat(shield, 0x11000003);
            m_moduleAccesser->getWorkManageModule().setFloat(unk120, 0x21000008);
            m_moduleAccesser->getWorkManageModule().setFloat(unk124, 0x21000009);
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x1d, m_moduleAccesser);
        }
        m_moduleAccesser->getStopModule().setHitStop((int)unk120, false);
        return true;
    }
    return false;
}

void Fighter::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power,
    soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getEffectModule().setShieldEffect(0x20);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x1a:
    case 0x1b:
    case 0x1d:
        break;
    default:
        return;
    }
    int collisionIndex = collisionLog->m_collsionIndex;
    soCollisionAttackData* attackData = attackModule->getData(collisionIndex, collisionLog->m_isAbsolute);
    if (power > unk120) {
        unk120 = power;
        unk124 = m_moduleAccesser->getDamageModule().getDamageLr(
            posX, posY, attackModule, attackData, collisionIndex, collisionLog->m_isAbsolute);
        moduleAccesser->getWorkManageModule().setFloat(attackData->m_hitStopDelay, 0x21000006);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) > 0) {
        moduleAccesser->getEffectModule().setShieldEffect(0x7f);
        moduleAccesser->getEffectModule().reqCommon(0x1b, 0.0f);
    } else {
        unk11C += power + attackData->m_subShield;
    }
}

void Fighter::setZoom(bool enable, short kind, float frames) {
    if (enable == true) {
        if (unk138_7 == false) {
            m_moduleAccesser->getCameraModule().zoomIn(kind, 0, Vec2f(0.0f, 0.0f), frames);
            unk138_7 = true;
        }
    } else if (unk138_7 == true) {
        unk138_7 = false;
        m_moduleAccesser->getCameraModule().zoomOut();
    }
}

template <>
inline void soEventObserver<soCollisionReflectorEventObserver>::addObserverSub(s32 manageId, soCollisionReflectorEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soCollisionReflectorEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soDamageEventObserver>::addObserverSub(s32 manageId, soDamageEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soDamageEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soCollisionAbsorberEventObserver>::addObserverSub(s32 manageId, soCollisionAbsorberEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soCollisionAbsorberEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soCaptureEventObserver>::addObserverSub(s32 manageId, soCaptureEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soCaptureEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soItemManageEventObserver>::addObserverSub(s32 manageId, soItemManageEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soItemManageEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soMotionEventObserver>::addObserverSub(s32 manageId, soMotionEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soMotionEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soTurnEventObserver>::addObserverSub(s32 manageId, soTurnEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soTurnEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<ftEntryEventObserver>::addObserverSub(s32 manageId, ftEntryEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void ftEntryEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

void ftEntryEventObserver::notifyEventPokemonDamage() { }
void ftEntryEventObserver::notifyEventPokemonInflict() { }
void ftEntryEventObserver::notifyEventPokemonAttack() { }
void ftEntryEventObserver::notifyEventPokemonRebirthEnd() { }
void ftEntryEventObserver::notifyEventStopRumble(int) { }
void ftEntryEventObserver::notifyEventSetRumble(int, bool) { }
void ftEntryEventObserver::notifyEventPokemonAppeal() { }
void ftEntryEventObserver::notifyEventPokemonSpecial(int, int) { }
void ftEntryEventObserver::notifyEventPokemonChangeCancel() { }
void ftEntryEventObserver::notifyEventPokemonCollect() { }
void ftEntryEventObserver::notifyEventPokemonTrainerUpdate() { }
bool ftEntryEventObserver::notifyEventPokemonRequestChange(Vec3f*, float*) { return false; } // HYPOTHESIS: bool, matches header
void ftEntryEventObserver::notifyEventPokemonStart(int) { }
void ftEntryEventObserver::notifyEventWarp() { }

// Fighter::setFlower (the map name); src/mo_melee/sora_melee/ft/ft_damage_transactor_impl.cpp calls it by this stand-in name.
extern "C" void fn_27_1392D4(Fighter* fighter, bool setStatus, float rate, float size, int unk4, bool unk5);

// The Kinoko process owns these helpers; its class is declared in its source file.
extern "C" float getFixScale__25ftStatusUniqProcessKinokoFP16soModuleAccesserii(soModuleAccesser*, int, int);
extern "C" void fn_27_137EA4(Fighter*, int, int, int, int, float);

void Fighter::setupEquipment() {
    int status;
    soModuleAccesser* moduleAccesser;
    ftOwner* owner = getOwner();
    moduleAccesser = m_moduleAccesser;
    if (!owner->isSubOwner() && owner->getCurry() == true) {
        setCurry(true, -1);
    }
    if (owner->getMetal() == true) {
        setMetal(true, -1.0f, -1);
    }
    if (owner->getFlower() == true) {
        float duration = soValueAccesser::getConstantFloat(moduleAccesser, 0xd88, 0);
        float minimum = soValueAccesser::getConstantFloat(moduleAccesser, 0xd86, 0);
        float rate = (soValueAccesser::getConstantFloat(moduleAccesser, 0xd89, 0) - minimum) / duration;
        fn_27_1392D4(this, true, 0.0f, rate, 1, false);
    }
    if (owner->getRabbitCap() == true) {
        moduleAccesser->getItemManageModule().attachItem((itKind)0x44, 0, true);
    }
    if (owner->getReflector() == true) {
        moduleAccesser->getItemManageModule().attachItem((itKind)1, 0, true);
    }
    if (owner->getInfiniteScalingKind() != 0) {
        status = owner->getInfiniteScalingSizeType() == 0 ? 3 : 5;
        fn_27_137EA4(this, 1, owner->getInfiniteScalingKind(), status, -1,
            getFixScale__25ftStatusUniqProcessKinokoFP16soModuleAccesserii(
                moduleAccesser, status, owner->getInfiniteScalingKind()));
    }
}

void Fighter::standby(int removeArticles) {
    soModuleAccesser* accesser = m_moduleAccesser;
    endFinal(true, false, false);
    setMetal(false, -1.0f, -1);
    fn_27_1392D4(this, false, 0.0f, 0.0f, 0, false);
    setCurry(false, -1);
    setHammer(false, 0);
    setSuperStar(false, -1, false);
    if (unk138_7 == true) {
        unk138_7 = false;
        m_moduleAccesser->getCameraModule().zoomOut();
    }
    setSlow(false, 1, 0, false);
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
        moduleAccesser->getEffectModule().removeScreen(8);
        moduleAccesser->getEffectModule().reqScreen(9);
        moduleAccesser->getWorkManageModule().offFlag(0x12000028);
        moduleAccesser->getSlowModule().clearWhole();
    }
    accesser->getStatusModule().changeStatusForce(0x10b, accesser);
    accesser->getItemManageModule().removeAll();
    if ((u32)removeArticles == 1) {
        accesser->getGenerateArticleManageModule().deactivate(true);
    }
}

void Fighter::deactivate() {
    endFinal(true, false, false);
    setMetal(false, -1.0f, -1);
    fn_27_1392D4(this, false, 0.0f, 0.0f, 0, false);
    setCurry(false, -1);
    setHammer(false, 0);
    setSuperStar(false, -1, false);
    if (unk138_7 == true) {
        unk138_7 = false;
        m_moduleAccesser->getCameraModule().zoomOut();
    }
    setSlow(false, 1, 0, false);
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
        moduleAccesser->getEffectModule().removeScreen(8);
        moduleAccesser->getEffectModule().reqScreen(9);
        moduleAccesser->getWorkManageModule().offFlag(0x12000028);
        moduleAccesser->getSlowModule().clearWhole();
    }
    if (m_moduleAccesser->getStatusModule().getStatusKind() != -1) {
        soModuleAccesser* accesser = m_moduleAccesser;
        endFinal(true, false, false);
        setMetal(false, -1.0f, -1);
        fn_27_1392D4(this, false, 0.0f, 0.0f, 0, false);
        setCurry(false, -1);
        setHammer(false, 0);
        setSuperStar(false, -1, false);
        if (unk138_7 == true) {
            unk138_7 = false;
            m_moduleAccesser->getCameraModule().zoomOut();
        }
        setSlow(false, 1, 0, false);
        soModuleAccesser* slowAccesser = m_moduleAccesser;
        if (slowAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
            slowAccesser->getEffectModule().removeScreen(8);
            slowAccesser->getEffectModule().reqScreen(9);
            slowAccesser->getWorkManageModule().offFlag(0x12000028);
            slowAccesser->getSlowModule().clearWhole();
        }
        accesser->getStatusModule().changeStatusForce(0x10b, accesser);
        accesser->getItemManageModule().removeAll();
        accesser->getGenerateArticleManageModule().deactivate(true);
    }
    onDeactivate();
    m_moduleAccesser->activate();
    getPhysicsModule(m_moduleAccesser).unk1C();
    m_moduleAccesser->getModelModule().destructInstance();
    StageObject::deactivate(true);
    if (m_outsideEventPresenter.m_manageID != -1) {
        m_outsideEventPresenter.m_manageID = -1;
        if (m_outsideEventPresenter.m_manageID > -1) {
            m_outsideEventPresenter.m_obsrvrList = 0;
        }
    }
}

void Fighter::setSlow(bool setStatus, int slowStrength, int duration, bool useTimerEffect) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        int statusKind = moduleAccesser->getStatusModule().getStatusKind();
        union {
            ftStatusFlagData status;
            u32 words[2];
        } flags;
        flags.status = *g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind);
        if ((flags.words[1] >> 27) & 1) {
            return;
        }
        if (useTimerEffect == true) {
            float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0);
            float frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a81, 0);
            frames *= rate;
            duration -= (int)frames;
            if (moduleAccesser->getWorkManageModule().isFlag(0x12000026) == true) {
                moduleAccesser->getEffectModule().removeScreen(8);
            }
            moduleAccesser->getEffectModule().reqScreen(8);
            moduleAccesser->getWorkManageModule().onFlag(0x12000027);
        }
        moduleAccesser->getSlowModule().set(slowStrength, duration);
        moduleAccesser->getWorkManageModule().onFlag(0x12000026);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x12000026) == true) {
        moduleAccesser->getSlowModule().clear();
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000027) == true) {
            moduleAccesser->getEffectModule().removeScreen(8);
            moduleAccesser->getEffectModule().reqScreen(9);
            moduleAccesser->getWorkManageModule().offFlag(0x12000027);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x12000026);
    }
}

void Fighter::setHammer(bool setStatus, int kind) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200000c);
        moduleAccesser->getWorkManageModule().offFlag(0x1200000d);
        moduleAccesser->getWorkManageModule().onFlag(0x1200000b);
        int duration;
        if (kind == 0) {
            duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5a6e, 0);
        } else {
            duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5a71, 0);
            float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0);
            float frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a72, 0);
            frames *= rate;
            duration -= (int)frames;
            moduleAccesser->getWorkManageModule().setInt(duration, 0x1000001b);
            moduleAccesser->getSoundModule().playSENo3d((SndID)0x1c, true);
        }
        moduleAccesser->getWorkManageModule().setInt(duration, 0x1000001b);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x1200000b) == true) {
        int handle = moduleAccesser->getWorkManageModule().getInt(0x1000001d);
        if (handle != -1) {
            moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
            moduleAccesser->getWorkManageModule().setInt(-1, 0x1000001d);
        }
        moduleAccesser->getEffectModule().removeCommon(0x14);
        moduleAccesser->getEffectModule().removeCommon(0x15);
        moduleAccesser->getEffectModule().removeCommon(0x16);
        moduleAccesser->getWorkManageModule().offFlag(0x1200000b);
    }
}

void Fighter::restoreScaling() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getSituationModule().getKind() != 5 &&
        moduleAccesser->getWorkManageModule().getInt(0x10000023) >= 2 &&
        !moduleAccesser->getWorkManageModule().isFlag(0x12000032) &&
        !moduleAccesser->getWorkManageModule().isFlag(0x12000018)) {
        if (moduleAccesser->getWorkManageModule().getInt(0x10000023) == 3) {
            moduleAccesser->getWorkManageModule().setInt(2, 0x10000023);
            moduleAccesser->getStatusModule().changeStatusRequest(0xf2, moduleAccesser);
        } else {
            moduleAccesser->getWorkManageModule().setInt(1, 0x10000023);
            moduleAccesser->getStatusModule().changeStatusRequest(0xf3, moduleAccesser);
        }
    } else {
        soModuleAccesser* accesser = m_moduleAccesser;
        float scale = accesser->getWorkManageModule().getFloat(0x1100000b);
        accesser->getPostureModule().setScale(scale);
        if (accesser->getWorkManageModule().isFlag(0x12000014) == true) {
            accesser->getWorkManageModule().offFlag(0x12000014);
            accesser->getParamCustomizeModule().removeCustomize(0);
            accesser->getSoundModule().setSEPitch(accesser->getWorkManageModule().getFloat(0x11000011));
        }
        if (accesser->getWorkManageModule().isFlag(0x12000015) == true) {
            accesser->getWorkManageModule().offFlag(0x12000015);
            accesser->getParamCustomizeModule().removeCustomize(1);
            accesser->getSoundModule().setSEPitch(accesser->getWorkManageModule().getFloat(0x11000011));
        }
        accesser->getWorkManageModule().setInt(0, 0x10000023);
    }
}


void Fighter::postCreate() { }

void Fighter::onActivate() { }

void Fighter::onDeactivate() { }

void Fighter::activateController(bool active) {
    m_moduleAccesser->getControllerModule().setOff(!active);
}

void Fighter::postStart(int) { }

void Fighter::standbyAdvFollow() {
    m_moduleAccesser->getStatusModule().changeStatusForce(0x10b, m_moduleAccesser);
}

bool Fighter::isAcceptFinal() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getSituationModule().getKind() == 5) {
        return false;
    }
    return moduleAccesser->getWorkManageModule().isFlag(0x12000006) != true;
}

void Fighter::onStartFinal() { }

void Fighter::onEndFinal() { }

void Fighter::processUpdate() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (soSlow::getInstance()->isEstimate() == true) {
        if (!moduleAccesser->getStopModule().isStop()) {
            ftOwner* owner = getOwner();
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            work.countDownInt(0x10000002, 0);
            work.countDownInt(0x10000004, 0);
            work.countDownInt(0x10000006, 0);
            work.countDownInt(0x10000009, 0);
            work.countDownInt(0x1000000a, 0);
            work.countDownInt(0x1000000b, 0);
            work.countDownInt(0x1000000e, 0);
            work.countDownInt(0x1000000f, 0);
            work.countDownInt(0x10000010, 0);
            work.countDownInt(0x10000011, 0);
            work.countDownInt(0x10000012, 0);
            work.countDownInt(0x10000026, 0);
            work.countDownInt(0x10000027, 0);
            work.countDownInt(0x10000020, 0);
            work.countDownInt(0x10000031, 0);
            work.countDownInt(0x10000039, 0);

            int metalFrame = moduleAccesser->getWorkManageModule().getInt(0x10000038);
            if (metalFrame > 0) {
                moduleAccesser->getWorkManageModule().setInt(metalFrame - 1, 0x10000038);
                if (metalFrame - 1 <= 0) {
                    soModuleAccesser* acc = m_moduleAccesser;
                    acc->getWorkManageModule().setInt(0, 0x10000038);
                    acc->getWorkManageModule().setInt(0, 0x1000002c);
                    acc->getWorkManageModule().setFloat(0.0f, 0x1100000d);
                    acc->getWorkManageModule().setInt(0, 0x1000003d);
                }
            }

            int statusKind = moduleAccesser->getStatusModule().getStatusKind();
            if (!g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind)->unk4_8) {
                float shield = moduleAccesser->getWorkManageModule().getFloat(0x11000003);
                float shieldMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xca1, 0) + owner->getShieldMaxAdd();
                shield += soValueAccesser::getConstantFloat(moduleAccesser, 0xca6, 0) + owner->getShieldRecoveryAdd();
                if (shield > shieldMax) {
                    shield = shieldMax;
                }
                moduleAccesser->getWorkManageModule().setFloat(shield, 0x11000003);
            }

            if (moduleAccesser->getWorkManageModule().isFlag(0x1200000e) == true) {
                int starFrame = moduleAccesser->getWorkManageModule().getInt(0x1000001e);
                moduleAccesser->getWorkManageModule().setInt(starFrame - 1, 0x1000001e);
                if (starFrame - 1 < soValueAccesser::getConstantInt(moduleAccesser, 0x5a78, 0)) {
                    int handle = moduleAccesser->getWorkManageModule().getInt(0x1000001f);
                    if (handle != -1) {
                        moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
                        moduleAccesser->getWorkManageModule().setInt(-1, 0x1000001f);
                    }
                }
                if (starFrame - 1 <= 0) {
                    setSuperStar(false, -1, false);
                }
            }

            if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
                int metalTime = moduleAccesser->getWorkManageModule().getInt(0x10000025);
                moduleAccesser->getWorkManageModule().setInt(metalTime - 1, 0x10000025);
                if (metalTime - 1 <= 0 || moduleAccesser->getWorkManageModule().getFloat(0x1100000c) <= 0.0f) {
                    if (!owner->getMetal()) {
                        setMetal(false, -1.0f, -1);
                    }
                }
            }

            // HYPOTHESIS: a Fighter flag bit at 0x138 enables the damage recovery below.
            if (unk138_loupeDamage == true && !owner->isSubOwner() && g_ftManager->isUnk6c_80() == true) {
                soDamageModule& damageModule = moduleAccesser->getDamageModule();
                float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xc8d, 0);
                if (damageModule.getDamage(0) < threshold) {
                    int frame = moduleAccesser->getWorkManageModule().getInt(0x1000002d);
                    frame++;
                    if (frame >= soValueAccesser::getConstantInt(moduleAccesser, 0x5a14, 0)) {
                        frame = 0;
                        float recovery = soValueAccesser::getConstantFloat(moduleAccesser, 0xc8e, 0);
                        moduleAccesser->getDamageModule().addDamage(recovery, 0);
                    }
                    moduleAccesser->getWorkManageModule().setInt(frame, 0x1000002d);
                }
            }

            if (!owner->isSubOwner() && owner->getFinalContinue() == true) {
                if (isAcceptFinal() == true && !g_ftManager->isProcessTechnique() && g_ftManager->isGameStarted() == true) {
                    g_ftManager->setFinal(m_entryId, false);
                }
            }
        }

        if (moduleAccesser->getWorkManageModule().isFlag(0x12000026) == true && moduleAccesser->getSlowModule().getFrame() <= 0) {
            setSlow(false, 1, 0, false);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true && moduleAccesser->getSlowModule().getWholeFrame() <= 0) {
            soModuleAccesser* acc = m_moduleAccesser;
            if (acc->getWorkManageModule().isFlag(0x12000028) == true) {
                acc->getEffectModule().removeScreen(8);
                acc->getEffectModule().reqScreen(9);
                acc->getWorkManageModule().offFlag(0x12000028);
                acc->getSlowModule().clearWhole();
            }
        }
    }

    StageObject::processUpdate();

    if (soSlow::getInstance()->isEstimate() == true && !moduleAccesser->getStopModule().isStop() &&
        moduleAccesser->getSituationModule().getKind() == 2) {
        if (g_ftCommonDataAccesser.getFtStatusData(getFtKind(), moduleAccesser->getStatusModule().getStatusKind())->unk0_5) {
            if (moduleAccesser->getControllerModule().getStickY() < -soValueAccesser::getConstantFloat(moduleAccesser, 0xc5e, 0)) {
                moduleAccesser->getGroundModule().setPassableCheck(false, 0);
            } else {
                moduleAccesser->getGroundModule().setPassableCheck(true, 0);
            }
        }
    }

    unk114 = 0.0f;
    unk118 = 0;
    unk120 = 0.0f;
    unk11C = 0.0f;
    unk138_0 = false;
    unk138_1 = unk138_2 = false;
    unk130 = 0.0f;
    unk128 = 0.0f;
    unk139_0 = true;
    processDamageFlySe();
}

void Fighter::processEnd() {
    StageObject::processEnd();
}

ftOwner* Fighter::getOwner() {
    return g_ftManager->getOwner(m_entryId);
}

Input* Fighter::getInput() {
    return static_cast<Input*>(g_ftManager->getInput(m_entryId));
}

Fighter* Fighter::getPartner() {
    return NULL;
}

bool Fighter::isObserv(char unk1) {
    return unk1 == 12;
}

void Fighter::playEatSE() { }

void Fighter::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { }

void Fighter::notifyEventCollisionShieldSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getEffectModule().setShieldEffect(0x20);
}

void Fighter::notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { }

bool Fighter::notifyEventCollisionAbsorberCheck() {
    return false;
}

void Fighter::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { }

bool Fighter::notifyEventCollisionSearchCheck() {
    return false;
}

void Fighter::onSetMetal(bool) { }

void Fighter::onHitReflector(soModuleAccesser* moduleAccesser, bool) { }

void Fighter::notifyDropItem(BaseItem* item) {
    m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fae), false, false, 0);
}

u32 Fighter::getChangeSucceedOption() {
    return 0;
}

bool Fighter::setupChangeSucceedEffect() {
    return false;
}

bool Fighter::isInclude(float* rect, Vec3f* out) {
    float range = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xbe6, 0);
    return soExternalValueAccesser::isIncludeRect(this, rect, out, range, 2.0f * range);
}

void Fighter::analyzeSeal(void*) { }

bool Fighter::isInhaled() {
    return m_moduleAccesser->getWorkManageModule().isFlag(0x12000036);
}

void Fighter::offInhaled() {
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000036);
}

void Fighter::toKnockOut(soDamageAttackerInfo* attackerInfo) {
    m_moduleAccesser->getDamageModule().restoreAttackerInfo(attackerInfo);
    toKnockOut();
}

void Fighter::setItemHoldAnim(BaseItem* item, u32 index, bool enabled) {
    int haveKind = item->getParam(itValueAccesser::Specialized_Param_Int_Have_Kind);
    switch (haveKind) {
    case 3:
    case 4:
    case 5:
    case 6:
        if (enabled == true) {
            int holdKind;
            switch (haveKind) {
            case 3: holdKind = 0; break;
            case 4: holdKind = 1; break;
            case 5: holdKind = 2; break;
            case 6: holdKind = 3; break;
            }
            attachItemHoldAnim(holdKind, index);
        } else {
            m_moduleAccesser->getMotionModule().removePartialAnimChr(0);
            m_moduleAccesser->getMotionModule().enablePartialCancelNode(false);
        }
        break;
    }
}

void Fighter::setHeartSwap(int givingPlayerEntryID, int givingPlayerEntryId, bool setStatus) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (givingPlayerEntryID != -1) {
        int taskId = m_taskId;
        Vec3f position = g_ftManager->getFighterCenterPos(givingPlayerEntryID, -1);
        int heartId = itManager::getInstance()->createManaphyHeart(&position, taskId, taskId);
        moduleAccesser->getWorkManageModule().setInt(heartId, 0x10000030);
        moduleAccesser->getWorkManageModule().setInt(givingPlayerEntryID, 0x1000002f);
        moduleAccesser->getStatusModule().changeStatusForce(0xf4, moduleAccesser);
    } else if (moduleAccesser->getWorkManageModule().getInt(0x1000002f) != -1) {
        moduleAccesser->getWorkManageModule().setInt(-1, 0x1000002f);
        if (setStatus == true) {
            int taskId = m_taskId;
            Vec3f position = g_ftManager->getFighterCenterPos(givingPlayerEntryId, -1);
            int heartId = itManager::getInstance()->createManaphyHeart(&position, taskId, taskId);
            moduleAccesser->getWorkManageModule().setInt(heartId, 0x10000030);
            if (moduleAccesser->getSituationModule().getKind() != 5) {
                moduleAccesser->getStatusModule().changeStatusForce(0xf5, moduleAccesser);
            }
        }
    }
}

void Fighter::setTeam(int team, int team2nd) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getTeamModule().setTeam(team, false);
    moduleAccesser->getTeamModule().set2nd(team2nd);
    if (moduleAccesser->getItemManageModule().isHaveItem(0) == true) {
        moduleAccesser->getItemManageModule().setHaveItemTeam(team, 0);
    }
}

void Fighter::notifyEventChangeAdvUnit() {
    ftOwner* owner = getOwner();
    if (owner->getAdventureUnitKind() == 0) {
        m_moduleAccesser->getCameraModule().setAdvPri(5, -1);
    } else {
        m_moduleAccesser->getCameraModule().setAdvPri(4, -1);
    }
}

void Fighter::notifyEventCollisionReflectorSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    if ((int)groupIndex == 1) {
        unk138_1 = true;
    }
    soModuleAccesser* acc = m_moduleAccesser;
    ftLogTransactor::notifyEventCollisionReflector(acc, getOwner());
}

bool Fighter::notifyEventCollisionReflectorCheck() {
    if (unk138_1 == true) {
        m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1f67), false, false, 0);
    } else if (unk138_2 == true) {
        m_moduleAccesser->getCollisionReflectorModule().clearAll(1);
        m_moduleAccesser->getWorkManageModule().setInt(1, 0x1000003a);
    }
    return false;
}

void Fighter::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 1) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk138_2 = true;
            return;
        } else {
            unk138_1 = true;
        }
    }
    soModuleAccesser* acc = m_moduleAccesser;
    ftLogTransactor::notifyEventCollisionReflector(acc, getOwner());
}

bool Fighter::checkTransitionStatus(u32 status) {
    if (g_ftCommonDataAccesser.getFtStatusData(getFtKind(), status)->unk4_18) {
        bool isStopped = m_moduleAccesser->getStopModule().isStop();
        if (isStopped == true) {
            return false;
        }
    }
    return true;
}

// The map names this ftOwner::setOperationCpu. The original call passes a Fighter, and the body is the controller log.
void ftOwner::setOperationCpu(bool isCpu) {
    reinterpret_cast<Fighter*>(this)->m_moduleAccesser->getControllerModule().setLogActive(isCpu);
}

void Fighter::notifyVisibilityItem(BaseItem* item, int nodeIndex, bool visible, u8 visibility) {
    if (visible != true) {
        return;
    }
    // MATCH-ONLY: the hold-anim flag is the raw visibility byte, not a normalized bool.
    bool enabled;
    reinterpret_cast<u8&>(enabled) = visibility;
    setItemHoldAnim(item, nodeIndex, enabled);
}

// HYPOTHESIS: capture-module slot 0x24 takes the task id and the two trailing ints.
struct ftFighterCaptureStatusView {
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual bool unk24(int taskId, int unk2, int unk3);
};

bool Fighter::notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) {
    return reinterpret_cast<ftFighterCaptureStatusView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk24(taskId, unk2, unk3);
}

bool Fighter::onInhaled() {
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000036);
    return true;
}

// HYPOTHESIS: these manager flags are unsigned-char bits. Returning one as bool
// keeps the normalizing conversion the direct bool bitfield compare folds away.
struct FtManagerStartedView {
    char pad[0x6c];
    u8 : 1;
    u8 m_isGameStarted : 1;
    u8 : 6;
    bool isGameStarted() const { return m_isGameStarted; }
};

struct FtManagerCursorView {
    char pad[0x6f];
    u8 : 3;
    u8 unk6f_10 : 1;
    u8 : 4;
    bool isUnk6f10() const { return unk6f_10; }
};

void Fighter::setLoupeDamage(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerStartedView*>(g_ftManager)->isGameStarted() == true) {
        if (unk138_loupeDamage == false) {
            unk138_loupeDamage = true;
        }
    } else if (unk138_loupeDamage == true) {
        unk138_loupeDamage = false;
    }
}

void Fighter::detachInstance() {
    m_moduleAccesser->activate();
    getPhysicsModule(m_moduleAccesser).unk1C();
    m_moduleAccesser->getModelModule().destructInstance();
}

void Fighter::eatItem(soLinkTouchItemEventArgs* args) {
    switch (args->m_item->soGetSubKind()) {
    case Item_Spring:
        return;
    default:
        touchItem(args);
        break;
    }
}

bool Fighter::notifyHaveItemPreCheck(BaseItem* item, bool* result) {
    if (*reinterpret_cast<int*>(result) == 2) {
        if ((item->getParam(itValueAccesser::Work_Param_Int_Trait) & itParam::TRAIT_MASK_QUICK) == 0) {
            *reinterpret_cast<int*>(result) = 0;
        }
    }
    switch (item->soGetSubKind()) {
    case Item_WarpStar:
        *reinterpret_cast<int*>(result) = 1;
        break;
    }
    return true;
}

void Fighter::detachItemHoldAnim(bool detachItem) {
    m_moduleAccesser->getMotionModule().removePartialAnimChr(0);
    m_moduleAccesser->getMotionModule().enablePartialCancelNode(false);
    if (detachItem == true) {
        m_moduleAccesser->getItemManageModule().setHaveItemHoldAnim(false, 0);
    }
}

void Fighter::setLoupe(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerStartedView*>(g_ftManager)->isGameStarted() == true) {
        if (unk138_loupe == false) {
            m_outsideEventPresenter.notifyOutsideEventSetLoupe(1);
            unk138_loupe = true;
        }
    } else if (unk138_loupe == true) {
        m_outsideEventPresenter.notifyOutsideEventSetLoupe(0);
        unk138_loupe = false;
    }
}

void Fighter::setCursor(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerCursorView*>(g_ftManager)->isUnk6f10() == true) {
        if (unk138_cursor == false) {
            m_outsideEventPresenter.notifyOutsideEventSetCursor(1);
            unk138_cursor = true;
        }
    } else if (unk138_cursor == true) {
        m_outsideEventPresenter.notifyOutsideEventSetCursor(0);
        unk138_cursor = false;
    }
}

void Fighter::setNameCursor(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerCursorView*>(g_ftManager)->isUnk6f10() == true) {
        if (unk138_nameCursor == false) {
            m_outsideEventPresenter.notifyOutsideEventSetNameCursor(1);
            unk138_nameCursor = true;
        }
    } else if (unk138_nameCursor == true) {
        m_outsideEventPresenter.notifyOutsideEventSetNameCursor(0);
        unk138_nameCursor = false;
    }
}

void Fighter::notifyEventSetDamage(float damage) {
    ftOwner* owner = getOwner();
    m_moduleAccesser->getDamageModule().initDamage(damage);
    if (owner->isSubOwner() == false) {
        m_outsideEventPresenter.notifyOutsideEventSetDamage(0.0f, false, false);
    }
}

// HYPOTHESIS: 0x12d is the node id whose world position becomes the rough position.
void Fighter::updateRoughPos() {
    int nodeId = m_moduleAccesser->getModelModule().getCorrectNodeId(0x12d);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeId, false);
    posture.setPrevRoughPos(&pos);
}

template class soEventObserver<soTurnEventObserver>;
template class soEventObserver<soMotionEventObserver>;
template class soEventObserver<soItemManageEventObserver>;
template class soEventObserver<ftEntryEventObserver>;

soCollisionReflectorEventObserver::~soCollisionReflectorEventObserver() {}

void Fighter::notifyEventTurn(float lr, float prevLr, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getStatusModule().onChangeLr(moduleAccesser, lr, prevLr);
    if (static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule)->isExtern() == true) {
        Vec3f reflection(-1.0f, 0.0f, 0.0f);
        moduleAccesser->getKineticModule().reflectSpeed(&reflection, soKineticEnergy::AttributeFlag(1));
        ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
        if (prevLr != motion.m_facing) {
            motion.m_prevMotionSpeed.m_x = -motion.m_prevMotionSpeed.m_x;
            motion.m_facing = prevLr;
        }
    }
}

void Fighter::notifyEventBeat() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x1b) {
        int count = moduleAccesser->getWorkManageModule().getInt(0x1000001b);
        if (count > 0) {
            int remain = count - soValueAccesser::getConstantInt(moduleAccesser, 0x5a73, 0);
            if (remain < 0) {
                remain = 0;
            }
            moduleAccesser->getWorkManageModule().setInt(remain, 0x1000001b);
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
        int frame = moduleAccesser->getSlowModule().getWholeFrame();
        int decrement = soValueAccesser::getConstantInt(moduleAccesser, 0x5a81, 0);
        moduleAccesser->getSlowModule().setWholeFrame(frame - decrement);
    }
}

// HYPOTHESIS: per-fighter flags; bit 2 requests the common Final Smash sound.
extern const u8 lbl_27_rodata_1E18[];

void Fighter::startFinal() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    startFinalCommon();
    moduleAccesser->getCameraModule().zoomOut();
    float slowRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xc29, 0);
    int slowFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x59e7, 0);
    m_outsideEventPresenter.notifyOutsideEventFinalSlow(slowRate, slowFrames);
    if (lbl_27_rodata_1E18[getFtKind()] & 4) {
        moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0xdb), false);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000008);
    m_outsideEventPresenter.notifyOutsideEventStartFinal();
    onStartFinal();
}

void Fighter::attachItemHoldAnim(int holdKind, u32 index) {
    const ftData* data = g_ftCommonDataAccesser.getData(m_kind);
    int motionKind;
    switch (holdKind) {
    case 0: motionKind = 7; break;
    case 1: motionKind = 6; break;
    case 2: motionKind = 8; break;
    case 3: motionKind = 9; break;
    }
    int nodeSet = data->itemHoldNodeSets[holdKind];
    if (nodeSet != 0 && m_moduleAccesser->getMotionModule().isAnimResFile(motionKind) == true) {
        m_moduleAccesser->getMotionModule().addPartialAnimChr(0, motionKind, nodeSet, false, 0.0f, 1.0f, false);
        if (holdKind == 2) {
            m_moduleAccesser->getMotionModule().enablePartialCancelNode(true);
        }
        m_moduleAccesser->getItemManageModule().setHaveItemHoldAnim(true, index);
    }
}

void Fighter::warp(Vec3f* pos, float lr, u32 showEffect) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getPostureModule().initPos(pos);
    moduleAccesser->getPostureModule().setLr(lr);
    moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
    if (showEffect == 1) {
        moduleAccesser->getEffectModule().req(static_cast<EfID>(0x104004e), 0x12e,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
    }
}

// HYPOTHESIS: common effect indices for the collision-hit statuses; negative means no effect.
extern const int lbl_27_rodata_1038[];

void Fighter::notifyEventChangeCollisionHit(int previousStatus, soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getCollisionHitModule().getTotalStatus(0);
    if (status != previousStatus) {
        int previousEffect = lbl_27_rodata_1038[previousStatus];
        if (previousEffect >= 0) {
            moduleAccesser->getEffectModule().removeCommon(previousEffect);
        }
    }
    if (status == 0) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000035) == true) {
            moduleAccesser->getCollisionReflectorModule().setStatusAll(2, 1);
        }
    } else {
        int effect = lbl_27_rodata_1038[status];
        if (effect >= 0) {
            moduleAccesser->getEffectModule().reqCommon(effect, 0.0f);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000035) == true) {
            moduleAccesser->getCollisionReflectorModule().clearAll(1);
        }
    }
}

// HYPOTHESIS: the outside-event damage hook forwards the damage, owner and module accesser.
extern "C" void fn_27_156ED8(ftOutsideEventPresenter*, soDamage*, ftOwner*, soModuleAccesser*);

void Fighter::notifyEventAddDamage(soDamage* damage, soModuleAccesser* moduleAccesser) {
    fn_27_156ED8(&m_outsideEventPresenter, damage, getOwner(), moduleAccesser);
    if (damage->m_damageAdd_ > 0.0f && unk139_0 == true) {
        moduleAccesser->getControllerModule().setRumble(4, 0, false, -1);
    }
    moduleAccesser->getCameraModule().setDamage(static_cast<unsigned short>(damage->m_damage), 0);
    bool knockedOut;
    if (static_cast<bool>(g_ftManager->m_isStamina) == true &&
        getOwner()->getHitPoint() < 1.0f &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == false) {
        knockedOut = true;
    } else {
        knockedOut = false;
    }
    if (knockedOut == true) {
        toKnockOut();
    }
}

void Fighter::notifyShootBulletItem(BaseItem* item) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    switch (moduleAccesser->getItemManageModule().getHaveItemKind(0)) {
    case 0xd:
    case 0x3a:
        if (static_cast<int>(moduleAccesser->getItemManageModule().getShootItemBullet(0)) > 0) {
            getOwner()->addLogInt(1, 0x1d, 0);
        }
        break;
    case 0x2c:
        if (static_cast<int>(moduleAccesser->getItemManageModule().getShootItemBullet(0)) > 0) {
            getOwner()->addLogInt(1, 0x1d, 0);
            moduleAccesser->getEffectModule().req(static_cast<EfID>(0xd), 0,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
        }
        break;
    }
    item->updateLogAttackInfo();
}

void Fighter::notifyEjectAttachItem(BaseItem* item, u32 index, bool silent) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    bool playSound = false;
    switch (item->soGetSubKind()) {
    case 0x2f:
        moduleAccesser->getControllerModule().endClatter(1);
        moduleAccesser->getWorkManageModule().offFlag(0x12000009);
        break;
    case 1:
        moduleAccesser->getCollisionReflectorModule().clearAll(1);
        playSound = true;
        moduleAccesser->getWorkManageModule().offFlag(0x12000035);
        onHitReflector(moduleAccesser, false);
        break;
    case 0x32:
        moduleAccesser->getWorkManageModule().offFlag(0x1200000f);
        playSound = true;
        break;
    case 0x44:
        moduleAccesser->getParamCustomizeModule().removeCustomize(2);
        moduleAccesser->getWorkManageModule().offFlag(0x12000013);
        playSound = true;
        break;
    }
    if (!silent && playSound == true) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fae), false, false, 0);
    }
}

extern soLogEventPresenter lbl_27_bss_2CBC;

void Fighter::notifyEventPikminFinalAttack(float damage, int attackerTaskId) {
    if (static_cast<u32>(attackerTaskId) == m_taskId ||
        m_moduleAccesser->getCameraModule().isClipIn(true) == true ||
        m_moduleAccesser->getCollisionHitModule().getWhole(0) != 0 ||
        m_moduleAccesser->getCollisionHitModule().getGlobal(0) != 0 ||
        m_moduleAccesser->getSituationModule().getKind() == 5 ||
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return;
    }
    Fighter* attacker = g_ftManager->getFighter(g_ftManager->m_finalEntryId, -1);
    if (attacker != 0) {
        soTeamModule& team = m_moduleAccesser->getTeamModule();
        int attackerTeam = soExternalValueAccesser::getTeamNo(attacker);
        int teamNo = team.getTeam()->getNo();
        if (teamNo == attackerTeam) {
            return;
        }
    }
    m_moduleAccesser->getDamageModule().addDamage(damage, 0);
    int entryId = g_ftManager->m_finalEntryId;
    if (g_ftManager->isValidEntryId(entryId)) {
        Fighter* fighter = g_ftManager->getFighter(entryId, -1);
        if (fighter != 0) {
            lbl_27_bss_2CBC.notifyLogEventCollisionHit(damage, fighter->m_taskId, m_taskId, 0);
        }
    }
}

bool Fighter::isHeartSwapEnableCondition() {
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000007) == true ||
        g_ftCommonDataAccesser.getFtStatusData(getFtKind(), m_moduleAccesser->getStatusModule().getStatusKind())->unk4_0 != 0 ||
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return false;
    }
    return true;
}

bool Fighter::isTreadPassive() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    switch (moduleAccesser->getSituationModule().getKind()) {
    case 0:
    case 1:
    case 2: {
        int statusKind = moduleAccesser->getStatusModule().getStatusKind();
        if (g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind)->unk0_1 == 2) {
            return false;
        }
        return moduleAccesser->getCollisionHitModule().getTotalStatus(0) == 0;
    }
    default:
        return false;
    }
}

void Fighter::dropDragoonAll() {
    u8 count = g_ftManager->getDragoonCount(m_entryId);
    for (int i = count - 1; i >= 0; i--) {
        int variation = g_ftManager->getDragoonVariation(m_entryId, static_cast<u8>(i));
        g_ftManager->removeDragoon(m_entryId, static_cast<u8>(variation));
        if (m_moduleAccesser->getItemManageModule().haveItem(static_cast<itKind>(0x14), static_cast<u8>(variation), 0, false)) {
            float rnd = 0.5f * randf();
            float idx = static_cast<float>(i);
            float angle = 45.0f + 45.0f * idx + 45.0f * rnd;
            m_moduleAccesser->getItemManageModule().dropItem(angle, 1.0f, 0);
        }
    }
}

bool Fighter::isEnableWarp() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (getCaptureModule(moduleAccesser).unk1C() == true ||
        moduleAccesser->getSituationModule().getKind() == 5) {
        return false;
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(0x12000034)) {
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xce2, 0);
        if (moduleAccesser->getKineticModule().getEnergy(4)->getSpeed().lengthSq() > limit * limit) {
            return false;
        }
    }
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    return g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind)->unk0_30 == 0;
}

bool Fighter::setMetal(bool setStatus, float health, int frames) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        int duration = frames;
        if (frames == -1) {
            moduleAccesser->getControllerModule().setRumble(0xd, 0, false, -1);
            duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5a7d, 0);
        }
        moduleAccesser->getWorkManageModule().setInt(duration, 0x10000025);
        float power = health;
        if (-1.0f == health) {
            power = soValueAccesser::getConstantFloat(moduleAccesser, 0xd9d, 0);
        }
        moduleAccesser->getWorkManageModule().setFloat(power, 0x1100000c);
        moduleAccesser->getParamCustomizeModule().addCustomize(3);
        moduleAccesser->getModelModule().switchEnvMap(true);
        moduleAccesser->getWorkManageModule().onFlag(0x12000012);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
        moduleAccesser->getParamCustomizeModule().removeCustomize(3);
        moduleAccesser->getModelModule().switchEnvMap(false);
        moduleAccesser->getWorkManageModule().offFlag(0x12000012);
    }
    // MATCH-ONLY: symbols.txt mangles Fighter::onSetMetal as (bool) while native setMetal forwards (bool, float, int).
    typedef void (*OnSetMetalFn)(Fighter*, bool, float, int);
    OnSetMetalFn* vtbl = *reinterpret_cast<OnSetMetalFn**>(reinterpret_cast<u8*>(this) + 0x3c);
    vtbl[0x2d0 / 4](this, setStatus, health, frames);
    return true;
}

static u8 s_invalidLoupeNo = 0xff;
static s32 s_invalidPhysicsId = -1;

void Fighter::createModel(u32 resId, void* unk2) {
    ftData* data = g_ftCommonDataAccesser.getData(m_kind);
    bool spycloak = getOwner()->getSpycloak();
    u8 loupeNo;
    if ((spycloak == true || getOwner()->isSubOwner() == true) ? true : false) {
        loupeNo = s_invalidLoupeNo;
    } else {
        loupeNo = static_cast<u8>(g_ftManager->getPlayerNo(m_entryId));
    }
    if (spycloak == true) {
        unk2 = NULL;
    } else if (unk2 == NULL) {
        unk2 = data->unk48;
    }
    m_moduleAccesser->getModelModule().constructInstance(
        spycloak == true ? static_cast<gfSceneRoot::LayerType>(5) : static_cast<gfSceneRoot::LayerType>(1),
        data->unk74, true, 1, g_soCommonDataAccesser.getResFileTex(),
        spycloak == true ? "FB" : NULL, "ExtMtl", resId, unk2, loupeNo, 0);
    soModelModule& model = m_moduleAccesser->getModelModule();
    int nodeId = model.getRealNodeId(model.getCorrectNodeId(0x12d));
    u32 etcResId = m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getEtcResId();
    if (etcResId != 0xffff) {
        u8 flag = (lbl_27_rodata_1E18[getFtKind()] >> 3) & 1;
        if (resId == 0xffff) {
            resId = etcResId;
        }
        getPhysicsModule(m_moduleAccesser).unk18(
            resId, data->unk6C, data->unk70, nodeId, s_invalidPhysicsId,
            soValueAccesser::getConstantIndefinite(m_moduleAccesser, 0xa800, 0), flag, -1);
    }
}

void Fighter::attachInstance() {
    ftData* data = g_ftCommonDataAccesser.getData(m_kind);
    bool spycloak = getOwner()->getSpycloak();
    u8 loupeNo;
    if ((spycloak == true || getOwner()->isSubOwner() == true) ? true : false) {
        loupeNo = s_invalidLoupeNo;
    } else {
        loupeNo = static_cast<u8>(g_ftManager->getPlayerNo(m_entryId));
    }
    void* unk2 = spycloak == true ? NULL : data->unk48;
    m_moduleAccesser->getModelModule().constructInstance(
        spycloak == true ? static_cast<gfSceneRoot::LayerType>(5) : static_cast<gfSceneRoot::LayerType>(1),
        data->unk74, true, 1, g_soCommonDataAccesser.getResFileTex(),
        spycloak == true ? "FB" : NULL, "ExtMtl", 0xffff, unk2, loupeNo, 0);
    int nodeId = m_moduleAccesser->getModelModule().getRealNodeId(
        m_moduleAccesser->getModelModule().getCorrectNodeId(0x12d));
    u32 etcResId = m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getEtcResId();
    if (etcResId != 0xffff) {
        ftKind kind = getFtKind();
        getPhysicsModule(m_moduleAccesser).unk18(
            etcResId, data->unk6C, data->unk70, nodeId, s_invalidPhysicsId,
            soValueAccesser::getConstantIndefinite(m_moduleAccesser, 0xa800, 0),
            (lbl_27_rodata_1E18[kind] >> 3) & 1, -1);
    }
    int hipNode = m_moduleAccesser->getModelModule().getCorrectNodeId(0x12d);
    int xRotNode = m_moduleAccesser->getModelModule().getCorrectNodeId(0x12e);
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(xRotNode, false);
    soMotionModule& motion = m_moduleAccesser->getMotionModule();
    motion.setShareAdjustNode(hipNode, pos.m_y / 10.688f);
    Vec3f trans = m_moduleAccesser->getMotionModule().calcNode(0x20, hipNode);
    getCaptureModule(m_moduleAccesser).unk14(xRotNode, hipNode, trans.m_y);
    getCatchModule(m_moduleAccesser).unk14(xRotNode, hipNode, trans.m_y);
    m_moduleAccesser->getShadowModule().setYOffset(trans.m_y);
}

void Fighter::setSuperStar(bool setStatus, int duration, bool checkStatus) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (checkStatus == true) {
        if (g_ftCommonDataAccesser.getFtStatusData(getFtKind(), moduleAccesser->getStatusModule().getStatusKind())->unk4_2 != 0) {
            return;
        }
    }
    if (setStatus == true) {
        int frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a77, 0);
        if (duration == -1) {
            duration = frames;
            moduleAccesser->getControllerModule().setRumble(0x12, 0, false, -1);
        }
        moduleAccesser->getWorkManageModule().setInt(duration, 0x1000001e);
        moduleAccesser->getCollisionHitModule().setInvincibleFrameGlobal(duration, true, 0);
        moduleAccesser->getEffectModule().removeTime(0x55);
        moduleAccesser->getEffectModule().reqTimeFollow(
            static_cast<EfID>(0x55), 0, duration,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f),
            soValueAccesser::getConstantFloat(moduleAccesser, 0xbe9, 0), 0);
        moduleAccesser->getEffectModule().removeCommon(0x17);
        moduleAccesser->getEffectModule().reqCommon(0x17, static_cast<float>(duration));
        if (moduleAccesser->getWorkManageModule().getInt(0x1000001f) == -1) {
            int handle = moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x2074), true);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x1000001f);
        }
        moduleAccesser->getWorkManageModule().onFlag(0x1200000e);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x1200000e) == true) {
        int handle = moduleAccesser->getWorkManageModule().getInt(0x1000001f);
        if (handle != -1) {
            moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
            moduleAccesser->getWorkManageModule().setInt(-1, 0x1000001f);
        }
        moduleAccesser->getEffectModule().removeTime(0x55);
        moduleAccesser->getEffectModule().removeCommon(0x17);
        moduleAccesser->getWorkManageModule().offFlag(0x1200000e);
    }
}

void Fighter::setFlower(bool setStatus, float rate, float size, bool unk4, bool unk5) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000009) == true) {
            float clatter = moduleAccesser->getControllerModule().getClatterTime(1);
            float maxClatter = soValueAccesser::getConstantFloat(moduleAccesser, 0xd84, 0);
            float next = clatter + rate;
            if (next > maxClatter) {
                next = maxClatter;
            }
            moduleAccesser->getControllerModule().setClatterTime(next, 1);
        } else if (moduleAccesser->getItemManageModule().attachItem(static_cast<itKind>(0x2f), 0, unk4) == true) {
            float base = rate + static_cast<float>(soValueAccesser::getConstantInt(moduleAccesser, 0x5a69, 0));
            float arg3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd83, 0);
            float arg2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd82, 0);
            moduleAccesser->getControllerModule().startClatter(base, arg2, arg3, 0, -1, 1, false);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000009) == true) {
            moduleAccesser->getWorkManageModule().addFloat(size, 0x11000006);
            float flowerScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd86, 0) +
                moduleAccesser->getWorkManageModule().getFloat(0x11000006) *
                soValueAccesser::getConstantFloat(moduleAccesser, 0xd88, 0);
            float maxScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd87, 0);
            if (flowerScale > maxScale) {
                flowerScale = maxScale;
            }
            float scale = flowerScale * soValueAccesser::getConstantFloat(moduleAccesser, 0xbe9, 0);
            moduleAccesser->getItemManageModule().scaleAttach(static_cast<itKind>(0x2f), scale);
        }
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x12000009) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x12000009);
        moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000006);
        if (unk5 == true) {
            moduleAccesser->getItemManageModule().dropAttach(static_cast<itKind>(0x2f), 90.0f, 0.0f);
        } else {
            moduleAccesser->getItemManageModule().ejectAttach(static_cast<itKind>(0x2f), true, false);
        }
    }
}

void Fighter::setCurry(bool setStatus, int duration) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        bool knockedOut;
        if (static_cast<bool>(g_ftManager->m_isStamina) == true &&
            getOwner()->getHitPoint() < 1.0f &&
            m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == false) {
            knockedOut = true;
        } else {
            knockedOut = false;
        }
        if (knockedOut == true) {
            return;
        }
        if (duration == -1) {
            duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5a6c, 0);
        }
        moduleAccesser->getWorkManageModule().setInt(duration, 0x10000019);
        if (!moduleAccesser->getWorkManageModule().isFlag(0x1200000a)) {
            float angle = soValueAccesser::getConstantFloat(moduleAccesser, 0xd8b, 0);
            moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000007);
            float angleSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xd8c, 0);
            moduleAccesser->getWorkManageModule().setFloat(angleSpeed, 0x11000008);
            moduleAccesser->getWorkManageModule().setInt(0, 0x1000001a);
            if (!getOwner()->getCurry()) {
                moduleAccesser->getEffectModule().removeAll(8);
                moduleAccesser->getEffectModule().reqEmit(static_cast<EfID>(0x53), 8);
            }
            if (moduleAccesser->getWorkManageModule().getInt(0x1000002e) == -1) {
                int handle = moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1f4d), true);
                moduleAccesser->getWorkManageModule().setInt(handle, 0x1000002e);
            }
            moduleAccesser->getWorkManageModule().onFlag(0x1200000a);
        }
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x1200000a) == true) {
        moduleAccesser->getEffectModule().removeAll(8);
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000008) == true) {
            moduleAccesser->getEffectModule().reqEmit(static_cast<EfID>(0x48), 8);
        }
        int handle = moduleAccesser->getWorkManageModule().getInt(0x1000002e);
        if (handle != -1) {
            moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
            moduleAccesser->getWorkManageModule().setInt(-1, 0x1000002e);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x1200000a);
    }
}

void Fighter::notifyEventCollisionAttack(float power, const soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(collisionLog->m_taskCategory), collisionLog->m_taskId);
    soCollisionAttackData* attackData = moduleAccesser->getCollisionAttackModule().getData(collisionLog->m_damageIndex, collisionLog->m_isAbsolute);
    int hitStop = soDamageUtilActor::calcHitStopFrame(moduleAccesser, power, 1.0f, 1.0f, attackData, 0);
    switch (collisionLog->m_kind) {
    case 1:
        if (task != NULL) {
            soCollisionHitModule* hitModule = soExternalValueAccesser::getCollisionHitModule(&dynamic_cast<StageObject&>(*task));
            if (hitModule->isReactionFrame() == true) {
                hitStop = hitModule->getReactionFrame();
            }
        }
        break;
    case 2:
        if (task != NULL) {
            soCollisionShieldModule* shieldModule = soExternalValueAccesser::getCollisionShieldModule(&dynamic_cast<StageObject&>(*task));
            if (shieldModule->isTurn(collisionLog->_35) == true) {
                unk138_0 = true;
            } else {
                unk130 = power;
                unk134 = moduleAccesser->getCollisionAttackModule().getPosX() < shieldModule->getPosX(collisionLog->_35) ? 1.0f : -1.0f;
            }
        }
        break;
    case 0:
        if (moduleAccesser->getSituationModule().getKind() == 0 && task != NULL) {
            if (!moduleAccesser->getCollisionAttackModule().checkRebound(moduleAccesser, &dynamic_cast<StageObject&>(*task), collisionLog, power, &unk128, &unk12C)) {
                return;
            }
            hitStop = soDamageUtilActor::calcHitStopFrame(moduleAccesser, unk128, 1.0f, 1.0f, attackData, 0);
        }
        break;
    }
    if (unk114 < power) {
        unk114 = power;
    }
    if (unk118 < hitStop) {
        unk118 = hitStop;
    }
    // MATCH-ONLY: symbols.txt mangles Fighter::notifyEventCollisionAttackFighter without power while native forwards (float, const soCollisionLog*, soModuleAccesser*).
    typedef void (*NotifyAttackFighterFn)(Fighter*, float, const soCollisionLog*, soModuleAccesser*);
    NotifyAttackFighterFn* vtbl = *reinterpret_cast<NotifyAttackFighterFn**>(reinterpret_cast<u8*>(this) + 0x3c);
    vtbl[0x2e8 / 4](this, power, collisionLog, moduleAccesser);
    moduleAccesser->getStatusModule().checkAttack(moduleAccesser, const_cast<soCollisionLog*>(collisionLog), power);
}

void Fighter::notifyHaveItem(itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool silent) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    int subKind = item->soGetSubKind();
    int trait = item->getParam(static_cast<itValueAccesser::ParamInt>(0x5bce));
    if (!silent) {
        moduleAccesser->getEffectModule().req(
            static_cast<EfID>(0x35), nodeIndex,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
        if (!(trait & 0x2000)) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fad), false, false, 0);
        }
    }
    setItemHoldAnim(item, index, true);
    switch (subKind) {
    case 0xd:
        moduleAccesser->getWorkManageModule().onFlag(0x1200002d);
        switch (moduleAccesser->getSituationModule().getKind()) {
        case 0:
            moduleAccesser->getStatusModule().changeStatusRequest(0xa4, moduleAccesser);
            break;
        default:
            moduleAccesser->getStatusModule().changeStatusRequest(0xad, moduleAccesser);
            break;
        }
        break;
    case 0x45: {
        moduleAccesser->getLinkModule().link(3, item->m_taskId);
        int targetNode = item->getParam(static_cast<itValueAccesser::ParamInt>(0x5bcd));
        moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, 0, targetNode, 3, true);
        moduleAccesser->getStatusModule().changeStatusRequest(0xdc, moduleAccesser);
        item->updateLogAttackInfo();
        break;
    }
    case 0x15: {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        moduleAccesser->getItemManageModule().warpItem(&pos, index);
        moduleAccesser->getLinkModule().link(3, item->m_taskId);
        int targetNode = item->getParam(static_cast<itValueAccesser::ParamInt>(0x5bcd));
        moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, 0, targetNode, 3, true);
        moduleAccesser->getStatusModule().changeStatusRequest(0xde, moduleAccesser);
        break;
    }
    case 0x1b:
    case 0x1d:
        setHammer(true, subKind == 0x1b);
        break;
    case 0:
        moduleAccesser->getWorkManageModule().onFlag(0x1200002b);
        break;
    }
    int playerNo = g_ftManager->getPlayerNo(m_entryId);
    if (!(item->m_2260 & (1 << playerNo)) && !(trait & 0x200000)) {
        getOwner()->addLogInt(1, 0x1e, 0);
        item->setHavedLog(playerNo, true);
    }
}

void Fighter::processFixCamera() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    StageObject::processFixCamera();
    if (moduleAccesser->getSituationModule().getKind() != 5) {
        if (moduleAccesser->getCameraModule().isClipIn(false) == true) {
            setLoupe(false);
            if (moduleAccesser->getWorkManageModule().isFlag(0x1200001b) == true &&
                moduleAccesser->getWorkManageModule().getInt(0x10000020) <= 0) {
                setCursor(true);
            } else {
                setCursor(false);
            }
            if (moduleAccesser->getWorkManageModule().isFlag(0x1200001c) == true) {
                setNameCursor(true);
            } else {
                setNameCursor(false);
            }
        } else {
            s8 loupeNo = moduleAccesser->getModelModule().getLoupeNo();
            if (moduleAccesser->getSituationModule().getKind() != 5 &&
                ((moduleAccesser->getWorkManageModule().isFlag(0x12000019) == true &&
                  moduleAccesser->getVisibilityModule().isVisible() == true) ||
                 moduleAccesser->getWorkManageModule().isFlag(0x1200003a) == true) &&
                static_cast<u8>(loupeNo) != s_invalidLoupeNo) {
                setLoupe(true);
                soCameraClipSphere* sphere = moduleAccesser->getCameraModule().getClipSphere(0);
                Vec3f offset(sphere->m_offset.m_x, sphere->m_offset.m_y, sphere->m_offset.m_z);
                Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(sphere->m_nodeId, &offset, false, false);
                float scale = moduleAccesser->getPostureModule().getScale();
                gfLoupe::setPlayerLoupeInfo(static_cast<u8>(loupeNo), &pos, 2.0f * (sphere->m_radius * scale));
            } else {
                setLoupe(false);
            }
            setCursor(false);
            setNameCursor(false);
        }
        if (CameraController::getInstance()->m_currentControllerKind != CameraController::Tech) {
            if (moduleAccesser->getCameraModule().isClipIn(true) == true) {
                setLoupeDamage(false);
            } else {
                if (moduleAccesser->getSituationModule().getKind() != 5 &&
                    moduleAccesser->getWorkManageModule().isFlag(0x1200001a) == true &&
                    !moduleAccesser->getWorkManageModule().isFlag(0x12000007) &&
                    !moduleAccesser->getWorkManageModule().isFlag(0x12000018) &&
                    moduleAccesser->getCollisionHitModule().getInvincibleFrameGlobal(0) <= 0) {
                    setLoupeDamage(true);
                } else {
                    setLoupeDamage(false);
                }
            }
        }
    } else {
        setCursor(false);
        setNameCursor(false);
        setLoupe(false);
        setLoupeDamage(false);
    }
    if (soSlow::getInstance()->isAdjust() == true && !g_ftManager->m_isHomerun) {
        int deadReason = checkDead();
        if (deadReason != -1) {
            toDead(static_cast<Dead::Reason>(deadReason));
        }
    }
}

int Fighter::checkDead() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    int deadReason = -1;
    if (!moduleAccesser->getWorkManageModule().isFlag(0x12000001) &&
        moduleAccesser->getSituationModule().getKind() != 5) {
        Vec3f pos;
        if (!moduleAccesser->getLinkModule().isModelConstraint()) {
            pos = moduleAccesser->getPostureModule().getPos();
        } else {
            soModelModule& model = moduleAccesser->getModelModule();
            pos = model.getNodeGlobalPosition(model.getCorrectNodeId(0x12d), false);
        }
        switch (soGroundUtil::checkDeadArea(&pos, 0.0f, 0.0f)) {
        case 2: deadReason = 2; break;
        case 3: deadReason = 3; break;
        case 0: deadReason = 0; break;
        case 1: deadReason = 1; break;
        }
        if (deadReason != -1) {
            Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2));
            if (speed.length() < 0.00001f ||
                (g_ftManager->m_mode == 1 && moduleAccesser->getWorkManageModule().isFlag(0x12000034) == true)) {
                bool ignore = false;
                m_outsideEventPresenter.notifyOutsideEventOutsideDeadArea(
                    static_cast<soGroundUtil::DeadAreaCheckResult>(deadReason), &ignore);
                if (ignore == true) {
                    deadReason = -1;
                }
            }
            if (deadReason == 0) {
                if (moduleAccesser->getWorkManageModule().isFlag(0x12000029)) {
                    deadReason = -1;
                } else if (!g_ftManager->m_noDeadUp) {
                    SituationKind sit = moduleAccesser->getSituationModule().getKind();
                    if (sit != 0 && sit != 1) {
                        if (speed.m_y <= soValueAccesser::getConstantFloat(moduleAccesser, 0xce6, 0) &&
                            !moduleAccesser->getWorkManageModule().isFlag(0x22000004) &&
                            !moduleAccesser->getStopModule().isDamage()) {
                            deadReason = -1;
                        }
                    }
                }
            }
        }
        if (g_ftManager->m_mode == 1) {
            if (unk139_1 != true) {
                if (moduleAccesser->getGroundModule().isCrush(0.5f, 0) == true) {
                    deadReason = 4;
                }
            }
            unk139_1 = false;
        }
    }
    return deadReason;
}

Fighter::Fighter(s32 entryId, ftKind kind, Heaps::HeapType instHeap, soModuleAccesser* acc)
    : StageObject(static_cast<ftInfo*>(&g_ftInfo)->getNamePtr(kind), 10, 6, 3, false, acc),
      soStatusEventObserver(-1, -1),
      soSituationEventObserver(-1, -1),
      soCollisionAttackEventObserver(-1, -1),
      soCollisionHitEventObserver(-1, -1),
      soCollisionShieldEventObserver(-1, -1),
      soCollisionReflectorEventObserver(-1, -1),
      soCollisionAbsorberEventObserver(-1, -1),
      soCollisionSearchEventObserver(-1, -1),
      soCaptureEventObserver(-1, -1),
      soItemManageEventObserver(-1, -1),
      soMotionEventObserver(-1, -1),
      soDamageEventObserver(-1, -1),
      ftEntryEventObserver(g_ftManager->getEntryEventManageId(entryId), -1),
      soTurnEventObserver(-1, -1),
      m_entryId(entryId),
      m_kind(kind),
      unk138_loupe(0),
      unk138_loupeDamage(0),
      unk138_cursor(0),
      unk138_nameCursor(0),
      unk138_7(0),
      unk139_0(1),
      unk139_1(0),
      m_outsideEventPresenter(-1, entryId) { }

Fighter::~Fighter() { }

static inline void resetScalingCore(soModuleAccesser* accesser) {
    float scale = accesser->getWorkManageModule().getFloat(0x1100000b);
    accesser->getPostureModule().setScale(scale);
    if (accesser->getWorkManageModule().isFlag(0x12000014) == true) {
        accesser->getWorkManageModule().offFlag(0x12000014);
        accesser->getParamCustomizeModule().removeCustomize(0);
        accesser->getSoundModule().setSEPitch(accesser->getWorkManageModule().getFloat(0x11000011));
    }
    if (accesser->getWorkManageModule().isFlag(0x12000015) == true) {
        accesser->getWorkManageModule().offFlag(0x12000015);
        accesser->getParamCustomizeModule().removeCustomize(1);
        accesser->getSoundModule().setSEPitch(accesser->getWorkManageModule().getFloat(0x11000011));
    }
    accesser->getWorkManageModule().setInt(0, 0x10000023);
}

void Fighter::startScaling(ScalingKind kind, ScalingType type) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000021) == true ||
        moduleAccesser->getWorkManageModule().isFlag(0x12000032) == true ||
        moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return;
    }
    int status = moduleAccesser->getWorkManageModule().getInt(0x10000023);
    int curKind = moduleAccesser->getWorkManageModule().getInt(0x10000024);
    if (kind == 2 || curKind != kind) {
        switch (type) {
        case 0:
            if (curKind != kind) {
                resetScalingCore(m_moduleAccesser);
            }
            moduleAccesser->getWorkManageModule().setInt(kind, 0x10000024);
            moduleAccesser->getWorkManageModule().setInt(4, 0x10000023);
            moduleAccesser->getStatusModule().changeStatusRequest(0xf2, moduleAccesser);
            break;
        case 1:
            if (curKind != kind) {
                resetScalingCore(m_moduleAccesser);
            }
            moduleAccesser->getWorkManageModule().setInt(kind, 0x10000024);
            moduleAccesser->getWorkManageModule().setInt(6, 0x10000023);
            moduleAccesser->getStatusModule().changeStatusRequest(0xf3, moduleAccesser);
            break;
        }
    } else {
        switch (type) {
        case 0:
            if (status < 3) {
                if (curKind != kind) {
                    resetScalingCore(m_moduleAccesser);
                }
                moduleAccesser->getWorkManageModule().setInt(kind, 0x10000024);
                moduleAccesser->getWorkManageModule().setInt(4, 0x10000023);
                moduleAccesser->getStatusModule().changeStatusRequest(0xf2, moduleAccesser);
            } else if (status > 4) {
                moduleAccesser->getWorkManageModule().setInt(kind, 0x10000024);
                moduleAccesser->getWorkManageModule().setInt(1, 0x10000023);
                moduleAccesser->getStatusModule().changeStatusRequest(0xf3, moduleAccesser);
            } else {
                if (kind == 1) {
                    int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a79, 0);
                    moduleAccesser->getWorkManageModule().setInt(frame, 0x10000021);
                } else {
                    int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a7b, 0);
                    moduleAccesser->getWorkManageModule().setInt(frame, 0x10000022);
                }
            }
            break;
        case 1:
            if (status < 3) {
                if (curKind != kind) {
                    resetScalingCore(m_moduleAccesser);
                }
                moduleAccesser->getWorkManageModule().setInt(kind, 0x10000024);
                moduleAccesser->getWorkManageModule().setInt(6, 0x10000023);
                moduleAccesser->getStatusModule().changeStatusRequest(0xf3, moduleAccesser);
            } else if (status < 5) {
                moduleAccesser->getWorkManageModule().setInt(kind, 0x10000024);
                moduleAccesser->getWorkManageModule().setInt(2, 0x10000023);
                moduleAccesser->getStatusModule().changeStatusRequest(0xf2, moduleAccesser);
            } else {
                if (kind == 1) {
                    int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a79, 0);
                    moduleAccesser->getWorkManageModule().setInt(frame, 0x10000021);
                } else {
                    int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a7b, 0);
                    moduleAccesser->getWorkManageModule().setInt(frame, 0x10000022);
                }
            }
            break;
        }
    }
}

void Fighter::notifyThrowItem(BaseItem* item, u32 nodeIndex, int* outUnk) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getMotionModule().getKind() == 0xfe) {
        if (item->soGetSubKind() == 0x48) {
            *reinterpret_cast<bool*>(outUnk) = true;
            return;
        }
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fae), false, false, 0);
    } else {
        if (item->getParam(static_cast<itValueAccesser::ParamInt>(0x5b69)) == 0) {
            if (item->soGetSubKind() == 0x48) {
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x70), false, false, 0);
            } else {
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fb0), false, false, 0);
            }
            if (!moduleAccesser->getWorkManageModule().isFlag(0x1200002a)) {
                moduleAccesser->getControllerModule().setRumble(0x1b, 0, false, -1);
            } else {
                moduleAccesser->getControllerModule().setRumble(0x1c, 0, false, -1);
            }
        } else {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fb0), false, false, 0);
            if (!moduleAccesser->getWorkManageModule().isFlag(0x1200002a)) {
                moduleAccesser->getControllerModule().setRumble(0x1d, 0, false, -1);
            } else {
                moduleAccesser->getControllerModule().setRumble(0x1e, 0, false, -1);
            }
        }
        int idx = moduleAccesser->getMotionModule().getKind() - 0xff;
        ftDataCommon::ThrowEffectTable* table = g_ftCommonData.dataCommon->throwEffectTable;
        if (idx >= 0 && idx < table->count) {
            moduleAccesser->getEffectModule().req(
                table->entries[idx].ef1, 0,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, table->entries[idx].yRot, 0.0f), 1.0f,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
            if (table->entries[idx].ef2 != 0) {
                moduleAccesser->getEffectModule().req(
                    table->entries[idx].ef2, nodeIndex,
                    &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
                    &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
            }
        }
    }
    item->updateLogAttackInfo();
}

void Fighter::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    const ftStatusFlagData* ftStatusData = g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind);
    ftOwner* owner = getOwner();
    int hitStop = 0;
    if (ftStatusData->unk4_7) {
        hitStop = soValueAccesser::getConstantInt(moduleAccesser, 0x5a03, 0);
        if (moduleAccesser->getPostureModule().setStickLR(soValueAccesser::getConstantFloat(moduleAccesser, 0xc70, 0)) == true) {
            moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
    moduleAccesser->getWorkManageModule().setInt(hitStop, 0x10000037);
    updateNodeSRT();
    for (int i = 1; i < 0x13; i++) {
        moduleAccesser->getStatusModule().unableTransitionTermGroup(i);
        moduleAccesser->getStatusModule().enableTransitionTermAll(i);
    }
    moduleAccesser->getStatusModule().clearTransitionTermAll(0x13);
    moduleAccesser->getStatusModule().unableTransitionTermGroup(0x13);
    moduleAccesser->getGroundModule().selectCliffHangData(0, 0);
    moduleAccesser->getCameraModule().setQuake(true);
    if (!ftStatusData->unk4_12) {
        moduleAccesser->getCameraModule().resetCameraRange(0);
        moduleAccesser->getCameraModule().setEnableUpdatePos(true, -1);
        moduleAccesser->getCameraModule().setEnableInterpolatePos(false, -1);
    }
    bool checkCatch = false;
    if (!moduleAccesser->getWorkManageModule().isFlag(0x12000018) &&
        moduleAccesser->getWorkManageModule().isFlag(0x1200002c) == true) {
        checkCatch = !ftStatusData->isUnk0_4();
    }
    moduleAccesser->getCollisionHitModule().setCheckCatch(checkCatch, 0);
    if (!ftStatusData->unk0_26) {
        getPhysicsModule(moduleAccesser).unk50();
    }
    if (statusData->m_situationKind != 5) {
        getPhysicsModule(moduleAccesser).unkD8(1);
    } else {
        getPhysicsModule(moduleAccesser).unkD8(0);
    }
    moduleAccesser->getKineticModule().resumeEnergyAll();
    moduleAccesser->getWorkManageModule().offFlag(0x12000001);
    moduleAccesser->getWorkManageModule().offFlag(0x12000029);
    moduleAccesser->getWorkManageModule().offFlag(0x12000034);
    moduleAccesser->getWorkManageModule().onFlag(0x12000019);
    moduleAccesser->getWorkManageModule().onFlag(0x1200001a);
    moduleAccesser->getWorkManageModule().offFlag(0x1200003a);
    if (!ftStatusData->unk4_10) {
        moduleAccesser->getWorkManageModule().setFlag(false, 0x1200001b);
        setCursor(false);
        bool showName = !ftStatusData->isUnk4_13();
        moduleAccesser->getWorkManageModule().setFlag(showName, 0x1200001c);
        setNameCursor(showName);
        moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000012);
        moduleAccesser->getWorkManageModule().offFlag(0x1200003c);
        moduleAccesser->getWorkManageModule().offFlag(0x12000038);
    }
    moduleAccesser->getWorkManageModule().setFlag(ftStatusData->unk4_6, 0x12000030);
    moduleAccesser->getWorkManageModule().setFlag(ftStatusData->unk4_5, 0x12000031);
    moduleAccesser->getWorkManageModule().setFlag(ftStatusData->unk4_1, 0x12000032);
    moduleAccesser->getWorkManageModule().setFlag(ftStatusData->unk4_15, 0x12000007);
    soDamageAttackerInfo attackerInfo;
    moduleAccesser->getDamageModule().getAttackerInfo(&attackerInfo);
    if (attackerInfo.m_directSoKind != -1 && statusData->m_situationKind == 0 && ftStatusData->unk0_0) {
        if (attackerInfo.m_clearFrame == -1) {
            attackerInfo.m_clearFrame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a2f, 0);
            moduleAccesser->getDamageModule().restoreAttackerInfo(&attackerInfo);
        }
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000039);
    if (moduleAccesser->getWorkManageModule().getInt(0x1000003c) <= 0) {
        moduleAccesser->getWorkManageModule().offFlag(0x12000004);
    }
    if (moduleAccesser->getItemManageModule().isHaveItem(2) == true) {
        moduleAccesser->getItemManageModule().dropItem(90.0f, 0.0f, 2);
    }
    if (moduleAccesser->getItemManageModule().getHaveItemTrait(0).m_mask & 0x20) {
        moduleAccesser->getItemManageModule().useItem(0ul);
    }
    if (moduleAccesser->getWorkManageModule().getFloat(0x1100000e) != 0.0f) {
        if (!ftStatusData->noTread || moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            if (!g_ftAudienceManager->m_audience->checkTumultRange(pos.m_x)) {
                moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x1100000e);
            }
        }
    }
    ftLogTransactor::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser, owner);
    if (getGlowModule(moduleAccesser).unk20() == true) {
        getGlowModule(moduleAccesser).unk80(statusKind, prevStatusKind, statusData, moduleAccesser);
        int actionKind = owner->getLog().m_logActionInfo.unk4;
        if (actionKind >= 0x13 && actionKind <= 0x3c) {
            getGlowModule(moduleAccesser).unk54(owner->getSpecialWeaponGlowAttack());
        }
    }
    getSlopeModule(moduleAccesser).setTopAngleUnlimit(ftStatusData->unk4_11);
    moduleAccesser->getKineticModule().setConsiderGroundFriction(!ftStatusData->isUnk4_14(), soKineticEnergy::AttributeFlag(1));
    moduleAccesser->getMotionModule().setShareAnimObjKind(-1);
    bool noScale = statusKind >= 0x111;
    if (ftStatusData->unk4_16) {
        noScale = false;
    }
    moduleAccesser->getMotionModule().setTransMoveSpeedNoScale(noScale);
    if (ftStatusData->unk4_17 == true) {
        soLinkEventArgs args(0x454);
        moduleAccesser->getLinkModule().sendEventNodes(-1, args, 0);
    }
}

#include <so/so_module_accesser.h>
#include <ft/ft_audience_manager.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_vector.h>
// TODO: clean up, give variables real names
namespace jj_12B4F0 {
#pragma fp_contract off

typedef unsigned int u32;
typedef int s32;

struct D8 {
    char pad00[0x0C];
    void* p0C;
    void* p10;
    void* p14;
    char pad18[8];
    void* p20;
    char pad24[0x14];
    void* p38;
    char pad3C[0x14];
    void* p50;
    char pad54[8];
    void* p5C;
    void* p60;
    void* p64;
    char pad68[8];
    void* p70;
    char pad74[0x14];
    void* p88;
    char pad8C[0x1C];
    void* pA8;
};

struct FighterBase {
    char pad[0x3C];
};

#define V8(p) \
    virtual void p##0(); virtual void p##1(); \
    virtual void p##2(); virtual void p##3(); \
    virtual void p##4(); virtual void p##5(); \
    virtual void p##6(); virtual void p##7();
#define V5(p) \
    virtual void p##0(); virtual void p##1(); \
    virtual void p##2(); virtual void p##3(); \
    virtual void p##4();
#define V16(p) V8(p##a) V8(p##b)
#define V24(p) V8(p##a) V8(p##b) V8(p##c)
#define V30(p) \
    virtual void p##0(); virtual void p##1(); \
    virtual void p##2(); virtual void p##3(); \
    virtual void p##4(); virtual void p##5(); \
    virtual void p##6(); virtual void p##7(); \
    virtual void p##8(); virtual void p##9(); \
    virtual void p##10(); virtual void p##11(); \
    virtual void p##12(); virtual void p##13(); \
    virtual void p##14(); virtual void p##15(); \
    virtual void p##16(); virtual void p##17(); \
    virtual void p##18(); virtual void p##19(); \
    virtual void p##20(); virtual void p##21(); \
    virtual void p##22(); virtual void p##23(); \
    virtual void p##24(); virtual void p##25(); \
    virtual void p##26(); virtual void p##27(); \
    virtual void p##28(); virtual void p##29();
#define V32(p) V8(p##a) V8(p##b) V8(p##c) V8(p##d)
#define V2(p) virtual void p##0(); virtual void p##1();
#define V1(p) virtual void p##0();

class FighterView : public FighterBase {
public:
    V32(a)
    V5(b)
    virtual void* slot37(void*);
    V32(c)
    V32(d)
    V32(e)
    V30(f)
    virtual void slot164();
    virtual void slot165(int);
    virtual void slot166(int, bool);
    virtual void slot167(int);
    virtual void slot168(int);
    V16(g)
    virtual void* slot185();
    char pad40[0x20];
    void* accesser;
};

class P0C {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
};

class P14 {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6(int, int);
};

class P20 {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5(int, int, int, int);
    V24(a)
    virtual void s30(int, int, int);
};

class P38_base {
public:
    char pad[8];
};

class P38 : public P38_base {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8(float);
};

class P10_base {
public:
    char pad[8];
};

class P10 : public P10_base {
public:
    V32(h)
    V32(i)
    V32(j)
    V16(k)
    virtual u32 s112(int);
};

class P64 {
public:
    V16(l)
    V1(m)
    virtual u32 s17(u32);
};

class P70 {
public:
    V32(n)
    V2(o)
    virtual void event(int, void*);
};

class P5C {
public:
    V8(p)
    virtual void q0();
    virtual void q1();
    virtual void q2();
    virtual void q3();
    virtual void q4();
    virtual void set(void*, int);
};

class P88 {
public:
    V8(q)
    virtual void call(int, int, float*, float*, float*, float*, int, float, int);
};

class POwner_base {
public:
    char pad[0x0C];
};

class POwner : public POwner_base {
public:
    virtual void s0();
    virtual int s1();
};

class PA8 {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6(int, int, int, int);
};

extern "C" void notifyOutsideEventEntryEnd__23ftOutsideEventPresenterFv(void*);
extern "C" u32 getRebirthXlu__7ftOwnerFv(void*);
extern "C" s32 fn_27_112AF0(void*);
extern "C" int getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(void*, u32, u32);
extern "C" const float lbl_27_rodata_1048;
extern "C" const float lbl_27_rodata_104C;

#define D (*(*(D8**)((char*)moduleAccesser + 0xD8)))

extern "C" void start__7FighterFP5Vec3fffib(
    FighterView* self, void*, float, float arg2, int arg3, bool arg4
) {
    void* moduleAccesser = self->accesser;

    ((P0C*)D.p0C)->s2();
    ((P38*)D.p38)->s8(arg2);
    self->slot165(arg3);

    switch (arg3) {
    case 0:
        if (((P10*)D.p10)->s112(0) == 1) {
            ((P14*)D.p14)->s6(0, 0);
            ((P70*)D.p70)->event(0x10E, moduleAccesser);
        } else {
            ((P14*)D.p14)->s6(2, 0);
            ((P70*)D.p70)->event(0xE, moduleAccesser);
            notifyOutsideEventEntryEnd__23ftOutsideEventPresenterFv((char*)self + 0x13C);
        }
        break;

    case 1:
    case 2:
        if (((P64*)D.p64)->s17(0x12000023) == 1) {
            ((P14*)D.p14)->s6(2, 0);
            ((P70*)D.p70)->event(0xBE, moduleAccesser);
        } else {
            self->slot167(1);
            self->slot168(1);

            if (((P10*)D.p10)->s112(0) == 1) {
                ((P14*)D.p14)->s6(0, 0);
                ((P70*)D.p70)->event(0, moduleAccesser);
            } else {
                ((P14*)D.p14)->s6(2, 0);
                ((P70*)D.p70)->event(0xE, moduleAccesser);
            }
        }

        notifyOutsideEventEntryEnd__23ftOutsideEventPresenterFv((char*)self + 0x13C);

        if (arg4 == 1) {
            void* owner = self->slot185();
            float values[12];
            values[0] = lbl_27_rodata_1048;
            values[1] = lbl_27_rodata_1048;
            values[2] = lbl_27_rodata_1048;
            values[3] = lbl_27_rodata_1048;
            values[4] = lbl_27_rodata_1048;
            values[5] = lbl_27_rodata_1048;
            values[6] = lbl_27_rodata_1048;
            values[7] = lbl_27_rodata_1048;
            values[8] = lbl_27_rodata_1048;
            values[9] = lbl_27_rodata_1048;
            values[10] = lbl_27_rodata_1048;
            values[11] = lbl_27_rodata_1048;
            int local = 0;

            ((P88*)D.p88)->call(0x1030017, 0x12E, &values[9], &values[6],
                                &values[3], &values[0], 0, lbl_27_rodata_104C, 0);

            ((P20*)D.p50)->s5(0x2288, 0, 0, 0);

            if (getRebirthXlu__7ftOwnerFv(owner) == 1) {
                int value = getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(
                    moduleAccesser, 0x5A3E, 0
                );
                ((P20*)D.p20)->s30(value, 0, 0);
            }

            if (((POwner*)owner)->s1() == 0) {
                int value = fn_27_112AF0(owner);
                if (value != -1) {
                    ((PA8*)D.pA8)->s6(value, 0, 0, 0);
                }
            }
            (void)local;
        }
        break;

    case 3:
        break;

    case 4: {
        D8* d8 = *(D8**)((char*)moduleAccesser + 0xD8);
        P5C* module = (P5C*)d8->p5C;
        void* old = self->slot37(d8);
        module->set(old, 1);
        ((P70*)D.p70)->event(0x10F, moduleAccesser);
        break;
    }

    case 5: {
        D8* d8 = *(D8**)((char*)moduleAccesser + 0xD8);
        P5C* module = (P5C*)d8->p5C;
        void* old = self->slot37(d8);
        module->set(old, 1);
        ((P70*)D.p70)->event(0x110, moduleAccesser);
        break;
    }

    default:
        break;
    }

    self->slot166(arg3, arg4);
}

#undef D
#undef V8
#undef V16
#undef V24
#undef V30
#undef V32
#undef V5
#undef V2
#undef V1
}

// TODO: clean up, give variables real names
namespace jj_12B9F0 {
typedef unsigned int u32;

struct Owner;

struct EventModule {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void notify(int, Owner*);
};

struct PostureModule {
    virtual void unused0();
    virtual void unused1();
    virtual void reset();
};

struct MotionHeader {
    char pad[8];
};

struct MotionModule : MotionHeader {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void setFrame(float);
};

struct Modules {
    char pad0[0x0c];
    PostureModule* posture;
    char pad1[0x38 - 0x10];
    MotionModule* motion;
    char pad2[0x70 - 0x3c];
    EventModule* event;
};

struct Owner {
    char pad[0xd8];
    Modules* modules;
};

struct ObjectHeader {
    char pad[0x3c];
};

struct Object : ObjectHeader {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual void unused41();
    virtual void unused42();
    virtual void update();
    virtual void unused44();
    virtual void unused45();
    virtual void unused46();
    virtual void unused47();
    virtual void unused48();
    virtual void unused49();
    virtual void unused50();
    virtual void unused51();
    virtual void unused52();
    virtual void unused53();
    virtual void unused54();
    virtual void unused55();
    virtual void unused56();
    virtual void unused57();
    virtual void unused58();
    virtual void unused59();
    virtual void unused60();
    virtual void unused61();
    virtual void unused62();
    virtual void unused63();
    virtual void unused64();
    virtual void unused65();
    virtual void unused66();
    virtual void unused67();
    virtual void unused68();
    virtual void unused69();
    virtual void unused70();
    virtual void unused71();
    virtual void unused72();
    virtual void unused73();
    virtual void unused74();
    virtual void unused75();
    virtual void unused76();
    virtual void unused77();
    virtual void unused78();
    virtual void unused79();
    virtual void unused80();
    virtual void unused81();
    virtual void unused82();
    virtual void unused83();
    virtual void unused84();
    virtual void unused85();
    virtual void unused86();
    virtual void unused87();
    virtual void unused88();
    virtual void unused89();
    virtual void unused90();
    virtual void unused91();
    virtual void unused92();
    virtual void unused93();
    virtual void unused94();
    virtual void unused95();
    virtual void unused96();
    virtual void unused97();
    virtual void unused98();
    virtual void unused99();
    virtual void unused100();
    virtual void unused101();
    virtual void unused102();
    virtual void unused103();
    virtual void unused104();
    virtual void unused105();
    virtual void unused106();
    virtual void unused107();
    virtual void unused108();
    virtual void unused109();
    virtual void unused110();
    virtual void unused111();
    virtual void unused112();
    virtual void unused113();
    virtual void unused114();
    virtual void unused115();
    virtual void unused116();
    virtual void unused117();
    virtual void unused118();
    virtual void unused119();
    virtual void unused120();
    virtual void unused121();
    virtual void unused122();
    virtual void unused123();
    virtual void unused124();
    virtual void unused125();
    virtual void unused126();
    virtual void unused127();
    virtual void unused128();
    virtual void unused129();
    virtual void unused130();
    virtual void unused131();
    virtual void unused132();
    virtual void unused133();
    virtual void unused134();
    virtual void unused135();
    virtual void unused136();
    virtual void unused137();
    virtual void unused138();
    virtual void unused139();
    virtual void unused140();
    virtual void unused141();
    virtual void unused142();
    virtual void unused143();
    virtual void unused144();
    virtual void unused145();
    virtual void unused146();
    virtual void unused147();
    virtual void unused148();
    virtual void unused149();
    virtual void unused150();
    virtual void unused151();
    virtual void unused152();
    virtual void unused153();
    virtual void unused154();
    virtual void unused155();
    virtual void unused156();
    virtual void unused157();
    virtual void unused158();
    virtual void unused159();
    virtual void unused160();
    virtual void unused161();
    virtual void unused162();
    virtual void unused163();
    virtual void unused164();
    virtual void enable(int);
    virtual void disable(int, int);

    char pad[0x60 - 0x40];
    Owner* owner;
};

extern "C" void fn_27_12B9F0(Object* self, void*, u32 mode, float, float frame) {
    Owner* owner = self->owner;
    if (mode == 1U) {
        owner->modules->event->notify(0xf6, owner);
    } else {
        owner->modules->posture->reset();
        self->enable(8);
        self->update();
        owner->modules->event->notify(0xbe, owner);
        self->disable(8, 0);
    }
    owner->modules->motion->setFrame(frame);
}
}

// TODO: clean up, give variables real names
namespace jj_12D228 {
class Owner;
class Object_vptr_c_base { public: char pad[12]; };
class Object_vptr_c : public Object_vptr_c_base {
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
virtual void slot13(int, int, int);
};
class Object_vptr_0 {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2();
virtual void slot3(int,int,int);
virtual void slot4();
virtual void slot5(int,int,int,int);
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
virtual int slot17(unsigned);
virtual void slot18(unsigned);
virtual void slot19(int,int);
virtual void slot20(int,unsigned);
virtual void slot21(int);
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
virtual void slot35(int,int,int*,int,int);
};
class Base3c { public: char pad[0x3c]; };
class Actor : public Base3c {
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
virtual int slot94(int);
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
virtual void slot172();
virtual void slot173();
virtual void slot174();
virtual void slot175();
virtual void slot176();
virtual void slot177();
virtual void slot178();
virtual void slot179();
virtual void slot180();
virtual void slot181();
virtual void slot182();
virtual void slot183();
virtual void slot184();
virtual Object_vptr_c* slot185();
virtual int slot186();
};
struct Modules { char pad0[0x50]; Object_vptr_0* sound; char pad54[4]; Object_vptr_0* ground; char pad5c[8]; Object_vptr_0* work; char pad68[0x20]; Object_vptr_0* other; };
struct Self : Actor { char pad40[0x20]; soModuleAccesser* accesser; };
extern Actor* g_Stage;
extern "C" unsigned int getCurry__7ftOwnerFv(Object_vptr_c*);
extern "C" void fn_27_12D228(Self* self, int arg) {
    soModuleAccesser* a = self->accesser;
    ((Modules**)((char*)a + 0xd8))[0]->work->slot18(0x12000006);
    ((Modules**)((char*)a + 0xd8))[0]->work->slot18(0x12000008);
    ((Modules**)((char*)a + 0xd8))[0]->work->slot20(arg, 0x1200001e);
    if (!((Modules**)((char*)a + 0xd8))[0]->work->slot17(0x1200000a) || getCurry__7ftOwnerFv(self->slot185()) == 1) {
        ((Modules**)((char*)a + 0xd8))[0]->other->slot21(8);
        ((Modules**)((char*)a + 0xd8))[0]->other->slot19(0x48, 8);
    }
    ((Modules**)((char*)a + 0xd8))[0]->ground->slot3(0, 0, 1);
    Actor* obj = g_Stage;
    if (obj) {
        int temp = obj->slot94(self->slot186());
        int constant = soValueAccesser::getConstantInt(a, 0x5a19, 0);
        int value = temp;
        ((Modules**)((char*)a + 0xd8))[0]->other->slot35(0, constant, &value, 0, 0x80);
    }
    ((Modules**)((char*)a + 0xd8))[0]->sound->slot5(0x1f71, 0, 0, 0);
    if (!arg) self->slot185()->slot13(1, 0x1e, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_12D450 {

extern "C" {
void fn_27_1392D4(Fighter* self, float f1, float f2, int a, int b, int c);
void setSlow__7FighterFbiib(Fighter* self, int a, int b, int c, int d);
}

extern const float lbl_27_rodata_1048;
extern const float lbl_27_rodata_1058;

class Call3CBase { public: char pad[0x3C]; };
class Call2CC : public Call3CBase {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
    virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
    virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
    virtual void s36(); virtual void s37(); virtual void s38(); virtual void s39();
    virtual void s40(); virtual void s41(); virtual void s42(); virtual void s43();
    virtual void s44(); virtual void s45(); virtual void s46(); virtual void s47();
    virtual void s48(); virtual void s49(); virtual void s50(); virtual void s51();
    virtual void s52(); virtual void s53(); virtual void s54(); virtual void s55();
    virtual void s56(); virtual void s57(); virtual void s58(); virtual void s59();
    virtual void s60(); virtual void s61(); virtual void s62(); virtual void s63();
    virtual void s64(); virtual void s65(); virtual void s66(); virtual void s67();
    virtual void s68(); virtual void s69(); virtual void s70(); virtual void s71();
    virtual void s72(); virtual void s73(); virtual void s74(); virtual void s75();
    virtual void s76(); virtual void s77(); virtual void s78(); virtual void s79();
    virtual void s80(); virtual void s81(); virtual void s82(); virtual void s83();
    virtual void s84(); virtual void s85(); virtual void s86(); virtual void s87();
    virtual void s88(); virtual void s89(); virtual void s90(); virtual void s91();
    virtual void s92(); virtual void s93(); virtual void s94(); virtual void s95();
    virtual void s96(); virtual void s97(); virtual void s98(); virtual void s99();
    virtual void s100(); virtual void s101(); virtual void s102(); virtual void s103();
    virtual void s104(); virtual void s105(); virtual void s106(); virtual void s107();
    virtual void s108(); virtual void s109(); virtual void s110(); virtual void s111();
    virtual void s112(); virtual void s113(); virtual void s114(); virtual void s115();
    virtual void s116(); virtual void s117(); virtual void s118(); virtual void s119();
    virtual void s120(); virtual void s121(); virtual void s122(); virtual void s123();
    virtual void s124(); virtual void s125(); virtual void s126(); virtual void s127();
    virtual void s128(); virtual void s129(); virtual void s130(); virtual void s131();
    virtual void s132(); virtual void s133(); virtual void s134(); virtual void s135();
    virtual void s136(); virtual void s137(); virtual void s138(); virtual void s139();
    virtual void s140(); virtual void s141(); virtual void s142(); virtual void s143();
    virtual void s144(); virtual void s145(); virtual void s146(); virtual void s147();
    virtual void s148(); virtual void s149(); virtual void s150(); virtual void s151();
    virtual void s152(); virtual void s153(); virtual void s154(); virtual void s155();
    virtual void s156(); virtual void s157(); virtual void s158(); virtual void s159();
    virtual void s160(); virtual void s161(); virtual void s162(); virtual void s163();
    virtual void s164(); virtual void s165(); virtual void s166(); virtual void s167();
    virtual void s168(); virtual void s169(); virtual void s170(); virtual void s171();
    virtual void s172(); virtual void s173(); virtual void s174(); virtual void s175();
    virtual void s176();
    virtual void call(int, float, int);
};

class Call48Base { public: char pad[8]; };
class Call48Offset : public Call48Base {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void call();
};

// Work manage module (+0x64). Slot k is at vptr + 8 + 4*k.
class WorkMgr {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void v5(u32, u32);
    virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void v12(u32);
    virtual void s13(); virtual void s14(); virtual void s15(); virtual void s16();
    virtual u32 v17(u32);
    virtual void s18();
    virtual void v19(u32);
};

class WorkMgr2 {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void s5(); virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void v12(u32, const void*);
};

// Posture (+0xC), param customize (+0xC4), sound (+0x50)
class ModC {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void v5(u32);
    virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void v16();
    virtual void s17(); virtual void s18(); virtual void s19(); virtual void s20(); virtual void s21(); virtual void s22();
    virtual void v23();
};

// Collision hit (+0x20)
class ModD {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void s5(); virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9();
    virtual void s10(); virtual void s11(); virtual void s12(); virtual void s13(); virtual void s14();
    virtual void s15(); virtual void s16(); virtual void s17(); virtual void s18();
    virtual void v19(u32, u32);
};

// Effect (+0x88)
class ModE {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void s5(); virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9();
    virtual void s10(); virtual void s11(); virtual void s12(); virtual void s13(); virtual void s14();
    virtual void s15(); virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23(); virtual void s24();
    virtual void s25(); virtual void s26(); virtual void s27(); virtual void s28(); virtual void s29();
    virtual void s30(); virtual void s31(); virtual void s32(); virtual void s33(); virtual void s34();
    virtual void s35(); virtual void s36(); virtual void s37(); virtual void s38(); virtual void s39();
    virtual void s40(); virtual void s41(); virtual void s42();
    virtual void v43(u32);
    virtual void v44(u32);
};

extern "C" void startFinalCommon__7FighterFv(Fighter* arg0) {
    soModuleAccesser* acc = arg0->m_moduleAccesser;

    if (soValueAccesser::getConstantInt(acc, 0x59EB, 0) == 0) {
        fn_27_1392D4(arg0, lbl_27_rodata_1048, lbl_27_rodata_1048, 0, 0, 0);
    }
    if (soValueAccesser::getConstantInt(acc, 0x59EC, 0) == 0) {
        ((Call2CC*)arg0)->call(0, lbl_27_rodata_1058, -1);
    }
    if (soValueAccesser::getConstantInt(acc, 0x59ED, 0) == 0) {
        arg0->setCurry(false, -1);
    }
    if (soValueAccesser::getConstantInt(acc, 0x59EE, 0) == 0) {
        setSlow__7FighterFbiib(arg0, 0, 1, 0, 0);
    }
    if (soValueAccesser::getConstantInt(acc, 0x59EF, 0) == 0) {
        soModuleAccesser* a = arg0->m_moduleAccesser;
        if (((WorkMgr*)a->m_enumerationStart->m_workManageModule)->v17(0x12000028) == 1) {
            ((ModE*)a->m_enumerationStart->m_effectModule)->v44(8);
            ((ModE*)a->m_enumerationStart->m_effectModule)->v43(9);
            ((WorkMgr*)a->m_enumerationStart->m_workManageModule)->v19(0x12000028);
            ((Call48Offset*)a->m_enumerationStart->m_slowModule)->call();
        }
    }
    if (soValueAccesser::getConstantInt(acc, 0x59F0, 0) == 0) {
        soModuleAccesser* b = arg0->m_moduleAccesser;
        ((WorkMgr*)b->m_enumerationStart->m_workManageModule)->v12(0x1100000B);
        ((ModC*)b->m_enumerationStart->m_postureModule)->v23();
        if (((WorkMgr*)b->m_enumerationStart->m_workManageModule)->v17(0x12000014) == 1) {
            ((WorkMgr*)b->m_enumerationStart->m_workManageModule)->v19(0x12000014);
            ((ModC*)b->m_enumerationStart->m_paramCustomizeModule)->v5(0);
            const soModuleEnumeration* en = b->m_enumerationStart;
            ModC* snd = (ModC*)en->m_soundModule;
            ((WorkMgr2*)en->m_workManageModule)->v12(0x11000011, en);
            snd->v16();
        }
        if (((WorkMgr*)b->m_enumerationStart->m_workManageModule)->v17(0x12000015) == 1) {
            ((WorkMgr*)b->m_enumerationStart->m_workManageModule)->v19(0x12000015);
            ((ModC*)b->m_enumerationStart->m_paramCustomizeModule)->v5(1);
            const soModuleEnumeration* en = b->m_enumerationStart;
            ModC* snd = (ModC*)en->m_soundModule;
            ((WorkMgr2*)en->m_workManageModule)->v12(0x11000011, en);
            snd->v16();
        }
        ((WorkMgr*)b->m_enumerationStart->m_workManageModule)->v5(0, 0x10000023);
    }
    if (soValueAccesser::getConstantInt(acc, 0x59F1, 0) == 0) {
        arg0->setSuperStar(false, -1, false);
    }
    ((ModD*)acc->m_enumerationStart->m_collisionHitModule)->v19(3, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_12D904 {


class Object_vptr_c;

#define MODULE_VTABLE(SLOT3, SLOT19, SLOT21) \
    virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3 SLOT3; \
    virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7(); \
    virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11(); \
    virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15(); \
    virtual void slot16(); virtual unsigned int slot17(unsigned int); virtual void slot18(); \
    virtual void slot19 SLOT19; virtual void slot20(); virtual void slot21 SLOT21; \
    virtual void slot22(); virtual void slot23(); virtual void slot24(int); virtual void slot25(); \
    virtual void slot26(); virtual void slot27(); virtual void slot28(); virtual void slot29(); \
    virtual void slot30(); virtual void slot31(); virtual void slot32(); virtual void slot33(); \
    virtual void slot34(); virtual void slot35(); virtual void slot36(int, int)

class Object_vptr_0 { public: MODULE_VTABLE((int, int, int), (int), (int, int, int)); };
class Object_vptr_effect { public: MODULE_VTABLE((), (int, int), (int)); };

class SlowBase { public: char pad[8]; };
class SlowModule : public SlowBase {
public:
    MODULE_VTABLE((int, int, int), (int), (int, int, int));
};

class Object_vptr_c_base { public: char pad[12]; };
class Object_vptr_c : public Object_vptr_c_base {
public:
    virtual void slot0(); virtual int slot1(); virtual void slot2(); virtual void slot3();
    virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
    virtual void slot16();
};

class FighterBase { public: char pad[0x3c]; };
class FighterView : public FighterBase {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
    virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
    virtual void slot16(); virtual void slot17(); virtual void slot18(); virtual void slot19();
    virtual void slot20(); virtual void slot21(); virtual void slot22(); virtual void slot23();
    virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27();
    virtual void slot28(); virtual void slot29(); virtual void slot30(); virtual void slot31();
    virtual void slot32(); virtual void slot33(); virtual void slot34(); virtual void slot35();
    virtual void slot36(); virtual void slot37(); virtual void slot38(); virtual void slot39();
    virtual void slot40(); virtual void slot41(); virtual void slot42(); virtual void slot43();
    virtual void slot44(); virtual void slot45(); virtual void slot46(); virtual void slot47();
    virtual void slot48(); virtual void slot49(); virtual void slot50(); virtual void slot51();
    virtual void slot52(); virtual void slot53(); virtual void slot54(); virtual void slot55();
    virtual void slot56(); virtual void slot57(); virtual void slot58(); virtual void slot59();
    virtual void slot60(); virtual void slot61(); virtual void slot62(); virtual void slot63();
    virtual void slot64(); virtual void slot65(); virtual void slot66(); virtual void slot67();
    virtual void slot68(); virtual void slot69(); virtual void slot70(); virtual void slot71();
    virtual void slot72(); virtual void slot73(); virtual void slot74(); virtual void slot75();
    virtual void slot76(); virtual void slot77(); virtual void slot78(); virtual void slot79();
    virtual void slot80(); virtual void slot81(); virtual void slot82(); virtual void slot83();
    virtual void slot84(); virtual void slot85(); virtual void slot86(); virtual void slot87();
    virtual void slot88(); virtual void slot89(); virtual void slot90(); virtual void slot91();
    virtual void slot92(); virtual void slot93(); virtual void slot94(); virtual void slot95();
    virtual void slot96(); virtual void slot97(); virtual void slot98(); virtual void slot99();
    virtual void slot100(); virtual void slot101(); virtual void slot102(); virtual void slot103();
    virtual void slot104(); virtual void slot105(); virtual void slot106(); virtual void slot107();
    virtual void slot108(); virtual void slot109(); virtual void slot110(); virtual void slot111();
    virtual void slot112(); virtual void slot113(); virtual void slot114(); virtual void slot115();
    virtual void slot116(); virtual void slot117(); virtual void slot118(); virtual void slot119();
    virtual void slot120(); virtual void slot121(); virtual void slot122(); virtual void slot123();
    virtual void slot124(); virtual void slot125(); virtual void slot126(); virtual void slot127();
    virtual void slot128(); virtual void slot129(); virtual void slot130(); virtual void slot131();
    virtual void slot132(); virtual void slot133(); virtual void slot134(); virtual void slot135();
    virtual void slot136(); virtual void slot137(); virtual void slot138(); virtual void slot139();
    virtual void slot140(); virtual void slot141(); virtual void slot142(); virtual void slot143();
    virtual void slot144(); virtual void slot145(); virtual void slot146(); virtual void slot147();
    virtual void slot148(); virtual void slot149(); virtual void slot150(); virtual void slot151();
    virtual void slot152(); virtual void slot153(); virtual void slot154(); virtual void slot155();
    virtual void slot156(); virtual void slot157(); virtual void slot158(); virtual void slot159();
    virtual void slot160(); virtual void slot161(); virtual void slot162(); virtual void slot163();
    virtual void slot164(); virtual void slot165(); virtual void slot166(); virtual void slot167();
    virtual void slot168(); virtual void slot169(); virtual void slot170(); virtual void slot171();
    virtual void slot172(); virtual void slot173(); virtual void slot174(); virtual void slot175();
    virtual void slot176(); virtual void slot177(int, int, float);
    virtual void slot178(); virtual void slot179(); virtual void slot180(); virtual void slot181();
    virtual void slot182(); virtual void slot183(); virtual void slot184();
    virtual Object_vptr_c* slot185();
    virtual unsigned int slot186();
    virtual void slot187(); virtual void slot188();
};

struct Self : FighterView {
    char pad40[0x20];
    soValueAccesser* accesser;
    char pad64[0xa8];
    int field10c;
};

struct ModuleBlock {
    char pad0[0x50];
    Object_vptr_effect* sound;
    Object_vptr_0* link;
    Object_vptr_0* visibility;
    Object_vptr_0* controller;
    Object_vptr_0* camera;
    Object_vptr_0* work;
    char pad68[0x20];
    Object_vptr_effect* effect;
    char pad8c[0x2c];
    SlowModule* slow;
};

extern "C" {
unsigned int getCurry__7ftOwnerFv(Object_vptr_c*);
unsigned int getMetal__7ftOwnerFv(Object_vptr_c*);
unsigned int getFlower__7ftOwnerFv(Object_vptr_c*);
void notifyOutsideEventEndFinal__23ftOutsideEventPresenterFv(void*);
void notifyOutsideEventFinalSlowCancel__23ftOutsideEventPresenterFv(void*);
void exitFinal__9ftManagerFii(void*, unsigned int, int);
void setCurry__7FighterFbi(void*, bool, int);
void fn_27_1392D4(Self*, int, int, int, float, float);
int getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(
    soModuleAccesser*, unsigned int, unsigned int);
float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
    soModuleAccesser*, unsigned int, unsigned int);
extern void* g_ftManager;
extern const unsigned char lbl_27_rodata_1E18[];
extern const float lbl_27_rodata_1048;
extern const float lbl_27_rodata_1058;
}

extern "C" void endFinal__7FighterFbbb(Self* self, unsigned int arg1, unsigned int arg2, int arg3) {
    soValueAccesser* accesser = self->accesser;
#define MODULES (*(ModuleBlock**)((char*)accesser + 0xd8))

    if (MODULES->work->slot17(0x12000006) == 1) {
        Object_vptr_c* owner = self->slot185();

        if (MODULES->work->slot17(0x12000007) == 1)
            notifyOutsideEventEndFinal__23ftOutsideEventPresenterFv((char*)self + 0x13c);

        MODULES->work->slot19(0x12000008);
        MODULES->work->slot19(0x12000007);
        MODULES->work->slot19(0x12000006);
        MODULES->work->slot19(0x1200001e);
        self->slot157();

        if (arg1 == 1) {
            MODULES->visibility->slot3(0, -1, 1);
            MODULES->effect->slot21(8);

            if (MODULES->work->slot17(0x1200000a) == 1 &&
                arg2 == 1 &&
                getCurry__7ftOwnerFv(owner) == 0) {
                MODULES->effect->slot19(0x53, 8);
            }

            int value = getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(
                (soModuleAccesser*)accesser, 0x5a1a, 0);
            MODULES->effect->slot36(0, value);
            MODULES->camera->slot22();
            notifyOutsideEventFinalSlowCancel__23ftOutsideEventPresenterFv((char*)self + 0x13c);
            MODULES->slow->slot16();
        }

        if (lbl_27_rodata_1E18[self->slot186()] & 4)
            MODULES->sound->slot19(0xdb, 0);

        exitFinal__9ftManagerFii(g_ftManager, (unsigned int)self->field10c, arg3);

        if (arg2 == 1) {
            if (owner->slot1() == 0 && getCurry__7ftOwnerFv(owner) == 1)
                setCurry__7FighterFbi(self, true, -1);

            if (getMetal__7ftOwnerFv(owner) == 1)
                self->slot177(1, -1, lbl_27_rodata_1058);

            if (getFlower__7ftOwnerFv(owner) == 1) {
                float a = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
                    (soModuleAccesser*)accesser, 0xd88, 0);
                float b = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
                    (soModuleAccesser*)accesser, 0xd86, 0);
                float c = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
                    (soModuleAccesser*)accesser, 0xd89, 0);
                float ratio = (c - b) / a;

                fn_27_1392D4(self, 1, 1, 0, lbl_27_rodata_1048, ratio);
            }
        }
    }
#undef MODULES
}

#undef MODULE_VTABLE
}

// TODO: clean up, give variables real names
namespace jj_12F9C8 {
#define SLOT(n) virtual void slot##n() = 0
#define V(n) virtual void v##n() = 0;
#define V10(p) V(p##0) V(p##1) V(p##2) V(p##3) V(p##4) V(p##5) V(p##6) V(p##7) V(p##8) V(p##9)

struct Accesser;

struct Vec3 {
    float x, y, z;
};

struct Vec2 {
    union {
        unsigned long long bits;
        float values[2];
    };

    Vec2(unsigned long long value) {
        union {
            unsigned long long bits;
            unsigned int words[2];
        } temporary;
        temporary.bits = value;
        reinterpret_cast<unsigned int*>(this)[0] = temporary.words[0];
        reinterpret_cast<unsigned int*>(this)[1] = temporary.words[1];
    }
};

struct StatePrefix {
    unsigned int fields[2];
};

class State : public StatePrefix {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    SLOT(18);
    SLOT(1C);
    SLOT(20);
    SLOT(24);
    SLOT(28);
    SLOT(2C);
    virtual unsigned int slot30() = 0;
    SLOT(34);
    SLOT(38);
    virtual float slot3C() = 0;
};

class StateModule {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    SLOT(18);
    SLOT(1C);
    SLOT(20);
    SLOT(24);
    SLOT(28);
    SLOT(2C);
    SLOT(30);
    SLOT(34);
    SLOT(38);
    SLOT(3C);
    SLOT(40);
    SLOT(44);
    SLOT(48);
    SLOT(4C);
    SLOT(50);
    SLOT(54);
    SLOT(58);
    SLOT(5C);
    SLOT(60);
    SLOT(64);
    SLOT(68);
    SLOT(6C);
    SLOT(70);
    SLOT(74);
    virtual State* getState() = 0;
};

class WorkModule {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    virtual int slot18(int) = 0;
    virtual void slot1C(int, int) = 0;
    SLOT(20);
    virtual void slot24(int) = 0;
    SLOT(28);
    SLOT(2C);
    SLOT(30);
    SLOT(34);
    SLOT(38);
    SLOT(3C);
    SLOT(40);
    SLOT(44);
    SLOT(48);
    virtual unsigned int isFlag(int) = 0;
    virtual void onFlag(int) = 0;
    virtual void offFlag(int) = 0;
};

class KineticModule {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    SLOT(18);
    SLOT(1C);
    SLOT(20);
    SLOT(24);
    SLOT(28);
    SLOT(2C);
    SLOT(30);
    SLOT(34);
    SLOT(38);
    virtual unsigned long long getSpeed(short*) = 0;
};

class SituationModule {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    virtual int getKind() = 0;
};

class StatusModule {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    virtual void changeStatusRequest(int, Accesser*) = 0;
};

class PostureModule {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    virtual Vec3 slot18() = 0;
    SLOT(1C);
    SLOT(20);
    SLOT(24);
    SLOT(28);
    SLOT(2C);
    SLOT(30);
    SLOT(34);
    SLOT(38);
    SLOT(3C);
    SLOT(40);
    SLOT(44);
    SLOT(48);
    SLOT(4C);
    SLOT(50);
    SLOT(54);
    SLOT(58);
    SLOT(5C);
    virtual float slot60() = 0;
};

class Unk40Module {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    SLOT(18);
    virtual int slot1C() = 0;
};

struct Unk38Prefix {
    unsigned int fields[2];
};

class Unk38Module : public Unk38Prefix {
public:
    SLOT(08);
    SLOT(0C);
    SLOT(10);
    SLOT(14);
    SLOT(18);
    SLOT(1C);
    SLOT(20);
    SLOT(24);
    SLOT(28);
    SLOT(2C);
    SLOT(30);
    SLOT(34);
    SLOT(38);
    SLOT(3C);
    SLOT(40);
    SLOT(44);
    SLOT(48);
    SLOT(4C);
    SLOT(50);
    SLOT(54);
    SLOT(58);
    SLOT(5C);
    SLOT(60);
    SLOT(64);
    SLOT(68);
    virtual void slot6C(int) = 0;
};

class Unk60Module {
public:
    V10() V10(1) V10(2) V10(3) V(40) V(41) V(42)
    virtual int slot43(int) = 0;
};

struct Modules {
    unsigned char pad00[0x0C];
    PostureModule* unkC;
    unsigned char pad10[4];
    SituationModule* situation;
    unsigned char pad18[0x20];
    Unk38Module* unk38;
    unsigned char pad3C[4];
    Unk40Module* unk40;
    unsigned char pad44[0x1C];
    Unk60Module* unk60;
    WorkModule* work;
    unsigned char pad68[8];
    StatusModule* status;
    unsigned char pad74[8];
    KineticModule* kinetic;
    unsigned char pad80[0x10];
    StateModule* state;
};

struct Accesser {
    unsigned char pad00[0xD8];
    Modules* modules;
};

struct FtPrefix {
    unsigned int fields[15];
};

class FtObject : public FtPrefix {
public:
    V10() V10(1) V10(2) V10(3) V10(4) V10(5) V10(6) V10(7) V10(8) V10(9)
    V10(10) V10(11) V10(12) V10(13) V10(14) V10(15) V(160)
    unsigned char pad[0x20];
    Accesser* acc;
};

struct FtManager {
    unsigned char pad[0x70];
    int unk70;
};

extern "C" {
extern void* ms_instance__6soSlow;
extern const float lbl_27_rodata_1048;
extern FtManager* g_ftManager;
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__6soSlowFv(void*);
unsigned int isAdjust__6soSlowFv(void*);
void processHit__11StageObjectFv(void*);
float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(void*, unsigned long, unsigned long);
}

extern "C" void processHit__7FighterFv(FtObject* object) {
    Accesser* acc = object->acc;

    processHit__11StageObjectFv(object);

    if (ms_instance__6soSlow == 0) {
        void* singleton = __nw__FUlQ25Heaps8HeapType(0x4C, 2);
        if (singleton != 0) {
            singleton = __ct__6soSlowFv(singleton);
        }
        ms_instance__6soSlow = singleton;
    }

    if (isAdjust__6soSlowFv(ms_instance__6soSlow) == 1U) {
        if (acc->modules->work->isFlag(0x12000000) == 1U) {
            acc->modules->work->offFlag(0x12000000);
            object->v160();
        } else {
            short idx = -1;
            Vec2 speed(acc->modules->kinetic->getSpeed(&idx));

            if (speed.values[1] <= -getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(acc, 0xBD0, 0)) {
                acc->modules->work->slot24(0x1000000D);
            } else {
                acc->modules->work->slot1C(0, 0x1000000D);
            }

            if (acc->modules->state->getState()->slot30() == 1U) {
                if (acc->modules->situation->getKind() != 3) {
                    if (acc->modules->work->slot18(0x10000027) <= 0) {
                        if (acc->modules->work->isFlag(0x1200000B) == 0) {
                            if (acc->modules->work->isFlag(0x1200002F) == 0) {
                                if (acc->modules->work->isFlag(0x12000031) == 0) {
                                    if (acc->modules->unk40->slot1C() == 0) {
                                        const Vec3& pos = acc->modules->unkC->slot18();
                                        float f30 = acc->modules->state->getState()->slot3C();
                                        float f31 = acc->modules->unkC->slot60();
                                        float scale = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(acc, 0xBE4, 0);
                                        f31 = scale * f31;
                                        if (acc->modules->situation->getKind() == 0) {
                                            if (f30 >= pos.y + f31) {
                                                acc->modules->status->changeStatusRequest(0xB6, acc);
                                            }
                                        } else if (speed.values[1] < lbl_27_rodata_1048) {
                                            object->acc->modules->status->changeStatusRequest(0xB3, object->acc);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        switch (g_ftManager->unk70) {
        case 0:
            if (object->acc->modules->unk60->slot43(1) == 0) {
                acc->modules->unk38->slot6C(1);
                acc->modules->work->onFlag(0x12000010);
            } else {
                acc->modules->unk38->slot6C(0);
                acc->modules->work->offFlag(0x12000010);
            }
            break;
        case -1: {
            unsigned int flag = 0x12000010;
            if (acc->modules->work->isFlag(flag) == 1U) {
                acc->modules->work->offFlag(flag);
                acc->modules->unk38->slot6C(0);
            }
            break;
        }
        default:
            break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_130808 {


extern "C" unsigned int* getFtStatusData__20ftCommonDataAccesserCF6ftKindi(void*, unsigned int, unsigned int);
extern "C" bool getMetal__7ftOwnerFv(ftOwner*);
extern "C" void fn_27_1392D4(Fighter*, float, float, int, int, int);
extern "C" void setHammer__7FighterFbi(Fighter*, int, int);
extern "C" void setSlow__7FighterFbiib(Fighter*, int, int, int, int);
extern "C" const float lbl_27_rodata_1048;
extern "C" const float lbl_27_rodata_1058;

class EffectSlots {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
 virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
 virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
 virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
 virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
 virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
 virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
 virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
 virtual void s36(); virtual void s37(); virtual void s38(); virtual void s39();
 virtual void s40(); virtual void s41(); virtual void s42();
 virtual void s43(int); virtual void s44(int);
};
class Call48 {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
 virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
 virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
 virtual void call();
};
struct Call48Base { char pad[8]; };
class Call48Offset : public Call48Base {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
 virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
 virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
 virtual void call();
};
class Call64 {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
 virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
 virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
 virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
 virtual void s20(); virtual void s21(); virtual void s22(); virtual void call();
};
class Call1C {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void call(int);
};
struct Call3CBase { char pad[12]; };
class Call3C : public Call3CBase {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
 virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
 virtual void s12(); virtual void call(int,int,int);
};
struct LinkArg { int kind; bool flag; int value; };
class LinkCall {
public:
 virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
 virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
 virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
 virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
 virtual void call(int, LinkArg*, int);
};

struct Bits { unsigned char a:7; unsigned char b:1; };
extern "C" void toDead__7FighterFQ37Fighter4Dead6Reason(Fighter* f, int arg) {
    soModuleAccesser* a = f->m_moduleAccesser;
    int status = a->m_enumerationStart->m_statusModule->getStatusKind();
    unsigned int* data = getFtStatusData__20ftCommonDataAccesserCF6ftKindi(&g_ftCommonDataAccesser, f->getFtKind(), status);
    float damage = a->m_enumerationStart->m_workManageModule->getFloat(0x1100000e);
    int flag = a->m_enumerationStart->m_workManageModule->isFlag(0x22000011);
    f->setMetal(getMetal__7ftOwnerFv(f->getOwner()), lbl_27_rodata_1058, -1);
    fn_27_1392D4(f, lbl_27_rodata_1048, lbl_27_rodata_1048, 0, 0, 0);
    f->setCurry(false, -1);
    setHammer__7FighterFbi(f, 0, 0);
    f->setSuperStar(false, -1, false);
    unsigned char* flags = (unsigned char*)f + 0x138;
    unsigned int bits = *flags;
    if ((bits & 1) == 1U) {
        ((Bits*)flags)->b = 0;
        ((Call64*)f->m_moduleAccesser->m_enumerationStart->m_cameraModule)->s22();
    }
    setSlow__7FighterFbiib(f, 0, 1, 0, 0);
    soModuleAccesser* b = f->m_moduleAccesser;
    if (b->m_enumerationStart->m_workManageModule->isFlag(0x12000028) == 1) {
        ((EffectSlots*)b->m_enumerationStart->m_effectModule)->s44(8);
        ((EffectSlots*)b->m_enumerationStart->m_effectModule)->s43(9);
        b->m_enumerationStart->m_workManageModule->offFlag(0x12000028);
        ((Call48Offset*)b->m_enumerationStart->m_slowModule)->call();
    }
    soModuleAccesser* c = f->m_moduleAccesser;
    c->m_enumerationStart->m_workManageModule->getFloat(0x1100000b);
    ((Call64*)c->m_enumerationStart->m_postureModule)->call();
    if (c->m_enumerationStart->m_workManageModule->isFlag(0x12000014) == 1) {
        c->m_enumerationStart->m_workManageModule->offFlag(0x12000014);
        ((Call1C*)c->m_enumerationStart->m_paramCustomizeModule)->call(0);
        soSoundModule* sound = c->m_enumerationStart->m_soundModule;
        c->m_enumerationStart->m_workManageModule->getFloat(0x11000011);
        ((Call48*)sound)->call();
    }
    if (c->m_enumerationStart->m_workManageModule->isFlag(0x12000015) == 1) {
        c->m_enumerationStart->m_workManageModule->offFlag(0x12000015);
        ((Call1C*)c->m_enumerationStart->m_paramCustomizeModule)->call(1);
        soSoundModule* sound = c->m_enumerationStart->m_soundModule;
        c->m_enumerationStart->m_workManageModule->getFloat(0x11000011);
        ((Call48*)sound)->call();
    }
    c->m_enumerationStart->m_workManageModule->setInt(0, 0x10000023);
    switch (arg) {
    case 0: case 1: case 2: case 3:
        a->m_enumerationStart->m_workManageModule->onFlag(0x12000017);
        break;
    default:
        a->m_enumerationStart->m_workManageModule->offFlag(0x12000017);
    }
    a->m_enumerationStart->m_workManageModule->setInt(arg, 0x10000028);
    a->m_enumerationStart->m_workManageModule->offFlag(0x12000036);
    LinkArg event;
    event.kind = 2;
    event.flag = false;
    event.value = arg;
    ((LinkCall*)a->m_enumerationStart->m_linkModule)->call(-1, &event, 0);
    if (f->m_moduleAccesser->m_enumerationStart->m_statusModule->getStatusKind() == 0xbc)
        ((Call3C*)f->getOwner())->call(1, 0x17, 0);
    a->m_enumerationStart->m_statusModule->changeStatusRequest(0xbd, a);
    g_ftAudienceManager->m_audience->reqHandclapDead(damage, ((*data >> 3) & 1) && flag == 0);
}
}

// TODO: clean up, give variables real names
namespace jj_130D64 {
extern "C" void endFinal__7FighterFbbb(void*, bool, bool, int);
extern "C" void fn_27_C6308(void*, int, int, int, int);
extern "C" void setSUlcide__9ftManagerFi(ftManager*, int);
extern "C" unsigned char lbl_27_rodata_1E18[];
extern "C" int lbl_27_bss_2CBC;
class ControllerCall {
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
virtual void slot57(int);
};
extern "C" void onDeadEnd__7FighterFv(Fighter* self) {
    soModuleAccesser* accesser = self->m_moduleAccesser;
    if ((lbl_27_rodata_1E18[self->getFtKind()] & 1) ||
        accesser->m_enumerationStart->m_workManageModule->isFlag(0x12000008) == 1) {
        endFinal__7FighterFbbb(self, true, false, 0);
    }
    if (!self->getOwner()->isSubOwner()) {
        soDamageAttackerInfo info;
        accesser->m_enumerationStart->m_damageModule->getAttackerInfo(&info);
        if (!*((unsigned char*)g_ftManager + 0x6d) ||
            !self->m_moduleAccesser->m_enumerationStart->m_workManageModule->isFlag(0x12000018)) {
            if (info.m_indirectSoKind == 0 && g_ftManager->isValidEntryId(info.m_indirectEntryId) == 1) {
                g_ftAudienceManager->m_audience->checkCheerDefeat(info.m_indirectEntryId);
                g_ftManager->setBeat(self->m_entryId, info.m_indirectEntryId);
            } else {
                g_ftManager->setSuicide(self->m_entryId);
            }
            int entry;
            ftManager* manager;
            soWorkManageModule* work;
            work = accesser->m_enumerationStart->m_workManageModule;
            entry = self->m_entryId;
            manager = g_ftManager;
            manager->setDead(entry, work->getInt(0x10000028), work->getInt(0x1000002a));
        }
        soWorkManageModule* work = accesser->m_enumerationStart->m_workManageModule;
        int attacker = info.m_indirectEntryId;
        int entry = self->m_entryId;
        fn_27_C6308(&lbl_27_bss_2CBC, entry, attacker, work->getInt(0x10000028), work->getInt(0x10000029));
    }
    ((ControllerCall*)accesser->m_enumerationStart->m_controllerModule)->slot57(1);
}
}

// TODO: clean up, give variables real names
namespace jj_131020 {


extern "C" const float lbl_27_rodata_1048;

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5(int, bool, bool, bool);
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
    virtual void slot18(unsigned int);
    virtual void slot19();
    virtual void slot20();
    virtual void slot21(int, int);
    virtual void slot22();
    virtual void slot23();
    virtual void* slot24();
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
    virtual void slot38(int, float);
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43(int);
};

struct Object_vptr_c_base {
    char pad[0xc];
};

class Object_vptr_c : public Object_vptr_c_base {
public:
    virtual void slot0();
    virtual void slot1();
};

struct SoundBase {
    char pad[8];
};

class SoundCall : public SoundBase {
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
    virtual void slot15(int, int);
};

struct MotionInner {
    char pad[0x18];
    void* ptr;
};

struct MotionData {
    char pad[8];
    int value;
};

extern "C" void toKnockOut__7FighterFv(Fighter* self) {
    soModuleAccesser* a = self->m_moduleAccesser;

    if (!self->getOwner()->isSubOwner()) {
        if (g_ftManager->isProcessHeartSwap(self->m_entryId) == 1) {
            soDamageAttackerInfo info;
            a->m_enumerationStart->m_damageModule->getAttackerInfo(&info);
            g_ftManager->toKnockOutHeartSwapOpposite(self->m_entryId, &info);
            return;
        }

        ((Object_vptr_0*)a->m_enumerationStart->m_effectModule)->slot38(
            0x28, lbl_27_rodata_1048);

        void* data = ((Object_vptr_0*)a->m_enumerationStart->m_soundModule)->slot24();
        ((Object_vptr_0*)a->m_enumerationStart->m_soundModule)->slot5(
            ((MotionData*)((MotionInner**)data)[0]->ptr)->value, true, true, false);

        if (!((*((unsigned char*)g_ftManager + 0x6c) >> 5) & 1)) {
            ((Object_vptr_0*)a->m_enumerationStart->m_effectModule)->slot43(7);
            SoundCall* sound = (SoundCall*)a->m_enumerationStart->m_slowModule;
            sound->slot15(
                soValueAccesser::getConstantInt(a, 0x5a4e, 0),
                soValueAccesser::getConstantInt(a, 0x5a4f, 0));

            soDamageAttackerInfo info;
            a->m_enumerationStart->m_damageModule->getAttackerInfo(&info);

            if (info.m_indirectSoKind == 0 &&
                g_ftManager->isValidEntryId(info.m_indirectEntryId) == 1) {
                g_ftAudienceManager->m_audience->checkCheerDefeat(info.m_indirectEntryId);
                g_ftManager->setBeat(self->m_entryId, info.m_indirectEntryId);
            } else {
                g_ftManager->setSuicide(self->m_entryId);
            }

            g_ftManager->setDead(self->m_entryId, 5, -1);
            self->m_outsideEventPresenter.notifyOutsideEventKnockout();
        }
    }

    self->setCurry(false, -1);
    ((Object_vptr_0*)a->m_enumerationStart->m_collisionHitModule)->slot21(0, 0);
    ((Object_vptr_0*)a->m_enumerationStart->m_workManageModule)->slot18(0x12000018);
    ((Object_vptr_0*)self->m_moduleAccesser->m_enumerationStart->m_controllerModule)->slot43(1);

    int status = a->m_enumerationStart->m_situationModule->getKind() == 0 ? 0x4b : 0x49;
    a->m_enumerationStart->m_statusModule->changeStatusRequest(status, a);
}
}

// TODO: clean up, give variables real names
namespace jj_131CD0 {
typedef unsigned int u32;

struct Vec3 {
    float x, y, z;
};

struct Work {
    virtual void dummy0();
    virtual void dummy1();
    virtual void dummy2();
    virtual void dummy3();
    virtual void dummy4();
    virtual void setInt(int, u32);
    virtual void dummy6();
    virtual void dummy7();
    virtual void dummy8();
    virtual void dummy9();
    virtual void dummy10();
    virtual void dummy11();
    virtual float getFloat(u32);
    virtual void setFloat(float, u32);
    virtual void dummy14();
    virtual void dummy15();
    virtual void dummy16();
    virtual void dummy17();
    virtual void dummy18();
    virtual void offFlag(u32);
};

struct Status {
    virtual void dummy0();
    virtual void dummy1();
    virtual void dummy2();
    virtual void dummy3();
    virtual void dummy4();
    virtual void dummy5();
    virtual void dummy6();
    virtual void dummy7();
    virtual void dummy8();
    virtual void dummy9();
    virtual void dummy10();
    virtual void dummy11();
    virtual void dummy12();
    virtual void dummy13();
    virtual void dummy14();
    virtual void dummy15();
    virtual u32 getStatusKind();
};

struct Posture {
    virtual void dummy0();
    virtual void dummy1();
    virtual void dummy2();
    virtual void dummy3();
    virtual Vec3 getPos();
};

struct Modules {
    char pad0[0xc];
    Posture* posture;
    char pad10[0x54];
    Work* work;
    char pad68[8];
    Status* status;
};

struct Accessor {
    char pad[0xd8];
    Modules* modules;
};

struct Pad {
    char bytes[0x3c];
};

struct Object : Pad {
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4();
    virtual void d5(); virtual void d6(); virtual void d7(); virtual void d8(); virtual void d9();
    virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14();
    virtual void d15(); virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19();
    virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23(); virtual void d24();
    virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29();
    virtual void d30(); virtual void d31(); virtual void d32(); virtual void d33(); virtual void d34();
    virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44();
    virtual void d45(); virtual void d46(); virtual void d47(); virtual void d48(); virtual void d49();
    virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54();
    virtual void d55(); virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59();
    virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63(); virtual void d64();
    virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69();
    virtual void d70(); virtual void d71(); virtual void d72(); virtual void d73(); virtual void d74();
    virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84();
    virtual void d85(); virtual void d86(); virtual void d87(); virtual void d88(); virtual void d89();
    virtual void d90(); virtual void d91(); virtual void d92(); virtual void d93(); virtual void d94();
    virtual void d95(); virtual void d96(); virtual void d97(); virtual void d98(); virtual void d99();
    virtual void d100(); virtual void d101(); virtual void d102(); virtual void d103(); virtual void d104();
    virtual void d105(); virtual void d106(); virtual void d107(); virtual void d108(); virtual void d109();
    virtual void d110(); virtual void d111(); virtual void d112(); virtual void d113(); virtual void d114();
    virtual void d115(); virtual void d116(); virtual void d117(); virtual void d118(); virtual void d119();
    virtual void d120(); virtual void d121(); virtual void d122(); virtual void d123(); virtual void d124();
    virtual void d125(); virtual void d126(); virtual void d127(); virtual void d128(); virtual void d129();
    virtual void d130(); virtual void d131(); virtual void d132(); virtual void d133(); virtual void d134();
    virtual void d135(); virtual void d136(); virtual void d137(); virtual void d138(); virtual void d139();
    virtual void d140(); virtual void d141(); virtual void d142(); virtual void d143(); virtual void d144();
    virtual void d145(); virtual void d146(); virtual void d147(); virtual void d148(); virtual void d149();
    virtual void d150(); virtual void d151(); virtual void d152(); virtual void d153(); virtual void d154();
    virtual void d155(); virtual void d156(); virtual void d157(); virtual void d158(); virtual void d159();
    virtual void d160(); virtual void d161(); virtual void d162(); virtual void d163(); virtual void d164();
    virtual void d165(); virtual void d166(); virtual void d167(); virtual void d168(); virtual void d169();
    virtual void d170(); virtual void d171(); virtual void d172(); virtual void d173(); virtual void d174();
    virtual void d175(); virtual void d176(); virtual void d177(); virtual void d178(); virtual void d179();
    virtual void d180(); virtual void d181(); virtual void d182(); virtual void d183(); virtual void d184();
    virtual void* getState();
    virtual u32 getKind();
    char pad40[0x20];
    Accessor* accessor;
    char pad64[0xa8];
    int id;
};

struct Audience {
    virtual void dummy0();
    virtual void dummy1();
    virtual void dummy2();
    virtual void dummy3();
    virtual void dummy4();
    virtual void dummy5();
    virtual void dummy6();
    virtual void dummy7();
    virtual void notify(int, float, float);
};

struct AudienceManager {
    char pad[8];
    Audience* audience;
};

extern "C" {
extern unsigned char g_ftCommonDataAccesser;
extern AudienceManager* g_ftAudienceManager;
extern const float lbl_27_rodata_1048;
u32* getFtStatusData__20ftCommonDataAccesserCF6ftKindi(void*, u32, u32);
void notifyEventChangeSituation__15ftLogTransactorFP7ftOwnerUl(void*, u32);
}

#define SET_INT(a, v, k) ((a)->modules->work->setInt((v), (k)))
#define SET_FLOAT(a, v, k) ((a)->modules->work->setFloat((v), (k)))
#define OFF_FLAG(a, k) ((a)->modules->work->offFlag((k)))

extern "C" void notifyEventChangeSituation__7FighterF13SituationKind13SituationKindP16soModuleAccesser(Object* self, int kind, u32 st, Accessor* accessor) {
    u32 notify;
    u32 special = 0;

    if (kind == 2) {
        special = 1;
    } else {
        st = accessor->modules->status->getStatusKind();
        u32* data = getFtStatusData__20ftCommonDataAccesserCF6ftKindi(&g_ftCommonDataAccesser, self->getKind(), st);
        void* state = self->getState();

        notify = (u32)kind <= 6 && ((1 << kind) & 0x43) && !((*data >> 3) & 1);

        SET_INT(accessor, 0, 0x10000003);
        SET_INT(accessor, 0, 0x10000004);
        SET_FLOAT(accessor, lbl_27_rodata_1048, 0x11000002);
        SET_INT(accessor, 0, 0x1000000c);
        SET_INT(accessor, 0, 0x1000000b);
        SET_INT(accessor, 0, 0x10000005);
        SET_INT(accessor, 0, 0x10000039);
        SET_INT(accessor, 0, 0x1000003b);
        OFF_FLAG(accessor, 0x1200000d);
        OFF_FLAG(accessor, 0x12000016);
        OFF_FLAG(accessor, 0x12000011);

        if (notify == 1) {
            Accessor* own = self->accessor;
            SET_INT(own, 0, 0x10000038);
            SET_INT(own, 0, 0x1000002c);
            SET_FLOAT(own, lbl_27_rodata_1048, 0x1100000d);
            SET_INT(own, 0, 0x1000003d);

            Posture* posture = accessor->modules->posture;
            int id = self->id;
            Audience* audience = g_ftAudienceManager->audience;
            float value = accessor->modules->work->getFloat(0x1100000e);
            audience->notify(id, posture->getPos().x, value);

            SET_FLOAT(accessor, lbl_27_rodata_1048, 0x1100000e);
            SET_INT(accessor, 0, 0x10000013);
            SET_INT(accessor, 0, 0x10000014);
        }

        if (kind != 1) {
            SET_INT(accessor, 0, 0x1000003e);
        }

        notifyEventChangeSituation__15ftLogTransactorFP7ftOwnerUl(state, notify);
    }

    SET_INT(accessor, special, 0x10000001);
}
}

// TODO: clean up, give variables real names
namespace jj_13231C {
extern const float lbl_27_rodata_1048;

struct Vec2 {
    f32 x;
    f32 y;
};

struct IVec2 {
    s32 x;
    s32 y;
};

union FI {
    f32 f[2];
    s32 i[2];
};

struct FlagBits {
    u8 high : 7;
    u8 low : 1;
};

class Iface {
public:
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
    virtual void f23(s32 a, u32 b, Vec2* c);
    virtual void f24();
};

extern "C" void fn_27_13231C(u8* self, u32 arg1, s32 arg2) {
    if (arg1 == 1) {
        if ((self[0x138] & 1) == 0) {
            IVec2 out;
            FI src;
            src.f[0] = lbl_27_rodata_1048;
            src.f[1] = lbl_27_rodata_1048;
            out.x = src.i[0];
            out.y = src.i[1];
            Iface* c = *(Iface**)(*(u8**)(*(u8**)(self + 0x60) + 0xd8) + 0x60);
            c->f23(arg2, 0, (Vec2*)&out);
            self[0x138] = self[0x138] | 1;
        }
    } else {
        u8 flags = self[0x138];
        if ((u32)(flags & 1) == 1) {
            ((FlagBits*)(self + 0x138))->low = 0;
            Iface* c = *(Iface**)(*(u8**)(*(u8**)(self + 0x60) + 0xd8) + 0x60);
            c->f24();
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1324D4 {
struct Accesser;
struct Event {
 int kind; u8 field4, field5, field6, pad7;
 int field8, fieldC, field10, field14, field18;
 u8 field1C;
};
struct ModuleBase { char pad[8]; };
class StatusModule {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3(int, Accesser*);
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
 virtual int slot16();
};
class WorkModule {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual int slot4(int);
 virtual void slot5(int,int);
 virtual void slot6();
 virtual void slot7();
 virtual void slot8();
 virtual void slot9();
 virtual void slot10();
 virtual void slot11();
 virtual float slot12(int);
 virtual void slot13(float,int);
 virtual void slot14();
 virtual void slot15();
 virtual void slot16();
 virtual int slot17(int);
 virtual void slot18(int);
 virtual void slot19(int);
};
class Module3C {
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
 virtual void slot9(int,int);
 virtual void slot10();
 virtual void slot11();
 virtual void slot12();
 virtual void slot13();
 virtual void slot14(int);
};
class Module40 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual u32 slot6();
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
 virtual void slot19(int);
};
class Module14 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual int slot3();
};
class Module20 {
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
 virtual void slot19(int,int);
};
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4(int,int);
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
 virtual u32 slot42(int);
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
 virtual void slot76(int,int,void*,int,int);
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
 virtual int slot88();
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
 virtual void slot99(int,int,int);
};
class Module58 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual void slot6();
 virtual void slot7(int);
};
class Module88 {
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
 virtual void slot33(int);
 virtual void slot34();
 virtual void slot35();
 virtual void slot36();
 virtual void slot37();
 virtual void slot38();
 virtual void slot39();
 virtual void slot40();
 virtual void slot41();
 virtual void slot42();
 virtual void slot43(int);
 virtual void slot44(int);
};
class Module5C {
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
 virtual void slot48(float,float,float,int,int,int,int);
 virtual void slot49();
 virtual void slot50();
 virtual void slot51();
 virtual void slot52();
 virtual void slot53();
 virtual void slot54();
 virtual void slot55(int,int,int,int);
};
class Module38 : public ModuleBase {
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
 virtual void slot14(void*,void*,int,int,int,int);
 virtual void slot15();
 virtual void slot16();
 virtual void slot17(float,int);
 virtual void slot18();
 virtual void slot19();
 virtual void slot20();
 virtual void slot21();
 virtual void slot22(float,float,int);
};
class Module4 : public ModuleBase {
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
 virtual int slot33(int);
};
class Module8 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual void slot6();
 virtual void slot7(float);
};
class ModuleC {
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
 virtual float slot9();
};
class Module10 : public ModuleBase {
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
 virtual void slot19(int,int);
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
 virtual int slot34(int);
 virtual void slot35();
 virtual void slot36();
 virtual void slot37();
 virtual void slot38();
 virtual Vec3f slot39(int);
};
class Module60 {
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
 virtual void slot13(int,int);
 virtual void slot14();
 virtual void slot15();
 virtual void slot16();
 virtual void slot17();
 virtual void slot18();
 virtual void slot19();
 virtual void slot20();
 virtual void slot21();
 virtual void slot22();
};
class ModuleB8 : public ModuleBase {
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
};

struct Modules {
 int pad0; Module4* m4; Module8* m8; ModuleC* mC;
 Module10* m10; Module14* m14; char pad18[8]; Module20* m20;
 char pad24[0x14]; Module38* m38; Module3C* m3C; Module40* m40;
 char pad44[0x10]; Object_vptr_0* m54; Module58* m58; Module5C* m5C;
 Module60* m60; WorkModule* work; char pad68[8]; StatusModule* status;
 char pad74[0x14]; Module88* m88; char pad8C[0x2C]; ModuleB8* mB8;
};
struct Accesser { char pad[0xD8]; Modules* modules; };
struct CommonData { char pad[0x48]; int** values; };
struct Bits { unsigned char high:7; unsigned char low:1; };
extern "C" {
extern int g_ftStatusUniqProcessTread;
extern char __RTTI__11StageObject[];
extern char __RTTI__6gfTask[];
extern char lbl_27_data_292B4[];
extern const float lbl_27_rodata_1018[];
unsigned int* getFtStatusData__20ftCommonDataAccesserCF6ftKindi(void*,unsigned int,unsigned int);
CommonData* getParamEtc__20ftCommonDataAccesserCF6ftKind(void*,int);
void removeConstraint__24ftStatusUniqProcessTreadFP16soModuleAccesser(void*,Accesser*);
void notifyEventLink__11StageObjectFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(Fighter*,Event*,Accesser*,StageObject*,int);
void linkMessageProcedure__28ftStatusUniqProcessLinkFinalFP18ftLinkFinalMessageP16soModuleAccesser(Event*,Accesser*,StageObject*,int);
void fn_27_18FB48(Event*,Accesser*,StageObject*,int);
void linkMessageProcedure__36ftStatusUniqProcessCaptainFinalOtherFP15soLinkEventArgsP16soModuleAccesser(Event*,Accesser*,StageObject*,int);
void fn_27_1392D4(Fighter*,float,float,int,int,int);
void setHammer__7FighterFbi(Fighter*,int,int);
void setSlow__7FighterFbiib(Fighter*,int,int,int,int);
void* __dynamic_cast(void*,long,const void*,const void*,short);
}
inline Accesser* own(Fighter* f) { return (Accesser*)f->m_moduleAccesser; }
inline int kind(Fighter* f) { return *(int*)((char*)f+0x110); }
inline void init(Fighter* f,Event* e) {
 e->fieldC=0;
 e->field10=(*getParamEtc__20ftCommonDataAccesserCF6ftKind(&g_ftCommonDataAccesser,kind(f))->values)[1];
}
inline float constant(Accesser* a,u32 id) {
 return soValueAccesser::getConstantFloat((soModuleAccesser*)a,id,0);
}
extern "C" void notifyEventLink__7FighterFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(Fighter* self, Event* event, Accesser* accesser, StageObject* sender,int arg) {
 const float* rd=lbl_27_rodata_1018;
 switch(event->kind) {
 case 6: {
  int status=accesser->modules->status->slot16();
  int bits=(*getFtStatusData__20ftCommonDataAccesserCF6ftKindi(&g_ftCommonDataAccesser,self->getFtKind(),status)>>29)&3;
  event->field4=1;
  event->field6=1;
  if(bits==1) event->field5=0;
  else {
   event->field5=1;
   event->field8=accesser->modules->m4->slot33(0x12C);
   switch(accesser->modules->m14->slot3()) {
   case 0: accesser->modules->status->slot3(0x6E,accesser); break;
   case 1: case 2: accesser->modules->status->slot3(0x70,accesser); break;
   }
  }
  break;
 }
 case 7:
  if(accesser->modules->status->slot16()==0x6D)
   removeConstraint__24ftStatusUniqProcessTreadFP16soModuleAccesser(&g_ftStatusUniqProcessTread,accesser);
  break;
 case 8: {
  int status=accesser->modules->status->slot16();
  switch(status) {
  case 0x34: case 0x36: case 0x38:
   event->field8=accesser->modules->m4->slot33(0x12C);
   event->field4=1;
   event->field18=accesser->modules->work->slot4(0x10000033);
   accesser->modules->status->slot3(status==0x36?0x37:0x35,accesser);
   break;
  }
  break;
 }
 case 9:
  accesser->modules->m3C->slot14(0);
  accesser->modules->status->slot3(0x3B,accesser);
  break;
 case 0xA:
  accesser->modules->m40->slot19(0);
  accesser->modules->status->slot3(0x40,accesser);
  break;
 case 0xB: {
  accesser->modules->work->slot5(event->field8,0x20000000);
  accesser->modules->status->slot3(0x42,accesser);
  if((event->field14==5 || event->field14==0x16) && event->field8==0x78) {
   accesser->modules->m60->slot13(0,-1);
   accesser->modules->work->slot18(0x12000029);
   accesser->modules->work->slot19(0x1200001A);
  }
  if(!event->field1C) {
   float rate=constant(accesser,0xCC8);
   float scale=*(float*)((char*)g_ftCommonDataAccesser.getParam((ftKind)kind(self))+0xB0);
   scale*=rate;
   rate=rd[13]/scale;
   if(rate>constant(accesser,0xCC9)) rate=constant(accesser,0xCC9);
   else if(rate<constant(accesser,0xCCA)) rate=constant(accesser,0xCCA);
   *(float*)&event->field18=rate;
   accesser->modules->m8->slot7((float)(double)rate);
  }
  init(self,event);
  break;
 }
 case 0xC: {
  accesser->modules->m40->slot19(0);
  u32 id=accesser->modules->m54->slot42(0);
  gfTask* task=gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)0xA,id);
  accesser->modules->work->slot18(0x22000014);
  void* obj=__dynamic_cast(task,0x3C,__RTTI__11StageObject,__RTTI__6gfTask,1);
  accesser->modules->m38->slot14(obj,&event->fieldC,event->field8,event->field18,1,1);
  break;
 }
 case 0xE:
  accesser->modules->m3C->slot14(0);
  accesser->modules->m3C->slot9(0,0);
  if(!event->field5) accesser->modules->status->slot3(0x3B,accesser);
  break;
 case 0x10:
  accesser->modules->status->slot3(0xCE,accesser);
  init(self,event); break;
 case 0x11: event->field8=accesser->modules->m4->slot33(0x12E); break;
 case 0x12: break;
 case 0x16: accesser->modules->work->slot18(0x22000011); break;
 case 0x17:
  accesser->modules->work->slot5(event->fieldC,0x20000002);
  if(*(u8*)&event->field10) accesser->modules->work->slot18(0x22000010);
  accesser->modules->status->slot3(0xD3,accesser); break;
 case 0x1A: accesser->modules->status->slot3(0xD4,accesser); break;
 case 0x36:
  accesser->modules->m58->slot7(0);
  accesser->modules->m88->slot33(0); break;
 case 0x1B: accesser->modules->status->slot3(0xD9,accesser); break;
 case 0x1C:
  accesser->modules->work->slot5(event->field8,0x20000000);
  accesser->modules->status->slot3(0xDB,accesser);
  init(self,event); break;
 case 0x1D:
  accesser->modules->m58->slot7(0);
  accesser->modules->m20->slot19(2,0);
  accesser->modules->m10->slot19(0,0);
  if(accesser->modules->m54->slot88()==0) {
   Object_vptr_0* module=accesser->modules->m54;
   int value=accesser->modules->m4->slot33(0x12E);
   module->slot76(0,value,lbl_27_data_292B4,1,0);
  }
  accesser->modules->work->slot18(0x1200003B); break;
 case 0x1E:
  accesser->modules->status->slot3(0xEC,accesser);
  init(self,event); break;
 case 0x1F: event->field8=accesser->modules->m4->slot33(0x130); break;
 case 0x837:
  if(accesser->modules->status->slot16()==0xEC && sender->soGetKind()==2 && sender->soGetSubKind()==0x3C)
   accesser->modules->status->slot3(0xE,accesser);
  break;
 case 0x452:
  accesser->modules->status->slot3(0x108,accesser);
  accesser->modules->work->slot18(0x1200003B); break;
 case 0x21:
  switch(event->field8) {
  case 0x1B2: break;
  case 0x1B3: accesser->modules->status->slot3(0xE9,accesser); break;
  case 0x1B4:
   accesser->modules->work->slot5(event->field8,0x20000000);
   accesser->modules->status->slot3(0xEA,accesser); break;
  }
  init(self,event); break;
 case 0x22:
  accesser->modules->work->slot5(event->field8,0x20000000);
  accesser->modules->status->slot3(0xD7,accesser); break;
 case 0x23:
  accesser->modules->status->slot3(0xD8,accesser);
  init(self,event); break;
 case 0x24: case 0x3B: case 0x3C: break;
 case 0x25: linkMessageProcedure__28ftStatusUniqProcessLinkFinalFP18ftLinkFinalMessageP16soModuleAccesser(event,accesser,sender,arg); break;
 case 0x3E:
  if(!accesser->modules->work->slot17(0x12000018))
   self->touchItem((soLinkTouchItemEventArgs*)event);
  break;
 case 0x3F: accesser->modules->m5C->slot55(15,0,0,-1); break;
 case 0x26:
  if(event->field8==-1) accesser->modules->status->slot3(0xE,accesser);
  else {
   accesser->modules->work->slot5(event->field8,0x20000000);
   init(self,event);
  } break;
 case 0x44D: accesser->modules->status->slot3(0xEF,accesser); break;
 case 0x44E:
  accesser->modules->m20->slot19(0,0);
  accesser->modules->work->slot18(0x22000010); break;
 case 0x28:
  if(event->fieldC==0xF7) {
   accesser->modules->m54->slot4(4,event->field10);
   accesser->modules->m54->slot99(4,0,1);
   accesser->modules->status->slot3(event->fieldC,accesser);
  } else if(accesser->modules->status->slot16()==0xF7 || accesser->modules->status->slot16()==0xF8) {
   accesser->modules->work->slot5(event->field8,0x20000001);
   accesser->modules->status->slot3(event->fieldC,accesser);
  } break;
 case 0x2A:
  if(accesser->modules->status->slot16()==0xF9) {
   accesser->modules->m38->slot22(rd[16],rd[16],0);
   accesser->modules->work->slot18(0x22000010);
  } break;
 case 0x2B:
  if(accesser->modules->m14->slot3()==0) accesser->modules->status->slot3(0,accesser);
  else accesser->modules->status->slot3(0xE,accesser);
  break;
 case 0x836:
  if(accesser->modules->m10->slot34(0)) {
   Vec3f pos=accesser->modules->m10->slot39(0);
   *(float*)&event->field8=pos.m_x;
   *(float*)&event->fieldC=pos.m_y;
   *(float*)&event->field10=pos.m_z;
   float x=accesser->modules->mC->slot9();
   event->field4=1;
   *(float*)&event->field8-=x;
  } else if(accesser->modules->status->slot16()==0x80) {
   *(float*)&event->field8=accesser->modules->work->slot12(0x21000005);
   *(float*)&event->fieldC=accesser->modules->work->slot12(0x21000006);
   *(float*)&event->field10=rd[12];
   *(float*)&event->field8-=accesser->modules->mC->slot9();
  } break;
 case 0x44F:
  switch(event->field8) {
  case 8: {
   int index=event->fieldC;
   event->fieldC=(*getParamEtc__20ftCommonDataAccesserCF6ftKind(&g_ftCommonDataAccesser,kind(self))->values)[index]; break;
  }
  case 9: {
   self->setMetal(false,rd[16],-1);
   fn_27_1392D4(self,rd[12],rd[12],0,0,0);
   self->setCurry(false,-1);
   setHammer__7FighterFbi(self,0,0);
   self->setSuperStar(false,-1,false);
   u8* flags=(u8*)self+0x138;
   if((*flags&1)==1U) {
    ((Bits*)flags)->low=0;
    own(self)->modules->m60->slot22();
   }
   setSlow__7FighterFbiib(self,0,1,0,0);
   Accesser* a=own(self);
   if(a->modules->work->slot17(0x12000028)==1U) {
    a->modules->m88->slot44(8);
    a->modules->m88->slot43(9);
    a->modules->work->slot19(0x12000028);
    a->modules->mB8->slot16();
   }
   break;
  }
  default: fn_27_18FB48(event,accesser,sender,arg); break;
  } break;
 case 0x450: linkMessageProcedure__36ftStatusUniqProcessCaptainFinalOtherFP15soLinkEventArgsP16soModuleAccesser(event,accesser,sender,arg); break;
 case 0x2C:
  accesser->modules->m54->slot4(4,event->field8);
  accesser->modules->m54->slot4(5,event->fieldC);
  accesser->modules->status->slot3(0x100,accesser); break;
 case 0x2D: accesser->modules->m38->slot17(*(float*)&event->fieldC,0); break;
 case 0x32:
  if(own(self)->modules->m14->slot3()==0) accesser->modules->status->slot3(0,accesser);
  else accesser->modules->status->slot3(0xE,accesser);
  break;
 case 0x34: {
  BaseItem* item=(BaseItem*)sender;
  float first=item->getParamFloat(0x2487);
  float second=item->getParamFloat(0x2488);
  float third=item->getParamFloat(8);
  accesser->modules->work->slot13(first*third,0x21000004);
  accesser->modules->work->slot13(second,0x21000005);
  accesser->modules->m40->slot19(0);
  accesser->modules->status->slot3(0x103,accesser); break;
 }
 case 0x35:
  accesser->modules->m5C->slot48(*(float*)&event->field8,*(float*)&event->fieldC,*(float*)&event->field10,0,-1,0,0); break;
 case 0x451: accesser->modules->work->slot19(0x12000001); break;
 case 2:
  if((own(self)->modules->status->slot16()==0xD1 || own(self)->modules->status->slot16()==0xD2 || own(self)->modules->status->slot16()==0xD0) && arg==0) {
   u32 id=*(u32*)((char*)sender+0x28);
   if(id==own(self)->modules->m40->slot6()) self->toDead((Fighter::Dead::Reason)event->field8);
  } break;
 case 0x13:
  if(own(self)->modules->status->slot16()==0xD1) accesser->modules->status->slot3(0xD2,accesser);
  break;
 default: break;
 }
 notifyEventLink__11StageObjectFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(self,event,accesser,sender,arg);
}
}

// TODO: clean up, give variables real names
namespace jj_135468 {

#pragma fp_contract off

extern const float lbl_27_rodata_1048;
extern "C" void setAttackRumble__6ftUtilFP16soModuleAccesserf(soModuleAccesser*, float);

inline float addFloats(float a, float b) {
    return a + b;
}

#define U(n) virtual void slot##n();

class Object_vptr_0 {
public:
    U(0) U(1)
    virtual void slot2(void*, int, int, float);
    virtual int slot3();
    U(4)
    virtual void slot5(int, void*, void*, soModuleAccesser*);
    virtual Object_vptr_0* slot6(int);
    U(7) U(8)
    virtual float slot9();
    U(10) U(11) U(12)
    virtual void slot13(int, float);
    U(14) U(15)
    virtual void slot16(int, int);
    virtual int slot17(int);
    U(18) U(19) U(20) U(21) U(22) U(23) U(24) U(25)
    U(26) U(27) U(28) U(29) U(30) U(31) U(32) U(33)
    U(34) U(35) U(36) U(37) U(38) U(39) U(40) U(41)
    U(42) U(43) U(44) U(45) U(46) U(47) U(48) U(49)
    U(50) U(51) U(52) U(53) U(54) U(55) U(56) U(57)
    U(58) U(59)
    virtual void slot60(int, int);

    unsigned char data[2];
};

class ObjectStatus {
public:
    U(0) U(1) U(2)
    virtual void slot3(int, soModuleAccesser*);
};

class ObjectInt {
public:
    U(0) U(1) U(2) U(3) U(4) U(5) U(6) U(7) U(8)
    virtual int slot9();
};

struct Modules {
    char pad0[0xc];
    Object_vptr_0* pc;
    char pad10[4];
    Object_vptr_0* p14;
    char pad18[0x2c];
    Object_vptr_0* p44;
    Object_vptr_0* p48;
    char pad4c[0x10];
    Object_vptr_0* p5c;
    char pad60[4];
    Object_vptr_0* p64;
    char pad68[8];
    Object_vptr_0* p70;
    char pad74[8];
    Object_vptr_0* p7c;
};

struct Accesser {
    char pad[0xd8];
    Modules* modules;
};

struct Self {
    char pad0[0x60];
    Accesser* acc;
    char pad64[0xb0];
    float f114;
    int i118;
    char pad11c[0xc];
    float f128;
    float f12c;
    float f130;
    float f134;
    unsigned char flags;
};

extern "C" int notifyEventCollisionAttackCheck__7FighterFUl(Self* self, int flags) {
    if ((((unsigned int)self->flags >> 7) & 1U) == 1U) {
        Modules* mods = self->acc->modules;
        Object_vptr_0* obj = mods->p48;
        float value = mods->pc->slot9();

        obj->slot2(
            soValueAccesser::getConstantIndefinite(
                (soModuleAccesser*)self->acc, 0xa411, 0),
            1, 1, value);

        Object_vptr_0* other = self->acc->modules->p44;
        other->slot6(((ObjectInt*)obj)->slot9());

        Object_vptr_0* last = self->acc->modules->p5c;
        last->slot60(
            soValueAccesser::getConstantInt(
                (soModuleAccesser*)self->acc, 0x5a02, 0),
            1);
        return 0;
    }

    if (lbl_27_rodata_1048 != (float)self->i118) {
        self->acc->modules->p44->slot16(self->i118, 0);
    }

    if (self->f114 > lbl_27_rodata_1048) {
        setAttackRumble__6ftUtilFP16soModuleAccesserf((soModuleAccesser*)self->acc, self->f114);
    }

    if ((flags & 1) && lbl_27_rodata_1048 != self->f128) {
        int frames = soValueAccesser::getConstantInt(
            (soModuleAccesser*)self->acc, 0x59de, 0);
        soModuleAccesser* acc = (soModuleAccesser*)self->acc;
        float add = soValueAccesser::getConstantFloat(acc, 0x7f7, 0);
        add = addFloats(
            self->f128 * soValueAccesser::getConstantFloat(acc, 0x7f6, 0),
            add);

        self->acc->modules->p64->slot13(0x21000001, add);
        self->acc->modules->p64->slot13(0x21000002, self->f12c);
        self->acc->modules->p64->slot13(0x21000003, (float)frames / add);
        ((ObjectStatus*)self->acc->modules->p70)->slot3(
            0x22, (soModuleAccesser*)self->acc);
        return 1;
    }

    if (self->acc->modules->p14->slot3() == 0 &&
        (flags & 4) &&
        lbl_27_rodata_1048 != self->f130 &&
        self->acc->modules->p64->slot17(0x12000002) == 0) {
        soModuleAccesser* acc;
        Object_vptr_0* obj = self->acc->modules->p7c->slot6(4);
        acc = (soModuleAccesser*)self->acc;

        float add = soValueAccesser::getConstantFloat(acc, 0xcb5, 0);
        float mul = soValueAccesser::getConstantFloat(acc, 0xcb4, 0);
        add = addFloats(self->f130 * mul, add);
        Vec2f speed(add, lbl_27_rodata_1048);
        speed.m_x *= -self->f134;

        soModuleAccesser* callAcc = (soModuleAccesser*)self->acc;
        Vec3f vec(
            lbl_27_rodata_1048,
            lbl_27_rodata_1048,
            lbl_27_rodata_1048);
        obj->slot5(0x15, &speed, &vec, callAcc);
        obj->data[1] |= 0x80;
    }

    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_13A89C {

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4(int);
    virtual void slot5(int, int);
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11(int, int, float*, float*, int, int, int, float, int);
    virtual void slot12();
    virtual void slot13();
    virtual void slot14(int, int);
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18(int);
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
    virtual void slot55(int, int, int, int);
};

class ObjectSound {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4(int);
    virtual void slot5(int, int, int, int);
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11(int, int, float*, float*, int, int, int, float, int);
    virtual void slot12();
    virtual void slot13();
    virtual void slot14(int, int);
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18(int);
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
    virtual void slot55(int, int, int, int);
};

struct Object_vptr_c_base { char pad[0xc]; };
class Object_vptr_c : public Object_vptr_c_base {
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
    virtual void slot13(int, int, int);
};

struct ObjectBase { char pad[0x3c]; };
class Object : public ObjectBase {
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
    virtual int slot40();
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
    virtual void slot150(int, float);
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
    virtual void slot172();
    virtual void slot173();
    virtual void slot174();
    virtual void slot175();
    virtual void slot176();
    virtual void slot177();
    virtual void slot178();
    virtual void slot179(soModuleAccesser*, int);
    virtual void slot180();
    virtual void slot181();
    virtual void slot182();
    virtual void slot183();
    virtual void slot184();
    virtual Object_vptr_c* slot185();
};

extern "C" {
extern const float lbl_27_rodata_1048;
extern const float lbl_27_rodata_104C;
u32 getReflector__7ftOwnerFv(void*);
u32 getRabbitCap__7ftOwnerFv(void*);
void setModelScale__8BaseItemFf(BaseItem*, float);
void setHavedLog__8BaseItemFib(BaseItem*, int, int);
}

#define PTR(p, off) (*(char**)((char*)(p) + (off)))
#define CTX(acc, off) ((Object_vptr_0*)PTR(PTR((acc), 0xD8), (off)))

extern "C" void notifyAttachItem__7FighterFP8BaseItemUlUlbb(void* self, BaseItem* item, int p5, int p6, int p7, int p8) {
    soModuleAccesser* acc = (soModuleAccesser*)PTR(self, 0x60);
    int state = ((Object*)item)->slot40();
    int r30 = -1;
    int r29 = -1;
    u32 r28 = 1;
    if (p8 == 0) {
        float v[6];
        float zero = lbl_27_rodata_1048;
        v[0] = zero;
        v[1] = zero;
        v[2] = zero;
        v[3] = zero;
        v[4] = zero;
        v[5] = zero;
        CTX(acc, 0x88)->slot11(0x35, p5, &v[3], &v[0], 0, 0, 0, lbl_27_rodata_104C, -1);
    }
    switch (state) {
    case 0x2F:
        if (p7 == 0) {
            CTX(acc, 0x64)->slot5(0, 0x10000018);
            CTX(acc, 0x64)->slot18(0x12000009);
            r29 = 0x1F86;
            r28 = 0;
        }
        break;
    case 0x32:
        if (p7 == 0) {
            CTX(acc, 0x64)->slot18(0x1200000F);
            r30 = 0x1FAD;
            r29 = 0x1F66;
        }
        break;
    case 1:
        if (p7 == 0) {
            CTX(acc, 0x28)->slot14(2, 1);
            r30 = 0x1FAD;
            r29 = 0x1F66;
        }
        if (getReflector__7ftOwnerFv(((Object*)self)->slot185()) == 1) {
            ((Object*)item)->slot150(0, lbl_27_rodata_104C);
            r28 = 0;
        }
        CTX(acc, 0x64)->slot18(0x12000035);
        CTX(acc, 0x64)->slot5(0, 0x1000003A);
        ((Object*)self)->slot179(acc, 1);
        break;
    case 0x44:
        if (p7 == 0) {
            CTX(acc, 0xC4)->slot4(2);
            CTX(acc, 0x64)->slot18(0x12000013);
            r30 = 0x1FAD;
            r29 = 0x1FA8;
        }
        if (getRabbitCap__7ftOwnerFv(((Object*)self)->slot185()) == 1) {
            ((Object*)item)->slot150(0, lbl_27_rodata_104C);
            r28 = 0;
        }
        setModelScale__8BaseItemFf(item, soValueAccesser::getConstantFloat(acc, 0xBE9, 0));
        CTX(acc, 0x5C)->slot55(0x11, 0, 0, -1);
        break;
    }
    if (p8 == 0) {
        if (r30 != -1) {
            ((ObjectSound*)CTX(acc, 0x50))->slot5(r30, 0, 0, 0);
        }
        if (r29 != -1) {
            ((ObjectSound*)CTX(acc, 0x50))->slot5(r29, 0, 0, 0);
        }
    }
    if (r28 == 1) {
        int no = g_ftManager->getPlayerNo(*(int*)((char*)self + 0x10C));
        state = no;
        u8 bits = *(u8*)((char*)item + 0x8D4);
        if ((bits & (1 << no)) == 0) {
            if ((item->getParam((itValueAccesser::ParamInt)0x5BCE) & 0x200000) == 0) {
                ((Object*)self)->slot185()->slot13(1, 0x1E, 0);
                setHavedLog__8BaseItemFib(item, state, 1);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_13ACC4 {


extern "C" const float lbl_27_rodata_1018[];

#define V8(p) \
    virtual void p##0(); virtual void p##1(); virtual void p##2(); virtual void p##3(); \
    virtual void p##4(); virtual void p##5(); virtual void p##6(); virtual void p##7();
#define V32(p) V8(p##0) V8(p##1) V8(p##2) V8(p##3)

class Object_vptr_0 {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4();
    virtual int slot5(int, int, int, int);
    virtual void slot6(int, int, int, int);
    virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10();
    virtual void slot11(); virtual void slot12(); virtual void slot13();
    virtual void slot14(int, int, float);
    virtual void slot15(int, int);
    virtual void slot16();
    virtual void slot17(int, float);
    virtual void slot18(); virtual void slot19();
    virtual void slot20(); virtual void slot21(); virtual void slot22();
    virtual void slot23(); virtual void slot24(); virtual void slot25();
    virtual int slot26(BaseItem*);
    virtual void slot27(); virtual void slot28(); virtual void slot29();
    virtual void slot30(); virtual void slot31(); virtual void slot32();
    virtual void slot33(); virtual void slot34(); virtual void slot35();
    virtual void slot36(); virtual void slot37(); virtual void slot38();
    virtual void slot39(); virtual void slot40(); virtual void slot41();
    virtual void slot42(); virtual void slot43(); virtual void slot44();
    virtual void slot45(); virtual void slot46(); virtual void slot47();
    virtual void slot48(); virtual void slot49(); virtual void slot50();
    virtual void slot51(); virtual void slot52(); virtual void slot53();
    virtual void slot54();
    virtual void slot55(int, int, int, int);
};

class WorkModule {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12(); virtual void slot13(); virtual void slot14();
    virtual void slot15(); virtual void slot16();
    virtual unsigned int slot17(unsigned int);
    virtual void slot18(unsigned int);
};

struct Object_vptr_c_base { char pad[0xc]; };
class Object_vptr_c : public Object_vptr_c_base {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12();
    virtual void slot13(int, int, int);
};

struct Object_vptr_8_base { char pad[8]; };
class Object_vptr_8 : public Object_vptr_8_base {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12(); virtual void slot13(); virtual void slot14();
    virtual void slot15(int, int);
    virtual void slot16(); virtual void slot17(); virtual void slot18();
    virtual void slot19(); virtual void slot20(); virtual void slot21();
    virtual void slot22(); virtual void slot23(); virtual void slot24();
    virtual void slot25(); virtual void slot26(); virtual void slot27();
    virtual void slot28(); virtual void slot29(); virtual void slot30();
    virtual void slot31(); virtual void slot32(); virtual void slot33();
    virtual void slot34(); virtual void slot35(); virtual void slot36();
    virtual void slot37(); virtual void slot38(); virtual void slot39();
    virtual void slot40(); virtual void slot41(); virtual void slot42();
    virtual void slot43(); virtual void slot44(); virtual void slot45();
    virtual void slot46(); virtual void slot47(); virtual void slot48();
    virtual void slot49();
    virtual void slot50(float, unsigned int);
};

struct EffectModule {
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void effect(int, int, float*, float*, float*, float*, int, float, int);
    virtual void s9(); virtual void s10();
    virtual void request(int, int, float*, float*, int, int, int, float, int);
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
    virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
    virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
    virtual void s36(); virtual void s37();
    virtual void setFloat(int, float);
    virtual void s39(); virtual void s40(); virtual void s41(); virtual void s42();
    virtual void enable(int);
    virtual void disable(int);
};

struct Modules {
    char pad0[0x38];
    Object_vptr_8* damage;
    char pad3c[0x14];
    Object_vptr_0* sound;
    char pad54[8];
    Object_vptr_0* motion;
    char pad60[4];
    WorkModule* work;
    char pad68[0x20];
    EffectModule* effect;
    char pad8c[0x1c];
    Object_vptr_0* item;
    char padac[0xc];
    Object_vptr_8* other;
};

struct Access {
    char pad[0xd8];
    Modules* modules;
};

struct FighterBase { char pad[0x3c]; };

struct FighterObject : FighterBase {
    V32(a) V32(b) V32(c) V32(d) V32(e)
    V8(f0) V8(f1)
    virtual void f176();
    virtual void f221(int, int, float);
    virtual void f178(); virtual void f179(); virtual void f180();
    virtual void f181(); virtual void f182(); virtual void f183();
    virtual void f184();
    virtual Object_vptr_c* getOwner();

    char pad40[0x20];
    Access* access;
    char pad64[0xa8];
    int entryId;
};

struct ItemObject : FighterBase {
    V32(a) V8(b)
    virtual int getKind();

    char pad40[0x87c];
    int field8bc;
    char pad8c0[4];
    int field8c4;
    char pad8c8[0xc];
    unsigned char playerMask;
    char pad8d5[0x3453];
    int field3d28;
};

extern "C" {
    void notifyOutsideEventGetItem__23ftOutsideEventPresenterF6itKindiii(void*, int, int, int, int);
    float getTabemonoHealAdd__7ftOwnerFv(void*);
    float getMaximTomatoHealAdd__7ftOwnerFv(void*);
    float getHeartHealAdd__7ftOwnerFv(void*);
    void addCollectionItem__7ftOwnerFUcUs(void*, int, unsigned short);
    void setTimerSlow__9ftManagerFibii(ftManager*, int, int, int, int);
    void setSlow__7FighterFbiib(void*, int, int, int, int);
    void setHavedLog__8BaseItemFib(void*, int, bool);
}

extern "C" int notifyUseItem__7FighterFP8BaseItemUlPi(
    FighterObject* fighter, ItemObject* item, int joint, signed char* result)
{
    int sound;
    ftManager* manager;
    int b;
    int a2;
    int entry;
    float ea[3];
    float eb[3];
    float ec[3];
    float ed[3];
    float lx2[3];
    float ly2[3];
    float ly[3];
    float lx[3];
    const float* data = lbl_27_rodata_1018;
    BaseItem* baseItem = (BaseItem*)item;
    Access* access = fighter->access;
    int kind = item->getKind();
    int flags = baseItem->getParam((itValueAccesser::ParamInt)0x5bce);
    int effect = -1;

    notifyOutsideEventGetItem__23ftOutsideEventPresenterF6itKindiii((char*)fighter + 0x13c, kind, item->field8c4,
                 item->field3d28, item->field8bc);

    if (flags & 0x80) {
        float value = baseItem->getParam((itValueAccesser::ParamFloat)0xdad);
        switch (kind) {
        case 0x40:
            value += getTabemonoHealAdd__7ftOwnerFv(fighter->getOwner());
            break;
        case 0x24:
            value += getMaximTomatoHealAdd__7ftOwnerFv(fighter->getOwner());
            break;
        case 0x20:
            value += getHeartHealAdd__7ftOwnerFv(fighter->getOwner());
            break;
        }

        Access* current = fighter->access;
        current->modules->damage->slot50(-value, 0);
        if (-value < data[12]) {
            Access* low = fighter->access;
            low->modules->sound->slot5(0x12, 0, 0, 0);
            low->modules->effect->setFloat(0x1f, data[12]);
            ly[0] = data[12];
            ly[1] = data[12];
            ly[2] = data[12];
            lx[0] = data[12];
            lx[1] = data[12];
            lx[2] = data[12];
            EffectModule* module = low->modules->effect;
            float scale = soValueAccesser::getConstantFloat(
                (soModuleAccesser*)low, 0xbe9, 0);
            module->request(0x36, 0, lx, ly, 0, 0, 0, scale, -1);
        } else {
            current->modules->motion->slot55(0xd, 0, 0, -1);
            sound = current->modules->sound->slot5(0x3b, 0, 0, 0);
            if (sound >= 0) {
                current->modules->sound->slot14(sound, 0, data[15]);
                current->modules->sound->slot17(sound, data[36]);
            }
            ly2[0] = data[12];
            ly2[1] = data[12];
            ly2[2] = data[12];
            lx2[0] = data[12];
            lx2[1] = data[12];
            lx2[2] = data[12];
            current->modules->effect->request(
                0x61, -2, lx2, ly2, 0, 0, 0, data[13], -1);
        }
    }

    if (flags & 0x2000) {
        int sound2 = baseItem->getParam((itValueAccesser::ParamInt)0x5bd2);
        access->modules->sound->slot5(sound2, 0, 0, 0);
    }

    switch (kind) {
    case 0x14: {
        unsigned char variation = (unsigned char)item->field8c4;
        *result = access->modules->item->slot26(baseItem);
        if (g_ftManager->addDragoon(fighter->entryId, variation) == 1) {
            access->modules->item->slot6(0x15, 0, 1, 0);
        }
        effect = 0x35;
        access->modules->sound->slot5(0x1fd0, 0, 0, 0);
        break;
    }
    case 0x27:
        fighter->f221(1, -1, data[16]);
        access->modules->sound->slot5(0x1f8a, 0, 0, 0);
        break;
    case 0x17:
        access->modules->sound->slot5(0xd7, 0, 0, 0);
        addCollectionItem__7ftOwnerFUcUs(fighter->getOwner(), kind, (unsigned short)item->field8c4);
        effect = 0x35;
        break;
    case 0x33:
        access->modules->sound->slot5(0xd6, 0, 0, 0);
        addCollectionItem__7ftOwnerFUcUs(fighter->getOwner(), kind, (unsigned short)item->field8c4);
        effect = 0x35;
        break;
    case 0x0b:
        access->modules->sound->slot5(0xd8, 0, 0, 0);
        addCollectionItem__7ftOwnerFUcUs(fighter->getOwner(), kind, (unsigned short)item->field8c4);
        effect = 0x77;
        access->modules->effect->enable(0x0c);
        break;
    case 0x49:
        access->modules->effect->enable(0x0b);
        break;
    case 0x35:
        switch (baseItem->getParam((itValueAccesser::ParamInt)0x5bd0)) {
        case 0: {
            b = soValueAccesser::getConstantInt(
                (soModuleAccesser*)access, 0x5a7f, 0);
            manager = g_ftManager;
            entry = fighter->entryId;
            int a = soValueAccesser::getConstantInt(
                (soModuleAccesser*)access, 0x5a7e, 0);
            setTimerSlow__9ftManagerFibii(manager, entry, 1, a, b);
            access->modules->sound->slot5(0x1f3e, 0, 0, 0);
            break;
        }
        case 1: {
            int b = soValueAccesser::getConstantInt(
                (soModuleAccesser*)access, 0x5a7f, 0);
            int a = soValueAccesser::getConstantInt(
                (soModuleAccesser*)access, 0x5a7e, 0);
            setSlow__7FighterFbiib(fighter, 1, a, b, 1);
            access->modules->sound->slot5(0x1f3e, 0, 0, 0);
            break;
        }
        case 2: {
            b = soValueAccesser::getConstantInt(
                (soModuleAccesser*)access, 0x5a7f, 0);
            a2 = soValueAccesser::getConstantInt(
                (soModuleAccesser*)access, 0x5a7e, 0);
            Access* current = fighter->access;
            if (current->modules->work->slot17(0x12000028) == 1) {
                current->modules->effect->disable(8);
            }
            current->modules->effect->enable(8);
            current->modules->work->slot18(0x12000028);
            current->modules->other->slot15(a2, b);
            access->modules->sound->slot5(0x1f3f, 0, 0, 0);
            break;
        }
        }
        break;
    case 0x10:
        ((Fighter*)fighter)->setCurry(true, -1);
        break;
    }

    if (effect != -1) {
        ed[0] = data[12];
        ed[1] = data[12];
        ed[2] = data[12];
        ec[0] = data[12];
        ec[1] = data[12];
        ec[2] = data[12];
        eb[0] = data[12];
        eb[1] = data[12];
        eb[2] = data[12];
        ea[0] = data[12];
        ea[1] = data[12];
        ea[2] = data[12];
        access->modules->effect->effect(effect, joint, ea, eb, ec, ed, 0, data[13], 0);
    }

    int player = g_ftManager->getPlayerNo(fighter->entryId);
    if (!(item->playerMask & (1 << player)) && kind != 0x14 && !(flags & 0x200000)) {
        fighter->getOwner()->slot13(1, 0x1e, 0);
        setHavedLog__8BaseItemFib(item, player, true);
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_13BF9C {

extern "C" const float lbl_27_rodata_1018[];

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual int slot5(int, int, int, int);
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual int slot11(int, int, float*, float*, float, int, int, int, int);
    virtual void slot12();
    virtual void slot13();
    virtual void slot14(int, int, float);
    virtual void slot15();
    virtual void slot16();
    virtual void slot17(int, float);
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
    virtual void slot38(int, float);
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
    virtual void slot55(int, int, int, int);
};

class Wrap {
    char pad[8];
public:
    virtual void c0();
    virtual void c1();
    virtual void c2();
    virtual void c3();
    virtual void c4();
    virtual void c5();
    virtual void c6();
    virtual void c7();
    virtual void c8();
    virtual void c9();
    virtual void c10();
    virtual void c11();
    virtual void c12();
    virtual void c13();
    virtual void c14();
    virtual void c15();
    virtual void c16();
    virtual void c17();
    virtual void c18();
    virtual void c19();
    virtual void c20();
    virtual void c21();
    virtual void c22();
    virtual void c23();
    virtual void c24();
    virtual void c25();
    virtual void c26();
    virtual void c27();
    virtual void c28();
    virtual void c29();
    virtual void c30();
    virtual void c31();
    virtual void c32();
    virtual void c33();
    virtual void c34();
    virtual void c35();
    virtual void c36();
    virtual void c37();
    virtual void c38();
    virtual void c39();
    virtual void c40();
    virtual void c41();
    virtual void c42();
    virtual void c43();
    virtual void c44();
    virtual void c45();
    virtual void c46();
    virtual void c47();
    virtual void c48();
    virtual void c49();
    virtual void c50(int);
};

struct Mods {
    char p0[0x38];
    Wrap* c;
    char p1[0x14];
    Object_vptr_0* d;
    char p2[8];
    Object_vptr_0* e;
    char p3[0x28];
    Object_vptr_0* f;
};

struct Ctrl {
    char pad[0xd8];
    Mods* mod;
};

struct Acc {
    char pad[0x60];
    Ctrl* ctrl;
};

extern "C" int fn_27_13BF9C(Acc* arg, u32 kind, float value) {
    const float* rd = lbl_27_rodata_1018;
    float high_b[3];
    float high_a[3];
    float low_a[3];
    float low_b[3];
    Ctrl* ctrl;
    Object_vptr_0* effect;
    Ctrl* initial = arg->ctrl;
    int r;
    initial->mod->c->c50(0);

    if (value < rd[12]) {
        if (kind == 1) {
            ctrl = arg->ctrl;
            ctrl->mod->d->slot5(0x12, 0, 0, 0);
            ctrl->mod->f->slot38(0x1f, rd[12]);

            low_a[0] = rd[12];
            low_a[1] = rd[12];
            low_a[2] = rd[12];
            low_b[0] = rd[12];
            low_b[1] = rd[12];
            low_b[2] = rd[12];

            effect = ctrl->mod->f;
            float scale = soValueAccesser::getConstantFloat(
                (soModuleAccesser*)ctrl, 0xbe9, 0);
            return effect->slot11(0x36, 0, low_b, low_a, scale, 0, 0, 0, -1);
        }
    } else {
        Ctrl* ctrl = initial;
        ctrl->mod->e->slot55(0xd, 0, 0, -1);
        r = ctrl->mod->d->slot5(0x3b, 0, 0, 0);
        if (r >= 0) {
            ctrl->mod->d->slot14(r, 0, rd[15]);
            ctrl->mod->d->slot17(r, rd[36]);
        }

        high_a[0] = rd[12];
        high_a[1] = rd[12];
        high_a[2] = rd[12];
        high_b[0] = rd[12];
        high_b[1] = rd[12];
        high_b[2] = rd[12];
        return ctrl->mod->f->slot11(0x61, -2, high_b, high_a, rd[13], 0, 0, 0, -1);
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_13C1F0 {


extern "C" const float lbl_27_rodata_1018[];

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual int slot5(int, int, int, int);
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11(int, int, float*, float*, float, int, int, int, int);
    virtual void slot12();
    virtual void slot13();
    virtual void slot14(int, int, float);
    virtual void slot15();
    virtual void slot16();
    virtual void slot17(int, float);
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
    virtual void slot38(int, float);
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
    virtual void slot50(int);
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55(int, int, int, int);
};

class Wrap {
    char pad[8];

public:
    virtual void c0();
    virtual void c1();
    virtual void c2();
    virtual void c3();
    virtual void c4();
    virtual void c5();
    virtual void c6();
    virtual void c7();
    virtual void c8();
    virtual void c9();
    virtual void c10();
    virtual void c11();
    virtual void c12();
    virtual void c13();
    virtual void c14();
    virtual void c15();
    virtual void c16();
    virtual void c17();
    virtual void c18();
    virtual void c19();
    virtual void c20();
    virtual void c21();
    virtual void c22();
    virtual void c23();
    virtual void c24();
    virtual void c25();
    virtual void c26();
    virtual void c27();
    virtual void c28();
    virtual void c29();
    virtual void c30();
    virtual void c31();
    virtual void c32();
    virtual void c33();
    virtual void c34();
    virtual void c35();
    virtual void c36();
    virtual void c37();
    virtual void c38();
    virtual void c39();
    virtual void c40();
    virtual void c41();
    virtual void c42();
    virtual void c43();
    virtual void c44();
    virtual void c45();
    virtual void c46();
    virtual void c47();
    virtual void c48();
    virtual void c49();
    virtual void c50(int);
};

struct Mods {
    char p0[0x38];
    Wrap* c;
    char p1[0x14];
    Object_vptr_0* d;
    char p2[8];
    Object_vptr_0* e;
    char p3[0x28];
    Object_vptr_0* f;
};

struct Ctrl {
    char pad[0xd8];
    Mods* mod;
};

struct Acc {
    char pad[0x60];
    Ctrl* ctrl;
};

extern "C" void fn_27_13C1F0(Acc* arg, float value) {
    const float* rd = lbl_27_rodata_1018;
    int r;
    Ctrl* initial = arg->ctrl;
    initial->mod->c->c50(0);

    if (value < rd[12]) {
        Ctrl* ctrl = arg->ctrl;
        ctrl->mod->d->slot5(0x12, 0, 0, 0);
        ctrl->mod->f->slot38(0x1f, rd[12]);

        float b[3];
        float a[3];
        a[0] = rd[12];
        a[1] = rd[12];
        a[2] = rd[12];
        b[0] = rd[12];
        b[1] = rd[12];
        b[2] = rd[12];

        Object_vptr_0* effect = ctrl->mod->f;
        float scale = soValueAccesser::getConstantFloat(
            (soModuleAccesser*)ctrl, 0xbe9, 0);
        effect->slot11(0x36, 0, b, a, scale, 0, 0, 0, -1);
        return;
    }

    {
        Ctrl* ctrl = initial;
        ctrl->mod->e->slot55(0xd, 0, 0, -1);
        r = ctrl->mod->d->slot5(0x3b, 0, 0, 0);
        if (r >= 0) {
            ctrl->mod->d->slot14(r, 0, rd[15]);
            ctrl->mod->d->slot17(r, rd[36]);
        }

        float a[3];
        float b[3];
        a[0] = rd[12];
        a[1] = rd[12];
        a[2] = rd[12];
        b[0] = rd[12];
        b[1] = rd[12];
        b[2] = rd[12];
        ctrl->mod->f->slot11(0x61, -2, b, a, rd[13], 0, 0, 0, -1);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_13CC24 {

struct Vec3 { float x, y, z; };

extern "C" void standby__8BaseItemFb(void*, int);

#define P(n) virtual void p##n();
#define P10(n) P(n##0) P(n##1) P(n##2) P(n##3) P(n##4) P(n##5) P(n##6) P(n##7) P(n##8) P(n##9)
#define P100(n) P10(n##0) P10(n##1) P10(n##2) P10(n##3) P10(n##4) P10(n##5) P10(n##6) P10(n##7) P10(n##8) P10(n##9)

struct Pad8 { u32 pad[2]; };

struct AttrFlag {
    u16 m_mask;
    AttrFlag(u16 bits) : m_mask(bits) {}
};

class M0C {
public:
    P(0) P(1) P(2) P(3)
    virtual Vec3 getPos();
    P(5) P(6) P(7) P(8)
    virtual float getF();
};

class M14 {
public:
    P(0) P(1) P(2)
    virtual u8 getKind();
};

class M7C {
public:
    P10(a) P(b0) P(b1) P(b2)
    virtual Vec2f getSumSpeed(AttrFlag);
};

class M64 {
public:
    P(0) P(1) P(2) P(3)
    virtual int getInt(int);
    virtual void setInt(int, int);
    P(6) P(7) P(8) P(9) P(10) P(11)
    virtual float getFloat(int);
    P(13) P(14) P(15) P(16)
    virtual bool isFlag(int);
};

class M08 {
public:
    P10(a) P(b0) P(b1)
    virtual float getF();
};

class M20 {
public:
    P10(a) P10(b) P10(c) P(d0)
    virtual int get(int);
};

class M5C {
public:
    P10(a) P10(b) P10(c) P10(d) P10(e) P(f0)
    virtual float get(int);
};

class MB8 : public Pad8 {
public:
    P10(a)
    virtual int get30();
    virtual int get34();
    P(c2) P(c3) P(c4) P(c5) P(c6)
    virtual int get4C();
    P(d8)
    virtual int get54();
};

class M60 {
public:
    P10(a) P10(b) P10(c) P10(d) P(e0) P(e1) P(e2) P(e3) P(e4) P(e5) P(e6)
    virtual void getC4(void*, int);
    virtual Vec3 getC8(int);
};

struct Obj {
    char pad[0x8bc];
    int val;
};

struct Item {
    int kind;
    Obj* obj;
    char pad[0xc];
};

class MA8 {
public:
    P10(a) P(b0) P(b1) P(b2) P(b3)
    virtual u32 check(int);
    P(b5) P(b6) P(b7) P(b8) P(b9) P(c0)
    virtual void get5C(Item*, int);
    P10(d) P10(e) P(f0) P(f1) P(f2) P(f3) P(f4)
    virtual int count();
    virtual void getItem(Item*, int);
    P(f9) P(g0)
    virtual void setD4(int, int, int);
    virtual void setD8(int, int, int);
};

class M38 : public Pad8 {
public:
    P10(a) P10(b) P10(c) P(d0) P(d1) P(d2) P(d3)
    virtual void set(void*);
};

struct Modules {
    char pad00[0x8];
    M08* m08;
    M0C* m0C;
    char pad10[0x4];
    M14* m14;
    char pad18[0x8];
    M20* m20;
    char pad24[0x14];
    M38* m38;
    char pad3C[0x20];
    M5C* m5C;
    M60* m60;
    M64* m64;
    char pad68[0x14];
    M7C* m7C;
    char pad80[0x28];
    MA8* mA8;
    char padAC[0xC];
    MB8* mB8;
};

struct Accesser {
    char pad[0xd8];
    Modules* modules;
};

struct SelfBase { char pad[0x3c]; };

class Self : public SelfBase {
public:
    P100(a) P10(b0) P10(b1) P10(b2) P10(b3) P10(b4) P10(b5) P10(b6) P(c0)
    virtual u32 getMask();
    P(c1) P(c2)
    virtual int getFC();

    char pad40[0x20];
    Accesser* acc;
};

struct Out {
    Vec3 pos;
    float f0C;
    u8 b10; char p11[3];
    Vec2f speed1;
    Vec2f speed2;
    char x24[0x28];
    int i4C, i50, i54;
    float f58;
    int p5C;
    int i60;
    float f64;
    int i68, i6C, i70, i74;
    u8 b78; char p79[3];
    int i7C, i80;
    u8 b84; char p85[3];
    int i88;
    float f8C;
    u8 b90; char p91[3];
    int i94, i98;
    u8 b9C; char p9D[3];
    float fA0, fA4;
    u8 bA8; char pA9[3];
    int iAC, iB0;
    u8 bB4; char pB5[3];
    int iB8, iBC;
    u8 bC0; char pC1[3];
    int iC4;
    int count;
    int arr[4];
    char xDC[0x10];
    Vec3 posEC;
    u8 bF8; char pF9[3];
    int iFC;
};

extern "C" void setupChangeSucceedWork__7FighterFPv(Self* self, Out* out) {
    Accesser* acc = self->acc;
    u32 mask = self->getMask();

    out->f0C = acc->modules->m0C->getF();
    const Vec3& pos = acc->modules->m0C->getPos();
    out->pos.x = pos.x;
    out->pos.y = pos.y;
    out->pos.z = pos.z;
    out->b10 = acc->modules->m14->getKind();
    out->speed1 = acc->modules->m7C->getSumSpeed(AttrFlag(1));
    out->speed2 = acc->modules->m7C->getSumSpeed(AttrFlag(2));
    out->i4C = acc->modules->m64->getInt(0x10000001);
    out->i50 = acc->modules->m64->getInt(0x10000003);
    out->i54 = acc->modules->m64->getInt(0x10000005);
    out->f58 = acc->modules->m08->getF();
    out->i60 = acc->modules->m64->getInt(0x1000002D);
    out->f64 = acc->modules->m64->getFloat(0x11000003);
    out->i68 = acc->modules->m64->getInt(0x10000023);
    out->i6C = acc->modules->m64->getInt(0x10000024);
    out->i70 = acc->modules->m64->getInt(0x10000021);
    out->i74 = acc->modules->m20->get(0);
    out->b78 = acc->modules->m64->isFlag(0x1200000E);
    out->i7C = acc->modules->m64->getInt(0x1000001E);
    if (mask & 0x100) {
        out->i80 = acc->modules->m64->getInt(0x1000001F);
        acc->modules->m64->setInt(-1, 0x1000001F);
    }
    out->b84 = acc->modules->m64->isFlag(0x12000012);
    out->i88 = acc->modules->m64->getInt(0x10000025);
    out->f8C = acc->modules->m64->getFloat(0x1100000C);
    out->b90 = acc->modules->m64->isFlag(0x1200000A);
    out->i94 = acc->modules->m64->getInt(0x10000019);
    if (mask & 0x400) {
        out->i98 = acc->modules->m64->getInt(0x1000002E);
        acc->modules->m64->setInt(-1, 0x1000002E);
    }
    out->b9C = acc->modules->m64->isFlag(0x12000009);
    out->fA0 = acc->modules->m5C->get(1);
    out->fA4 = acc->modules->m64->getFloat(0x11000006);
    if (mask & 0x40) {
        out->bA8 = acc->modules->m64->isFlag(0x12000026);
        out->iAC = acc->modules->mB8->get30();
        out->iB0 = acc->modules->mB8->get34();
        out->bC0 = acc->modules->m64->isFlag(0x12000027);
        out->bB4 = acc->modules->m64->isFlag(0x12000028);
        out->iB8 = acc->modules->mB8->get4C();
        out->iBC = acc->modules->mB8->get54();
    }
    acc->modules->m60->getC4(out->xDC, 0);
    const Vec3& pos2 = acc->modules->m60->getC8(0);
    out->posEC.x = pos2.x;
    out->posEC.y = pos2.y;
    out->posEC.z = pos2.z;
    out->bF8 = acc->modules->m64->isFlag(0x12000008);
    if (mask & 0x8) {
        if (acc->modules->mA8->check(0) == 1) {
            Item item;
            acc->modules->mA8->get5C(&item, 0);
            acc->modules->mA8->setD4(0, 0, 1);
            out->iC4 = item.obj->val;
            standby__8BaseItemFb(item.obj, 0);
        }
        for (int i = acc->modules->mA8->count() - 1; i >= 0; i--) {
            Item item;
            acc->modules->mA8->getItem(&item, i);
            if (item.kind != 0x2F) {
                acc->modules->mA8->setD8(i, 0, 1);
                out->arr[out->count] = item.obj->val;
                out->count++;
                standby__8BaseItemFb(item.obj, 0);
            }
        }
    }
    out->iFC = self->getFC();
    acc->modules->m38->set(out->x24);
}
}

// TODO: clean up, give variables real names
namespace jj_13DF5C {

struct Entry { unsigned int id; float value; unsigned int extra; };

namespace Match {
class soNullable {
public:
    virtual void isNull();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3(soModuleAccesser*);
    virtual void slot4();
    virtual void slot5(int);
    virtual int slot6();
    virtual void slot7();
    virtual void slot8(int);
    virtual void slot9(float, float, float);
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13(int);
    virtual void slot14();
    virtual void slot15(int, int);
    virtual void slot16(int, int);
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22(int);
    virtual void slot23();
    virtual void slot24(int, int);
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
};
}
struct Prefix { char pad[0x3c]; };
class Object : public Prefix {
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
    virtual void slot172();
    virtual void slot173();
    virtual void slot174();
    virtual void slot175();
    virtual void slot176();
    virtual void slot177();
    virtual void slot178();
    virtual void slot179();
    virtual void slot180();
    virtual void slot181();
    virtual void process(Entry*);
    virtual void slot183();
    virtual void slot184();
    virtual void* getData();
    char pad40[0x20];
    soModuleAccesser* acc;
};

extern "C" {
int getGlowAttack__7ftOwnerFv(void*);
int getGlowDefense__7ftOwnerFv(void*);
int getSealCount__7ftOwnerFv(void*);
void fn_27_112AC8(void*, int, Entry*);
extern const float lbl_27_rodata_1054;
extern const float lbl_27_rodata_1058;
}

extern "C" void fn_27_13DF5C(Object* self) {
    soModuleAccesser* acc;
    Match::soNullable* module;
    void* data = self->getData();
    module = (Match::soNullable*)self->acc->m_enumerationStart->m_glowModule;
    if (module->slot6() != 0) return;
    int first = getGlowAttack__7ftOwnerFv(data);
    int second = getGlowDefense__7ftOwnerFv(data);
    module->slot17();
    int count = getSealCount__7ftOwnerFv(data);
    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            Entry entry;
            fn_27_112AC8(data, i, &entry);
            switch (entry.id) {
            case 0: first += (int)(entry.value * lbl_27_rodata_1054); break;
            case 1: second += (int)(entry.value * lbl_27_rodata_1054); break;
            case 0x6:
            case 0x33:
                module->slot16(4, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(5, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x7:
            case 0x30:
            case 0x32:
            case 0x34:
            case 0x39:
            case 0x3b:
                module->slot16(6, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(7, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x8:
            case 0x31:
            case 0x36:
                module->slot16(9, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(10, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(11, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(17, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(23, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(20, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(19, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x9:
            case 0x2f:
                module->slot16(1, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0xa:
                module->slot16(2, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(3, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(13, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0xb:
                module->slot16(8, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0xc:
                module->slot16(15, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0xd:
                module->slot16(18, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0xe:
                module->slot16(21, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0xf:
                module->slot16(22, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x10:
                module->slot16(16, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x11:
                module->slot16(14, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x12:
            case 0x38:
                module->slot16(24, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x13:
            case 0x3a:
                module->slot16(4, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(5, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(6, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(7, (int)(entry.value * lbl_27_rodata_1054));
                module->slot16(8, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x42:
                module->slot16(25, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x40:
                module->slot16(12, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x14:
                module->slot15(2, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x15:
                module->slot15(3, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x16:
                module->slot15(5, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x17:
                module->slot15(17, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x43:
                module->slot15(21, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x44:
                module->slot15(4, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x45:
                module->slot15(19, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x18:
            case 0x35:
            case 0x37:
                module->slot24(0, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x19:
                module->slot24(2, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x1a:
                module->slot24(5, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x1b:
                module->slot24(3, (int)(entry.value * lbl_27_rodata_1054));
                break;
            case 0x23:
            case 0x5a:
            case 0x5b:
                self->acc->m_enumerationStart->m_damageModule->setNoReactionMode2nd(entry.value, lbl_27_rodata_1058, 2);
                break;
            case 0x1d:
                module->slot22((int)(entry.value * lbl_27_rodata_1054));
                break;
            default:
                self->process(&entry);
                break;
            }
        }
    }
    module->slot5(first);
    module->slot8(second);
    acc = self->acc;
    float middle;
    float third = soValueAccesser::getConstantFloat(acc, 0xd24, 0);
    middle = soValueAccesser::getConstantFloat(acc, 0xd23, 0);
    float low = soValueAccesser::getConstantFloat(acc, 0xd22, 0);
    module->slot9(low, middle, third);
    module->slot13(soValueAccesser::getConstantInt(self->acc, 0x5a50, 0));
    module->slot3(self->acc);
    module->slot29();
}
}

// TODO: clean up, give variables real names
namespace jj_13F114 {
extern "C" {
extern char lbl_27_data_2A924[];
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dl__FPv(void*);
}

struct EvMgr {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f5(s16 a, s16 b);
    virtual void f6();
    virtual void f7();
    virtual u32 f8(s32 id);
};

struct Obj {
    void* vtbl;
    s16 m4;
    s16 m6;
    s16 m8;
};

extern "C" Obj* fn_27_13F114(Obj* p, s32 flag) {
    if (p != nullptr) {
        s16 id = p->m4;
        p->vtbl = lbl_27_data_2A924;
        if (((EvMgr*)getInstance__13soEventSystemFv())->f8(id) == 1) {
            EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), p->m4);
            mgr->f5(p->m6, p->m8);
        }
        p->m8 = -1;
        p->m4 = -1;
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_13F1D4 {
extern "C" void fn_27_13F1D4(void) {}
}

