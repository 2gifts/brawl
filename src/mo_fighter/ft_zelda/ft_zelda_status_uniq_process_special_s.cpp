#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftZeldaStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialS();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

// Grounded start clears the lift-off speed of the first kinetic energy.
void ftZeldaStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        moduleAccesser->getKineticModule().getEnergy(1)->clearSpeed();
    }
}

// BYTES: this unit owns the shared soStatusUniqProcess default virtuals (global in the original), in this order.
bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) { return true; }
void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) { }
bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) { }
bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusUniqProcess::execFixCamera(soModuleAccesser*) { }
void soStatusUniqProcess::execFixPos(soModuleAccesser*) { }
void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) { }
void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) { }
void soStatusUniqProcess::execStop(soModuleAccesser*) { }
void soStatusUniqProcess::execStatus(soModuleAccesser*) { }
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) { }
ftZeldaStatusUniqProcessSpecialS::~ftZeldaStatusUniqProcessSpecialS() { }
void soStatusUniqProcess::initStatus(soModuleAccesser*) { }

ftZeldaStatusUniqProcessSpecialS g_ftZeldaStatusUniqProcessSpecialS;
