// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/deathpod/em_deathpod.h>
#include <em/em_shot.h>

// The Deathpod: an Enemy with six wnemSwHolder articles. The class overrides only its destructor; its own code is the
// activator of its articles, which aims each shot from one of two launch modes chosen by the shot work. Everything else
// in this unit is the module storage and the template instances of em/em_article_mediator.h.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h; this one is used only here so far.
extern "C" {
// HYPOTHESIS: aims the shot from the given model node (its first argument is handed to a node position query of the
// model module); the last argument selects how the direction is derived.
void fn_41_477EC(u32 nodeId, emShotParam* param, soModuleAccesser* acc, bool, int mode);
}

// HYPOTHESIS: the node id the shot work holds when the shot comes from the Deathpod's second launch point.
static const u32 Deathpod_ShotNodeAlt = 11;

// ---- construction ---------------------------------------------------------------------------------------------------

emDeathpod::emDeathpod(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiEntries,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(16, 0),
    m_areaInstances(10, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emDeathpod::~emDeathpod() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emDeathpod::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    // emShotWork::m_unk8 is typed as a pointer in em/em_shot.h; the Deathpod reads it as a node id.
    u32 nodeId = reinterpret_cast<u32>(lbl_41_bss_504.m_unk8);
    if (nodeId == Deathpod_ShotNodeAlt) {
        fn_41_477EC(nodeId, param, moduleAccesser, true, 3);
    } else {
        fn_41_477EC(nodeId, param, moduleAccesser, true, 2);
    }
    fn_41_47D8C(&pos, false, article, param, moduleAccesser);
    return false;
}
