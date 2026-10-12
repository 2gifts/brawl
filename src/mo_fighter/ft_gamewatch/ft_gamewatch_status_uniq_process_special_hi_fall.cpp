#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: extendParam group 3 of the GameWatch data (ftData::extendParam[2]); the speed value sits at +0xC.
struct ftGameWatchSpecialHiFallGroup {
    u8 unk0[0xC];
    float m_value;
};

class ftGamewatchStatusUniqProcessSpecialHiFall : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialHiFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGamewatchStatusUniqProcessSpecialHiFall g_ftGamewatchStatusUniqProcessSpecialHiFall;

void ftGamewatchStatusUniqProcessSpecialHiFall::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftGameWatchSpecialHiFallGroup* group = static_cast<ftGameWatchSpecialHiFallGroup*>(
        g_ftCommonDataAccesser.getData(Fighter_GameWatch)->extendParam[2]);
    kinetic.changeKinetic(0, moduleAccesser);
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(1));
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xbd0, 0);
    speed *= group->m_value;
    energy.m_speedTarget.m_y = speed;
    energy.m_accel.m_y = speed;
}

// HYPOTHESIS: 0x118 is the status kind that continues the special without removing article 2.
void ftGamewatchStatusUniqProcessSpecialHiFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x118) {
        return;
    }
    static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)
        ->remove(2, 0);
}
