#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <so/so_module_hyp.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <types.h>

class soModuleAccesser;
class acAnimCmd;

// HYPOTHESIS: parameter lists of the virtuals this unit does not define yet are unknown and left empty.
class soTerritoryModule : public soNullable {
public:
    virtual ~soTerritoryModule();
    virtual void createAreaDataSetAll();
    virtual void initAreaInstance();
    virtual void activate();
    virtual void createTerritory();
    virtual void setEnable();
    virtual bool isTouch();
    virtual bool isOutSide();
    virtual soHyp::soTerritoryRect* getRect();
    virtual void setRect();
};

class soTerritoryModuleImpl : public soTerritoryModule, public soAnimCmdEventObserver {
    char _0x14[4];
    soHyp::soTerritoryRect m_rect;
    char _0x28[0x14];
    int m_0x3c;

public:
    virtual ~soTerritoryModuleImpl();
    virtual void createAreaDataSetAll();
    virtual void initAreaInstance();
    virtual void activate();
    virtual void createTerritory();
    virtual void setEnable();
    virtual bool isTouch();
    virtual bool isOutSide();
    virtual soHyp::soTerritoryRect* getRect();
    virtual void setRect();

    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
