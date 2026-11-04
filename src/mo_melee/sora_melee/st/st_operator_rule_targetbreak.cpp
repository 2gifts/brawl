#include <if/if_mngr.h>
#include <st/operator/st_operator_rule_targetbreak.h>
#include <st_tbreak/st_tbreak.h>
#include <types.h>

stOperatorRuleTargetBreak::stOperatorRuleTargetBreak() : stOperatorRuleMelee(0) { }

stOperatorRuleTargetBreak::~stOperatorRuleTargetBreak() { }

void stOperatorRuleTargetBreak::decisionExtraRule() {
    if (m_isComplete) {
        m_decisionKind = Decision_Complete;
        m_resultInfo->m_decisionKind = gmResultInfo::Decision_Complete;
    } else {
        m_decisionKind = Decision_Failure;
        m_resultInfo->m_decisionKind = gmResultInfo::Decision_Failure;
    }
}

bool stOperatorRuleTargetBreak::isSuddenDeath() {
    return false;
}

void stOperatorRuleTargetBreak::updateExtraInfo() {
    if (g_IfMngr) {
        int count = 0;
        if (g_Stage && g_Stage->m_stageKind == Stages::TargetBreak) {
            count = static_cast<stTargetBreak*>(g_Stage)->m_targetsLeft;
        }
        // HYPOTHESIS: dispCorps consumes character kind and costume byte pairs.
        struct {
            int unk0;
            u8 unk4;
        } unk[10];
        for (int i = 0; i < count; i++) {
            unk[i].unk0 = 0x2C;
            unk[i].unk4 = m_level;
        }
        g_IfMngr->dispCorps(unk, count);
    }
}

bool stOperatorRuleTargetBreak::notifyEventDead(int entryId, u32 playerIndex, int arg) {
    ++unk284;
    stOperatorRuleMelee::notifyEventDead(entryId, playerIndex, arg);
    return false;
}
