#pragma once

// Article-specific parts of the kinetic hierarchy: the pool members, the non-polymorphic root and the generic mediator.
// The template shapes (soInstancePoolInfo, soLineInvertHierarchy) are shared with the fighters (ft_builder_kinetic.h).
#ifndef WN_KINETIC_GENERIC_BUILDER
#error wn_kinetic_mediator_impl.h requires the article generic kinetic policy
#endif
#include <ft/builder/ft_builder_kinetic.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>

// BYTES: the Diddy barrel builds its normal energies with the out-of-line five-vector constructor; units that define
// WN_KINETIC_NORMAL_HOLDER_DEFAULT expand the constructor inline (tagged overload) instead.
template <typename Next, typename InitInfo>
class soKineticEnergyHolder<soKineticEnergyNormal, Next, InitInfo> {
public:
    enum { Attribute = InitInfo::EnergyAttribute };
    virtual ~soKineticEnergyHolder() { }
private:
    soKineticEnergyNormal m_energy;
public:
#ifdef WN_KINETIC_NORMAL_HOLDER_DEFAULT
    soKineticEnergyHolder(soModuleAccesser* acc) :
        m_energy(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                 Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false, 0) {
#else
    soKineticEnergyHolder(soModuleAccesser* acc) :
        m_energy(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                 Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false) {
#endif
        acc->getKineticModule().addEnergy(&m_energy, InitInfo::EnergyIndex,
            soKineticEnergy::AttributeFlag(InitInfo::EnergyAttribute), -1);
        m_energy.disable();
    }
    soKineticEnergyNormal* getEnergy() { return &m_energy; }
};

// With WN_KINETIC_GENERIC_IN_FIGHTER_TU the fighter pool templates (ft_builder_kinetic.h) carry these members instead.
#ifndef WN_KINETIC_GENERIC_IN_FIGHTER_TU
template <typename Info>
class soInstancePoolSubNull {
public:
    template <typename Helper>
    int forEachHolderModuleAccesser(Helper, soModuleAccesser*) { return 0; }
};

template <typename Info, typename Next>
class soInstancePoolSub {
    typedef typename Info::template HolderOf<Next>::Type Holder;
public:
    virtual ~soInstancePoolSub() { }
private:
    soInstancePoolSubNull<Info> m_next;
    Holder m_holder;
public:
    soInstancePoolSub(soModuleAccesser* acc) : m_holder(acc) { }
    // HYPOTHESIS: direct access to the sole energy in this pool.
    typename Info::Type* getEnergy() { return m_holder.getEnergy(); }
#ifdef WN_KINETIC_POOL_SUB_LOOPS
    // WN_KINETIC_POOL_SUB_LOOPS (opt-in): the update loop over the energies of one pool is a member of the sub-pool.
    // BYTES, link order: the transactor's updateEnergy<Energy> is then instantiated with this sub-pool's functions
    // (updateEnergy<E>, getInstanceAt, forEachHolderModuleAccesser per energy, as in the original Ikarus unit) instead
    // of all four ahead of them.
    template <typename T> void updateEnergy(soModuleAccesser* acc) {
        for (int i = 0; i < Info::Count; i++) {
            typename Info::Type* energy = getInstanceAt(i);
            if (energy->isEnable() == true && !energy->isSuspend())
                T::updateEnergy(energy, acc);
        }
    }
#endif
    // MATCH-ONLY: native mediator loops call this per-energy lookup separately.
    typename Info::Type* getInstanceAt(int index) __attribute__((never_inline)) {
        if (index == 0) return m_holder.getEnergy();
        return 0;
    }
    template <typename Helper>
    int forEachHolderModuleAccesser(Helper helper, soModuleAccesser* acc)
        __attribute__((never_inline)) {
        typename Info::Type* energy = m_holder.getEnergy();
        if ((helper.m_flag.m_mask & Holder::Attribute) &&
            energy->isEnable() == true && !energy->isSuspend())
            Helper::TransactorType::updateEnergy(energy, acc);
        return m_next.forEachHolderModuleAccesser(helper, acc);
    }
};

// The empty root has no vptr. The first pool owns its vptr before its subpool.
class soInstancePoolRoot {
public:
    soInstancePoolRoot(soModuleAccesser*) { }
    void clearSpeed() { }
    template <typename T> void updateEnergy(soModuleAccesser*) { }
    template <typename Helper> void updateEnergy1(const Helper&, soModuleAccesser*) { }
};

template <typename Info, typename Parent, typename Next, int Index>
class soInstancePool : public Parent {
public:
    virtual ~soInstancePool() { }
private:
    soInstancePoolSub<Info, Next> m_sub;
public:
    soInstancePool(soModuleAccesser* acc) : Parent(acc), m_sub(acc) { }
    // HYPOTHESIS: the tag selects the energy at this hierarchy level.
    typename Info::Type* getEnergy(const soIntToType<Index>&) { return m_sub.getEnergy(); }
    soInstancePoolSub<Info, Next>& getSub() { return m_sub; }
    void clearSpeed() {
        for (int i = 0; i < Info::Count; i++)
            soKineticTransactHelper::checkClearSpeed(m_sub.getInstanceAt(i));
        Parent::clearSpeed();
    }
    template <typename T> void updateEnergy(soModuleAccesser* acc) {
#ifdef WN_KINETIC_POOL_SUB_LOOPS
        m_sub.template updateEnergy<T>(acc);
#else
        for (int i = 0; i < Info::Count; i++) {
            typename Info::Type* energy = m_sub.getInstanceAt(i);
            if (energy->isEnable() == true && !energy->isSuspend())
                T::updateEnergy(energy, acc);
        }
#endif
        Parent::template updateEnergy<T>(acc);
    }
    template <typename Helper> void updateEnergy1(const Helper& helper, soModuleAccesser* acc) {
        m_sub.forEachHolderModuleAccesser(helper, acc);
        Parent::updateEnergy1(helper, acc);
    }
};
#endif

// WN_KINETIC_MEDIATOR_LATE (opt-in): the unit includes wn_kinetic_mediator.h itself, after the other class templates of its
// article (BYTES, link order: see that header).
#ifndef WN_KINETIC_MEDIATOR_LATE
#include <wn/wn_kinetic_mediator.h>
#endif
