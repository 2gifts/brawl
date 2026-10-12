#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftToonLinkStatusUniqProcessSpecialBoomerang : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialBoomerang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Common parameter block read by the initial throw check. Offsets are native; field meanings are HYPOTHESIS.
struct ftToonLinkBoomerangCommonView {
    u8 unk00[0x18];
    float m_stickThreshold; // 0x18: the stick must stay below this magnitude
    int m_flickFrame;       // 0x1c
    float m_flickFrameAdd;  // 0x20
};

void ftToonLinkStatusUniqProcessSpecialBoomerang::initStatus(soModuleAccesser* moduleAccesser) {
    float stickX = __fabsf(moduleAccesser->getControllerModule().getStickX());
    ftToonLinkBoomerangCommonView* common = reinterpret_cast<ftToonLinkBoomerangCommonView*>(g_ftCommonDataAccesser.getParamCommon());
    if (stickX >= common->m_stickThreshold) {
        return;
    }
    soControllerModule& controller = moduleAccesser->getControllerModule();
    float limit = (float)common->m_flickFrame + common->m_flickFrameAdd;
    // HYPOTHESIS: the throw is only started while the flick input is still inside its window.
    if ((float)controller.getFlickNoResetX() >= limit) {
        return;
    }
    // The boomerang launch (article cast and throw call) is not reconstructed yet.
}

void ftToonLinkStatusUniqProcessSpecialBoomerang::execStatus(soModuleAccesser*) { }

void ftToonLinkStatusUniqProcessSpecialBoomerang::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003d)) {
        ftData* data = g_ftCommonDataAccesser.getData(static_cast<ftKind>(0x29));
        soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
        articles->removeExist(*reinterpret_cast<int*>(reinterpret_cast<u8*>(data) + 0xa4), 0);
    }
}

ftToonLinkStatusUniqProcessSpecialBoomerang g_ftToonLinkStatusUniqProcessSpecialBoomerang;
