#include <so/transition/so_transition_module_impl.h>
#include <types.h>

// The cache keeps one "is known" mask and one "value" mask for the general term flag (1 entry) and for the controller
// button on / trigger states (17 entries each).

void soGeneralTermCache::clearAll() {
    m_flags = 0;
    m_flags2 = 0;
    m_buttonOnMask = 0;
    m_buttonOnMask2 = 0;
    m_buttonTriggerMask = 0;
    m_buttonTriggerMask2 = 0;
}

void soGeneralTermCache::clearController() {
    m_buttonOnMask = 0;
    m_buttonOnMask2 = 0;
    m_buttonTriggerMask = 0;
    m_buttonTriggerMask2 = 0;
}

bool soGeneralTermCache::isFlag(u32 flagID, u8* resultOut) {
    if ((s32)flagID <= -1 || (s32)flagID >= 1) {
        return false;
    }
    u32 mask = 1 << flagID;
    if (m_flags & mask) {
        *resultOut = (m_flags2 & mask) != 0;
        return true;
    }
    return false;
}

void soGeneralTermCache::setFlag(u32 flagID, bool saveToMask2) {
    if ((s32)flagID <= -1 || (s32)flagID >= 1) {
        return;
    }
    u32 mask = 1 << flagID;
    m_flags |= mask;
    if (saveToMask2 == true) {
        m_flags2 |= mask;
    } else {
        m_flags2 &= ~mask;
    }
}

bool soGeneralTermCache::isButtonOn(u32 buttonID, u8* resultOut) {
    if ((s32)buttonID <= -1 || (s32)buttonID >= 17) {
        return false;
    }
    u32 mask = 1 << buttonID;
    if (m_buttonOnMask & mask) {
        *resultOut = (m_buttonOnMask2 & mask) != 0;
        return true;
    }
    return false;
}

void soGeneralTermCache::setButtonOn(u32 buttonID, bool saveToMask2) {
    if ((s32)buttonID <= -1 || (s32)buttonID >= 17) {
        return;
    }
    u32 mask = 1 << buttonID;
    m_buttonOnMask |= mask;
    if (saveToMask2 == true) {
        m_buttonOnMask2 |= mask;
    } else {
        m_buttonOnMask2 &= ~mask;
    }
}

bool soGeneralTermCache::isButtonTrigger(u32 buttonID, u8* resultOut) {
    if ((s32)buttonID <= -1 || (s32)buttonID >= 17) {
        return false;
    }
    u32 mask = 1 << buttonID;
    if (m_buttonTriggerMask & mask) {
        *resultOut = (m_buttonTriggerMask2 & mask) != 0;
        return true;
    }
    return false;
}

void soGeneralTermCache::setButtonTrigger(u32 buttonID, bool saveToMask2) {
    if ((s32)buttonID <= -1 || (s32)buttonID >= 17) {
        return;
    }
    u32 mask = 1 << buttonID;
    m_buttonTriggerMask |= mask;
    if (saveToMask2 == true) {
        m_buttonTriggerMask2 |= mask;
    } else {
        m_buttonTriggerMask2 &= ~mask;
    }
}

soGeneralTermCache::soGeneralTermCache() {
    m_flags = 0;
    m_flags2 = 0;
    m_buttonOnMask = 0;
    m_buttonOnMask2 = 0;
    m_buttonTriggerMask = 0;
    m_buttonTriggerMask2 = 0;
}

soGeneralTermCache::~soGeneralTermCache() { }
