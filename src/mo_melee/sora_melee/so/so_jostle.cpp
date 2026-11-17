#include <so/jostle/so_jostle.h>
#include <types.h>

soJostle::soJostle(u8 priority, s32 arg) {
    m_0x0 = 0;
    m_0x4 = arg;
    m_0x14 = 0xFFFF;
    m_priority = priority;
    m_0x19 = 2;
    m_0x1A = 0;
    m_0x8 = 0.0f;
    m_logNo = -1;
    m_0x10 = 0.0f;
    m_0xC = 0.0f;
}

soJostle::~soJostle() { }

void soJostle::reset() {
    m_logNo = -1;
    m_0x8 = 0.0f;
    m_0x10 = 0.0f;
    m_0xC = 0.0f;
}

void soJostle::initLog() {
    m_logNo = -1;
    m_0x8 = 0.0f;
}

bool soJostle::setLogNo(s32 logNo) {
    if (m_logNo == -1) {
        m_logNo = logNo;
        return true;
    }
    return false;
}

void soJostle::setPriority(u8 priority) {
    m_priority = priority;
}
