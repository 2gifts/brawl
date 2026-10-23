// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot, or its own
// declaration adds one and every later Weapon virtual (deactivateArticle ...) is called one slot too far.
#define WN_WEAPON_ANIMCMD_LONG
#include <em/prim/em_prim.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>

// The Primid: an Enemy with two wnemSimple shots and six wnemSwHolder articles. The variants (metal, big, boomerang,
// fire, super scope, sword) share this class; only the boomerang Primid does extra work in the code below.

// ---- the shared shot helpers of the enemy units ------------------------------------------------------------------------
// The shot description and the work block a shooting enemy fills before it activates an article. The helpers live in a
// shared enemy unit that is not reconstructed yet, so they keep their address names.

// HYPOTHESIS: field roles from the reads below; the description comes from the enemy's constant parameters.
struct emShotParam {
    u8 unk0[0x8];
    s32 m_type;      // 0x8, 2 selects the second launch position and the flag of fn_41_47544
    u32 m_shotKind;  // 0xC, 1/2: boomerang shots of the boomerang Primid, 4: a plain wnemSimple shot
    u8 unk10[0x4];
    float m_speed;   // 0x14
};

// The work block of the shot helpers (0x34 bytes of the Enemy unit's bss).
struct emShotWork {
    emShotParam* m_param;
    u8 unk4[0x2];
    bool m_unk6;
    u8 unk7;
    void* m_unk8;
    u8 unkC[0x28];
};
extern emShotWork lbl_41_bss_504;

// A flag derived from the shot description (true when its m_type is 2).
struct emShotFlag {
    bool m_value;
    emShotFlag() : m_value(false) { }
};

extern "C" {
// Reads the shot description of the given parameter id into the work block and returns it.
emShotParam* fn_41_473DC(soModuleAccesser* acc, int paramId);
// The launch position of the shot.
Vec3f fn_41_47470(emShotParam* param, soModuleAccesser* acc);
void fn_41_47544(emShotFlag* flag, emShotParam* param);
void fn_41_4768C(void* target, emShotParam* param, soModuleAccesser* acc, bool, int);
void fn_41_47A2C(emShotParam* param, soModuleAccesser* acc, bool);
void fn_41_47D8C(Vec3f* pos, bool flag, wnemSwHolder* article, emShotParam* param, soModuleAccesser* acc);
void fn_41_47E80(Vec3f* pos, bool flag, wnemSimple* article, emShotParam* param, soModuleAccesser* acc);
// wnemSimple's activation.
void fn_41_3091C(wnemSimple* article, emShotWork* work, int founderTaskId, int ownerId, int, bool flag, Vec3f* pos,
                 int team, float lr, float speed, int, u32 life, s16);
// Enemy: an id the shot is linked to (0xFFFF without one).
int fn_41_3BE4(Enemy* enemy);
// emAIModuleImpl: notifies the AI of an event.
void fn_41_28F74(emAIModuleImpl* ai, int, int);
}

// HYPOTHESIS: the work int holds the parameter id of the shot being fired (it is handed straight to fn_41_473DC), and
// the value float is the shot's lifetime in frames (it becomes the life argument of the wnemSimple activation).
static const int WorkInt_ShotParamId = 0x10000007;
static const int ValueFloat_ShotLife = 0x1100000c;

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
        int ownerId = fn_41_3BE4(&enemy);
        StageObject* owner = moduleAccesser->m_stageObject;
        soPostureModule& posture = moduleAccesser->getPostureModule();
        int founderTaskId = owner->m_taskId;
        u32 life = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_ShotLife, 0);
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
