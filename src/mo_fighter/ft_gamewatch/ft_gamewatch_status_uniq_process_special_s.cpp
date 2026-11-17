#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <mt/mt_prng.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Judge (side special) draws a number from 1 to 9 that differs from the last two draws.
enum {
    Work_Int_JudgeLastNumber = 0x10000043,
    Work_Int_JudgePrevNumber = 0x10000044,
};

// HYPOTHESIS: extendParam group 2 of the GameWatch data (ftData::extendParam[1]); the air speed divisor sits at +0x0.
struct ftGameWatchSpecialSGroup {
    float m_airSpeedDiv;
};

// HYPOTHESIS: the GameWatch data continues past extendParam; +0xB0 points at the nine Judge number weights.
struct ftGameWatchSpecialSData : public ftData {
    u8 unk94[0x1C];
    const int* m_judgeWeights; // 0xB0
};

class ftGamewatchStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialS();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftGamewatchStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workModule = moduleAccesser->getWorkManageModule();
    ftGameWatchSpecialSData* data =
        static_cast<ftGameWatchSpecialSData*>(g_ftCommonDataAccesser.getData(Fighter_GameWatch));
    ftGameWatchSpecialSGroup* group = static_cast<ftGameWatchSpecialSGroup*>(data->extendParam[1]);
    const int* weights = data->m_judgeWeights;

    int last = workModule.getInt(Work_Int_JudgeLastNumber);
    int prev = workModule.getInt(Work_Int_JudgePrevNumber);
    int candidates[9];
    int thresholds[9];
    int total = 0;
    int i;
    int count;
    for (i = count = 0; i < 9; i++) {
        if (i != last && i != prev) {
            candidates[count] = i;
            total += weights[i];
            thresholds[count] = total;
            count++;
        }
    }

    int roll = randi(total);
    int number;
    for (i = 0; i < 9; i++) {
        if (roll < thresholds[i]) {
            number = candidates[i];
            break;
        }
    }

    workModule.setInt(number, Work_Int_JudgeLastNumber);
    workModule.setInt(last, Work_Int_JudgePrevNumber);
    workModule.setInt(number + 0x1D0, 0x20000000);
    workModule.setInt(number + 0x1D9, 0x20000001);

    if (moduleAccesser->getSituationModule().getKind() == 2) {
        ftKineticEnergyStop& energy = dynamic_cast<ftKineticEnergyStop&>(
            *moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        speed.m_x = speed.m_x / group->m_airSpeedDiv;
        energy.m_speed = speed;
    }
}

void ftGamewatchStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workModule = moduleAccesser->m_enumerationStart->m_workManageModule;
    float damage = workModule->getFloat(0x21000004);
    if (damage != 0.0f) {
        moduleAccesser->m_enumerationStart->m_damageModule->addDamage(damage, 0);
        damage = 0.0f;
    }
    workModule->setFloat(damage, 0x21000004);
}

// BYTES: this unit owns the shared soStatusUniqProcess default virtuals (global in the original), in this order.
bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) { return true; }
void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) { }
bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) { }
bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusUniqProcess::execFixCamera(soModuleAccesser*) { }
void soStatusUniqProcess::execFixPos(soModuleAccesser*) { }
void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) { }
void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) { }
void soStatusUniqProcess::execStop(soModuleAccesser*) { }
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) { }
ftGamewatchStatusUniqProcessSpecialS::~ftGamewatchStatusUniqProcessSpecialS() { }
void soStatusUniqProcess::execStatus(soModuleAccesser*) { }
void soStatusUniqProcess::initStatus(soModuleAccesser*) { }

ftGamewatchStatusUniqProcessSpecialS g_ftGamewatchStatusUniqProcessSpecialS;
