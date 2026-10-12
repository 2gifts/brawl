#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_vector.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: float constant from lbl_92_rodata_10 (value not recovered; referenced by symbol)
extern float lbl_92_rodata_10;
// Typeinfo references (REL-local data).
extern char lbl_92_data_639C[];
extern char lbl_92_data_62B8[];
extern char lbl_92_data_1874[];
// Vec2f copy assignment and constructor (out of line in the target).
Vec2f* __as__5Vec2fFRC5Vec2f(Vec2f* self, const Vec2f* other);
}
static Vec2f* ftDonkeyVec2fCt(Vec2f* out, float x, float y) {
    out->m_x = x;
    out->m_y = y;
    return out;
}

void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);

// HYPOTHESIS: situation module (soModuleEnumeration::m_situationModule, +0x14); only slot 0x14 is used
class ftDonkeySituationModuleStub {
public:
    virtual ~ftDonkeySituationModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual int unk14();
};

// HYPOTHESIS: kinetic module view; slots 0x20 (getEnergy), 0x3C (sum speed query by flag pointer), 0x5C are used
class ftDonkeyKineticStub {
public:
    virtual ~ftDonkeyKineticStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void* getEnergy(int index);
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual Vec2f unk3C(void* flag);
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C(int mode, soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: energy fields reached through raw offsets (names not recovered)
static inline float* energyFloatAt(void* energy, u32 offset) {
    return reinterpret_cast<float*>(static_cast<u8*>(energy) + offset);
}
static inline Vec2f* energyVec2fAt(void* energy, u32 offset) {
    return reinterpret_cast<Vec2f*>(static_cast<u8*>(energy) + offset);
}
static inline void setEnergyFlagByte5(void* energy, u8 mask) {
    *(static_cast<u8*>(energy) + 5) = *(static_cast<u8*>(energy) + 5) & mask;
}

class ftDonkeyStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    ftDonkeySituationModuleStub* situation = reinterpret_cast<ftDonkeySituationModuleStub*>(moduleAccesser->m_enumerationStart->m_situationModule);
    ftDonkeyKineticStub* kinetic = reinterpret_cast<ftDonkeyKineticStub*>(moduleAccesser->m_enumerationStart->m_kineticModule);
    float* param = *reinterpret_cast<float**>(reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(1))) + 0x80);
    s16 isRef = 1;
    void* energy639C = __dynamic_cast(kinetic->getEnergy(1), 0, lbl_92_data_639C, lbl_92_data_1874, isRef);
    void* energy62B8 = __dynamic_cast(kinetic->getEnergy(3), 0, lbl_92_data_62B8, lbl_92_data_1874, isRef);
    s16 minusOne = -1;
    Vec2f sumSpeed = kinetic->unk3C(&minusOne);
    if (situation->unk14() != 0) {
        kinetic->unk5C(0xa, moduleAccesser);
        sumSpeed.m_x /= param[0];
        sumSpeed.m_y = lbl_92_rodata_10;
        __as__5Vec2fFRC5Vec2f(energyVec2fAt(energy62B8, 0x8), &sumSpeed);
        Vec2f tmp;
        ftDonkeyVec2fCt(&tmp, param[1], lbl_92_rodata_10);
        __as__5Vec2fFRC5Vec2f(energyVec2fAt(energy62B8, 0x20), &tmp);
        *energyFloatAt(energy639C, 0xc) = lbl_92_rodata_10;
        *energyFloatAt(energy639C, 0x10) = -param[2];
        setEnergyFlagByte5(energy639C, ~0x40);
    } else {
        kinetic->unk5C(6, moduleAccesser);
    }
}

void ftDonkeyStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    ftDonkeySituationModuleStub* situation = reinterpret_cast<ftDonkeySituationModuleStub*>(moduleAccesser->m_enumerationStart->m_situationModule);
    ftDonkeyKineticStub* kinetic = reinterpret_cast<ftDonkeyKineticStub*>(moduleAccesser->m_enumerationStart->m_kineticModule);
    float* param = *reinterpret_cast<float**>(reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(1))) + 0x80);
    s16 isRef = 1;
    void* energy639C = __dynamic_cast(kinetic->getEnergy(1), 0, lbl_92_data_639C, lbl_92_data_1874, isRef);
    void* energy62B8 = __dynamic_cast(kinetic->getEnergy(3), 0, lbl_92_data_62B8, lbl_92_data_1874, isRef);
    if (situation->unk14() != 0) {
        kinetic->unk5C(0xa, moduleAccesser);
        Vec2f tmp;
        ftDonkeyVec2fCt(&tmp, param[1], lbl_92_rodata_10);
        __as__5Vec2fFRC5Vec2f(energyVec2fAt(energy62B8, 0x20), &tmp);
        *energyFloatAt(energy639C, 0x10) = -param[2];
        if (!workManage.isFlag(0x22000011)) {
            setEnergyFlagByte5(energy639C, ~0x40);
        }
    } else {
        kinetic->unk5C(6, moduleAccesser);
    }
}

ftDonkeyStatusUniqProcessSpecialS g_ftDonkeyStatusUniqProcessSpecialS;
