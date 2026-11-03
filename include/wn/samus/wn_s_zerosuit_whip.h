#pragma once

#include <wn/weapon.h>

class wnSZerosuitWhip : public Weapon {
public:
    virtual void deactivate(bool);
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    virtual void processFixPosition();
    virtual bool notifyEventCollisionAttackCheck(u32);
};
