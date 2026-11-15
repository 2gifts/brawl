#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>
#include <types.h>

// The flight of Pit's arrow (status 1): the arrow turns toward the owner's stick while it flies.
class wnPitBowArrowStatusUniqProcessFly : public soStatusUniqProcess {
public:
    virtual ~wnPitBowArrowStatusUniqProcessFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPitBowArrowStatusUniqProcessFly g_wnPitBowArrowStatusUniqProcessFly;

void wnPitBowArrowStatusUniqProcessFly::initStatus(soModuleAccesser* moduleAccesser) {
    // Life of the flight: constant 0xfa5 plus constant 0xfa6 per frame the shot was charged (work float 0x21000000).
    int life = static_cast<int>(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0) +
                                soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0) *
                                    moduleAccesser->getWorkManageModule().getFloat(0x21000000));
    moduleAccesser->getWorkManageModule().setInt(life, 0x20000000);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float speed;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000003) == true) {
        speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa1, 0);
    } else {
        speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0);
    }
    float angle = 0.017453292f * moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    energy.m_speed = Vec2f(speed * static_cast<float>(cos(angle)), speed * static_cast<float>(sin(angle)));
    moduleAccesser->getWorkManageModule().setFloat(speed, 0x11000001);
}

void wnPitBowArrowStatusUniqProcessFly::execStatus(soModuleAccesser* moduleAccesser) {
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    Vec2f stick(moduleAccesser->getControllerModule().getStickX(), moduleAccesser->getControllerModule().getStickY());
    float tilt = stick.length();
    if (tilt > 1e-05f) {
        float x = stick.m_x;
        float y = stick.m_y;
        float diff = angle - 57.29578f * static_cast<float>(atan2(y, x));
        while (diff > 180.0f) {
            diff -= 360.0f;
        }
        while (diff < -180.0f) {
            diff += 360.0f;
        }
        float turn = tilt * soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
        if (diff >= 45.0f) {
            if (diff <= 135.0f) {
                angle -= turn;
            }
        } else if (diff <= -45.0f) {
            if (diff >= -135.0f) {
                angle += turn;
            }
        }
    }
    moduleAccesser->getPostureModule().setRot(&Vec3f(-angle, 90.0f, 0.0f), 0);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float radians = 0.017453292f * angle;
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    energy.m_speed = Vec2f(speed * static_cast<float>(cos(radians)), speed * static_cast<float>(sin(radians)));
    moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000002);
}
