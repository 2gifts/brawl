#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <st/operator/st_operator_rule_kumite.h>
#include <types.h>

stOperatorRuleKumite::stOperatorRuleKumite(u8 arg) : stOperatorRuleMelee(arg) { }

stOperatorRuleKumite::~stOperatorRuleKumite() { }

bool stOperatorRuleKumite::isSuddenDeath() {
    return false;
}

void stOperatorRuleKumite::calcScore() {
    setNormalResultInfo();
}

void stOperatorRuleKumite::notifyEventSetupCorps() {
    if (m_condition->m_meleeInitData.m_corpsKind >= 5 &&
        m_condition->m_meleeInitData.m_corpsKind <= 10) {
        initCorpsRule(g_GameGlobal->m_corps);
    }
}

bool stOperatorRuleKumite::notifyEventDead(int entryId, u32 playerIndex, int arg) {
    if (m_condition->m_playersInitData[playerIndex].m_state == 1 &&
        m_condition->m_playersInitData[playerIndex].m_characterKind < 0x2E) {
        g_sndSystem->playSE(static_cast<SndID>(0x1B), -1, 0, 0, -1);
    }
    return stOperatorRuleMelee::notifyEventDead(entryId, playerIndex, arg);
}

void stOperatorRuleKumite::createRandomMyCharaList(u32 characterKind) {
    int i;
    int last = unk260 - 1;
    for (i = 0; i < unk260; i++) {
        int index = randi(last);
        u8 value = unk268[i];
        unk268[i] = unk268[index];
        unk268[index] = value;
    }
    if (unk268[0] == characterKind) {
        u8 value = unk268[0];
        unk268[0] = unk268[1];
        unk268[1] = value;
    }
    for (int i = 0; i < unk260; i++) {
        switch (unk268[i]) {
        case 3:
        case 4: {
            gmCharacterKind value = randi(99) < 50 ? Character_Samus : Character_ZeroSuitSamus;
            unk268[i] = value;
            break;
        }
        case 14:
        case 15: {
            gmCharacterKind value = randi(99) < 50 ? Character_Zelda : Character_Sheik;
            unk268[i] = value;
            break;
        }
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
            int value = randi(99);
            if (value < 33) {
                unk268[i] = 29;
            } else if (value < 66) {
                unk268[i] = 31;
            } else {
                unk268[i] = 33;
            }
            break;
        }
    }
    unk264 = 0;
}
