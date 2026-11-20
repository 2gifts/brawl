#pragma force_active off
#define WN_MARIO_FIREBALL_UNIT // BYTES: the fill constructor counts with a signed index and initializes the members in its initializer list
#include <so/event/so_event_presenter.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <so/item/so_item_manage_event_presenter.h>
#include <so/motion/so_motion_event_presenter.h>
#include <so/turn/so_turn_event_presenter.h>
#include <so/collision/so_collision_shield_event_presenter_shield.h>
#include <ft/ft_entry.h>
#include <so/so_array.h>
#include <new>

// TODO: clean up - the whole addObserverSub body lives in this macro as a MATCH-ONLY explicit specialization; once the
// observer header can emit it directly, instantiate it with a plain template instead.
// MATCH-ONLY: the explicit specialization keeps the original's check-flag declaration order.
#define OBSERVER_ADD_SUB(T)     template <>     void soEventObserver<T>::addObserverSub(s32 manageId, T* obsvr, s8 p3) {         bool check4 = false;         bool check3 = false;         bool check2 = false;         bool check1 = false;         s32 removeID;         s32 checkID = m_manageID;         if (m_manageID >= 0 && m_unitID >= 0)             check1 = true;         if (check1 && m_sendID > -1)             check2 = true;         if (check2 && soEventSystem::getInstance()->getInstanceManager().isContain(checkID) == true)             check3 = true;         if (check3 && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)             check4 = true;         if (check4 == true) {             removeID = m_manageID;             removeObserver(removeID);         }         if (manageId <= -1 || m_unitID <= -1 || soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)             return;         soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);         if (eventManager->isNullUnit(m_unitID) != true) {             m_sendID = addObserverPrivate(manageId, obsvr, p3);             if (m_sendID > -1 && m_sendID > -1)                 m_manageID = manageId;         }     }

OBSERVER_ADD_SUB(soTurnEventObserver)
OBSERVER_ADD_SUB(ftEntryEventObserver)
OBSERVER_ADD_SUB(soMotionEventObserver)
OBSERVER_ADD_SUB(soGimmickEventObserver)
OBSERVER_ADD_SUB(soItemManageEventObserver)
OBSERVER_ADD_SUB(soCollisionShieldEventObserver)

typedef soEventObserver<soTurnEventObserver> ObsTurn;
typedef soEventObserver<ftEntryEventObserver> ObsEntry;
typedef soEventObserver<soMotionEventObserver> ObsMotion;
typedef soEventObserver<soGimmickEventObserver> ObsGimmick;
typedef soEventObserver<soItemManageEventObserver> ObsItemManage;
typedef soEventObserver<soCollisionShieldEventObserver> ObsCollisionShield;

// MATCH-ONLY: the explicit instantiations are forced by out-of-line functions that use the observers.
#pragma dont_inline on
void vela_Turn_sub(ObsTurn* o, soTurnEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Entry_sub(ObsEntry* o, ftEntryEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Motion_sub(ObsMotion* o, soMotionEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Gimmick_id(ObsGimmick* o) { o->getObserverId(); }
void vela_Gimmick_ct(void* p) { new (p) ObsGimmick(0); }
void vela_Gimmick_init(ObsGimmick* o) { o->initialize(0, 0); }
void vela_Gimmick_sub(ObsGimmick* o, soGimmickEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_ItemManage_sub(ObsItemManage* o, soItemManageEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_CollisionShield_sub(ObsCollisionShield* o, soCollisionShieldEventObserver* t) { o->addObserverSub(0, t, 0); }
#pragma dont_inline reset
