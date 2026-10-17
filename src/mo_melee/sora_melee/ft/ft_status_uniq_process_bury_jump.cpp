#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBuryJump : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBuryJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessBuryJump g_ftStatusUniqProcessBuryJump;

// Jumping out of the ground (buried status): starts the fighter off with a pure vertical jump speed (common constants 0xD06 and
// 0xBC5 multiplied), with the stick, stop and gravity energies cleared of horizontal speed.
void ftStatusUniqProcessBuryJump::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float jumpSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xD06, 0);
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC5, 0);
    jumpSpeed = scale * jumpSpeed;
    kinetic.changeKinetic(0, moduleAccesser);
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    gravity.m_speedY = jumpSpeed;
    stop.m_speed = Vec2f(0.0f, 0.0f);
    controller.m_speed = Vec2f(0.0f, 0.0f);
    work.setFloat(0.0f, 0x21000006);
}
