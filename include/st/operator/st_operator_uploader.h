#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <types.h>

class nteWatching;

// Uploads the match record over Wi-Fi once the operator starts; processBegin is a line-numbered state machine.
class stOperatorUploader : public stOperator {
public:
    // HYPOTHESIS: the resumable step of processBegin (a value and the source line to resume at).
    struct Step {
        u32 m_value;
        int m_line;
        Step();
        void reset();
    };

    Step m_step; // 0x48
    bool m_isDone; // 0x50
    nteWatching* m_uploader; // 0x54

    stOperatorUploader();
    virtual void processBegin();
    virtual ~stOperatorUploader();
};
static_assert(sizeof(stOperatorUploader) == 0x58, "Class is wrong size!");
