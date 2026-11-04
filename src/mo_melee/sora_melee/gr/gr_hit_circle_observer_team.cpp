#include <gr/gr_hit_circle_observer.h>
#include <types.h>

grHitCircleObserverTeam::~grHitCircleObserverTeam() {
    m_current = m_start;
    for (u32 i = 0, count = m_count; i < count; i++) {
        delete m_current->m_data;
        m_current->m_data = NULL;
        m_current = m_current->m_next;
    }
    clear();
}

void grHitCircleObserverTeam::enableHitCircle(bool isEnable, u32 id) {
    m_current = m_start;
    bool anyEnabled = false;
    for (u32 i = 0, count = m_count; i < count; i++) {
        grHitCircleData* circle = m_current->m_data;
        if (id == circle->m_id) {
            circle->m_isEnable = isEnable;
        }
        if (m_current->m_data->m_isEnable) {
            anyEnabled = true;
        }
        m_current = m_current->m_next;
    }
    m_isEnable = anyEnabled;
}

void grHitCircleObserverTeam::enableHitCircle(bool isEnable) {
    m_current = m_start;
    u32 count = m_count;
    for (u32 i = 0; i < count; i++) {
        m_current->m_data->m_isEnable = isEnable;
        m_current = m_current->m_next;
    }
    m_isEnable = isEnable;
}

void grHitCircleObserverTeam::setPos(Vec2f* pos, u32 id) {
    m_current = m_start;
    for (u32 i = 0, count = m_count; i < count; i++) {
        grHitCircleData* circle = m_current->m_data;
        if (id == circle->m_id) {
            float y;
            float x;
            x = pos->m_x;
            y = pos->m_y;
            circle->m_pos.m_x = x;
            circle->m_pos.m_y = y;
            return;
        }
        m_current = m_current->m_next;
    }
}
