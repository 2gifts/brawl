#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftLuigiStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftLuigiStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftLuigiStatusUniqProcessSpecialLw::initStatus(soModuleAccesser*) { }

void ftLuigiStatusUniqProcessSpecialLw::execStatus(soModuleAccesser* moduleAccesser) {
    float horizontalDecel;
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
        horizontalDecel = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        horizontalDecel += soValueAccesser::getConstantFloat(moduleAccesser, 0xFC0, 0);
        moduleAccesser->getWorkManageModule().setFloat(horizontalDecel, 0x21000004);

        ftKineticEnergyController& controller =
            dynamic_cast<ftKineticEnergyController&>(
                *moduleAccesser->getKineticModule().getEnergy(2));
        float maxSpeed;
        if (moduleAccesser->getKineticModule().getKineticType() == 0x69) {
            maxSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFBE, 0);
        } else {
            maxSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC3, 0);
        }
        if (maxSpeed > horizontalDecel) {
            maxSpeed -= horizontalDecel;
        } else {
            maxSpeed = 0.0f;
        }

        Vec2f limit(maxSpeed, 0.0f);
        controller.m_speedLimit = limit;
    }

    float liftInput;
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003D) == false &&
        moduleAccesser->getWorkManageModule().isFlag(0x22000012) == true &&
        (moduleAccesser->getControllerModule().getTrigger() &
         soController::getButtonMask(soController::Pad_Button_Special)) != 0) {
        liftInput = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC1, 0);
        if (moduleAccesser->getSituationModule().getKind() != 2) {
            moduleAccesser->getKineticModule().changeKinetic(0x6A, moduleAccesser);
            moduleAccesser->getSituationModule().setKind(static_cast<SituationKind>(2), false);
            moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        }
    } else {
        liftInput = 0.0f;
    }

    moduleAccesser->getWorkManageModule().setFloat(liftInput, 0x21000005);
}

ftLuigiStatusUniqProcessSpecialLw g_ftLuigiStatusUniqProcessSpecialLw;
