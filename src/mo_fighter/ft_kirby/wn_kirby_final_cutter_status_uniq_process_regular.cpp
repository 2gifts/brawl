#define MT_VEC2F_ASSIGN_NOINLINE
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_value_accesser.h>
#include <math.h>
#include <types.h>

class wnKirbyFinalCutterStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    virtual ~wnKirbyFinalCutterStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
};

wnKirbyFinalCutterStatusUniqProcessRegular g_wnKirbyFinalCutterStatusUniqProcessRegular;

void wnKirbyFinalCutterStatusUniqProcessRegular::initStatus(soModuleAccesser* moduleAccesser) { }
void wnKirbyFinalCutterStatusUniqProcessRegular::exitStatus(soModuleAccesser* moduleAccesser, int) { }

void wnKirbyFinalCutterStatusUniqProcessRegular::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    // HYPOTHESIS: the energy is the controller-style normal energy (dynamic_cast target typeinfo not verified).
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(0));
    float angle = work.getFloat(0x11000001);
    float power = work.getFloat(0x11000002);
    float scale = moduleAccesser->getPostureModule().getLr();  // HYPOTHESIS: posture slot 0x2C (float getter)
    float x = scale * (power * (float)cos(angle));
    float y = -power * (float)sin(angle);
    Vec2f speed(x, y);
    energy.m_speed = speed;
    energy.enable();
    float limit = power - soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0);
    if (limit < 0.01f) limit = 0.01f;
    work.setFloat(limit, 0x11000002);
}

void wnKirbyFinalCutterStatusUniqProcessRegular::execMapCorrection(soModuleAccesser* moduleAccesser) { }
