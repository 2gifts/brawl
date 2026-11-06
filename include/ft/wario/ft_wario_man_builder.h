#pragma once

// Builder view of ftWarioMan for ft_wario_man.cpp only (ft_wario.h keeps the Fighter-based declaration for the
// status units, which cannot coexist with the builder headers).
#define FT_WARIO_MAN_REAL_ACCESSER
#include <ft/wario/ft_wario_builder.h>

// Wario-Man shares Wario's module set, with the stock kinetic module and a smaller article builder (the bike only).
// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnWarioBike pool), not reconstructed yet.
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x2B1C, Fighter_WarioMan> ftWarioManGenerateArticleManageModuleBuilder;

class ftWarioManBuildConfig : public ftWarioBuildConfig {
public:
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl> KineticModuleBuildConfig;
    typedef ftWarioManGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

static_assert(sizeof(ftFighterBuilder<ftWarioManBuildConfig>) == 0xB23C, "Wario-Man builder extent");
class ftWarioMan : public ftFighterBuilder<ftWarioManBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable; // +0xB23C
public:
    ftWarioManDataView* m_extendParam; // 0xB24C
    soAnimObj m_animObj;                          // 0xB250 the Waft gauge animation

    ftWarioMan(s32 entryId,
               Heaps::HeapType instHeap,
               Heaps::HeapType nwModelInstHeap,
               Heaps::HeapType nwMotionInstHeap);
    ftWarioExtendParam* getExtendParam();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onStart(int startKind);
    virtual void toDead(Dead::Reason deadReason);
    virtual void onDeadEnd();
    virtual u32 getChangeSucceedOption() { return 0x7FAD; }
    virtual void notifyEventCollisionSearch(soCollisionLog*, soModuleAccesser*);
    virtual void notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void change(void* unk1, int unk2);
    virtual void setupChangeSucceedWork(void* changeSucceedWork);
    virtual void changeSucceedCore(void* changeSucceedWork, int unk2);
    virtual void processUpdate();
    virtual void processFixPosition();
};
static_assert(sizeof(ftWarioMan) == 0xB278, "Wario-Man allocation size is wrong!");
