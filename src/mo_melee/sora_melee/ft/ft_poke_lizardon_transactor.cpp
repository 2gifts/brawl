#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/poke/ft_poke_lizardon_transactor.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/team/so_team.h>
#include <wn/wn_weapon_builder.h>

// The breath article (its header needs the weapon builder chain): the members this unit uses.
class wnPokeLizardonBreath : public Weapon {
public:
    void activate(int founderTaskId, int team, const Vec3f& position, int kind, float lr, float speed, float scale,
                  float speedMul, float angle);
};

// HYPOTHESIS: inline clamp helper (fsel based), as in the Bowser breath status.
static inline float clampf(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

ftPokeLizardonTransactor::ftPokeLizardonTransactor() { }
ftPokeLizardonTransactor::~ftPokeLizardonTransactor() { }

// No active breath, both gauges full (Kirby reads his copy's constants).
void ftPokeLizardonTransactor::init(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
    float gauge = moduleAccesser->getConstantFloatKirby(0xfa6);
    moduleAccesser->getWorkManageModule().setFloat(gauge, 0x11000017);
    float gauge2 = moduleAccesser->getConstantFloatKirby(0xfa8);
    moduleAccesser->getWorkManageModule().setFloat(gauge2, 0x11000016);
}

void ftPokeLizardonTransactor::initTransact(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
    float gauge = moduleAccesser->getConstantFloatKirby(0xfa6);
    moduleAccesser->getWorkManageModule().setFloat(gauge, 0x11000017);
    float gauge2 = moduleAccesser->getConstantFloatKirby(0xfa8);
    moduleAccesser->getWorkManageModule().setFloat(gauge2, 0x11000016);
}

void ftPokeLizardonTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateTransact(moduleAccesser);
}

// Outside the breath status the gauges regenerate; during it they drain while work int 0x20000002 is 1. Each stays
// between its minimum and maximum constants.
void ftPokeLizardonTransactor::processUpdateTransact(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isEstimate() == true && !moduleAccesser->getStopModule().isStop()) {
        float gauge = moduleAccesser->getWorkManageModule().getFloat(0x11000017);
        float gaugeMin = moduleAccesser->getConstantFloatKirby(0xfa7);
        float gaugeMax = moduleAccesser->getConstantFloatKirby(0xfa6);
        float gauge2 = moduleAccesser->getWorkManageModule().getFloat(0x11000016);
        float gauge2Min = moduleAccesser->getConstantFloatKirby(0xfa9);
        float gauge2Max = moduleAccesser->getConstantFloatKirby(0xfa8);
        if (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true) != 0x112) {
            gauge += moduleAccesser->getConstantFloatKirby(0xfac);
            gauge2 += moduleAccesser->getConstantFloatKirby(0xfad);
        } else if (moduleAccesser->getWorkManageModule().getInt(0x20000002) == 1) {
            gauge -= moduleAccesser->getConstantFloatKirby(0xfaa);
            gauge2 -= moduleAccesser->getConstantFloatKirby(0xfab);
        }
        moduleAccesser->getWorkManageModule().setFloat(clampf(gauge, gaugeMin, gaugeMax), 0x11000017);
        moduleAccesser->getWorkManageModule().setFloat(clampf(gauge2, gauge2Min, gauge2Max), 0x11000016);
    }
}

// The breath is article 0xd (the only article the transactor starts).
bool ftPokeLizardonTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    return activeArticle1(dynamic_cast<wnPokeLizardonBreath*>(article), acc);
}

// The breath parameters of the fighter (or of Kirby's copy): only the mouth offset is read here.
struct ftPokeLizardonBreathParamView {
    u8 unk0[0x44];
    float m_offsetZ; // 0x44
    float m_offsetY; // 0x48
};

// Bowser's breath start with Lizardon's mouth node and gauges: along the head angle (work float 0x21000004) plus a random
// spread, the two gauge ratios scaling its speed and size.
bool ftPokeLizardonTransactor::activeArticle1(wnPokeLizardonBreath* weapon, soModuleAccesser* acc) {
    int subKind = acc->getStageObject().soGetSubKind();
    const ftPokeLizardonBreathParamView* param = static_cast<const ftPokeLizardonBreathParamView*>(
        g_ftCommonDataAccesser.getPokeLizardonBreathParam(static_cast<ftKind>(subKind)));
    Vec3f offset(0.0f, param->m_offsetY, param->m_offsetZ);
    Vec3f pos = acc->getPostureModule().getPos();
    float lr = acc->getPostureModule().getLr();
    acc->getPostureModule().getScale(); // HYPOTHESIS: an unused scale read.
    float speedMul = 1.0f;
    float angle = acc->getWorkManageModule().getFloat(0x21000004);
    if (subKind == Fighter_Kirby) {
        pos = acc->getModelModule().getNodeGlobalPosition(0x19c, &offset, false, false);
    } else {
        pos = acc->getModelModule().getNodeGlobalPosition(0x21, &offset, false, false);
    }
    float speed = acc->getWorkManageModule().getFloat(0x11000017) / acc->getConstantFloatKirby(0xfa6);
    float scale = acc->getWorkManageModule().getFloat(0x11000016) / acc->getConstantFloatKirby(0xfa8);
    float baseAngle = acc->getConstantFloatKirby(0xfa0);
    float spread = acc->getConstantFloatKirby(0xfa1);
    angle = baseAngle + angle;
    angle += spread * randf() - 0.5f * spread;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), pos, 0, lr, speed, scale, speedMul,
                     0.017453292f * angle);
    return false;
}
