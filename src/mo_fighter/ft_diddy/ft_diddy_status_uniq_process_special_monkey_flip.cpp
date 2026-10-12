#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;

extern "C" {
// HYPOTHESIS: float constant from lbl_115_rodata_30 (value not recovered; referenced by symbol)
extern float lbl_115_rodata_30;
// Typeinfo references (REL-local data). lbl_115_data_2760 is the static source type of the dynamic casts.
extern char lbl_115_data_2760[];
extern char lbl_115_data_7524[];
extern char lbl_115_data_7444[];
}

void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);

// HYPOTHESIS: status module (soModuleEnumeration::m_statusModule, +0x70); only slot 0x48 is used
class ftDiddyStatusModuleStub {
public:
    virtual ~ftDiddyStatusModuleStub();
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
    virtual int unk48();
};

// HYPOTHESIS: posture module (soModuleEnumeration::m_postureModule, +0x0C); only slot 0x2C is used
class ftDiddyPostureModuleStub {
public:
    virtual ~ftDiddyPostureModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual float unk2C();
};

// HYPOTHESIS: kinetic energy subclass view; only resetEnergy (slot 0x1C) is used
class ftDiddyEnergyStub {
public:
    virtual ~ftDiddyEnergyStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void resetEnergy(int mode, Vec2f* speed, Vec3f* accel, soModuleAccesser* moduleAccesser);
};

class ftDiddyStatusUniqProcessSpecialMonkeyFlip : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialMonkeyFlip() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// MATCH-ONLY: keep the typeinfo base and the isRef flag in registers across the two casts.
#pragma opt_propagation off
void ftDiddyStatusUniqProcessSpecialMonkeyFlip::initStatus(soModuleAccesser* moduleAccesser) {
    if (reinterpret_cast<ftDiddyStatusModuleStub*>(moduleAccesser->m_enumerationStart->m_statusModule)->unk48() != 0x11b) {
        return;
    }
    float f29 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
    float f30 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0);
    float f31 = reinterpret_cast<ftDiddyPostureModuleStub*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk2C();
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        f29 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
    }
    s16 isRef = 1;
    const char* staticType = lbl_115_data_2760;
    ftDiddyEnergyStub* energy1 = static_cast<ftDiddyEnergyStub*>(__dynamic_cast(
        moduleAccesser->getKineticModule().getEnergy(1), 0, lbl_115_data_7524, staticType, isRef));
    Vec2f speed1(lbl_115_rodata_30, f30);
    Vec3f accel1(lbl_115_rodata_30, lbl_115_rodata_30, lbl_115_rodata_30);
    energy1->resetEnergy(0, &speed1, &accel1, moduleAccesser);
    *(reinterpret_cast<u8*>(energy1) + 5) |= 0x80;

    ftDiddyEnergyStub* energy3 = static_cast<ftDiddyEnergyStub*>(__dynamic_cast(
        moduleAccesser->getKineticModule().getEnergy(3), 0, lbl_115_data_7444, staticType, isRef));
    Vec2f speed2(f29 * f31, lbl_115_rodata_30);
    Vec3f accel2(lbl_115_rodata_30, lbl_115_rodata_30, lbl_115_rodata_30);
    energy3->resetEnergy(6, &speed2, &accel2, moduleAccesser);
    *(reinterpret_cast<u8*>(energy3) + 5) |= 0x80;
}

#pragma opt_propagation reset
void ftDiddyStatusUniqProcessSpecialMonkeyFlip::execStatus(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlip::execStop(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlip::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftDiddyStatusUniqProcessSpecialMonkeyFlip g_ftDiddyStatusUniqProcessSpecialMonkeyFlip;
