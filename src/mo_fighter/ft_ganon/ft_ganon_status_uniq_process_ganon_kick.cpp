#include <ft/ft_kinetic_energy.h>
#include <ft/ganon/ft_ganon_status_uniq_process_ganon_kick.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: this translation unit holds Ganondorf's Wizard's Foot status processes. Only Kick::onHit is
// reconstructed; the KickEnd bodies are not (their work and parameter ids are listed in the notes).

// Group 4 of the fighter's extend parameters (ftData + 0x88); defined in ft_ganon.cpp.
extern "C" void* fn_109_862C();

void ftGanonStatusUniqProcessGanonKick::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonKick::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonKickEnd::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonKickEnd::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonKickEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

void ftGanonStatusUniqProcessGanonKick::onHit(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    // HYPOTHESIS: work int 0x20000000 is 0 for the ground kick, 0x20000001 counts the hits that still slow the kick.
    int variant = work.getInt(0x20000000);
    moduleAccesser->getSituationModule().getKind();
    switch (status.getStatusKind()) {
    case 0x115:
        if (variant == 0) {
            int hitsLeft = work.getInt(0x20000001);
            if (hitsLeft >= 1) {
                // HYPOTHESIS: the first float of group 4 is the speed multiplier applied per hit.
                float* param = static_cast<float*>(fn_109_862C());
                // HYPOTHESIS: ftKineticEnergyMotion::unk40 is a speed multiplier (shared with other fighters).
                float current = work.getFloat(0x21000004);
                float speedMul = current * param[0];
                dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0)).unk40 = speedMul;
                work.setFloat(speedMul, 0x21000004);
                work.setInt(hitsLeft - 1, 0x20000001);
            }
        }
        break;
    case 0x11e:
        break;
    }
}

ftGanonStatusUniqProcessGanonKick g_ftGanonStatusUniqProcessGanonKick;
ftGanonStatusUniqProcessGanonKickEnd g_ftGanonStatusUniqProcessGanonKickEnd;
