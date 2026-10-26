#include <if/if_center.h>
#include <if/if_mngr.h>
#include <st/operator/st_operator_rule_stage_end_request.h>
#include <types.h>

extern "C" {
// stOperatorBgm::requestStop (sora_melee 0x242418)
void fn_27_242418(void* bgm);
}

stOperatorRuleStageEndRequest::stOperatorRuleStageEndRequest() {
    m_state = 0;
}

stOperatorRuleStageEndRequest::~stOperatorRuleStageEndRequest() { }

void stOperatorRuleStageEndRequest::processBegin() {
    if (m_isPause != true && m_isExit != true) {
        switch (m_state) {
        case 1:
            if (isFinishCheck() == true) {
                if (g_IfMngr != NULL) {
                    static_cast<IfCenter*>(g_IfMngr->m_ifCenter)->setCenterState(7);
                }
                if (m_operatorBgm != NULL) {
                    fn_27_242418(m_operatorBgm);
                }
                m_state = 2;
            }
            break;
        case 2:
            if (g_IfMngr != NULL) {
                if (static_cast<IfCenter*>(g_IfMngr->m_ifCenter)->m_state == 0) {
                    m_state = 3;
                }
            } else {
                m_state = 3;
            }
            break;
        }
    }
}

bool stOperatorRuleStageEndRequest::isEnd() {
    return m_state == 3;
}

void stOperatorRuleStageEndRequest::startOperator() {
    m_state = 1;
}

bool stOperatorRuleStageEndRequest::isSuddenDeath() {
    return false;
}
