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
    // HYPOTHESIS: the four vectors are zero (lbl_27_rodata_2200 holds 0.0f).
    Vec3f pos;
    Vec3f rot;
    Vec3f posRange;
    Vec3f rotRange;
    pos.m_x = pos.m_y = pos.m_z = 0.0f;
    rot.m_x = rot.m_y = rot.m_z = 0.0f;
    posRange.m_x = posRange.m_y = posRange.m_z = 0.0f;
    rotRange.m_x = rotRange.m_y = rotRange.m_z = 0.0f;
    // HYPOTHESIS: scale constant from lbl_27_rodata_2204 (not verified).
    float scale = 1.0f;
    moduleAccesser->getEffectModule().req(static_cast<EfID>(4), node, &pos, &rot, scale, &posRange, &rotRange, false, 0);
    moduleAccesser->getControllerModule().setRumble(0x19, 0, false, -1);
}

ftStatusUniqProcessShieldBreakFly g_ftStatusUniqProcessShieldBreakFly;
