#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <wn/weapon.h>
#include <types.h>

// HYPOTHESIS: parameter fields named by their use in the turn status (int at +0x34, floats at +0x18).
struct wnToonLinkBoomerangParam {
    u8 unk0[0x18];
    float unk18;
    u8 unk1C[0x18];
    int unk34;
};

// Local view of the boomerang article: only the parameter pointer at 0x20E4 is used here.
class wnToonLinkBoomerang : public Weapon {
    u8 unkA8[0x20E4 - sizeof(Weapon)];
public:
    wnToonLinkBoomerangParam* m_param;
};

// Local view of the energy used by the turn status: its +0x10 member is the acceleration vector.
class wnToonLinkBoomerangTurnEnergyView : public soKineticEnergyNormal {
};

// HYPOTHESIS: Vec2f copy assignment, called out of line by its mangled symbol in the original.
extern "C" void __as__5Vec2fFRC5Vec2f(Vec2f* self, const Vec2f* source);

class wnToonLinkBoomerangStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBoomerangStatusUniqProcessTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnToonLinkBoomerangStatusUniqProcessTurn g_wnToonLinkBoomerangStatusUniqProcessTurn;

void wnToonLinkBoomerangStatusUniqProcessTurn::initStatus(soModuleAccesser* a) {
    wnToonLinkBoomerangParam* param = dynamic_cast<wnToonLinkBoomerang&>(a->getStageObject()).m_param;
    soKineticEnergy* base = a->getKineticModule().getEnergy(0);
    wnToonLinkBoomerangTurnEnergyView& energy = dynamic_cast<wnToonLinkBoomerangTurnEnergyView&>(*base);
    // HYPOTHESIS: rodata constants lbl_121_rodata_A0 (0.0f) and lbl_121_rodata_A4 (1.0f); values unverified.
    const float kA0 = 0.0f;
    const float kA4 = 1.0f;
    Vec2f accel(kA0, kA0);
    __as__5Vec2fFRC5Vec2f(&energy.m_accel, &accel);
    float value = a->getWorkManageModule().getFloat(0x11000001);
    value = (value > kA0) ? value + kA4 : value - kA4;
    a->getWorkManageModule().setFloat(value, 0x11000001);
    a->getWorkManageModule().setInt(param->unk34, 0x20000000);
    Vec3f rot = a->getPostureModule().getRot(1);
    rot.m_y = param->unk18;
    a->getPostureModule().setRot(&rot, 1);
}

void wnToonLinkBoomerangStatusUniqProcessTurn::execStatus(soModuleAccesser* moduleAccesser) { }
void wnToonLinkBoomerangStatusUniqProcessTurn::execFixPos(soModuleAccesser* moduleAccesser) { }
