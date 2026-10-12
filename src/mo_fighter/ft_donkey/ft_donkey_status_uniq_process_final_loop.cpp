#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

class ftDonkeyStatusUniqProcessFinalLoop : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinalLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDonkeyStatusUniqProcessFinalLoop::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int handle = moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1B4B), true);
    workManage.setInt(handle, 0x10000042);
    workManage.setInt(-1, 0x2000000A);
    workManage.setFloat(1.0f, 0x21000006);
    workManage.setFloat(1.0f, 0x21000007);
    workManage.setFloat(1.0f, 0x21000008);
    workManage.setInt(0, 0x10000043);
}
void ftDonkeyStatusUniqProcessFinalLoop::execStatus(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessFinalLoop::execFixPosCounter(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessFinalLoop::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x134) {
        static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(0, 0);
        int handle = moduleAccesser->getWorkManageModule().getInt(0x10000042);
        if (handle >= 0) {
            moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
            moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
        }
    }
}

ftDonkeyStatusUniqProcessFinalLoop g_ftDonkeyStatusUniqProcessFinalLoop;
