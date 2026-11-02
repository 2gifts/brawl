#pragma once

#include <so/status/so_status_module_impl.h>

class Fighter;
class soCollisionLog;

class ftMetaknightStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalEnd() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    void notifyEventCollisionAttackCheck(soModuleAccesser* moduleAccesser);

    // Hits the linked target with attack slot index.
    void attackFinal(Fighter* target, int index, soModuleAccesser* moduleAccesser);
    // The fighter collision callback forwards its damage-power argument here.
    void notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    // Finds the first link slot in 6..11 that is not linked; -1 when all are in use.
    int getFreeFinalLinkWork(soModuleAccesser* moduleAccesser);
};

extern ftMetaknightStatusUniqProcessFinalEnd g_ftMetaknightStatusUniqProcessFinalEnd;
