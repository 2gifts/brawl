#define SO_TRANSITION_TERM_GROUP_UNIT
#define SO_INSTANCE_UNIT_LOCAL_TEMPLATE
#define SO_INSTANCE_UNIT_EXTERNAL_GET_ATTRIBUTE
#include <so/transition/so_transition_module_impl.h>

typedef soInstanceUnitFullProperty<soTransitionTerm> TermUnit;
typedef soInstanceManagerFullPropertyEccentric<soTransitionTerm> TermManager;

void TermManager::getAttributeArray(soAttributeFlag mask, soArray<soTransitionTerm*>& arr) {
    for (s32 i = 0; i < m_array->size(); i++) {
        TermUnit unit = m_array->at(i);
        soAttributeFlag flag = unit.getAttribute();
        if (flag.m_mask & mask.m_mask) {
            soTransitionTerm* term = &unit.m_element;
            arr.push(term);
        }
    }
}

static inline s32 searchTerm(const TermManager* manager, s32 id) {
    for (s32 i = 0; i < manager->m_array->size(); i++) {
        TermUnit unit = manager->m_array->at(i);
        if (id == unit.m_id)
            return i;
    }
    return -1;
}

soAttributeFlag TermManager::getAttribute(s32 id) const {
    if (searchTerm(this, id) < 0)
        return soAttributeFlag();
    s32 idx = searchTerm(this, id);
    if (idx < 0)
        return soAttributeFlag();
    return m_array->at(idx).getAttribute();
}

void TermManager::getPriorityArray(soArray<soTransitionTerm*>& arr) {
    for (s32 i = 0; i < m_array->size(); i++) {
        soTransitionTerm* term = &m_array->at(i).m_element;
        arr.push(term);
    }
}
