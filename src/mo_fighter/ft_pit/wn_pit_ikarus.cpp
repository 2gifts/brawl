#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define WN_KINETIC_GENERIC_BUILDER
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/kinetic/so_kinetic_transactor.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_type_list.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/pit/wn_pit_ikarus.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_transactor.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnPitIkarus::activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        pos, 0.0f, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagBit7 = true;
    desc.flagBit6 = false;
    desc.flagBit5 = false;
    desc.flagBit4 = false;
    desc.flagBit3 = false;
    desc.flagBit2 = false;
    desc.flagBit1 = false;
    desc.flagBit0 = false;
    desc.flag4DBit7 = false;
    desc.flag4DBit6 = false;
    Weapon::activate(&desc);
    m_moduleAccesser->getTeamModule().setHitTeam(-1);
    m_moduleAccesser->getTeamModule().set2nd(5);
    m_moduleAccesser->getTeamModule().setHit2nd(5);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000001);
    m_moduleAccesser->getPostureModule().setScale(unk2);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

bool wnPitIkarus::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags != 0) {
        m_moduleAccesser->getStatusModule().changeStatusForce(4, m_moduleAccesser);
        return true;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

wnPitIkarus::~wnPitIkarus() { }

soKineticEnergy3f::~soKineticEnergy3f() { }

template <typename E>
void soKineticTransactor::updateEnergy(E* energy, soModuleAccesser* moduleAccesser) {
    if (energy->isEnable() == true && !energy->isSuspend())
        energy->updateEnergy(moduleAccesser);
}
template void soKineticTransactor::updateEnergy<soKineticEnergy3f>(soKineticEnergy3f*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<soKineticEnergyRotNormal>(soKineticEnergyRotNormal*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<wnKineticEnergyNormal>(wnKineticEnergyNormal*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<wnKineticEnergyGravity>(wnKineticEnergyGravity*, soModuleAccesser*);

// HYPOTHESIS: the transactor reaches an energy of the Ikarus instance pools through an overload selected by an empty
// soIntToType<pool level> tag (as in wn_diddy_barrel_kinetic_transactor.cpp).
static inline soKineticEnergy3f* getEnergy(void* pools, const soIntToType<0>&) {
    return reinterpret_cast<soKineticEnergy3f*>(static_cast<u8*>(pools) + 0x10);
}
static inline soKineticEnergyRotNormal* getEnergy(void* pools, const soIntToType<1>&) {
    return reinterpret_cast<soKineticEnergyRotNormal*>(static_cast<u8*>(pools) + 0x64);
}

void wnPitIkarusKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soKineticEnergy3f* energy = getEnergy(pools, soIntToType<0>());
    energy->resetEnergy(-1, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) const {
    moduleAccesser->getKineticModule().unableEnergyAll();

    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
    float angle = 90.0f * workManage.getFloat(0x21000000) / frames;
    soKineticEnergyRotNormal* energy = getEnergy(pools, soIntToType<1>());
    Vec3f rotation(0.0f, angle, 0.0f);
    energy->m_rotSpeed = rotation;
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x1F) {
        wnKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }

    switch (mode) {
    case 0x20: {
        wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    case 0x21: {
        const wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    case 0x22: {
        wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(static_cast<const void*>(pools), moduleAccesser);
        break;
    }
    case 0x23: {
        const wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(static_cast<const void*>(pools), moduleAccesser);
        break;
    }
    }
}
