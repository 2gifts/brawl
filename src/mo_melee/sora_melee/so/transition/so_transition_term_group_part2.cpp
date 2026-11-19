#define SO_TRANSITION_TERM_GROUP_UNIT
#define SO_INSTANCE_UNIT_LOCAL_TEMPLATE
#include <so/transition/so_transition_module_impl.h>

typedef soInstanceUnitFullProperty<soTransitionTerm> TermUnit;
typedef soInstanceManagerFullPropertyEccentric<soTransitionTerm> TermManager;

static inline s32 searchTerm(TermManager* manager, int unitId) {
    for (s32 i = 0; i < manager->m_array->size(); i++) {
        TermUnit unit = manager->m_array->at(i);
        if (unitId == unit.m_id)
            return i;
    }
    return -1;
}

// The unit record constructor (fn_27_ABB78) is the instantiation of the template in the header.
template soInstanceUnitFullProperty<soTransitionTerm>::soInstanceUnitFullProperty(soTransitionTerm&, s32, soAttributeFlag, s16);

static inline void addGeneralTermTo(soTransitionTermGroup* group, int unitID, soGeneralTerm* term) {
    if (group->m_transitionTermInstanceManager.m_array->size() > 0) {
        if (unitID <= -1) {
            group->m_transitionTermInstanceManager.m_array->at(group->m_transitionTermInstanceManager.m_array->size() - 1).m_element.addGeneralTerm(term);
        } else {
            s32 idx = searchTerm(&group->m_transitionTermInstanceManager, unitID);
            if (idx >= 0)
                group->m_transitionTermInstanceManager.m_array->at(idx).m_element.addGeneralTerm(term);
        }
    }
}

void soTransitionTermGroup::addGeneralTerm(int unitID, soGeneralTerm term) {
    addGeneralTermTo(this, unitID, &term);
}

void soTransitionTermGroup::addGeneralTermLastTerm(soGeneralTerm term) {
    addGeneralTermTo(this, m_unitID, &term);
}

// Same body as the destructor in the previous unit (the destructor inlines it).
static inline void clearTerms(soTransitionTermGroup* self) {
    soArrayVector<soTransitionTerm*, 25> terms;
    for (s32 i = 0; i < self->m_transitionTermInstanceManager.m_array->size(); i++) {
        soTransitionTerm* term = &self->m_transitionTermInstanceManager.m_array->at(i).m_element;
        terms.push(term);
    }
    for (s32 i = 0; i < terms.size(); i++) {
        if (terms.at(i) != 0)
            terms.at(i)->clearGeneralTerm();
    }
    self->m_transitionTermInstanceManager.m_array->clear();
    self->m_unitID = -1;
}

void soTransitionTermGroup::clearTransitionTermAll() {
    clearTerms(this);
}

bool soTransitionTermGroup::isFull() {
    return m_transitionTermInstanceManager.m_array->isFull();
}
