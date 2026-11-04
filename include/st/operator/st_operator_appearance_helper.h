#pragma once

#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorAppearanceHelper : public stOperator {
public:
    void endOperator();
    virtual void processBegin();
    virtual ~stOperatorAppearanceHelper();
};
