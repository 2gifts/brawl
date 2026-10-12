#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule, +0x54); only slot 0x48 is used
class ftDonkeyLinkModuleStub {
public:
    virtual ~ftDonkeyLinkModuleStub();
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
};

// HYPOTHESIS: capture-side module (soModuleEnumeration +0x40); only slot 0x68 is used
class ftDonkeyCaptureModuleStub {
public:
    virtual ~ftDonkeyCaptureModuleStub();
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
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68(int);
};

// HYPOTHESIS: collision hit module (soModuleEnumeration +0x20); only slot 0x80 is used
class ftDonkeyCollisionHitStub {
public:
    virtual ~ftDonkeyCollisionHitStub();
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
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual void unk70();
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80(int, int, int);
};

class ftDonkeyStatusUniqProcessShoulderStart : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessShoulderStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessShoulderStart::initStatus(soModuleAccesser* moduleAccesser) {
    struct {
        int kind;
        u8 unk4;
    } param;
    param.kind = 0x1b;
    param.unk4 = 0;
    reinterpret_cast<ftDonkeyLinkModuleStub*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk48(0, &param, 0);
    reinterpret_cast<ftDonkeyCaptureModuleStub*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk68(1);
    ftDonkeyCollisionHitStub* hit = reinterpret_cast<ftDonkeyCollisionHitStub*>(moduleAccesser->m_enumerationStart->m_collisionHitModule);
    int value = soValueAccesser::getConstantInt(moduleAccesser, 0x5a2e, 0);
    hit->unk80(value, 0, 0);
}

ftDonkeyStatusUniqProcessShoulderStart g_ftDonkeyStatusUniqProcessShoulderStart;
