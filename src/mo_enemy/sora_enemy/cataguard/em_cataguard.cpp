// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/cataguard/em_cataguard.h>
#include <em/em_shot.h>

// The Cataguard: an Enemy with three pools of wnemSwHolder articles. The class overrides only its destructor; its own
// code is the activator of its articles, which throws shots of kind 4 at the angle the shot work float holds. Everything
// else in this unit is the module storage and the template instances of em/em_article_mediator.h.
//
// TODO(nonmatching): the mediator's getActiveNum, 82.5%, (b) a different call set: the original calls its counting
// functor out of line (soEachCounter<...>::operator()<wnemSwHolder>, 0x43B8C, used once per pool, so three times here)
// where em/em_article_mediator.h inlines a plain counter. A functor form of getActiveNum that keeps every other enemy's
// copy byte-identical is proposed for the shared header.
// TODO(link): besides that, the unit's first four functions (0x4198C-0x42348, wnemPrimProc::Boomerang*) are the
// Primid's boomerang procs, a unit of their own; and the three pools' getInstanceAt copies must come out in reverse
// pool order (id 2, 1, 0), as in every multi-pool enemy, where MWCC emits them in pool order.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h; this one is used only here and by the
// Bombhead so far (em_bombhead.cpp).
extern "C" {
// Sets the launch velocity and acceleration of the shot in the shot work from a random speed of its description and
// the given angle (degrees); the x components are scaled by the facing of the enemy when the flag is set.
void fn_41_47B4C(emShotParam* param, soModuleAccesser* acc, bool faceLr, float angle);
}

// HYPOTHESIS: the launch angle (degrees) of a thrown shot, left in the work floats by the AI or status code (the
// Bombhead reads the same float).
static const u32 WorkFloat_ShotAngle = 0x11000004;

// ---- construction ---------------------------------------------------------------------------------------------------

emCataguard::emCataguard(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(23, 0),
    m_areaInstances(12, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emCataguard::~emCataguard() { }

// ---- the activator --------------------------------------------------------------------------------------------------

// HYPOTHESIS: shot kind 4 is the thrown shot; the others fly from the launch position along the shot description.
bool emCataguard::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    if (param->m_shotKind == 4) {
        fn_41_47B4C(param, moduleAccesser, true, moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotAngle));
    } else {
        // The launch position is computed a second time and dropped.
        fn_41_47470(param, moduleAccesser);
    }
    fn_41_47D8C(&pos, flag.m_value, article, param, moduleAccesser);
    if (param->m_shotKind == 4) {
        return true;
    }
    return param->m_shotKind == 0;
}
