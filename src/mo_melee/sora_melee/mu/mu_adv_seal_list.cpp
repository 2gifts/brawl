#include <mu/adv/mu_adv_seal_list.h>
#include <math.h>
#include <types.h>

static bool update_pos(float* pos, float* speed, float target, float rate, float damp);


void muAdvSealList::init(int max, int dispRows, int cols, bool wrap) {
    m_max = max;
    m_rows = max / cols;
    m_dispRows = dispRows;
    m_flags = 0;
    m_scroll.y = 0;
    m_cur.y = 0;
    unk2 = 0;
    unk3 = 0;
    unk4 = 0;
    if (!wrap) {
        m_noWrap = true;
    }
    m_cols = cols;
    m_dispCols = cols;
    m_scroll.x = 0;
    m_cur.x = 0;
    reset();
    m_isBarMode = false;
    m_isBarMode2 = false;
    m_isChanged = true;
}

void muAdvSealList::setMax(int max) {
    m_max = max;
    m_rows = max / m_cols;
    reset();
}

void muAdvSealList::flush() {
    reset();
}

void muAdvSealList::setCurIdx(int index) {
    m_cur.y = index / m_cols;
    m_cur.x = index - m_cur.y * m_cols;
}

u32 muAdvSealList::setBarMode(bool barMode, bool barMode2) {
    if (barMode && m_rows > m_dispRows) {
        m_isBarMode = true;
    } else {
        m_isBarMode = false;
    }
    if (barMode2) {
        m_isBarMode2 = true;
    } else {
        m_isBarMode2 = false;
    }
    m_isChanged = true;
    return m_isBarMode;
}

void muAdvSealList::update() {
    if (m_isScrollMove) {
        m_isUpdated = true;
        bool done = true;
        if (!update_pos(&m_scrollPos.y, &m_scrollSpeed.y, m_scroll.y, 0.25f, 0.5f)) {
            done = false;
        }
        if (m_cols > 1) {
            if (!update_pos(&m_scrollPos.x, &m_scrollSpeed.x, m_scroll.x, 0.25f, 0.5f)) {
                done = false;
            }
        }
        if (done) {
            m_flags &= 0xFE;
        }
    }
    if (m_flags & 2) {
        bool done = true;
        if (!update_pos(&m_dispCurPos.y, &m_dispCurSpeed.y, m_dispCur.y, 0.125f, 0.25f)) {
            done = false;
        }
        if (m_cols > 1) {
            if (!update_pos(&m_dispCurPos.x, &m_dispCurSpeed.x, m_dispCur.x, 0.125f, 0.25f)) {
                done = false;
            }
        }
        if (done) {
            m_isCursorMove = false;
        }
    }
}

static bool update_pos(float* pos, float* speed, float target, float rate, float damp) {
    bool done = false;
    float accel = rate * (target - *pos);
    if (__fabsf(accel) < 0.015625f) {
        if (accel >= 0.0f) {
            accel = 0.015625f;
        } else {
            accel = -0.015625f;
        }
    }
    *speed = *speed + damp * (accel - *speed);
    *pos = *pos + *speed;
    if (*speed > 0.0f) {
        if (*pos >= target) {
            done = true;
        }
    } else if (*speed < 0.0f) {
        if (*pos <= target) {
            done = true;
        }
    }
    if (done) {
        *pos = target;
        *speed = 0.0f;
    }
    return done;
}
