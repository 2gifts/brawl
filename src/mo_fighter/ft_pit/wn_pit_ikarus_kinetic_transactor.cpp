#define WN_KINETIC_GENERIC_BUILDER
#define WN_WEAPON_BUILDER
#define SO_KINETIC_ENERGY_3F_FULL // the transactor sets the 3f energy's speed, acceleration and limit
#define MT_VEC3F_ASSIGN_NOINLINE // BYTES: the energy settings copy through the out-of-line __as__5Vec3f (weak copy here)
#define MT_VEC3F_CTOR_NOINLINE // BYTES: the rotation speed is built with the out-of-line Vec3f constructor
#include <ft/builder/ft_dol_array_list.h>
#include <wn/pit/wn_pit_ikarus.h>
#include <mt/mt_prng.h>
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_type_list.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_transactor.h>

// The Ikarus kinetic transactor: its own unit after the Ikarus (the original emits it after the Ikarus unit's thunks).

// HYPOTHESIS: the transactor reaches an energy of the Ikarus instance pools through an overload selected by an empty
// soIntToType<pool level> tag (as in wn_diddy_barrel_kinetic_transactor.cpp).
static inline soKineticEnergy3f* getEnergy(void* pools, const soIntToType<0>&) {
    return reinterpret_cast<soKineticEnergy3f*>(static_cast<u8*>(pools) + 0x10);
}
static inline soKineticEnergyRotNormal* getEnergy(void* pools, const soIntToType<1>&) {
    return reinterpret_cast<soKineticEnergyRotNormal*>(static_cast<u8*>(pools) + 0x64);
}

static inline const soKineticEnergy3f* getEnergy(const void* pools, const soIntToType<0>&) {
    return reinterpret_cast<const soKineticEnergy3f*>(static_cast<const u8*>(pools) + 0x10);
}
static inline const wnKineticEnergyNormal* getEnergy(const void* pools, const soIntToType<2>&) {
    return reinterpret_cast<const wnKineticEnergyNormal*>(static_cast<const u8*>(pools) + 0xB4);
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

void wnPitIkarusKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soKineticEnergy3f* energy = getEnergy(pools, soIntToType<0>());
    energy->resetEnergy(-1, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
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

void wnPitIkarusKineticTransactor::changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    wnKineticEnergyNormal* energy = const_cast<wnKineticEnergyNormal*>(getEnergy(pools, soIntToType<2>()));
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser) const {
    moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    moduleAccesser->getKineticModule().unableEnergyAll();

    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    Vec3f speed;
    speed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x11000001) * params->getParamFloat(moduleAccesser, 0xFA4, 0);
    speed.m_y = params->getParamFloat(moduleAccesser, 0xFA5, 0);
    float rnd = randf();
    float high = params->getParamFloat(moduleAccesser, 0xFA9, 0);
    float low = params->getParamFloat(moduleAccesser, 0xFA8, 0);
    speed.m_z = low + rnd * (high - low);
    soKineticEnergy3f* energy = const_cast<soKineticEnergy3f*>(getEnergy(pools, soIntToType<0>()));
    energy->m_speed3f = speed;

    float gravity = -params->getParamFloat(moduleAccesser, 0xFA6, 0);
    Vec3f accel;
    accel.m_x = 0.0f;
    accel.m_y = gravity;
    accel.m_z = 0.0f;
    energy->m_accel3f = accel;
    float fall = -params->getParamFloat(moduleAccesser, 0xFA7, 0);
    Vec3f limit;
    limit.m_x = -1.0f;
    limit.m_y = fall;
    limit.m_z = -1.0f;
    energy->m_limit3f = limit;
    energy->enable();
}

