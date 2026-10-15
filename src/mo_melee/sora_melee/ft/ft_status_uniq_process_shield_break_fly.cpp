#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <mt/mt_vector.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: class shape from the shared Shield_Break_Fly status; the entry spawns effect 4 on the
// node 0x12c with zero position and range vectors, then sets controller rumble 0x19 (no repeat).
class ftStatusUniqProcessShieldBreakFly : public soStatusUniqProcess {
public:
    ftStatusUniqProcessShieldBreakFly() { }
    virtual ~ftStatusUniqProcessShieldBreakFly() { }
    virtual void initStatus(soModuleAccesser*);
};

void ftStatusUniqProcessShieldBreakFly::initStatus(soModuleAccesser* moduleAccesser) {
    int node = moduleAccesser->getModelModule().getCorrectNodeId(0x12c);
    // HYPOTHESIS: the four vectors are zero (lbl_27_rodata_2200 holds 0.0f, lbl_27_rodata_2204 the 1.0f effect scale); they live in one block, filled in address order.
    struct {
        Vec3f rotRange;
        Vec3f posRange;
        Vec3f rot;
        Vec3f pos;
    } zero;
    zero.rotRange.m_x = 0.0f;
    zero.rotRange.m_y = 0.0f;
    zero.rotRange.m_z = 0.0f;
    zero.posRange.m_x = 0.0f;
    zero.posRange.m_y = 0.0f;
    zero.posRange.m_z = 0.0f;
    zero.rot.m_x = 0.0f;
    zero.rot.m_y = 0.0f;
    zero.rot.m_z = 0.0f;
    zero.pos.m_x = 0.0f;
    zero.pos.m_y = 0.0f;
    zero.pos.m_z = 0.0f;
    moduleAccesser->getEffectModule().req(static_cast<EfID>(4), node, &zero.pos, &zero.rot, 1.0f, &zero.posRange, &zero.rotRange, false, 0);
    moduleAccesser->getControllerModule().setRumble(0x19, 0, false, -1);
}

ftStatusUniqProcessShieldBreakFly g_ftStatusUniqProcessShieldBreakFly;
