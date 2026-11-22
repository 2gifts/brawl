#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_vs_melee.h>
#include <sr/sr_common.h>
#include <types.h>


static gfScene* s_selStage;

sqVsMelee* sqVsMelee::create() {
    return new (Heaps::GlobalMode) sqVsMelee("sqVsMelee");
}

void sqVsMelee::start() {
    s32 kind = gfSceneManager::getInstance()->unk31C;
    s_selStage = gfSceneManager::getInstance()->searchScene("scSelStage");
    switch (kind) {
    case 0:
        unk8 = 0;
        unkC = 0;
        g_GameGlobal->initSelCharData();
        g_GameGlobal->m_modeMelee->gmInitVSMelee();
        break;
    case 0xff:
        unk8 = 0xf;
        unkC = 0xf;
        break;
    }
}

void sqVsMelee::exit() { }

#include <mu/menu.h>
// TODO: clean up, give variables real names
namespace jj_21940 {

extern "C" {
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
void gmSetRuleToGlobalModeMelee__17gmGlobalModeMeleeFv(void*);
void fn_80055760(u32, s32);
gmItSwitch* gmGetItemSwitchData_NormalStage__Fi(u32);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFii(int, int);
void fn_8010F960(u16, void*, void*, int);
}

struct SwitchData {
    u64 item;
    u32 pokemon;
    u32 assist;
};
struct ModeData {
    u8 pad0[8];
    u8 gameMode : 6;
    u8 modeRest : 2;
    u8 rule : 3;
    u8 count : 3;
    u8 rest : 2;
    u8 padA[9];
    u8 teams;
    u8 pad14[2];
    u8 frequency;
    u8 pad17[3];
    u16 stage;
    u8 pad1C[0x14];
    SwitchData switches;
    u8 pad40[0x58];
    struct Player {
        u8 kind, state, id, no;
        s8 stocks;
        u8 file, color, controller, start;
        u8 pad9[2];
        u8 team;
        u8 padC[0x50];
    } players[4];
};
struct SelectData {
    u8 pad0[0x33];
    u8 teams;
    u8 pad34[0x84];
    ModeData::Player players[4];
};
struct StageData {
    u8 pad[0x22];
    u16 stage;
    u8 substage;
};

extern "C" void fn_1_21940(void*, int stage) {
    ModeData* mode = (ModeData*)g_GameGlobal->m_modeMelee;
    SelectData* select = (SelectData*)g_GameGlobal->m_selCharData;
    StageData* stageSelect = (StageData*)g_GameGlobal->m_selStageData;
    gmGlobalRecord::MenuData* record = g_GameGlobal->getGlobalRecordMenuDatap();
    gmInitGlobalModeMelee__FP17gmGlobalModeMelee(mode);
    gmSetRuleToGlobalModeMelee__17gmGlobalModeMeleeFv(mode);
    if (stage == 0) {
        stage = stageSelect->stage;
    }
    mode->stage = stage;
    mode->gameMode = 0;
    mode->teams = select->teams;
    fn_80055760((u16)stage, stageSelect->substage);
    mode->switches = *(SwitchData*)gmGetItemSwitchData_NormalStage__Fi(mode->stage);
    mode->switches.item &= *(u64*)&record->m_itemSwitch;
    mode->frequency = record->m_itemFrequency;
    mode->count = 0;
    ModeData* output = mode;
    for (int i = 0; i < 4; ++i, select = (SelectData*)((u8*)select + 0x5c), output = (ModeData*)((u8*)output + 0x5c)) {
        if (select->players[0].state != 3) {
            int character = select->players[0].kind;
            output->players[0].kind = character;
            output->players[0].state = select->players[0].state;
            output->players[0].team = select->players[0].team;
            int costume = select->players[0].file;
            int kind = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(character);
            output->players[0].file = kind == 0x28 ? 0 : getFighterColorFileNo__6muMenuFii(kind, costume);
            output->players[0].color = select->players[0].file;
            output->players[0].id = i;
            output->players[0].start = mode->count;
            output->players[0].stocks = -1;
            if (mode->rule == 1) {
                output->players[0].stocks = g_GameGlobal->m_setRule->m_stockCount;
            }
            mode->count++;
        }
    }
    fn_8010F960(mode->stage, (u8*)mode + 0x1c, (u8*)mode + 0x5c, 0);
}
}

