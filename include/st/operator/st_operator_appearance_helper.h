#pragma once

#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorAppearanceHelper : public stOperator {
public:
    s32 unk48; // HYPOTHESIS: set to 1 by startOperator

    void startOperator();
    void endOperator();
    virtual void processBegin();
    virtual ~stOperatorAppearanceHelper();
};
