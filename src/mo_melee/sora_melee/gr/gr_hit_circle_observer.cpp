#include <gr/gr_hit_circle_observer.h>
#include <types.h>

grHitCircleObserver::~grHitCircleObserver() {
    m_current = m_start;
    for (u32 i = 0, count = m_count; i < count; i++) {
        delete m_current->m_data;
        m_current->m_data = NULL;
        m_current = m_current->m_next;
    }
    clear();
}

void grHitCircleObserver::processUpdate() {
    m_current = m_start;
    u32 count = m_count;
    for (u32 i = 0; i < count; i++) {
        if (!m_current->m_data->m_isDisable) {
            m_current->m_data->update();
            m_current = m_current->m_next;
        }
    }
}

u32 grHitCircleObserver::addHitCircle(grHitCircleData* data) {
    grHitCircleObserverGroup* group = getGroup(data->m_groupIndex);
    if (group == NULL) {
        group = new (Heaps::StageInstance) grHitCircleObserverGroup();
        group->m_groupIndex = data->m_groupIndex;
        if (m_start == NULL) {
            grHitCircleListNode<grHitCircleObserverGroup*>* node =
                new (Heaps::StageInstance) grHitCircleListNode<grHitCircleObserverGroup*>();
            m_current = node;
            m_end = node;
            m_start = node;
            node->m_data = group;
        } else {
            m_end->m_next = new (Heaps::StageInstance) grHitCircleListNode<grHitCircleObserverGroup*>();
            m_end->m_next->m_data = group;
            m_end->m_next->m_prev = m_end;
            m_end = m_end->m_next;
        }
        m_count++;
    }
    data->m_id = m_nextId;
    group->addHitCircle(data);
    m_nextId++;
    return data->m_id;
}

void grHitCircleObserver::enableHitCircle(bool isEnable, u8 groupIndex, int team, u32 id) {
    m_current = m_start;
    for (u32 i = 0, count = m_count; i < count; i++) {
        grHitCircleObserverGroup* group = m_current->m_data;
        if (groupIndex == group->m_groupIndex) {
            group->enableHitCircle(isEnable, team, id);
            return;
        }
        m_current = m_current->m_next;
    }
}

void grHitCircleObserver::enableHitCircle(bool isEnable, u8 groupIndex) {
    m_current = m_start;
    for (u32 i = 0, count = m_count; i < count; i++) {
        grHitCircleObserverGroup* group = m_current->m_data;
        if (groupIndex == group->m_groupIndex) {
            group->enableHitCircle(isEnable);
            return;
        }
        m_current = m_current->m_next;
    }
}

void grHitCircleObserver::setPos(Vec2f* pos, u8 groupIndex, int team, u32 id) {
    m_current = m_start;
    for (u32 i = 0, count = m_count; i < count; i++) {
        grHitCircleObserverGroup* group = m_current->m_data;
        if (groupIndex == group->m_groupIndex) {
            group->setPos(pos, team, id);
            return;
        }
        m_current = m_current->m_next;
    }
}
