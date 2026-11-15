#include <StaticAssert.h>
#include <ft/donkey/ft_donkey_transactor.h>
#include <ft/ft_entry.h>
#include <so/stageobject.h>
#include <so/so_module_accesser.h>

ftDonkeyTransactor::ftDonkeyTransactor() { }
ftDonkeyTransactor::~ftDonkeyTransactor() { }

void ftDonkeyTransactor::init(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
}

void ftDonkeyTransactor::exit(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003d)) {
        int handle = moduleAccesser->getWorkManageModule().getInt(0x10000041);
        moduleAccesser->getEffectModule().remove(handle);
    }
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
}

// Starts the full-charge glow when the charge reaches the maximum (Kirby's copy uses its own effect and node) and
// removes it when the charge drops again.
void ftDonkeyTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int chargeMax = moduleAccesser->getConstantIntKirby(0x5dc0);
    int charge = work.getInt(0x10000040);
    bool glowing = work.isFlag(0x1200003d);
    if (charge >= chargeMax && !glowing) {
        int handle;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            Vec3f offset(0.0f, 8.0f, 0.0f);
            Vec3f rot(0.0f, 0.0f, 0.0f);
            handle = moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x1060003), 0x190, &offset, &rot, 1.0f, false, 0, 0, -1);
        } else {
            Vec3f offset(0.0f, 0.0f, 0.0f);
            Vec3f rot(0.0f, 0.0f, 0.0f);
            handle = moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x20004), 0x22, &offset, &rot, 1.0f, false, 0, 0, -1);
        }
        work.setInt(handle, 0x10000041);
        work.onFlag(0x1200003d);
        moduleAccesser->getEffectModule().reqCommon(0x1a, 0.0f);
    }
    if (charge < chargeMax && glowing) {
        int handle = work.getInt(0x10000041);
        moduleAccesser->getEffectModule().remove(handle);
        work.setInt(0, 0x10000041);
        work.offFlag(0x1200003d);
        moduleAccesser->getEffectModule().removeCommon(0x1a);
    }
}
