#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnPikminPikminStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessTurn() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessTurn g_wnPikminPikminStatusUniqProcessTurn;

void wnPikminPikminStatusUniqProcessTurn::execFixPos(soModuleAccesser* a) {
    if (a->getWorkManageModule().getInt(0x10000011) & 0x40) {
        // HYPOTHESIS: CorrectKind 2 is the ground-stick mode used while the bike-style flag is set (not Correct_Air).
        a->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(2), 0);
    } else {
        a->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    }
}
