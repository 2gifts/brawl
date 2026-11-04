#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftGamewatchStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftGamewatchStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {}
void ftGamewatchStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workModule = moduleAccesser->m_enumerationStart->m_workManageModule;
    float damage = workModule->getFloat(0x21000004);
    if (damage != 0.0f) {
        moduleAccesser->m_enumerationStart->m_damageModule->addDamage(damage, 0);
        damage = 0.0f;
    }
    workModule->setFloat(damage, 0x21000004);
}

ftGamewatchStatusUniqProcessSpecialS g_ftGamewatchStatusUniqProcessSpecialS;
