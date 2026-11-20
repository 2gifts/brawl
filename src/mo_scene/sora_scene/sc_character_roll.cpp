#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <ft/ft_manager.h>
#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <revolution/GX/GXMisc.h>
#include <sc/sc_melee.h>
#include <sc/sc_melee_local.h>
#include <sr/sr_common.h>
#include <st/loader/st_loader_manager.h>
#include <st/operator/st_operator_appearance_fighter_null.h>
#include <st/operator/st_operator_bgm.h>
#include <st/operator/st_operator_fighter_rebirth.h>
#include <st/operator/st_operator_null_go.h>
#include <st/operator/st_operator_rule_stage_end_request.h>
#include <types.h>

// A static pair of the module that its initializer function fills in (the same shape as the one of scButton).
// HYPOTHESIS: the names; the pairs are not read in this unit.
struct scCharacterRollEntry {
    int m_id;
    int m_shown;
    scCharacterRollEntry(int id, int shown) : m_id(id), m_shown(shown) { }
};
static scCharacterRollEntry s_entryHidden(0xFF, 0);
static scCharacterRollEntry s_entryShown(0xFF, 1);

// The scene of the staff roll (module sora_scene): a melee scene on the staff roll stage (stCharacterRoll).
class scCharacterRoll : public scMelee {
public:
    scCharacterRoll() : scMelee("scCharacterRoll") { }
    static scCharacterRoll* create();
    virtual void start();
    virtual s32 exit();
    virtual ~scCharacterRoll();
};
static_assert(sizeof(scCharacterRoll) == 0x100, "Class is wrong size!");

scCharacterRoll* scCharacterRoll::create() {
    return new (Heaps::GlobalMode) scCharacterRoll();
}

// TODO: the copy of the melee mode at the start still differs (the original keeps the copy address in one register).
void scCharacterRoll::start() {
    GXSetMisc(1, 8);
    fn_1_139E0(this);
    lbl_1_bss_90 = *reinterpret_cast<scMeleeModeCopy*>(g_GameGlobal->m_modeMelee);
    g_GameGlobal->initGameFrame();
    g_gfApplication->setPause(0, 0);
    fn_1_1A4DC(this);
    m_unkD1 = 0;
    m_loaderPlayers[0] = g_stLoaderManager->m_loaderPlayers[0];
    m_loaderPlayers[1] = g_stLoaderManager->m_loaderPlayers[1];
    m_loaderPlayers[2] = g_stLoaderManager->m_loaderPlayers[2];
    m_loaderPlayers[3] = g_stLoaderManager->m_loaderPlayers[3];
    m_loaderPlayers[4] = g_stLoaderManager->m_loaderPlayers[4];
    m_loaderPlayers[5] = g_stLoaderManager->m_loaderPlayers[5];
    m_loaderPlayers[6] = g_stLoaderManager->m_loaderPlayers[6];
    m_loaderStage = g_stLoaderManager->m_loaderStage;
    m_loaderStageStarFox = g_stLoaderManager->m_loaderStageAlt;
    m_loaderStageSe = g_stLoaderManager->m_loaderStageSe;
    m_loaderPokemonSe = g_stLoaderManager->m_loaderPokemonSe;
    m_loaderInfo = g_stLoaderManager->m_loaderInfo;
    m_loaderSandbag = g_stLoaderManager->m_loaderSandbag;
    m_loaderStageAdvCommon = g_stLoaderManager->m_loaderStageAdvCommon;
    g_ftManager->setMode(0);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_loaderPlayers[i] != NULL) {
            static_cast<stLoaderPlayer*>(m_loaderPlayers[i])->m_mode = stLoaderPlayer::Mode_Entry;
        }
        if (m_loaderPlayers[i] != NULL) {
            static_cast<stLoaderPlayer*>(m_loaderPlayers[i])->_0x58[0] = 0;
        }
        if (m_loaderPlayers[i] != NULL) {
            fn_27_23BB10(static_cast<stLoaderPlayer*>(m_loaderPlayers[i]));
        }
        if (m_loaderPlayers[i] != NULL) {
            static_cast<scLoaderPlayerFlags*>(m_loaderPlayers[i])->m_unk5D_b7 = 0;
        }
        if (m_loaderPlayers[i] != NULL) {
            stOperatorFighterRebirth* rebirth = new (Heaps::System) stOperatorFighterRebirth();
            m_operatorFighterRebirths[i] = rebirth;
            if (rebirth != NULL) {
                rebirth->unk50 = m_loaderPlayers[i];
            }
        }
    }
    aiMgr::setAdventureMd(0);
    CameraController::getInstance()->changeInput(CameraController::AI);
    g_cmAIController->unk94_10 = 0;
    m_operatorBgm = new (Heaps::System) stOperatorBgm();
    m_operatorAppearanceFighter = new (Heaps::System) stOperatorAppearanceFighterNull();
    m_operatorReadyGo = new (Heaps::System) stOperatorNullGo(true);
    m_operatorNetwork = new (Heaps::System) stOperatorNetworkNull();
    m_operatorRuleGameMode = new (Heaps::System) stOperatorRuleStageEndRequest();
    stOperatorInfo* info = m_operatorInfo;
    if (info != NULL) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            info->m_loaderPlayers[i] = static_cast<stLoaderPlayer*>(m_loaderPlayers[i]);
        }
    }
    if (m_operatorAppearanceFighter != NULL) {
        static_cast<stOperatorAppearanceFighter*>(m_operatorAppearanceFighter)->unk90 = m_operatorReadyGo;
    }
    if (m_operatorAppearanceFighter != NULL) {
        stOperatorAppearanceFighter* appearance = static_cast<stOperatorAppearanceFighter*>(m_operatorAppearanceFighter);
        for (int i = 0; i < MAX_PLAYERS; i++) {
            appearance->unk74[i] = m_loaderPlayers[i];
        }
    }
    if (m_operatorReadyGo != NULL) {
        stOperatorReadyGo* readyGo = static_cast<stOperatorReadyGo*>(m_operatorReadyGo);
        for (int i = 0; i < MAX_PLAYERS; i++) {
            (&readyGo->unk58)[i] = reinterpret_cast<u32>(m_loaderPlayers[i]);
        }
    }
    if (m_operatorReadyGo != NULL) {
        static_cast<stOperatorReadyGo*>(m_operatorReadyGo)->unk74 = reinterpret_cast<u32>(m_operatorBgm);
    }
    if (m_operatorRuleGameMode != NULL) {
        m_operatorRuleGameMode->setOperatorBgm(m_operatorBgm);
    }
    unkD8_b1 = 0;
}

s32 scCharacterRoll::exit() {
    fn_1_194A4(this, 0);
    fn_1_19968(this, 1);
    GXSetMisc(1, 0);
    return 0;
}

scCharacterRoll::~scCharacterRoll() { }
