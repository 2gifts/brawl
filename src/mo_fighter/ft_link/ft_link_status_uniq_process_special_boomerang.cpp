#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: generate-article manage module (soModuleEnumeration::m_generateArticleManageModule); only the slot below is used.
class ftGenerateArticleManageStub {
public:
    virtual ~ftGenerateArticleManageStub();
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
    virtual void unk48(int, int);
};

class ftLinkStatusUniqProcessSpecialBoomerang : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialBoomerang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// Common parameter block read by the initial throw check. Offsets are native; field meanings are HYPOTHESIS.
struct ftLinkBoomerangCommonView {
    u8 unk00[0x18];
    float m_stickThreshold; // 0x18: the stick must stay below this magnitude
    int m_flickFrame;       // 0x1c
    float m_flickFrameAdd;  // 0x20
};

// The throw is only started while the stick is below the threshold and the flick input is still inside its window.
// The boomerang launch (article cast, throw call and the flags it sets) is not reconstructed yet.
void ftLinkStatusUniqProcessSpecialBoomerang::initStatus(soModuleAccesser* moduleAccesser) {
    float stickX = __fabsf(moduleAccesser->getControllerModule().getStickX());
    ftLinkBoomerangCommonView* common = reinterpret_cast<ftLinkBoomerangCommonView*>(g_ftCommonDataAccesser.getParamCommon());
    if (stickX >= common->m_stickThreshold) {
        return;
    }
    soControllerModule& controller = moduleAccesser->getControllerModule();
    float limit = (float)common->m_flickFrame + common->m_flickFrameAdd;
    if ((float)controller.getFlickNoResetX() >= limit) {
        return;
    }
}

void ftLinkStatusUniqProcessSpecialBoomerang::execStatus(soModuleAccesser*) { }

// The boomerang article is released on leaving the status unless the throw flag is still set.
// HYPOTHESIS: the article id comes from ftData+0xA4 (Link common data).
void ftLinkStatusUniqProcessSpecialBoomerang::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003D)) {
        ftData* data = g_ftCommonDataAccesser.getData(Fighter_Link);
        reinterpret_cast<ftGenerateArticleManageStub*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)
            ->unk48(*reinterpret_cast<int*>(reinterpret_cast<u8*>(data) + 0xA4), 0);
    }
}

ftLinkStatusUniqProcessSpecialBoomerang g_ftLinkStatusUniqProcessSpecialBoomerang;
