#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule); only the slots below are used.
class ftLinkModuleStub {
public:
    virtual ~ftLinkModuleStub();
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

// HYPOTHESIS: catch module (soModuleEnumeration::m_catchModule); only the slots below are used.
class ftCatchModuleStub {
public:
    virtual ~ftCatchModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C(int, int);
};

// HYPOTHESIS: output block passed to the link module; fields at +0x14 and +0x18 are written back by the link call.
struct ftDonkeyThrowLinkParam {
    int m_08;
    u8 m_0C;
    int m_10;
    int m_14;
    int m_18;
    int m_1C;
    float m_20;
    u8 m_24;
};

class ftDonkeyStatusUniqProcessThrowCommon : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessThrowCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDonkeyStatusUniqProcessThrowCommon g_ftDonkeyStatusUniqProcessThrowCommon;

// HYPOTHESIS: constant from lbl_92_rodata_A0 (value not verified).
static const float s_throwCommonScale = 1.0f;

void ftDonkeyStatusUniqProcessThrowCommon::initStatus(soModuleAccesser* moduleAccesser) {
    int base = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    ftDonkeyThrowLinkParam param;
    param.m_08 = 0x1c;
    param.m_0C = 0;
    param.m_10 = base - 0x39;
    param.m_14 = -1;
    param.m_18 = -1;
    param.m_1C = -1;
    param.m_20 = s_throwCommonScale;
    param.m_24 = 1;
    reinterpret_cast<ftLinkModuleStub*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk48(0, &param, 0);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int linkValue = reinterpret_cast<ftLinkModuleStub*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk54(0);
    workManage.setInt(linkValue, 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(param.m_14, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(param.m_18, 0x20000004);
}

void ftDonkeyStatusUniqProcessThrowCommon::exitStatus(soModuleAccesser* moduleAccesser, int) {
    int zero = 0;
    reinterpret_cast<ftCatchModuleStub*>(moduleAccesser->m_enumerationStart->m_catchModule)->unk2C(zero, zero);
}
