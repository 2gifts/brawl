// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot, or its own
// declaration adds one and every later Weapon virtual (deactivateArticle ...) is called one slot too far.
#define WN_WEAPON_ANIMCMD_LONG
#include <em/prim/em_prim.h>
#include <em/em_shot.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>

// The Primid: an Enemy with two wnemSimple shots and six wnemSwHolder articles. The variants (metal, big, boomerang,
// fire, super scope, sword) share this class; only the boomerang Primid does extra work in the code below.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

extern "C" {
// emAIModuleImpl: notifies the AI of an event.
void fn_41_28F74(emAIModuleImpl* ai, int, int);
}

// ---- construction ---------------------------------------------------------------------------------------------------

emPrim::emPrim(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiEntries,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(44, 0),
    m_areaInstances(17, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emPrim::~emPrim() { }

// ---- the activators -------------------------------------------------------------------------------------------------

bool emPrim::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    if (enemy.getEnemyKind() == Enemy_Prim_Boomerang) {
        if (param->m_shotKind == 1 || param->m_shotKind == 2) {
            lbl_41_bss_504.m_unk6 = false;
            fn_41_4768C(lbl_41_bss_504.m_unk8, param, moduleAccesser, false, 0);
        }
    }
    fn_41_47A2C(param, moduleAccesser, true);
    fn_41_47D8C(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// TODO(nonmatching): 98.4%, register allocation only (the target keeps the module accesser in r29 and reuses it for
// the life value; ours keeps it in r30). Calls, arguments, constants and branches agree.
bool emPrim::activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    fn_41_47A2C(param, moduleAccesser, true);
    if (param->m_shotKind == 4) {
        int ownerId = enemy.getExtendBrresResourceID();
        StageObject* owner = moduleAccesser->m_stageObject;
        soPostureModule& posture = moduleAccesser->getPostureModule();
        int founderTaskId = owner->m_taskId;
        u32 life = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0);
        int team = soExternalValueAccesser::getTeamNo(owner);
        fn_41_3091C(article, &lbl_41_bss_504, founderTaskId, ownerId, 0, flag.m_value, &pos, team, posture.getLr(),
                    param->m_speed, -1, life, -1);
    } else {
        fn_41_47E80(&pos, flag.m_value, article, param, moduleAccesser);
    }
    return false;
}

// ---- events ---------------------------------------------------------------------------------------------------------

void emPrim::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    if (m_enemyKind == Enemy_Prim_Boomerang) {
        switch (eventInfo->m_eventKind) {
        // HYPOTHESIS: the boomerang came back to the thrower; the AI is told (event 0x4D) and the event consumed.
        case 0x13fe:
            fn_41_28F74(&m_aiModule, 1, 0x4d);
            eventInfo->_0x4 = true;
            break;
        }
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}
