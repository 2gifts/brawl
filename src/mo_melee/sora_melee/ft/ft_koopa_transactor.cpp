#include <StaticAssert.h>
#include <ft/koopa/ft_koopa_transactor.h>
#include <wn/koopa/wn_koopa_breath.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <mt/mt_prng.h>
#include <so/team/so_team.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>

ftKoopaTransactor::ftKoopaTransactor() { }
ftKoopaTransactor::~ftKoopaTransactor() { }

// HYPOTHESIS: resets the breath work (no active breath, both fire gauges full; Kirby reads his copy's constants).
void ftKoopaTransactor::init(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
    float gauge = moduleAccesser->getConstantFloatKirby(0xfa6);
    moduleAccesser->getWorkManageModule().setFloat(gauge, 0x11000014);
    float gaugeMax = moduleAccesser->getConstantFloatKirby(0xfa8);
    moduleAccesser->getWorkManageModule().setFloat(gaugeMax, 0x11000013);
}

void ftKoopaTransactor::exit(soModuleAccesser* moduleAccesser) { }

// BYTES: the gauge update stays a tail call.
#pragma dont_inline on
void ftKoopaTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateSpecialNTransact(moduleAccesser);
}
#pragma dont_inline reset

// HYPOTHESIS: inline clamp helper (fsel based), as in the breath status.
static inline float clampf(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// Outside the breath status both fire gauges regenerate; during it they drain while work int 0x20000002 is 1.
void ftKoopaTransactor::processUpdateSpecialNTransact(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isEstimate() == true && !moduleAccesser->getStopModule().isStop()) {
        float gauge = moduleAccesser->getWorkManageModule().getFloat(0x11000014);
        float gauge2 = moduleAccesser->getWorkManageModule().getFloat(0x11000013);
        if (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true) != 0x112) {
            gauge += moduleAccesser->getConstantFloatKirby(0xfaa);
            gauge2 += moduleAccesser->getConstantFloatKirby(0xfab);
        } else if (moduleAccesser->getWorkManageModule().getInt(0x20000002) == 1) {
            gauge -= moduleAccesser->getConstantFloatKirby(0xfac);
            gauge2 -= moduleAccesser->getConstantFloatKirby(0xfad);
        }
        float gaugeMin = moduleAccesser->getConstantFloatKirby(0xfa7);
        moduleAccesser->getWorkManageModule().setFloat(clampf(gauge, gaugeMin, moduleAccesser->getConstantFloatKirby(0xfa6)), 0x11000014);
        float gauge2Min = moduleAccesser->getConstantFloatKirby(0xfa9);
        moduleAccesser->getWorkManageModule().setFloat(clampf(gauge2, gauge2Min, moduleAccesser->getConstantFloatKirby(0xfa8)), 0x11000013);
    }
}

// The fire breath is article 0xd.
bool ftKoopaTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnKoopaBreath* weapon = dynamic_cast<wnKoopaBreath*>(article);
        if (weapon == NULL) {
            return false;
        }
        return activeArticle(weapon, acc);
    }
    return false;
}

// The breath parameters of the fighter (or of Kirby's copy): only the mouth offset is read here.
struct ftKoopaBreathParamView {
    u8 unk0[0x44];
    float m_offsetZ; // 0x44
    float m_offsetY; // 0x48
};

// Starts a flame from the mouth node (Kirby's own node for the copy) along the head angle (work float 0x21000004) plus a
// random spread; the two gauge ratios scale its speed and size.
bool ftKoopaTransactor::activeArticle(wnKoopaBreath* weapon, soModuleAccesser* acc) {
    int subKind = acc->getStageObject().soGetSubKind();
    Vec3f pos = acc->getPostureModule().getPos();
    float lr = acc->getPostureModule().getLr();
    acc->getPostureModule().getScale(); // HYPOTHESIS: an unused scale read.
    float speedMul = 1.0f;
    const ftKoopaBreathParamView* param =
        static_cast<const ftKoopaBreathParamView*>(g_ftCommonDataAccesser.getKoopaBreathParam(static_cast<ftKind>(subKind)));
    Vec3f offset(0.0f, param->m_offsetY, param->m_offsetZ);
    float angle = acc->getWorkManageModule().getFloat(0x21000004);
    if (subKind == Fighter_Kirby) {
        pos = acc->getModelModule().getNodeGlobalPosition(0x19c, &offset, false, false);
    } else {
        pos = acc->getModelModule().getNodeGlobalPosition(0x33, &offset, false, false);
    }
    float speed = acc->getWorkManageModule().getFloat(0x11000014) / acc->getConstantFloatKirby(0xfa6);
    float scale = acc->getWorkManageModule().getFloat(0x11000013) / acc->getConstantFloatKirby(0xfa8);
    float baseAngle = acc->getConstantFloatKirby(0xfa0);
    float spread = acc->getConstantFloatKirby(0xfa1);
    angle = baseAngle + angle;
    angle += spread * randf() - 0.5f * spread;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), &pos, lr, 0, speed, scale, speedMul,
                     0.017453292f * angle);
    return false;
}
