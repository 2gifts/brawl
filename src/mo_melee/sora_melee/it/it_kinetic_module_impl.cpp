#include <it/item.h>
#include <it/kinetic/it_kinetic_energy.h>
#include <it/kinetic/it_kinetic_module_impl.h>
#include <new>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <types.h>

// HYPOTHESIS: the item status data keeps the kinetic type of the status in bits 11-18 of its word at +0xC.
struct itStatusData : public soStatusData {
    char unk0[0xC];
    int unkC : 11;
    int m_kineticType : 8;
    int unkC_19 : 13;
};

void itKineticModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                  soModuleAccesser* moduleAccesser) {
    changeKinetic(static_cast<itStatusData*>(statusData)->m_kineticType, moduleAccesser);
}

bool itKineticModuleImpl::isObserv(char group) {
    bool result = false;
    if (soKineticModuleImpl::isObserv(group) || group == 'f') {
        result = true;
    }
    return result;
}

// TODO: clean up: these calls only stand in for the module's use of its energy members (the constructor and the
// energy users are not decompiled yet); they make the unit put out the energies' inline functions.
#pragma dont_inline on
void itKineticModuleImpl_useEnergies(void* p, itKineticEnergy3f* e3f, itKineticEnergyGravity* eg, itKineticEnergy* e,
                                     const Vec2f& v2, const Vec3f& v3) {
    new (p) itKineticRotEnergy3f();
    e3f->itKineticEnergy3f::getSpeed3f();
    e3f->itKineticEnergy3f::clearSpeed();
    e3f->itKineticEnergy3f::getSpeed();
    e3f->itKineticEnergy3f::onConsiderGroundFriction();
    e3f->itKineticEnergy3f::offConsiderGroundFriction();
    eg->itKineticEnergyGravity::clearSpeed();
    eg->itKineticEnergyGravity::getSpeed();
    eg->itKineticEnergyGravity::resetEnergy(0, v2, v3, NULL);
    e->itKineticEnergy::getSpeed();
    e->itKineticEnergy::resetEnergy(0, v2, v3, NULL);
    e->itKineticEnergy::clearSpeed();
    delete e3f;
    delete eg;
    delete e;
    delete (itKineticEnergyInterface*)p;
    delete (itKineticEnergyStop*)p;
}
#pragma dont_inline reset
