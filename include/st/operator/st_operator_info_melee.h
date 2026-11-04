#pragma once

#include <st/operator/st_operator_info.h>
#include <types.h>

class stOperatorInfoMelee : public stOperatorInfo {
public:
    bool m_isEnableOperation : 1;
    bool m_isStopRequest : 1;
    bool m_11b_5 : 1;
    bool m_11b_4 : 1;
    bool m_11b_3 : 1;
    bool m_11b_2 : 1;
    bool m_11b_1 : 1;
    bool m_11b_0 : 1;
    bool m_11c_7 : 1;
    union {
        char _11d[0x1f];
        struct {
            char _pad11d[0x1b];
            s8 m_138;
        };
    };
    u32 m_13c;
    u32 m_140;
    u32 m_144;
    u32 m_148;
    u32 m_14c;
    u32 m_150;

    stOperatorInfoMelee();
    virtual void processBegin();
    virtual ~stOperatorInfoMelee();
    virtual void startOperator();
    virtual bool isEnableOperation();
    virtual u32 getEndType();
    virtual u32 getErrorLevel();
    virtual void stopOperatorRequest();
};
