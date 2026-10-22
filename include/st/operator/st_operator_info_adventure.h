#pragma once

#include <st/operator/st_operator_info.h>
#include <types.h>

class stOperatorInfoAdventure : public stOperatorInfo {
public:
    bool m_isEnableOperation;
    bool m_isStopRequest;
    bool m_11d;
    bool m_11e;
    bool m_11f;
    u32 m_120;
    u32 m_124;
    u32 m_128;
    u32 m_12c;

    stOperatorInfoAdventure();
    virtual void processBegin();
    virtual ~stOperatorInfoAdventure();
    virtual void startOperator();
    virtual bool isEnableOperation();
    virtual u32 getEndType();
    virtual u32 getErrorLevel();
    virtual void setPlayerCursorStatus(u8 index, u8 status);
    virtual void setPlayerNameCursorStatus(u8 index, u8 status);
    virtual void setPlayerLoupeStatus(u8 index, u8 status);
    virtual void updatePlayerCursor(bool);
    virtual void renderDebug();
    virtual void stopOperatorRequest();
};
