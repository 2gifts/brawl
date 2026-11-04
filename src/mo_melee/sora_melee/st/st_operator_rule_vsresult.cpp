#include <st/operator/st_operator_rule_vsresult.h>
#include <types.h>
#include <if/if_vs_result_task.h>
#include <st/st_common_gimmick.h>

stOperatorRuleVsResult::stOperatorRuleVsResult() {
    unkCC = 0;
}

stOperatorRuleVsResult::~stOperatorRuleVsResult() { }

bool stOperatorRuleVsResult::isEnd() {
    if (unkCC == NULL) {
        return false;
    }
    bool ended = unkCC->m_state == ifVsResultTask::State_End;
    if (ended == true) {
        if (unkCC != NULL) {
            unkCC->exit();
            unkCC = NULL;
        }
    }
    return ended;
}

void stOperatorRuleVsResult::startOperator() {
    if (unkCC == NULL) {
        ifVsResultTask* task = new (Heaps::MenuInstance) ifVsResultTask;
        task->init();
        unkCC = task;
        task->loadResource(static_cast<stCommonGimmick*>(g_Stage)->m_fileData);
    }
}
