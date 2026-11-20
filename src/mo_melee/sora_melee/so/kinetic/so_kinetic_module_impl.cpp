// soKineticModuleImpl (from getSumSpeed3f on; the constructor and the first methods are in so_kinetic_module_impl_87084.cpp).
// TODO: notifyEventAnimCmd (the kinetic anim cmds) is not decompiled yet.
#define SO_KINETIC_MODULE_IMPL_CTOR_2
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_instance_manager.h>
#include <types.h>

// Runs fn on every energy of the manager up to index last (clamped to the manager's size).
// HYPOTHESIS: an inline helper of the instance manager; the member pointer is converted from the energy's base classes.
template <class T>
static inline void forEachEnergyCall(soInstanceManagerFullProperty<T*>*& manager, void (T::*fn)(), int first, int last) {
    if ((int)manager->size() <= last) {
        last = manager->size() - 1;
    }
    for (int i = first; i <= last; i++) {
        T* energy = manager->atIndex(i);
        if (energy != NULL) {
            (energy->*fn)();
        }
    }
}

template <class T>
static inline void forEachEnergy(soInstanceManagerFullProperty<T*>*& manager, void (T::*fn)()) {
    forEachEnergyCall(manager, fn, 0, manager->size() - 1);
}

// TODO: clean up: still unnamed soValueAccesser helper (asks the object's value accesser whether its speed may be
// cleared).
extern "C" bool fn_27_8C964(soModuleAccesser* moduleAccesser);

// TODO: clean up: MATCH-ONLY public view of soKineticModuleImpl for the free helper below; make the helper a member
// once so/kinetic/so_kinetic_module_impl.h can change without upsetting the Pokemon Trainer units (compiler ICE).
struct soKineticModuleImplView {
    u8 _0[0x14];
    soInstanceManagerFullProperty<soKineticEnergy*>* m_kineticEnergyInstanceManager;
    int m_kineticType;
    soModuleAccesser* m_moduleAccesser;
};

// HYPOTHESIS: name; clears the speed of the energies of the attribute (clearSpeedAll passes every attribute).
void soKineticModuleImplClearSpeed(soKineticModuleImpl* module, soKineticEnergy::AttributeFlag flag) {
    soKineticModuleImplView* view = (soKineticModuleImplView*)module;
    if (fn_27_8C964(view->m_moduleAccesser)) {
        soArrayVector<soKineticEnergy**, 24> energies;
        view->m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
        int num = energies.size();
        for (int i = 0; i < num; i++) {
            (*energies.at(i))->clearSpeed();
        }
    }
}

Vec3f soKineticModuleImpl::getSumSpeed3f(soKineticEnergy::AttributeFlag flag) {
    Vec3f sum(0.0f, 0.0f, 0.0f);
    if (flag.m_mask == -1) {
        for (int i = m_kineticEnergyInstanceManager->size() - 1; i >= 0; i--) {
            soKineticEnergy* energy = m_kineticEnergyInstanceManager->atIndexFast(i);
            if (energy->isEnable()) {
                Vec3f speed = energy->getSpeed3f();
                Vec3fAdd(&sum, &sum, &speed);
            }
        }
    } else {
        int num = m_kineticEnergyInstanceManager->size();
        for (int i = 0; i < num; i++) {
            soInstanceUnitFullProperty<soKineticEnergy*>& unit = m_kineticEnergyInstanceManager->atUnitIndexFast(i);
            soAttributeFlag attribute = unit.getAttribute();
            if (attribute.m_mask & flag.m_mask) {
                soKineticEnergy* energy = unit.m_element;
                if (energy->isEnable()) {
                    Vec3f speed = energy->getSpeed3f();
                    Vec3fAdd(&sum, &sum, &speed);
                }
            }
        }
    }
    return sum;
}

Vec3f soKineticModuleImpl::getSumRotation(soKineticEnergy::AttributeFlag flag) {
    Vec3f sum(0.0f, 0.0f, 0.0f);
    if (flag.m_mask == -1) {
        for (int i = m_kineticEnergyInstanceManager->size() - 1; i >= 0; i--) {
            soKineticEnergy* energy = m_kineticEnergyInstanceManager->atIndexFast(i);
            if (energy->isEnable()) {
                Vec3f rotation = energy->getRotation();
                Vec3fAdd(&sum, &sum, &rotation);
            }
        }
    } else {
        int num = m_kineticEnergyInstanceManager->size();
        for (int i = 0; i < num; i++) {
            soInstanceUnitFullProperty<soKineticEnergy*>& unit = m_kineticEnergyInstanceManager->atUnitIndexFast(i);
            soAttributeFlag attribute = unit.getAttribute();
            if (attribute.m_mask & flag.m_mask) {
                soKineticEnergy* energy = unit.m_element;
                if (energy->isEnable()) {
                    Vec3f rotation = energy->getRotation();
                    Vec3fAdd(&sum, &sum, &rotation);
                }
            }
        }
    }
    return sum;
}

void soKineticModuleImpl::clearSpeedAll() {
    soKineticModuleImplClearSpeed(this, soKineticEnergy::AttributeFlag(-1));
}

void soKineticModuleImpl::suspendEnergyAll() {
    forEachEnergy(m_kineticEnergyInstanceManager, static_cast<void (soKineticEnergy::*)()>(&soSuspendable::suspend));
}

void soKineticModuleImpl::resumeEnergyAll() {
    forEachEnergy(m_kineticEnergyInstanceManager, static_cast<void (soKineticEnergy::*)()>(&soSuspendable::resume));
}

void soKineticModuleImpl::unableEnergyAll() {
    forEachEnergy(m_kineticEnergyInstanceManager, static_cast<void (soKineticEnergy::*)()>(&soEnable::disable));
}

void soKineticModuleImpl::setConsiderGroundFriction(bool consider, soKineticEnergy::AttributeFlag flag) {
    soArrayVector<soKineticEnergy**, 24> energies;
    m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
    if (consider == true) {
        int num = energies.size();
        for (int i = 0; i < num; i++) {
            (*energies.at(i))->onConsiderGroundFriction();
        }
    } else {
        int num = energies.size();
        for (int i = 0; i < num; i++) {
            (*energies.at(i))->offConsiderGroundFriction();
        }
    }
}

void soKineticModuleImpl::mulSpeed(Vec3f* speed, soKineticEnergy::AttributeFlag flag) {
    soArrayVector<soKineticEnergy**, 24> energies;
    m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
    int num = energies.size();
    for (int i = 0; i < num; i++) {
        (*energies.at(i))->mulSpeed(speed);
    }
}

void soKineticModuleImpl::mulAccel(Vec3f* accel, soKineticEnergy::AttributeFlag flag) {
    soArrayVector<soKineticEnergy**, 24> energies;
    m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
    int num = energies.size();
    for (int i = 0; i < num; i++) {
        (*energies.at(i))->mulAccel(accel);
    }
}

void soKineticModuleImpl::reflectSpeed(Vec3f* speed, soKineticEnergy::AttributeFlag flag) {
    soArrayVector<soKineticEnergy**, 24> energies;
    m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
    int num = energies.size();
    for (int i = 0; i < num; i++) {
        (*energies.at(i))->reflectSpeed(speed);
    }
}

void soKineticModuleImpl::reflectAccel(Vec3f* accel, soKineticEnergy::AttributeFlag flag) {
    soArrayVector<soKineticEnergy**, 24> energies;
    m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
    int num = energies.size();
    for (int i = 0; i < num; i++) {
        (*energies.at(i))->reflectAccel(accel);
    }
}

bool soKineticModuleImpl::isObserv(char group) {
    return group == 0xE;
}
