#include <mu/adv/mu_adv_seal_list.h>
#include <math.h>
#include <snd/snd_system.h>
#include <types.h>

static bool input_core(int dir, int trigger, s8 stick, int power, int* cur, int* scroll, int* disp, int max, u8* flags,
                       u8 repeat, int* dispCur, float* dispCurPos, float* dispCurSpeed, float* scrollPos,
                       float* scrollSpeed, u8* hold, u8 noSe);

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

bool muAdvSealList::input(u32 buttons, u32 trigger, s8 stick) {
    bool changed = false;
    s8 absStick = stick < 0 ? -stick : stick;
    int power = absStick - 0x20;
    if (power <= 0) {
        unk2 = 0;
    }
    if (input_core(((buttons & 4) ? 1 : 0) + ((buttons & 8) ? -1 : 0), trigger & 0xC, stick, power, &m_cur.y, &m_scroll.y,
                   &m_dispRows, m_rows, &m_flags, unk2, &m_dispCur.y, &m_dispCurPos.y, &m_dispCurSpeed.y,
                   &m_scrollPos.y, &m_scrollSpeed.y, &unk4, unk3)) {
        changed = true;
    }
    if (m_cols > 1) {
        if (input_core(((buttons & 2) && m_cur.x != m_cols - 1 ? 1 : 0) + ((buttons & 1) && m_cur.x != 0 ? -1 : 0), 0, 0, power, &m_cur.x, &m_scroll.x, &m_dispCols, m_cols, &m_flags, unk2,
                       &m_dispCur.x, &m_dispCurPos.x, &m_dispCurSpeed.x, &m_scrollPos.x, &m_scrollSpeed.x, &unk4,
                       unk3)) {
            changed = true;
        }
    }
    if (unk2 < 4) {
        unk2++;
    }
    if (unk3 < 4 && power > 0) {
        unk3++;
    } else {
        unk3 = 0;
    }
    return changed;
}


static bool input_core(int dir, int trigger, s8 stick, int power, int* cur, int* scroll, int* disp, int max, u8* flags,
                       u8 repeat, int* dispCur, float* dispCurPos, float* dispCurSpeed, float* scrollPos,
                       float* scrollSpeed, u8* hold, u8 noSe) {
    int newCur = *cur;
    int newScroll = *scroll;
    if (*hold & 1) {
        if (trigger != 0 || power > 0) {
            *hold = 0;
        }
    } else if (*hold & 2) {
        if (power <= 0) {
            *hold = 0;
        }
    }
    u8 f = *flags;
    if ((f & 0x10) || power > 0) {
        if (max > *disp && *hold == 0) {
            bool wrap = false;
            int step;
            if (power > 0) {
                if (repeat < 4 && power >= 16) {
                    wrap = true;
                }
                step = power / 32 + 1;
            } else if (f & 0x20) {
                step = 1;
                if (trigger != 0) {
                    wrap = true;
                }
            } else if (trigger != 0) {
                step = *disp;
                wrap = true;
            } else {
                step = *disp > (int)(max * (1.0f / 13.0f)) ? *disp : (int)(max * (1.0f / 13.0f));
            }
            if (dir > 0 || (power > 0 && stick < 0)) {
                int last = max - *disp;
                if (newScroll == last) {
                    if (wrap) {
                        newScroll = 0;
                        noSe = 0;
                        if (dir > 0) {
                            *hold |= 1;
                        } else {
                            *hold |= 2;
                        }
                    }
                } else {
                    newScroll += step;
                    if (newScroll > last) {
                        newScroll = last;
                    }
                }
            } else if (dir < 0 || (power > 0 && stick > 0)) {
                if (newScroll == 0) {
                    if (wrap) {
                        newScroll = max - *disp;
                        noSe = 0;
                        if (dir > 0) {
                            *hold |= 1;
                        } else {
                            *hold |= 2;
                        }
                    }
                } else {
                    newScroll -= step;
                    if (newScroll < 0) {
                        newScroll = 0;
                    }
                }
            }
            newCur = newScroll + (*cur - *scroll);
            if (newScroll != *scroll && !noSe) {
                g_sndSystem->playSE(static_cast<SndID>(7), -1, 0, 0, -1);
            }
        }
    } else {
        if (dir > 0) {
            newCur++;
            if (newCur >= max) {
                newCur = 0;
                newScroll = 0;
            } else {
                int d = *disp;
                int top = newCur - (d - 3);
                int last = max - d;
                if (top > last) {
                    top = last;
                }
                if (top > newScroll && top <= last) {
                    newScroll = top;
                }
            }
        } else if (dir < 0) {
            newCur--;
            if (newCur < 0) {
                newCur = max - 1;
                newScroll = max - *disp;
                if (newScroll < 0) {
                    newScroll = 0;
                }
            } else {
                int top = newCur - 2;
                if (top < 0) {
                    top = 0;
                }
                if (top < newScroll && top >= 0) {
                    newScroll = top;
                }
            }
        }
        if (newCur != *cur && !(f & 0x40)) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
    }
    if (newCur != *cur || newScroll != *scroll) {
        *cur = newCur;
        *scroll = newScroll;
        *flags |= 1;
        if (*scroll > *scrollPos && *scrollSpeed < 0.5f) {
            *scrollSpeed = 0.5f;
        } else if (*scroll < *scrollPos && *scrollSpeed > -0.5f) {
            *scrollSpeed = -0.5f;
        }
        int dc = *cur - *scroll;
        if (*dispCur != dc) {
            *dispCur = dc;
            *flags |= 2;
            if (*dispCur > *dispCurPos && *dispCurSpeed < 0.5f) {
                *dispCurSpeed = 0.5f;
            } else if (*dispCur < *dispCurPos && *dispCurSpeed > -0.5f) {
                *dispCurSpeed = -0.5f;
            }
        }
        return true;
    }
    return false;
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
