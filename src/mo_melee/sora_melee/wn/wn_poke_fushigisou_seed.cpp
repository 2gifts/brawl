#include <wn/poke/wn_poke_fushigisou_seed.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>

// Registers the Pokemon seed article class (kind 74) in the class info table during static initialization.
wnClassInfoImpl<74, wnPokeFushigisouSeed, wnPokeFushigisouSeedData> g_wnPokeFushigisouSeedClassInfo;

void wnPokeFushigisouSeed::onDeactivate() { }

// A hit on a target bursts the seed (status 2).
bool wnPokeFushigisouSeed::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPokeFushigisouSeed::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 2: {
        float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA0, 0);
        if (randf() >= threshold) {
            Vec3f position = moduleAccesser->getPostureModule().getPos();
            if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
                moduleAccesser->getEffectModule().req(ef_ptc_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
            } else {
                moduleAccesser->getEffectModule().req(ef_ptc_kb_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
            }
        }
        break;
    }
    case 1: {
        Vec3f position = moduleAccesser->getPostureModule().getPos();
        if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
            moduleAccesser->getEffectModule().req(ef_ptc_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        } else {
            moduleAccesser->getEffectModule().req(ef_ptc_kb_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        }
        break;
    }
    }
}
