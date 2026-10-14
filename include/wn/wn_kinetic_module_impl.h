#pragma once
// The kinetic module of the generic article (Weapon): the fighter kinetic module plus a status observer that switches
// the kinetic mode when a status asks for one.

#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_event_presenter.h>

class wnKineticModuleImpl : public soKineticModuleImpl, public soStatusEventObserver {
    s32 m_outsideGroundEnergyId; // +0x2C: energy index fed by the outside movement-ground speed (-1: none)
public:
    wnKineticModuleImpl(soModuleAccesser* acc, soInstanceManagerFullProperty<soKineticEnergy*>* energies, soKineticMediator* mediator);
    ~wnKineticModuleImpl() { }
    virtual void changeKinetic(int kineticType, soModuleAccesser* acc);
    virtual void addSpeed(Vec3f* speed, soModuleAccesser* acc);
    virtual void addSpeedOutside(soKineticEnergy::OutsideType type, Vec3f* speed);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* data, soModuleAccesser* acc);

    // Energy 0 is the main one, energy 1 the extra one; the kinetic type selects how they are set up.
    void initializeNormal(int kineticType, soModuleAccesser* acc);
    void initializeStop(int kineticType, soModuleAccesser* acc);
    void initializeReset(int kineticType, soModuleAccesser* acc);
    static void setOutsideMovementGroundEnergyId(soKineticModule* module, int energyId);
};
static_assert(sizeof(wnKineticModuleImpl) == 0x30, "Weapon kinetic module extent");
