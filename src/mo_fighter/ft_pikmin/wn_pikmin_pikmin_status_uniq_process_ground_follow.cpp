#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

extern "C" {
void fn_113_26470(void* self, soModuleAccesser* moduleAccesser);
}

class wnPikminPikminStatusUniqProcessGroundFollow : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessGroundFollow() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessGroundFollow g_wnPikminPikminStatusUniqProcessGroundFollow;

void wnPikminPikminStatusUniqProcessGroundFollow::execStatus(soModuleAccesser* a) {
    if (!a->getWorkManageModule().isFlag(0x12000003)) {
        if (a->getWorkManageModule().getInt(0x10000011) & 0x40) {
            a->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(2), 0);
        } else {
            a->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
        }
    } else {
        a->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    }
}

void wnPikminPikminStatusUniqProcessGroundFollow::execFixPosCounter(soModuleAccesser* a) {
    fn_113_26470(this, a);
    if (a->getWorkManageModule().isFlag(0x22000001) == 1) {
        a->getWorkManageModule().offFlag(0x22000001);
        Vec3f pos = a->getPostureModule().getPos();
        float y = pos.m_y;
        float diff = a->getWorkManageModule().getFloat(0x11000005) - y;
        if (diff < -5.0f) {
            if (a->getSituationModule().getKind() == 0) {
                if (a->getGroundModule().isPassableGround(0)) {
                    a->getStatusModule().changeStatusRequest(11, a);
                }
                return;
            }
        }
        a->getStatusModule().changeStatusRequest(12, a);
    }
}
