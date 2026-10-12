#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessKirbyFinalDamageFly : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKirbyFinalDamageFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessKirbyFinalDamageFly g_ftStatusUniqProcessKirbyFinalDamageFly;

void ftStatusUniqProcessKirbyFinalDamageFly::initStatus(soModuleAccesser* moduleAccesser) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(pos.m_x, 0x21000004);
    moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(pos.m_y, 0x21000005);
    moduleAccesser->m_enumerationStart->m_workManageModule->setInt(0x29, 0x20000000);
    moduleAccesser->m_enumerationStart->m_workManageModule->onFlag(0x12000001);
}
void ftStatusUniqProcessKirbyFinalDamageFly::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessKirbyFinalDamageFly::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageFly::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessKirbyFinalDamageFly::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
