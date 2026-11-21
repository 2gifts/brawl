// TODO: clean up - MATCH-ONLY (compiler workaround): MWCC hits an internal compiler error (CTemplateTools.c:256) on the
// explicit builder instantiations of ft_builder_noinline.h depending on how many preprocessor names the unit has seen
// (adding or removing an unused #ifdef name flips it). This unused name keeps the unit compiling; change it if the ICE returns.
#ifdef FT_POKETRAINER_ICE_DODGE_0
#endif
#ifdef FT_POKETRAINER_ICE_DODGE_WAND_X0
#endif
#define FT_FIGHTER_ANIMCMD_LONG
#define FT_POKETRAINER_DISGUISE_PUSH_RANGE
#define FT_POKETRAINER_ARTICLE_POOLS
#define FT_ANIMCMD_ARRAY_NULL_INLINE_CTOR
#define WN_WEAPON_ANIMCMD_LONG
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define SO_EVENT_PRESENTER_STORE_NULL_LIST
#define FT_PRESENTER_RELOAD_MANAGE_ID
#define FT_POKETRAINER_PRESENTER_CHECK_MANAGE_ID
#define FT_NULL_TAIL_MODULES
#define FT_SOUND_GENERATOR_DTOR_INLINE
#define FT_GIMMICK_POOL_INLINE_DTOR
#define FT_VIRTUAL_NODE_POOL_INLINE_DTOR
#define FT_GENERAL_WORK_ABSTRACT_DTOR_INLINE
#define FT_TEAM_INLINE_DTOR
#define FT_MODULE_BUILDER
#define FT_POKETRAINER_NULL_SHAKE
#define FT_POKETRAINER_AREA_CONFIG
#define FT_POKETRAINER_MOTION
#define FT_POKETRAINER_HIT_CATEGORY
#define FT_POKETRAINER_ARRAY_NULL_INLINE
#define FT_POKETRAINER_MOTION_DTOR
#define FT_POKETRAINER_ATTACK_SEVEN
#define SO_OTHER_ANIM_CTOR
#define SO_INSTANCE_UNIT_EXTERNAL_FULL_PROPERTY_DTOR
#define SO_COLLISION_GROUP_ALIGNED
#define WN_WEAPON_BUILDER
#define SO_NULLABLE_INLINE_CTOR
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define FT_EXTEND_PARAM_EX_SETUP_OUT_OF_LINE
#define SO_GENERAL_WORK_SIMPLE_CTOR_INLINE
#define FT_EXTEND_PARAM_EX_DTOR_OUT_OF_LINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_event_unit_instance.h>
#include <ft/poke/ft_poke_trainer.h>
#pragma force_active on
#pragma force_active off
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <so/so_slow.h>
#include <so/motion/so_motion_change_param.h>
#include <mt/mt_prng.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_external_value_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <st/stage.h>
#include <ai/ai_mgr.h>
#include <types.h>
#include <so/templates/so_array_value_wnPokeTrainerMBall_BallInfo.h>
#define FT_NOINLINE_SKIP_SHARED_DEFS
#define FT_BC ftPokeTrainerBuildConfig
#include <ft/builder/ft_builder_noinline.h>
#include <cm/cm_camera_controller.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/so_module_accesser.h>
#include <types.h>

// ftPokeTrainerWanderingModule pieces (sora_melee 0x387594..0x389D54): the Pokemon Trainer walking beside the stage; start,
// replace and the fix-position steps are not decompiled yet.

// TODO: clean up - MATCH-ONLY: the unit's literal pool is not split, so the two literals are read through their pool
// labels (0.0f and 57.29578f, degrees per radian) to keep the relocations equal.
extern "C" {
extern const float lbl_27_rodata_7C8C;
extern const float lbl_27_rodata_7CB0[];
}

// TODO: clean up - placeholder fn_ name for the one-line accessor at 0x388920 (whether the AI manager is enabled).
extern "C" u8 fn_27_388920() {
    return g_aiMgr->m_enable;
}

Vec3f ftPokeTrainerWanderingModule::getTargetUpperPos(soModuleAccesser* acc) {
    Vec3f pos;
    pos.m_x = acc->getWorkManageModule().getFloat(0x11000013);
    pos.m_y = CameraController::getInstance()->m_stageCameraBounds.m_up;
    if (acc->getWorkManageModule().isFlag(0x1200003f) == true) {
        pos.m_z = lbl_27_rodata_7C8C;
    } else {
        pos.m_z = acc->getPostureModule().getPos().m_z;
    }
    return pos;
}

static inline float clampAngle(float value, float min, float max) {
    float result = nw4r::math::FSelect(value - min, value, min);
    return nw4r::math::FSelect(result - max, max, result);
}

void ftPokeTrainerWanderingModule::setDirectionTarget(soModuleAccesser* acc) {
    Vec3f pos = acc->getPostureModule().getPos();
    float targetX = acc->getWorkManageModule().getFloat(0x11000013);
    float targetY = acc->getWorkManageModule().getFloat(0x11000014);
    float dy = targetY - pos.m_y;
    float dx = targetX - pos.m_x;
    float angle = lbl_27_rodata_7CB0[0] * (float)atan2(dx, dy);
    float limit = soValueAccesser::getConstantFloat(acc, 0xfa3, 0);
    float clamped = clampAngle(angle, -limit, limit);
    acc->getWorkManageModule().setFloat(clamped, 0x11000018);
    acc->getStageObject().updateNodeSRT();
}
