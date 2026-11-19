#pragma force_active off
#define WN_MARIO_FIREBALL_UNIT // BYTES: the fill constructor counts with a signed index and initializes the members in its initializer list
#include <so/event/so_event_presenter.h>
#include <so/link/so_link_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/capture/so_capture_event_presenter.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/collision/so_collision_hit_event_presenter.h>
#include <so/collision/so_collision_attack_event_presenter.h>
#include <so/collision/so_collision_search_event_presenter.h>
#include <so/collision/so_collision_shield_event_presenter_absorber.h>
#include <so/collision/so_collision_shield_event_presenter_reflector.h>
#include <so/so_array.h>
#include <new>

// MATCH-ONLY: the explicit specialization keeps the original's check-flag declaration order.
#define OBSERVER_ADD_SUB(T)     template <>     void soEventObserver<T>::addObserverSub(s32 manageId, T* obsvr, s8 p3) {         bool check4 = false;         bool check3 = false;         bool check2 = false;         bool check1 = false;         s32 removeID;         s32 checkID = m_manageID;         if (m_manageID >= 0 && m_unitID >= 0)             check1 = true;         if (check1 && m_sendID > -1)             check2 = true;         if (check2 && soEventSystem::getInstance()->getInstanceManager().isContain(checkID) == true)             check3 = true;         if (check3 && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)             check4 = true;         if (check4 == true) {             removeID = m_manageID;             removeObserver(removeID);         }         if (manageId <= -1 || m_unitID <= -1 || soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)             return;         soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);         if (eventManager->isNullUnit(m_unitID) != true) {             m_sendID = addObserverPrivate(manageId, obsvr, p3);             if (m_sendID > -1 && m_sendID > -1)                 m_manageID = manageId;         }     }

OBSERVER_ADD_SUB(soLinkEventObserver)
OBSERVER_ADD_SUB(soDamageEventObserver)
OBSERVER_ADD_SUB(soStatusEventObserver)
OBSERVER_ADD_SUB(soAnimCmdEventObserver)
OBSERVER_ADD_SUB(soCaptureEventObserver)
OBSERVER_ADD_SUB(soSituationEventObserver)
OBSERVER_ADD_SUB(soCollisionHitEventObserver)
OBSERVER_ADD_SUB(soCollisionAttackEventObserver)
OBSERVER_ADD_SUB(soCollisionSearchEventObserver)
OBSERVER_ADD_SUB(soCollisionAbsorberEventObserver)
OBSERVER_ADD_SUB(soCollisionReflectorEventObserver)

typedef u32 VelaElm;
typedef soArrayVector<VelaElm, 1> VelaVec;

#pragma dont_inline on
void vela_vec(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
typedef soEventObserver<soLinkEventObserver> ObsLink;
typedef soEventObserver<soDamageEventObserver> ObsDamage;
typedef soEventObserver<soStatusEventObserver> ObsStatus;
typedef soEventObserver<soAnimCmdEventObserver> ObsAnimCmd;
typedef soEventObserver<soCaptureEventObserver> ObsCapture;
typedef soEventObserver<soSituationEventObserver> ObsSituation;
typedef soEventObserver<soCollisionHitEventObserver> ObsCollisionHit;
typedef soEventObserver<soCollisionAttackEventObserver> ObsCollisionAttack;
typedef soEventObserver<soCollisionSearchEventObserver> ObsCollisionSearch;
typedef soEventObserver<soCollisionAbsorberEventObserver> ObsCollisionAbsorber;
typedef soEventObserver<soCollisionReflectorEventObserver> ObsCollisionReflector;

void vela_Link_sub(ObsLink* o, soLinkEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Damage_ct(void* p) { new (p) ObsDamage(0); }
void vela_Damage_init(ObsDamage* o) { o->initialize(0, 0); }
void vela_Damage_dt(ObsDamage* o) { delete o; }
void vela_Damage_sub(ObsDamage* o, soDamageEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Status_ct(void* p) { new (p) ObsStatus(0); }
void vela_Status_init(ObsStatus* o) { o->initialize(0, 0); }
void vela_Status_sub(ObsStatus* o, soStatusEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_AnimCmd_ct(void* p) { new (p) ObsAnimCmd(0); }
void vela_AnimCmd_init(ObsAnimCmd* o) { o->initialize(0, 0); }
void vela_AnimCmd_sub(ObsAnimCmd* o, soAnimCmdEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Capture_ct(void* p) { new (p) ObsCapture(0); }
void vela_Capture_init(ObsCapture* o) { o->initialize(0, 0); }
void vela_Capture_dt(ObsCapture* o) { delete o; }
void vela_Capture_sub(ObsCapture* o, soCaptureEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_Situation_ct(void* p) { new (p) ObsSituation(0); }
void vela_Situation_init(ObsSituation* o) { o->initialize(0, 0); }
void vela_Situation_sub(ObsSituation* o, soSituationEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_CollisionHit_sub(ObsCollisionHit* o, soCollisionHitEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_CollisionAttack_ct(void* p) { new (p) ObsCollisionAttack(0); }
void vela_CollisionAttack_init(ObsCollisionAttack* o) { o->initialize(0, 0); }
void vela_CollisionAttack_sub(ObsCollisionAttack* o, soCollisionAttackEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_CollisionSearch_ct(void* p) { new (p) ObsCollisionSearch(0); }
void vela_CollisionSearch_init(ObsCollisionSearch* o) { o->initialize(0, 0); }
void vela_CollisionSearch_sub(ObsCollisionSearch* o, soCollisionSearchEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_CollisionAbsorber_ct(void* p) { new (p) ObsCollisionAbsorber(0); }
void vela_CollisionAbsorber_init(ObsCollisionAbsorber* o) { o->initialize(0, 0); }
void vela_CollisionAbsorber_dt(ObsCollisionAbsorber* o) { delete o; }
void vela_CollisionAbsorber_sub(ObsCollisionAbsorber* o, soCollisionAbsorberEventObserver* t) { o->addObserverSub(0, t, 0); }
void vela_CollisionReflector_ct(void* p) { new (p) ObsCollisionReflector(0); }
void vela_CollisionReflector_init(ObsCollisionReflector* o) { o->initialize(0, 0); }
void vela_CollisionReflector_dt(ObsCollisionReflector* o) { delete o; }
void vela_CollisionReflector_sub(ObsCollisionReflector* o, soCollisionReflectorEventObserver* t) { o->addObserverSub(0, t, 0); }
#pragma dont_inline reset

// TODO: clean up, give variables real names
namespace jj_D3B50 {
extern "C" char lbl_27_data_21018[];
extern "C" void* getInstance__13soEventSystemFv();
extern "C" void* getManager__13soEventSystemFl(void*, long);
extern "C" void __dl__FPv(void* p);

struct EvSys {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual u32 v8(s16 id);
};

struct EvMgr {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v5(s16 a, s16 b);
};

struct Obj {
    void* vtbl;
    s16 m4;
    s16 m6;
    s16 m8;
};

extern "C" Obj* fn_27_D3B50(Obj* p, s32 flags) {
    if (p != nullptr) {
        s16 id = p->m4;
        p->vtbl = lbl_27_data_21018;
        if (((EvSys*)getInstance__13soEventSystemFv())->v8(id) == 1) {
            EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), p->m4);
            mgr->v5(p->m6, p->m8);
        }
        p->m8 = -1;
        p->m4 = -1;
        if (flags > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

