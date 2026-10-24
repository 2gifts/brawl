// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/spar/em_spar.h>
#include <em/em_shot.h>
#include <so/so_value_accesser.h>

// The Spar: an Enemy with three wnemSwHolder articles. Besides its module storage and its article mediator (the template
// instances of em/em_article_mediator.h), the class has one function of its own: the activation of its articles.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emSpar::emSpar(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiEntries,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(39, 0),
    m_areaInstances(10, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emSpar::~emSpar() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emSpar::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    if (param->m_lifeConstantId > ConstantId_CommonLast) {
        emShotLifeConstant* lifeConstant = static_cast<emShotLifeConstant*>(
            soValueAccesser::getConstantIndefinite(moduleAccesser, param->m_lifeConstantId, 0));
        param->m_life = Enemy::getValueFloat(&lifeConstant->m_life, moduleAccesser);
    }
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    if (param->m_shotKind == 0 || param->m_shotKind == 2) {
        Vec3f target;
        target.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
        target.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
        target.m_z = 0.0f;
        fn_41_47C64(param, moduleAccesser, &pos, &target, 0);
    } else {
        fn_41_47A2C(param, moduleAccesser, true);
    }
    fn_41_47D8C(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
