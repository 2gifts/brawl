#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/so_value_accesser.h>
#include <so/damage/so_damage_util_actor.h>
#include <types.h>

extern "C" {
// Target constants verified from ft_donkey.o: lbl_92_rodata_88 = 90.0f; lbl_92_rodata_8C = 0.0f.
}

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule, +0x54); slots 0x30 and 0x54 are used
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
    virtual bool unk30(int);
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual u32 unk54(int);
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

// HYPOTHESIS: item manage module (soModuleEnumeration::m_itemManageModule, +0xa8); slots 0x40/0x84 are used
class ftItemManageModuleStub {
public:
    virtual ~ftItemManageModuleStub();
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
    virtual bool unk40(int);
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
    virtual void unk80();
    virtual void unk84(float, float, int);
};

// HYPOTHESIS: view of the hit record passed to checkDamage; damage at +0x0C, task id at +0x98
struct ftDonkeyHitRecord {
    u8 unk00[0x0C];
    float damage;
    u8 unk10[0x98 - 0x10];
    u32 taskId;
};

class ftDonkeyStatusUniqProcessShoulderCommon : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessShoulderCommon() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
};

void ftDonkeyStatusUniqProcessShoulderCommon::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int value = -1;
    switch (nextStatus) {
    case 0x12a:
        value = 0x1e0;
        break;
    case 0x12b:
        value = 0x1e1;
        break;
    case 0x12c:
        value = 0x1e2;
        break;
    case 0x12d:
        value = 0x1e3;
        break;
    default:
        if (nextStatus >= 0x120 && nextStatus < 0x12a) {
            return;
        }
        reinterpret_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
        break;
    }
    if (value != -1) {
        moduleAccesser->getWorkManageModule().setInt(value, 0x20000000);
    }
    reinterpret_cast<ftDonkeyCaptureModuleStub*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk68(0);
}

bool ftDonkeyStatusUniqProcessShoulderCommon::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    ftDonkeyHitRecord* hit = static_cast<ftDonkeyHitRecord*>(data);
    if (!reinterpret_cast<ftDonkeyLinkModuleStub*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk30(0)) {
        return false;
    }
    if (hit->taskId == reinterpret_cast<ftDonkeyLinkModuleStub*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk54(0)) {
        return false;
    }
    float damage = soValueAccesser::getConstantFloat(moduleAccesser, 0x7d2, 0) * hit->damage;
    if (soDamageUtilActor::getDamageLevel(moduleAccesser, damage) < 3) {
        return true;
    }
    if (reinterpret_cast<ftItemManageModuleStub*>(moduleAccesser->m_enumerationStart->m_itemManageModule)->unk40(0)) {
        // Target constants verified from ft_donkey.o: lbl_92_rodata_88 = 90.0f; lbl_92_rodata_8C = 0.0f.
        reinterpret_cast<ftItemManageModuleStub*>(moduleAccesser->m_enumerationStart->m_itemManageModule)->unk84(90.0f, 0.0f, 0);
    }
    return false;
}

ftDonkeyStatusUniqProcessShoulderCommon g_ftDonkeyStatusUniqProcessShoulderCommon;
