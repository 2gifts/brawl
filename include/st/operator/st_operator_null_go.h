#pragma once

#include <st/operator/st_operator_ready_go.h>
#include <types.h>

// The ready-go operator of the modes that start without the countdown (the map pairs it with stOperatorRuleMelee):
// optionally runs the go step (m_runGo) or starts the match at once.
class stOperatorNullGo : public stOperatorReadyGo {
public:
    bool m_runGo; // 0x100

    stOperatorNullGo(bool runGo);
    virtual void processBegin();
    virtual ~stOperatorNullGo();
    virtual bool isEnd();
    virtual void startOperator();
};
