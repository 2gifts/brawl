#include <so/catch/so_catch_module.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule); only the slots below are used.
class ftLucarioLinkModuleStub {
public:
    virtual ~ftLucarioLinkModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48(int, void*, int);
    virtual void unk4C();
    virtual void unk50();
    virtual int unk54(int);
};

// HYPOTHESIS: output block passed to the link module when the throw starts; layout matches the donkey throw block.
struct ftLucarioThrowLinkParam {
    int m_08;
    u8 m_0C;
    int m_10;
    int m_14;
    int m_18;
    int m_1C;
    float m_20;
    u8 m_24;
};

class ftLucarioStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftLucarioStatusUniqProcessSpecialSThrow : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialSThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

// MATCH-ONLY: soCatchModule in the shared header stops at catchCut; the target also calls a later
// virtual (vtable slot 15) that is not declared there. This local view extends the same layout.
class ftCatchModuleView {
public:
    virtual ~ftCatchModuleView() { }
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();
    virtual void setCatch(int taskId);
    virtual bool isCatch();
    virtual void onCatchFlag();
    virtual void offCatchFlag();
    virtual void catchCut(bool, bool);
    virtual void unk9();
    virtual void unk10();
    virtual void unk11();
    virtual bool unk12();
};

static ftCatchModuleView* getCatchView(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

void ftLucarioStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) {}
void ftLucarioStatusUniqProcessSpecialS::execStatus(soModuleAccesser*) {}
void ftLucarioStatusUniqProcessSpecialS::exitStatus(soModuleAccesser*, int) {}

void ftLucarioStatusUniqProcessSpecialSThrow::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    // HYPOTHESIS: the catch module is typed void* in the accessor; cast it to soCatchModule.
    soCatchModule* catchModule = static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule);
    catchModule->setCatch(-1);
}

// Throw link scale (target data value 1.0).
static const float s_throwLinkScale = 1.0f;

// Starts the throw once neither the release flag nor the thrown flag is set: sends the throw block to the link module.
void ftLucarioStatusUniqProcessSpecialSThrow::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    bool releaseFlag = workManage.isFlag(0x22000011);
    bool thrownFlag = workManage.isFlag(0x22000010);
    if (!releaseFlag && thrownFlag) {
        ftLucarioThrowLinkParam param;
        param.m_08 = 0xb;
        param.m_0C = 0;
        param.m_10 = 0x77;
        param.m_14 = -1;
        param.m_18 = -1;
        param.m_1C = -1;
        param.m_20 = s_throwLinkScale;
        param.m_24 = 1;
        reinterpret_cast<ftLucarioLinkModuleStub*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk48(0, &param, 0);
        const soModuleEnumeration* enumeration = moduleAccesser->m_enumerationStart;
        soWorkManageModule& throwWork = *enumeration->m_workManageModule;
        int linkValue = reinterpret_cast<ftLucarioLinkModuleStub*>(enumeration->m_linkModule)->unk54(0);
        throwWork.setInt(linkValue, 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(param.m_14, 0x20000003);
        moduleAccesser->getWorkManageModule().setInt(param.m_18, 0x20000004);
        moduleAccesser->getWorkManageModule().setFloat(param.m_20, 0x21000004);
        workManage.onFlag(0x22000011);
    }
}

void ftLucarioStatusUniqProcessSpecialSThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soCatchModule* catchModule = static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule);
    catchModule->catchCut(false, false);
}

bool ftLucarioStatusUniqProcessSpecialSThrow::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        return false;
    }
    return getCatchView(moduleAccesser)->unk12();
}

ftLucarioStatusUniqProcessSpecialS g_ftLucarioStatusUniqProcessSpecialS;
ftLucarioStatusUniqProcessSpecialSThrow g_ftLucarioStatusUniqProcessSpecialSThrow;
