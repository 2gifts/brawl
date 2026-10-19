#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/catch/so_catch_module.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/yoshi/ft_yoshi_transactor.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <wn/yoshi/wn_yoshi_tamago.h>
#include <so/team/so_team.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/stageobject.h>

// Translate Yoshi's original status ID to Kirby's current copied status ID.

struct ftYoshiCaptureNodeEvent : soLinkEventArgs {
    int captureNode;
    ftYoshiCaptureNodeEvent() : soLinkEventArgs(0x1f), captureNode(-1) { }
};

ftYoshiTransactor::ftYoshiTransactor() { }
ftYoshiTransactor::~ftYoshiTransactor() { }

bool ftYoshiTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    Vec3f position;
    switch (article->getArticleId()) {
    case 13: {
        wnYoshiTamago* weapon = dynamic_cast<wnYoshiTamago*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(
            *acc->getResourceModule().getResourceIdAccesser());
        u32 resourceId = resources.getKirbyResId(4);
        int captureNode = -1;
        int captureStatus = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x118, acc, 0);
        int founderTaskId;
        position.m_x = 0.0f;
        position.m_y = 0.0f;
        position.m_z = 0.0f;
        if (acc->getStatusModule().getStatusKind() == captureStatus) {
            founderTaskId = acc->getLinkModule().getNodeTaskId(0);
            ftYoshiCaptureNodeEvent event;
            acc->getLinkModule().sendEventNodes(0, event, 0);
            captureNode = event.captureNode;
        } else {
            int node = acc->getModelModule().getCorrectNodeId(0x131);
            position = acc->getModelModule().getNodeGlobalPosition(node, false);
            founderTaskId = acc->getStageObject().m_taskId;
        }
        soTeamModule& team = acc->getTeamModule();
        float lr = acc->getPostureModule().getLr();
        int teamNo = team.getTeam()->getNo();
        weapon->activate(founderTaskId, resourceId, teamNo, &position, captureNode, lr);
        return true;
    }
    default:
        return false;
    }
}

bool ftYoshiTransactor::activeArticle1(wnYoshiTamago* weapon, soModuleAccesser* acc, u32 resourceId) {
    int founderTaskId;
    int captureNode = -1;
    int captureStatus = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x118, acc, 0);
    Vec3f position(0.0f, 0.0f, 0.0f);
    if (acc->getStatusModule().getStatusKind() == captureStatus) {
        founderTaskId = acc->getLinkModule().getNodeTaskId(0);
        ftYoshiCaptureNodeEvent event;
        acc->getLinkModule().sendEventNodes(0, event, 0);
        captureNode = event.captureNode;
    } else {
        int node = acc->getModelModule().getCorrectNodeId(0x131);
        position = acc->getModelModule().getNodeGlobalPosition(node, false);
        founderTaskId = acc->getStageObject().m_taskId;
    }
    soTeamModule& team = acc->getTeamModule();
    float lr = acc->getPostureModule().getLr();
    int teamNo = team.getTeam()->getNo();
    weapon->activate(founderTaskId, resourceId, teamNo, &position, captureNode, lr);
    return true;
}

// Egg Lay capture status, shared with Kirby's copied ability.
class ftYoshiStatusUniqProcessSpecialNSwallow : public soStatusUniqProcess {
public:
    virtual ~ftYoshiStatusUniqProcessSpecialNSwallow() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

// Catch-module view: the header covers the vtable prefix up to catchCut (0x2c); the slots after it are
// not named yet, so they are kept as unkN. unk3C is the checkDamage hook of the catch module.
class ftYoshiCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool unk3C(soModuleAccesser* moduleAccesser, void* data);
};

ftYoshiStatusUniqProcessSpecialNSwallow g_ftYoshiStatusUniqProcessSpecialNSwallow;

void ftYoshiStatusUniqProcessSpecialNSwallow::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x118) {
        return;
    }
    if (nextStatus == 0x15b) {
        return;
    }
    static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
}
// Fighter::notifyEventLink handles 0x1d by hiding the captured fighter and
// disabling its hit/ground collision. Event 0x1e selects its egg-capture status
// and returns the hit group and hit number used by the absolute attack.
struct ftYoshiSwallowCaptureEvent : soLinkEventArgs {
    ftYoshiSwallowCaptureEvent() : soLinkEventArgs(0x1d) { }
};
struct ftYoshiSwallowHitEvent : soLinkEventArgs {
    int unk8;
    int targetGroup;
    int targetNo;
    int unk14;
    float unk18;
    bool unk1c;
    ftYoshiSwallowHitEvent() : soLinkEventArgs(0x1e), unk8(-1),
        targetGroup(-1), targetNo(-1), unk14(-1), unk18(-1.0f), unk1c(true) { }
};
static_assert(sizeof(ftYoshiSwallowHitEvent) == 0x20, "Egg Lay event layout is wrong!");

void ftYoshiStatusUniqProcessSpecialNSwallow::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soGenerateArticleManageModule* articles;
    soCollisionAttackModule* attack;
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    articles = static_cast<soGenerateArticleManageModule*>(
        moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    attack = moduleAccesser->m_enumerationStart->m_collisionAttackModule;
    soLinkModule* link = moduleAccesser->m_enumerationStart->m_linkModule;
    if (work->isFlag(0x22000012)) {
        ftYoshiSwallowCaptureEvent event;
        link->sendEventNodes(0, event, 0);
        work->offFlag(0x22000012);
    }
    if (work->isFlag(0x22000013)) {
        int kind = moduleAccesser->getStageObject().soGetSubKind();
        int articleId = 0;
        if (kind == 4) {
            articleId = 1;
        } else if (kind == 5) {
            articleId = 13;
        }
        articles->generate(articleId, false, false);
        int targetTaskId = link->getNodeTaskId(0);
        ftYoshiSwallowHitEvent event;
        link->sendEventNodes(0, event, 0);
        int node = moduleAccesser->m_enumerationStart->m_modelModule->getCorrectNodeId(0x12c);
        attack->hitAbsolute(0, node, targetTaskId, event.targetGroup, event.targetNo);
        work->offFlag(0x22000013);
    }
}
bool ftYoshiStatusUniqProcessSpecialNSwallow::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    return static_cast<ftYoshiCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->unk3C(moduleAccesser, data);
}
