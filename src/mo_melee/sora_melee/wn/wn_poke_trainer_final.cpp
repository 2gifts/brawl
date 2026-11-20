#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode
#define WN_POKE_TRAINER_FINAL_UNIT
#define FT_MODULE_BUILDER
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define WN_ANIMCMD_RESET_MODE_BOX
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE
#define WN_ANIMCMD_STACK_PTR
#define WN_ARTICLE_ANIMCMD_INT
#define SO_BUILD_DATA_SLOPE_LIMIT_EXTERNAL
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define SO_OTHER_ANIM_CTOR
#define SO_COLLISION_GROUP_ALIGNED
#define FT_POKETRAINER_MOTION_DTOR
#define FT_POKETRAINER_ATTACK_SEVEN
#include <wn/wn_module_accesser_builder.h>
#include <wn/poke/wn_poke_trainer_final.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

#pragma push
#pragma pool_data off
wnPokeTrainerFinal::wnPokeTrainerFinal(s32 articleId, const wnArticleBuildInfo& info, const wnPokeTrainerFinalData* data) :
    wnWeaponBuilder<wnPokeTrainerFinalModuleAccesserBuildConfig>(
        "PokeTrainerFinal", articleId, info,
        wnModuleAccesserBuildData(0x12, data, info.getHeapModule()->getInstanceHeap(), static_cast<Heaps::HeapType>(0x1F), // HYPOTHESIS: heap type of the model instances
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, reinterpret_cast<s32>(data->unk38), 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}
#pragma pop

wnPokeTrainerFinalParamAccesser::~wnPokeTrainerFinalParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnPokeTrainerFinalModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>
