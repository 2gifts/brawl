#pragma once

// The territory module of the enemies (mo_enemy/sora_enemy/territory/em_territory_module_impl.cpp): the rectangle an
// enemy keeps to, and the area data sets (one per territory side) the enemy's area module checks. soTerritoryModule
// and soTerritoryModuleImpl are classes of sora_melee; they are recovered here from the RTTI and vtable of the enemy
// version and from the inline accessors the unit has weak copies of.

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_module_hyp.h>
#include <so/so_null.h>
#include <types.h>

class acAnimCmd;
class soModuleAccesser;

// HYPOTHESIS: the module interface. The slots are counted from the first virtual function after soNullable's
// (vtable offset 0x10, ...); their code is in sora_melee and nothing in sora_enemy tells what they do (Enemy::init
// calls unk10 right after setRect).
class soTerritoryModule : public soNullable {
public:
    virtual ~soTerritoryModule() { }
    virtual void unk10() = 0;
    virtual void unk14(int area) = 0;
    virtual void unk18() = 0;
    virtual void unk1C() = 0;
    virtual void unk20() = 0;
    virtual bool unk24(int area) = 0;
    virtual bool unk28(int area) = 0;
    virtual soHyp::soTerritoryRect* getRect() = 0;
    virtual void setRect(soHyp::soTerritoryRect* rect) = 0;
    virtual bool checkArea(u32 area) = 0;
    virtual bool isDoCreate() = 0;
};

// HYPOTHESIS: the constructor arguments are the ones the Enemy constructor passes emTerritoryModuleImpl (see there).
class soTerritoryModuleImpl : public soTerritoryModule, public soAnimCmdEventObserver {
public:
    soModuleAccesser* m_moduleAccesser; // 0x14
    soHyp::soTerritoryRect m_rect;      // 0x18
    u8 m_unk28[0x50 - 0x28];

    soTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5, int p6, int p7,
                          int p8);
    virtual ~soTerritoryModuleImpl();
    virtual void unk10();
    virtual void unk14(int area);
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual bool unk24(int area);
    virtual bool unk28(int area);
    virtual soHyp::soTerritoryRect* getRect() { return &m_rect; }
    virtual void setRect(soHyp::soTerritoryRect* rect) {
        m_rect.m_left = rect->m_left;
        m_rect.m_right = rect->m_right;
        m_rect.m_top = rect->m_top;
        m_rect.m_bottom = rect->m_bottom;
    }
    virtual bool checkArea(u32 area);
    virtual bool isDoCreate();
    // Fills the area data set of the area index (the four floats are its rectangle sides).
    virtual void createAreaDataSet(int area, float left, float right, float top, float bottom);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
static_assert(sizeof(soTerritoryModuleImpl) == 0x50, "Class is wrong size!");

// The enemy territory module (soModuleAccesser slot territoryModule).
class emTerritoryModuleImpl : public soTerritoryModuleImpl {
public:
    emTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5, int p6, int p7,
                          int p8);
    virtual ~emTerritoryModuleImpl();
    virtual bool checkArea(u32 area);
    virtual bool isDoCreate();
    virtual void createAreaDataSet(int area, float left, float right, float top, float bottom);
};
static_assert(sizeof(emTerritoryModuleImpl) == 0x50, "Class is wrong size!");
