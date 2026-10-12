// Vec2f's constructor is emitted out of line in the target; include the vector header with inlining off.
#pragma dont_inline on
#include <mt/mt_vector.h>
#pragma dont_inline off
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: float constant from lbl_115_rodata_A8 (value not recovered; referenced by symbol)
extern float lbl_115_rodata_A8;
// Typeinfo references (REL-local data).
extern char lbl_115_data_2760[];
extern char lbl_115_data_7524[];
extern char lbl_115_data_73FC[];
extern char lbl_115_data_7930[];
extern float lbl_115_rodata_AC;
}

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);
void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);

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

// HYPOTHESIS: status module (soModuleEnumeration::m_statusModule); only slot 0x48 is used
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

// HYPOTHESIS: kinetic module view for the sum-speed query (slot 0x3C takes a pointer to a short flag)
class ftDiddyKineticSumStub {
public:
    virtual ~ftDiddyKineticSumStub();
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
    virtual Vec2f unk3C(void* flag);
};

// Out-of-line Vec2f(float, float) constructor: the target calls its own copy (symbol __ct__5Vec2fFff).
extern "C" Vec2f* __ct__5Vec2fFff(Vec2f* out, float x, float y) {
    out->m_x = x;
    out->m_y = y;
    return out;
}

class ftDiddyStatusUniqProcessSpecialBarrelJetDamage : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJetDamage() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().onFlag(0x1200003d);
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    s16 isRef = 1;
    ftDiddyEnergyStub* energy1 = static_cast<ftDiddyEnergyStub*>(__dynamic_cast(
        moduleAccesser->getKineticModule().getEnergy(1), 0, lbl_115_data_7524, lbl_115_data_2760, isRef));
    ftDiddyEnergyStub* energy4 = static_cast<ftDiddyEnergyStub*>(__dynamic_cast(
        moduleAccesser->getKineticModule().getEnergy(4), 0, lbl_115_data_73FC, lbl_115_data_2760, isRef));

    Vec2f sumSpeed;
    int kind = reinterpret_cast<ftDiddyStatusModuleStub*>(moduleAccesser->m_enumerationStart->m_statusModule)->unk48();
    if (kind == 0x127) {
    } else if (kind > 0x127) {
        if (kind < 0x129) {
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
                sumSpeed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
                sumSpeed.m_y = moduleAccesser->getWorkManageModule().getFloat(0x2100000a);
            } else {
                s16 flag = 1;
                sumSpeed = reinterpret_cast<ftDiddyKineticSumStub*>(moduleAccesser->m_enumerationStart->m_kineticModule)->unk3C(&flag);
            }
        }
    } else if (kind >= 0x126) {
        sumSpeed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
        sumSpeed.m_y = moduleAccesser->getWorkManageModule().getFloat(0x2100000a);
    }

    Vec2f speed4;
    energy4->resetEnergy(2, __ct__5Vec2fFff(&speed4, sumSpeed.m_x, sumSpeed.m_y),
                         &Vec3f(lbl_115_rodata_A8, lbl_115_rodata_A8, lbl_115_rodata_A8), moduleAccesser);
    *(reinterpret_cast<u8*>(energy4) + 5) |= 0x80;

    Vec2f speed1(lbl_115_rodata_A8, lbl_115_rodata_A8);
    energy1->resetEnergy(0, &speed1, &Vec3f(lbl_115_rodata_A8, lbl_115_rodata_A8, lbl_115_rodata_A8), moduleAccesser);
    *(reinterpret_cast<u8*>(energy1) + 5) |= 0x80;
}

// MATCH-ONLY: keep the typeinfo flag in one register for both the cast and the sum-speed query.
#pragma opt_propagation off
void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::execStatus(soModuleAccesser* moduleAccesser) {
    int kind = reinterpret_cast<ftDiddyStatusModuleStub*>(moduleAccesser->m_enumerationStart->m_statusModule)->unk48();
    if (kind == 0x127) {
        return;
    }
    if (kind > 0x127) {
        if (kind >= 0x129) {
            return;
        }
    } else {
        return;
    }
    if (!(moduleAccesser->getWorkManageModule().getFloat(0x2100000b) > lbl_115_rodata_A8)) {
        return;
    }
    moduleAccesser->getWorkManageModule().subFloat(lbl_115_rodata_AC, 0x2100000b);
    if (!(moduleAccesser->getWorkManageModule().getFloat(0x2100000b) > lbl_115_rodata_A8)) {
        return;
    }
    s16 flag = 1;
    ftDiddyEnergyStub* energy2 = static_cast<ftDiddyEnergyStub*>(__dynamic_cast(
        moduleAccesser->getKineticModule().getEnergy(2), 0, lbl_115_data_7930, lbl_115_data_2760, flag));
    Vec2f sumSpeed = reinterpret_cast<ftDiddyKineticSumStub*>(moduleAccesser->m_enumerationStart->m_kineticModule)->unk3C(&flag);
    Vec2f speed(sumSpeed.m_x, lbl_115_rodata_A8);
    energy2->resetEnergy(0, &speed, &Vec3f(lbl_115_rodata_A8, lbl_115_rodata_A8, lbl_115_rodata_A8), moduleAccesser);
    *(reinterpret_cast<u8*>(energy2) + 5) |= 0x80;
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
}
#pragma opt_propagation reset

void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::execStop(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftDiddyStatusUniqProcessSpecialBarrelJetDamage g_ftDiddyStatusUniqProcessSpecialBarrelJetDamage;
