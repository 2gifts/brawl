// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/bombhead/em_bombhead.h>
#include <em/em_shot.h>

// The Bombhead (Bombed): an Enemy with one wnemSimple and one wnemSwHolder article. The class overrides only its
// destructor; its own code is the two activators of its articles. Everything else in this unit is the module storage
// and the template instances of em/em_article_mediator.h (with one instance per pool, the instance holders'
// constructors are inlined into the Bombhead's).

// The shared shot helpers of the enemy activators are declared in em/em_shot.h; this one is used only here so far.
extern "C" {
// Sets the launch velocity and acceleration of the shot in the shot work from a random speed of its description and
// the given angle (degrees); the x components are scaled by the facing of the enemy when the flag is set (the same
// computation as the Popperam's inline one, see popperam/em_popperam.cpp).
void fn_41_47B4C(emShotParam* param, soModuleAccesser* acc, bool faceLr, float angle);
}

// HYPOTHESIS: the launch angle (degrees) of the wnemSwHolder shot, left in the work floats by the Bombhead's AI or
// status code.
static const u32 WorkFloat_ShotAngle = 0x11000004;

// ---- construction ---------------------------------------------------------------------------------------------------

emBombhead::emBombhead(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(29, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emBombhead::~emBombhead() { }

// ---- the activators -------------------------------------------------------------------------------------------------

// The wnemSwHolder shot is thrown at the angle the shot work float holds.
bool emBombhead::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    fn_41_47B4C(param, moduleAccesser, true, moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotAngle));
    fn_41_47D8C(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

bool emBombhead::activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    fn_41_47A2C(param, moduleAccesser, true);
    fn_41_47E80(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
