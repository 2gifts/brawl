#pragma once

// The generic kinetic mediator of the articles: soKineticMediatorImpl<energy type list, transactor info> owns one
// instance pool per energy and forwards the mediator interface to the transactors.
//
// BYTES, link order: at the end of a unit MWCC instantiates the deferred members of the class templates template by
// template, the template that was declared LAST first. The original article units emit the mediator destructor ahead
// of the parameter accesser queries and the builder's deactivateDescendantForce, so there this template is declared
// after wnExtendParamAccesser and wnWeaponBuilder. wn_kinetic_mediator_impl.h includes this header at its end; a unit
// that needs the original order defines WN_KINETIC_MEDIATOR_LATE and includes it after those templates.
#include <wn/wn_kinetic_mediator_impl.h>

template <typename Change, typename Update, typename Speed, typename Helper>
struct soKineticTransactorInfo {
    typedef Change ChangeType;
    typedef Update UpdateType;
    typedef Speed SpeedType;
    typedef Helper HelperType;
};

template <typename List, typename Transactors>
class soKineticMediatorImpl : public soKineticMediator {
    typedef soLineInvertHierarchy<List, soInstancePool, soInstancePoolRoot> Pools;
    Pools m_pools;
public:
#ifdef WN_KINETIC_MEDIATOR_OUT_OF_CLASS
    // BYTES: link order (see the definitions below the class).
    soKineticMediatorImpl(soModuleAccesser* acc);
    ~soKineticMediatorImpl();
#else
    soKineticMediatorImpl(soModuleAccesser* acc) : m_pools(acc) { }
    ~soKineticMediatorImpl() { }
#endif
    virtual void changeKinetic(int mode, soModuleAccesser* acc) {
        Transactors::ChangeType::changeKinetic(mode, &m_pools, acc);
        m_pools.clearSpeed();
    }
    virtual void updateEnergy(soModuleAccesser* acc) {
#ifdef WN_KINETIC_UPDATE_TRANSACTOR
        m_pools.template updateEnergy<WN_KINETIC_UPDATE_TRANSACTOR>(acc);
#else
        m_pools.template updateEnergy<typename Transactors::UpdateType>(acc);
#endif
    }
    virtual void updateEnergy1(soModuleAccesser* acc, soKineticAttributeMask flag) {
#ifdef WN_KINETIC_UPDATE_TRANSACTOR
        soKineticUpdateEnergyHolderHelper<WN_KINETIC_UPDATE_TRANSACTOR> helper(flag);
#else
        soKineticUpdateEnergyHolderHelper<typename Transactors::UpdateType> helper(flag);
#endif
        m_pools.updateEnergy1(helper, acc);
    }
    virtual void updateEnergy2(soArray<soKineticEnergy**>* energies, soModuleAccesser* acc) {
        for (int i = 0; i < energies->size(); i++) {
            soKineticEnergy* energy = **&energies->at(i);
            if (energy->isEnable() != false && energy->isSuspend() != true) {
                if (energy->isEnable() == true && energy->isSuspend() == false) {
                    if (energy->isEnable() == true && energy->isSuspend() == false)
                        energy->updateEnergy(acc);
                }
            }
        }
    }
    virtual void postUpdateEnergy() { }
    virtual void addSpeed(void* speed, soModuleAccesser* acc) {
        Transactors::SpeedType::addSpeed(speed, &m_pools, acc);
    }
    virtual void addSpeedOutside(int type, void* speed, soModuleAccesser* acc) {
        Transactors::SpeedType::addSpeedOutside(type, speed, &m_pools, acc);
    }
    virtual void notifyEventChangeStatus(void*, void*, void*, void*) { }
    virtual int getMediateNum() { return soTypeListLength<List>::value; }
};

#ifdef WN_KINETIC_MEDIATOR_OUT_OF_CLASS
// BYTES: link order. A member function of a class template that is defined outside the class (not inline) is
// instantiated only at the end of the unit, after the weak virtual functions of the non-template classes. The pool
// vtables and every function the constructor references (the energy constructor, the holder / pool destructors, in
// construction order) are then emitted there, as in the original article units with their own mediator.
template <typename List, typename Transactors>
soKineticMediatorImpl<List, Transactors>::soKineticMediatorImpl(soModuleAccesser* acc) : m_pools(acc) { }
template <typename List, typename Transactors>
soKineticMediatorImpl<List, Transactors>::~soKineticMediatorImpl() { }
#endif
