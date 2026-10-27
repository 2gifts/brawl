// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot, or its own
// declaration adds one and every later Weapon virtual (deactivateArticle ...) is called one slot too far.
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/prim/em_prim.h>
#include <em/em_shot.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>

// The Primid: an Enemy with two wnemSimple shots and six wnemSwHolder articles. The variants (metal, big, boomerang,
// fire, super scope, sword) share this class; only the boomerang Primid does extra work in the code below.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emPrim::emPrim(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
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
            emWeaponUtil::calcWeaponRotation(lbl_41_bss_504.m_nodeId, param, moduleAccesser, false, 0);
        }
    }
    fn_41_47A2C(param, moduleAccesser, true);
    fn_41_47D8C(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// TODO(nonmatching): 98.4%, (d) register allocation only: the same 97 instructions, calls, constants, branches and
// value lifetimes; only the coloring order differs (the original colors the owner above the module accesser and the
// posture module below it, ours colors the accesser second and the branch locals in declaration order). Hoisting
// the branch locals, a pointer instead of the reference, a switch, an early return, named float arguments and an
// inline helper for the plain-shot branch (six parameter orders) all keep the original's order out of reach.
// The branch is the body of emWeaponUtil::activeSimpleWeapon (fn_41_47E80) with getExtendBrresResourceID in place
// of getSimpleWeaponResourceID; that function colors its locals in the same order the original does here.
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
        // HYPOTHESIS: the boomerang came back to the thrower; AI unit 0x4D gets flag 1 and the event is consumed.
        case 0x13fe:
            m_aiModule.setFlag(1, 0x4d);
            eventInfo->_0x4 = true;
            break;
        }
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}
