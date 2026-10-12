#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnSheikNeedleStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessMove() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikNeedleStatusUniqProcessMove g_wnSheikNeedleStatusUniqProcessMove;

void wnSheikNeedleStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 2 && status != 3) {
        soPostureModule& posture = moduleAccesser->getPostureModule();
        soEffectModule& effect = moduleAccesser->getEffectModule();
        Vec3f zero(0.0f, 0.0f, 0.0f);
        posture.setRot(&zero, 0);
        Vec3f rot(0.0f, 0.0f, 0.0f);
        Vec3f pos = posture.getPos();
        // HYPOTHESIS: effect scale from lbl_27_rodata_7770; effect id 0x22 from the requesting status.
        effect.req((EfID)0x22, &pos, &rot, 1.0f, 0, -1);
    }
}
void wnSheikNeedleStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) { }
