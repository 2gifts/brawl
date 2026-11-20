// TODO: clean up: these are the first functions of the soKineticModuleImpl translation unit (so_kinetic_module_impl.cpp,
// 0x87558-0x88C84, which has no source yet); they sit in their own range because the Matching
// soInstanceUnitFullProperty<soKineticEnergy*>::getAttribute chunk unit (0x8754C) splits the original unit.
#define SO_KINETIC_MODULE_IMPL_CTOR_2
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_instance_manager.h>
#include <types.h>

soKineticModuleImpl::soKineticModuleImpl(soModuleAccesser* acc, soInstanceManagerFullProperty<soKineticEnergy*>* energies) :
    soAnimCmdEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), (s8)-1),
    m_kineticEnergyInstanceManager(energies), m_kineticType(-1), m_moduleAccesser(acc) { }

void soKineticModuleImpl::activate() {
    clearSpeedAll();
}

void soKineticModuleImpl::deactivate() {
    unableEnergyAll();
}

void soKineticModuleImpl::addEnergy(soKineticEnergy* kineticEnergy, int id, soKineticEnergy::AttributeFlag attribute, short priority) {
    m_kineticEnergyInstanceManager->add(kineticEnergy, id, soAttributeFlag(attribute.m_mask), priority);
}

void soKineticModuleImpl::changeKinetic(int kineticType, soModuleAccesser* moduleAccesser) {
    m_kineticType = kineticType;
}

int soKineticModuleImpl::getKineticType() {
    return m_kineticType;
}

void soKineticModuleImpl::eraseEnergy(int index) {
    m_kineticEnergyInstanceManager->erase(index);
}

void soKineticModuleImpl::eraseEnergyAll() {
    m_kineticEnergyInstanceManager->clear();
}

soKineticEnergy* soKineticModuleImpl::getEnergy(int index) {
    return m_kineticEnergyInstanceManager->at(index);
}

bool soKineticModuleImpl::isContain(int index) {
    return m_kineticEnergyInstanceManager->isContain(index);
}

void soKineticModuleImpl::updateEnergy(soModuleAccesser* moduleAccesser, soKineticEnergy::AttributeFlag flag) {
    if (flag.m_mask == -1) {
        for (int i = m_kineticEnergyInstanceManager->size() - 1; i >= 0; i--) {
            soKineticEnergy* energy = m_kineticEnergyInstanceManager->atIndex(i);
            if (energy->isEnable() && energy->isSuspend() != true) {
                energy->updateEnergy(moduleAccesser);
            }
        }
    } else {
        soArrayVector<soKineticEnergy**, 24> energies;
        m_kineticEnergyInstanceManager->getAttributeArray(soAttributeFlag(flag.m_mask), energies);
        int num = energies.size();
        for (int i = 0; i < num; i++) {
            soKineticEnergy* energy = *energies.at(i);
            if (energy->isEnable() && energy->isSuspend() != true) {
                energy->updateEnergy(moduleAccesser);
            }
        }
    }
}

Vec2f soKineticModuleImpl::getSumSpeed(soKineticEnergy::AttributeFlag flag) {
    Vec2f sum(0.0f, 0.0f);
    if (flag.m_mask == -1) {
        for (int i = m_kineticEnergyInstanceManager->size() - 1; i >= 0; i--) {
            soKineticEnergy* energy = m_kineticEnergyInstanceManager->atIndexFast(i);
            if (energy->isEnable()) {
                sum += energy->getSpeed();
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
                    sum += energy->getSpeed();
                }
            }
        }
    }
    return sum;
}
