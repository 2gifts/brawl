#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <sq/sq_lib.h>
#include <st/loader/st_loader_manager.h>
#include <st/operator/st_operator_rule.h>
#include <types.h>

// muMenu::exchangeGmCharacterKind2MuSelchkind and getNumCharColor: the symbols take three integers, the callers
// give one (as in mu_event.cpp and mu_tour_rule.cpp).
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
extern "C" int getNumCharColor__6muMenuFiii(int kind);

gmCharacterKind sqGetPatchCHKind(gmCharacterKind kind) {
    switch (kind) {
    case Character_PokemonTrainer:
        switch (randi(3)) {
        case 0: return Character_PokeLizardon_Trainer;
        case 1: return Character_PokeZenigame_Trainer;
        default: return Character_PokeFushigisou_Trainer;
        }
    case Character_SamusSZeroSuit:
        return randi(2) == 0 ? Character_Samus : Character_ZeroSuitSamus;
    case Character_ZeldaShiek:
        return randi(2) == 0 ? Character_Zelda : Character_Sheik;
    default:
        return kind;
    }
}

int sqGetPatchInfoColor(gmCharacterKind kind, int color, int player) {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    int charKind = stOperatorRule::exchangeGmCharacterKindToMuCharKind(kind);
    if (charKind != 0x23) {
        int used = 0;
        for (int i = 0; i < 4; i++) {
            if (i != player && melee->m_playersInitData[i].m_state != 3 &&
                (int)stOperatorRule::exchangeGmCharacterKindToMuCharKind(
                    (gmCharacterKind)melee->m_playersInitData[i].m_characterKind) == charKind) {
                used |= 1 << (u8)melee->m_playersInitData[i].m_colorNo;
            }
        }
        if (used & (1 << color)) {
            int selch = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
            int count = selch == 0x28 ? 1 : getNumCharColor__6muMenuFiii(selch);
            for (int i = 0; i < count; i++) {
                color = (color + 1) % count;
                if (!(used & (1 << color))) {
                    break;
                }
            }
        }
    }
    return color;
}

void sqWriteMetamorphosisHumanFighter() {
    if (g_stLoaderManager) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (g_stLoaderManager->m_loaderPlayers[i]) {
                gmPlayerInitData* player = &g_GameGlobal->m_modeMelee->m_playersInitData[i];
                if (player->m_state == 0) {
                    player->m_characterKind = g_GameGlobal->m_resultInfo->m_playersResultInfo[i].m_characterKind;
                }
            }
        }
    }
}
