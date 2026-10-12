#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <math.h>

// HYPOTHESIS: posture module (soModuleEnumeration::m_postureModule); only the slots below are used.
class ftPostureModuleStub2 {
public:
    virtual ~ftPostureModuleStub2();
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
    virtual void unk44(Vec3f*, int);
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
    virtual void unk84(float);
};

// HYPOTHESIS: generate-article manage module (soModuleEnumeration::m_generateArticleManageModule); only the slots below are used.
class ftGenerateArticleStub {
public:
    virtual ~ftGenerateArticleStub();
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
    virtual void unk44(int, int);
};

// HYPOTHESIS: extendParam group 3 of the GameWatch data (ftData::extendParam[2]); floats at +0x0, +0x4 and +0x8.
struct ftGameWatchSpecialHiStartGroup {
    float m_0;
    float m_4;
    float m_8;
};

// HYPOTHESIS: lean table from lbl_107_rodata_48 (values not verified).
static const float s_hiStartLean[3] = { 1.0f, 2.0f, 3.0f };
// HYPOTHESIS: constant from lbl_107_rodata_50 (value not verified).
static const float s_hiStartVec = 4.0f;

class ftGamewatchStatusUniqProcessSpecialHiStart : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialHiStart() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGamewatchStatusUniqProcessSpecialHiStart g_ftGamewatchStatusUniqProcessSpecialHiStart;

void ftGamewatchStatusUniqProcessSpecialHiStart::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    ftGameWatchSpecialHiStartGroup* group = static_cast<ftGameWatchSpecialHiStartGroup*>(
        g_ftCommonDataAccesser.getData(Fighter_GameWatch)->extendParam[2]);
    if (!workManage.isFlag(0x22000011)) {
        float stick = moduleAccesser->getControllerModule().getStickX();
        if (fabs(stick) >= group->m_0) {
            float magnitude = fabs(stick) - group->m_0;
            float lean = (stick > s_hiStartLean[2]) ? s_hiStartLean[0] : s_hiStartLean[1];
            float speed = magnitude * lean * group->m_4;
            Vec3f offset(speed, s_hiStartLean[2], s_hiStartLean[2]);
            reinterpret_cast<ftPostureModuleStub2*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk44(&offset, 0);
            reinterpret_cast<ftPostureModuleStub2*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk84(s_hiStartLean[2]);
            reinterpret_cast<ftPostureModuleStub2*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk54();
            dynamic_cast<soKineticEnergyNormal*>(moduleAccesser->getKineticModule().getEnergy(0))->m_accel.m_y = s_hiStartVec * speed;
            workManage.onFlag(0x22000011);
        }
    }
}

void ftGamewatchStatusUniqProcessSpecialHiStart::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (kind == 0x19) {
        ftGameWatchSpecialHiStartGroup* group = static_cast<ftGameWatchSpecialHiStartGroup*>(
            g_ftCommonDataAccesser.getData(Fighter_GameWatch)->extendParam[2]);
        moduleAccesser->getWorkManageModule().setFloat(group->m_8, 0x11000000);
    }
    if (kind != 0x117) {
        reinterpret_cast<ftGenerateArticleStub*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->unk44(2, 0);
    }
    Vec3f offset(s_hiStartVec, s_hiStartVec, s_hiStartVec);
    reinterpret_cast<ftPostureModuleStub2*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk44(&offset, 0);
}
