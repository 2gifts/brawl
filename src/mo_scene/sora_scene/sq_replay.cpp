#include <gf/gf_scene.h>
#include <sq/sq_replay.h>
#include <sr/sr_common.h>
#include <types.h>
#include <gm/gm_global.h>

sqReplay* sqReplay::create() {
    return new (Heaps::GlobalMode) sqReplay("sqReplay");
}

void sqReplay::start() {
    unk8 = 0;
}

void sqReplay::exit() { }

void sqReplay::setupKumiteReplay() {
    GameGlobal* global = g_GameGlobal;
    gmMeleeInitData* melee = &global->m_modeMelee->m_meleeInitData;
    gmGlobalCorps* corps = global->m_corps;
    int count = 0;
    switch (melee->m_corpsKind) {
    case 5:
        count = 10;
        break;
    case 6:
        count = 100;
        break;
    }
    corps->_0 = melee->m_numPlayers;
    corps->m_numFightersToBeat = 0;
    corps->_1 = 5;
    corps->m_numFightersInCurrentStage = count;
    corps->m_isAllStarList = 2;
    corps->m_flags = (corps->m_flags & ~0x80) | 0x40;
}

bool sqReplay::checkHomerunVirsusReplay() {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee->m_meleeInitData.m_isHomerunSingle ||
        melee->m_meleeInitData.m_isHomerunCombi ||
        ((u32)melee->m_meleeInitData.m_homerunTeamA == 1 &&
         (u32)melee->m_meleeInitData.m_homerunTeamB == 1)) {
        return false;
    }
    if (melee->m_meleeInitData.m_homerunTeamA == 0) {
        melee->m_meleeInitData.m_homerunTeamA = 1;
        melee->m_playersInitData[0].m_stockCount = 0;
        melee->m_playersInitData[1].m_stockCount = -1;
    } else {
        melee->m_meleeInitData.m_homerunTeamA = 0;
        melee->m_meleeInitData.m_homerunTeamB = 1;
        melee->m_playersInitData[0].m_stockCount = -1;
        melee->m_playersInitData[1].m_stockCount = 0;
    }
    return true;
}
