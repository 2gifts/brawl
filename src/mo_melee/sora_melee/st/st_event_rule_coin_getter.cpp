#include <st/stage.h>
#include <st/st_event_rule.h>
#include <types.h>

// HYPOTHESIS: one 0x2AC-byte player record of the block the rule watches (team flag at 0x25, coin counts at 0x38/0x4C).
struct stEventRuleCoinGetterPlayer {
    u8 unk0[0x25];
    u8 m_team;     // 0x25
    u8 unk26[0x38 - 0x26];
    int m_lost;    // 0x38
    u8 unk3C[0x4C - 0x3C];
    int m_coins;   // 0x4C
    u8 unk50[0x2AC - 0x50];
};

stEventRuleCoinGetter::stEventRuleCoinGetter() : stEventRule(1) {
    m_players = NULL;
    m_teamMode = 0;
    m_goal = 0;
}

stEventRuleCoinGetter::~stEventRuleCoinGetter() { }

// Lost coins fail the event; reaching the goal (with the partner team's coins in team mode) wins it.
bool stEventRuleCoinGetter::chechEventRule() {
    if (m_players == NULL) {
        return false;
    }
    stEventRuleCoinGetterPlayer* players = static_cast<stEventRuleCoinGetterPlayer*>(m_players);
    int lost = 0;
    for (int i = 0; i < 4; i++) {
        if (players[i].m_team == 0) {
            lost += players[i].m_lost;
        }
    }
    if (lost > 0) {
        unk8 = 8;
        unkC = 3;
        return true;
    }
    int coins = 0;
    for (int i = 0; i < 4; i++) {
        if (players[i].m_team == 0) {
            coins += players[i].m_coins;
        }
    }
    if (m_goal <= coins) {
        unk8 = 7;
        unkC = 4;
        return true;
    }
    if (m_teamMode == 1) {
        int otherCoins = 0;
        for (int i = 0; i < 4; i++) {
            if (players[i].m_team == 1) {
                otherCoins += players[i].m_coins;
            }
        }
        if (m_goal <= otherCoins) {
            unk8 = 8;
            unkC = 3;
            return true;
        }
    }
    if (g_Stage != NULL) {
        return g_Stage->isEventEnd(0, &unk8, &unkC);
    }
    return false;
}
