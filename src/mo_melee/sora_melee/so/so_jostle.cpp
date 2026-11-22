#include <gf/gf_area.h>
#include <mt/mt_vector.h>
#include <nw4r/math.h>
#include <nw4r/ut/ut_algorithm.h>
#include <so/jostle/so_jostle.h>
#include <st/stage.h>
#include <stdlib.h>
#include <types.h>

extern "C" int randi__Fl(long);


soJostle::soJostle(u8 priority, s32 arg) {
    m_area = NULL;
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

// HYPOTHESIS: speed is the push speed of this entry, otherSpeed/otherWidth the other entry's speed and push width
// (0 = take the width from the jostle data for this entry's size kind m_0x1B).
void soJostle::move(float* speed, Vec2f* pos, Vec2f* otherPos, soJostleLog* log, float otherSpeed, float otherWidth) {
    float moveSpeed = otherSpeed;
    float width = otherWidth;

    if (0.0f == otherWidth) {
        if (m_0x1B >= 2) {
            width = getData()[1];
        } else {
            width = getData()[m_0x1B];
        }
    }

    if (pos->m_x == otherPos->m_x) {
        if (0.0f == *speed) {
            if (randi__Fl(2) == 0) {
                width = width;
            } else {
                width = -width;
            }
            *speed = width;
        } else {
            if (*speed < 0.0f) {
                width = width;
            } else {
                width = -width;
            }
            *speed = width;
        }
    } else if (pos->m_x > otherPos->m_x) {
        *speed = width;
    } else {
        *speed = -width;
    }

    soJostleLog* entry = &log[m_logNo];
    float dist = (float)(double)nw4r::math::FAbs(otherPos->m_x - pos->m_x);

    if (*speed > 0.0f) {
        if (entry->m_front == 0 || dist < entry->m_frontDist) {
            if (0.0f == moveSpeed) {
                entry->m_front = 2;
            } else if (moveSpeed > 0.0f) {
                entry->m_front = 3;
            } else {
                entry->m_front = 1;
            }
            entry->m_frontDist = dist;
        }
    } else if (entry->m_back == 0 || dist < entry->m_backDist) {
        if (0.0f == moveSpeed) {
            entry->m_back = 2;
        } else if (moveSpeed > 0.0f) {
            entry->m_back = 3;
        } else {
            entry->m_back = 1;
        }
        entry->m_backDist = dist;
    }

    u32 both = (entry->m_front != 0 && entry->m_back != 0);
    if (both == 1) {
        m_0x8 = 0.0f;
        return;
    }
    m_0x8 = *speed;
}

void soJostle::check(soJostle* other, soJostleLog* log, bool both) {
    if (m_area != NULL && m_area->testIntersect(other->m_area) == true) {
        bool otherMove = false;
        bool thisMove = false;
        if ((s8)m_priority == (s8)other->m_priority) {
            otherMove = true;
            thisMove = true;
        } else {
            if ((s8)m_priority > (s8)other->m_priority) {
                otherMove = true;
            }
            if ((s8)m_priority <= (s8)other->m_priority) {
                thisMove = true;
            }
        }
        Vec2f pos = m_area->m_pos;
        Vec2f otherPos = other->m_area->m_pos;
        float speed = 0.0f;
        if (thisMove == true) {
            move(&speed, &pos, &otherPos, log, other->m_0x10, other->m_0xC);
        }
        if (both == true && otherMove == true) {
            other->move(&speed, &otherPos, &pos, log, m_0x10, m_0xC);
        }
    }
}

void soJostle::updatePosZ(soJostleLog* log) {
    int front;
    int dir = 2;
    if (log != NULL) {
        soJostleLog* entry = &log[m_logNo];
        front = entry->m_front;
        u32 both = 0;
        if (front != 0 && entry->m_back != 0) {
            both = 1;
        }
        if (static_cast<unsigned long>(both) == 1UL) {
            int back = entry->m_back;
            if (front == back) {
                if (front == 2) {
                    dir = m_0x19;
                } else if (front == 3) {
                    dir = 1;
                } else {
                    dir = 3;
                }
            } else if (__abs(front - back) >= 2) {
                dir = m_0x19;
            } else if (front == 1 || back == 1) {
                dir = 3;
            } else {
                dir = 1;
            }
        } else {
            int neither = 0;
            if (front == 0 && entry->m_back == 0) {
                neither = 1;
            }
            if (neither == 0) {
                if (front == 0) {
                    front = entry->m_back;
                }
                if (front == 1) {
                    dir = 3;
                } else if (front == 3) {
                    dir = 1;
                } else if (m_0x8 < 0.0f) {
                    dir = 1;
                } else {
                    dir = 3;
                }
            }
        }
    }
    int forced = (s8)m_0x1A;
    float accel = getData()[2];
    float maxSpeed = getData()[3];
    if (forced != 0 && dir != 2) {
        dir = forced;
    }
    switch (dir) {
        case 2:
            if ((float)(double)nw4r::ut::Abs(m_0x10) < accel) {
                m_0x10 = 0.0f;
            } else if (m_0x10 > 0.0f) {
                m_0x10 -= accel;
            } else {
                m_0x10 += accel;
            }
            break;
        case 3:
            m_0x10 += accel;
            if (m_0x10 > maxSpeed) {
                m_0x10 = maxSpeed;
            }
            break;
        case 1:
            m_0x10 -= accel;
            if (m_0x10 < -maxSpeed) {
                m_0x10 = -maxSpeed;
            }
            float limit = g_Stage->getEnableZ();
            if (limit > 0.0f) {
                if (m_0x10 < -limit) {
                    m_0x10 = -limit;
                }
            }
            break;
    }
}

void soJostle::setPriority(u8 priority) {
    m_priority = priority;
}
