#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <wn/samus/wn_samus_supermissile.h>
#include <wn/wn_kinetic_energy_normal.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusSuperMissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusSuperMissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusSuperMissileStatusUniqProcess g_wnSamusSuperMissileStatusUniqProcess;

void wnSamusSuperMissileStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusSuperMissileStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    const wnSamusSuperMissileParam* param = dynamic_cast<wnSamusSuperMissile&>(moduleAccesser->getStageObject()).getData()->m_param;
    switch (status) {
    case 0: {
        int count = moduleAccesser->getWorkManageModule().getInt(0x10000006);
        moduleAccesser->getPostureModule().getLr();
        if (count <= param->unk8) {
            if (count == param->unk8) {
                wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
                Vec2f speed = energy.getSpeed();
                if (speed.length() > 0.00001f) {
                    speed.normalize();
                    energy.m_accel = speed * param->unk10;
                }
            }
            moduleAccesser->getWorkManageModule().incInt(0x10000006);
        }
        break;
    }
    }
}
