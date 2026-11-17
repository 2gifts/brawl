#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_SCALE_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_pk_star_storm.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// TODO(link): 100% exact; .rodata/.data/.bss carved in ft_ness splits.txt. Does not link yet: the sized kinetic module builder (soKineticMediatorNull,
//   2 energies) makes our object emit weak copies of the soInstanceManagerFullPropertyVector<P15soKineticEnergy,2> constructor/destructor,
//   soInstanceUnitFullProperty<P15soKineticEnergy> and soKineticMediatorNull destructors (+0x90 text), where the original calls the
//   copy that lives in sora_melee (fn_27_1AD79C) and its bss is 0x60 bigger. Needs those template members declared extern in
//   wn_module_accesser_builder.h under FT_REL_LINK_EXTERN (shared header).
//   Batch 11 finding: the extern declarations alone will not link. The original constructor calls fn_27_1AD79C (sora_melee,
//   0xA0: by its call site the soInstanceManagerFullPropertyVector<soKineticEnergy*, 2>(bool) constructor), and the original
//   kinetic builder destructor calls the sora_melee functions currently named __dt__18ftFighterBuildDataFv (for
//   soKineticMediatorNull's destructor) and __dt__61soInstanceManagerFullPropertyVector<20soAnimCmdControlUnit,6>Fv (for
//   the <soKineticEnergy*, 2> vector destructor). Those two names are right: ftFighterBuildData's destructor is defined by
//   the Matching ft_fighter_build_data.cpp and the vector<soAnimCmdControlUnit, 6> destructor is called by Matching article
//   units, so the original linker folded identical code (TIPS: "the call in the target names the first symbol of that
//   address"). Renaming them would break those units; linking this unit needs a way to bind our calls to the folded
//   address (an alias at the same address) rather than a rename.

wnNessPKStarStorm::wnNessPKStarStorm(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKStarStormData* data) :
    wnWeaponBuilder<wnNessPKStarStormModuleAccesserBuildConfig>(
        "NessPKStarStorm", articleId, info,
        wnModuleAccesserBuildData(106, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq, info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

bool wnNessPKStarStorm::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

// The star starts at the given position and angle (in radians): it moves along the diagonal the angle gives (a downward vector
// turned by the angle) with the speed and the acceleration of the parameter block.
void wnNessPKStarStorm::activate(int founderTaskId, int team, Vec3f* pos, float angle) {
    wnActivateDesc desc;
    Vec2f dir;
    Vec2f speed;
    Vec2f accel;
    Vec2f speedUnused;
    Vec2f accelUnused;
    Vec2f target;
    Vec2f limit;
    Vec2f speedTemp;
    Vec2f accelTemp;
    Vec2f targetTemp;
    Vec2f limitTemp;
    float lr;
    if (angle > 0.0f) {
        lr = 1.0f;
    } else {
        lr = -1.0f;
    }
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    dir.m_x = 0.0f;
    dir.m_y = -1.0f;
    dir.rot(angle, &dir);
    float speedMul = m_param->m_speed;
    dir.scale(speedMul, &speedTemp);
    Vec2f::copy(speedUnused, speedTemp);
    Vec2f::copy(speed, speedTemp);
    float accelMul = m_param->m_accel;
    dir.scale(accelMul, &accelTemp);
    Vec2f::copy(accelUnused, accelTemp);
    Vec2f::copy(accel, accelTemp);
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    kinetic.changeKinetic(2, m_moduleAccesser);
    normal.m_speed = speed;
    normal.m_accel = accel;
    speed.scale(10.0f, &targetTemp);
    Vec2f::copy(target, targetTemp);
    normal.m_speedTarget = target;
    speed.scale(10.0f, &limitTemp);
    Vec2f::copy(limit, limitTemp);
    normal.m_speedLimit = limit;
    normal.enable();
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Vec3f rot;
    rot.m_x = angle * -lr * 57.29578f;
    rot.m_y = 0.0f;
    rot.m_z = 0.0f;
    posture.setRot(&rot, 0);
}
