#pragma once

#include <ft/ft_status_uniq_process_shoot.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Shoot statuses of the held shooting items. The work int 0x20000000 holds the item kind (0xD cracker launcher,
// 0x18 fire flower, 0x2C ray gun, 0x3D super scope) and selects the per-item routines, which live in one source file each.
class ftStatusUniqProcessItemShoot : public ftStatusUniqProcessShoot {
public:
    virtual ~ftStatusUniqProcessItemShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void attachHoldAnim(soModuleAccesser* moduleAccesser, int holdKind);
    void detachHoldAnim(soModuleAccesser* moduleAccesser);

    void initStatus_SScope(soModuleAccesser* moduleAccesser);
    void execStatus_SScope(soModuleAccesser* moduleAccesser);
    void execFixPosCounter_SScope(soModuleAccesser* moduleAccesser);
    void execFixPos_SScope(soModuleAccesser* moduleAccesser);
    void exit_SScope(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void toHoldFire_SScope(soModuleAccesser* moduleAccesser);
    void toRapid_SScope(soModuleAccesser* moduleAccesser);
    void toEnd_SScope(soModuleAccesser* moduleAccesser);

    void initStatus_LGun(soModuleAccesser* moduleAccesser);
    void execStatus_LGun(soModuleAccesser* moduleAccesser);
    void execFixPosCounter_LGun(soModuleAccesser* moduleAccesser);
    void execFixPos_LGun(soModuleAccesser* moduleAccesser);
    void exit_LGun(soModuleAccesser* moduleAccesser, int nextStatusKind);

    void initStatus_FFlower(soModuleAccesser* moduleAccesser);
    void execStatus_FFlower(soModuleAccesser* moduleAccesser);
    void execFixPosCounter_FFlower(soModuleAccesser* moduleAccesser);
    void execFixPos_FFlower(soModuleAccesser* moduleAccesser);
    void exit_FFlower(soModuleAccesser* moduleAccesser, int nextStatusKind);

    void initStatus_CLauncher(soModuleAccesser* moduleAccesser);
    void execStatus_CLauncher(soModuleAccesser* moduleAccesser);
    void execFixPosCounter_CLauncher(soModuleAccesser* moduleAccesser);
    void execFixPos_CLauncher(soModuleAccesser* moduleAccesser);
    void exit_CLauncher(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void setHoldAngle(soModuleAccesser* moduleAccesser, float delta);
};
