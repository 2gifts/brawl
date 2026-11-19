#define SO_TRANSITION_TERM_GROUP_UNIT
#define SO_TRANSITION_TERM_GROUP_ASSIGN
#define FT_MODULE_BUILDER
#define SO_INSTANCE_UNIT_LOCAL_TEMPLATE
#define SO_INSTANCE_UNIT_EXTERNAL_FULL_PROPERTY_CTOR
#include <so/transition/so_transition_module_impl.h>

typedef soInstanceUnitFullProperty<soTransitionTerm> TermUnit;
typedef soInstanceManagerFullPropertyEccentric<soTransitionTerm> TermManager;

// TODO: clean up -- view of the first flag bit of soTransitionTerm::m_flags (the "enabled" bit); soTransitionTerm should
// declare m_flags as a bitfield (or provide enable()/disable()) instead of this cast.
struct TermFlagBits {
    u8 enabled : 1;
    void disable() { enabled = 0; }
};

inline s32 TermManager::unkFindIndex(s32 priority) {
    for (s32 i = 0; i < m_array->size(); i++) {
        s32 current = m_array->at(i).m_10;
        if (current <= -1)
            return i;
        if (current > priority)
            return i;
    }
    return m_array->size();
}

static inline s32 searchTerm(TermManager* manager, int unitId) {
    for (s32 i = 0; i < manager->m_array->size(); i++) {
        TermUnit unit = manager->m_array->at(i);
        if (unitId == unit.m_id)
            return i;
    }
    return -1;
}

soTransitionTermGroup::soTransitionTermGroup() {
    m_transitionTermInstanceManager.m_array = 0;
    m_unitID = -1;
}

soTransitionTermGroup::soTransitionTermGroup(soArray<TermUnit>* terms) {
    m_transitionTermInstanceManager.m_array = terms;
    m_unitID = -1;
}

// Only the term array pointer is copied (fn_27_AB2E0 in the original).
soTransitionTermGroup& soTransitionTermGroup::operator=(const soTransitionTermGroup& other) {
    if (this != &other)
        m_transitionTermInstanceManager.m_array = other.m_transitionTermInstanceManager.m_array;
    return *this;
}

// NEAR-MISS: the original keeps the loop counter in a higher register than the term pointer.
// TODO: clean up -- the u16* attribute mask is read through an s16* cast and the target kind through (s16);
// the term fields should have the signed types, and the "enabled" test should go through a named accessor.
u32 soTransitionTermGroup::checkEstablish(soModuleAccesser* moduleAccesser, u32* targetKindOut, int* termIDOut, u32* returnWord, u16* attrMask, soGeneralTermCache* generalTermCache) {
    *targetKindOut = 0;
    *termIDOut = -1;
    soArray<TermUnit>* arr = m_transitionTermInstanceManager.m_array;
    s32 count = arr->size();
    for (s32 i = 0; i < count; i++) {
        TermUnit& unit = arr->at(i);
        bool skip = false;
        if (*(s16*)attrMask != 0) {
            soAttributeFlag flag = unit.getAttribute();
            if ((flag.m_mask & *(s16*)attrMask) == 0)
                skip = true;
        }
        if (skip)
            continue;
        u32 word = 0;
        if (((TermFlagBits*)&unit.m_element)->enabled) {
            if (unit.m_element.checkEstablish(moduleAccesser, &word, generalTermCache) == 1) {
                *targetKindOut = (s16)unit.m_element.m_targetKind;
                *termIDOut = unit.m_id;
                *returnWord = word;
                return 1;
            }
        }
    }
    return 0;
}

void soTransitionTermGroup::enableTerm(int unitId) {
    s32 idx = searchTerm(&m_transitionTermInstanceManager, unitId);
    if (idx >= 0)
        m_transitionTermInstanceManager.m_array->at(idx).m_element.m_flags |= 0x80;
}

void soTransitionTermGroup::unableTerm(int unitId) {
    s32 idx = searchTerm(&m_transitionTermInstanceManager, unitId);
    if (idx >= 0)
        ((TermFlagBits*)&m_transitionTermInstanceManager.m_array->at(idx).m_element)->disable();
}

void soTransitionTermGroup::enableTermAll() {
    s32 n = m_transitionTermInstanceManager.m_array->size();
    s32 i = 0;
    if (n > 0)
        for (; i < n; i++)
            m_transitionTermInstanceManager.m_array->at(i).m_element.m_flags |= 0x80;
}

void soTransitionTermGroup::unableTermAll() {
    s32 n = m_transitionTermInstanceManager.m_array->size();
    s32 i = 0;
    if (n > 0)
        for (; i < n; i++)
            ((TermFlagBits*)&m_transitionTermInstanceManager.m_array->at(i).m_element)->disable();
}

// The destructor and clearTransitionTermAll (next unit) share this body.
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

soTransitionTermGroup::~soTransitionTermGroup() {
    clearTerms(this);
}

// NEAR-MISS (fn_27_AB900): the original returns the helper result through r3 before the idx < 0 test.
s32 soInstanceManagerFullPropertyEccentric<soTransitionTerm>::add(soTransitionTerm& elm, s32 id, soAttributeFlag attr, s16 p4) {
    if (id > -1) {
        if (searchTerm(this, id) >= 0)
            return -1;
    }
    if (id <= -1)
        id = -1;
    if (m_array->isEmpty() == true) {
        TermUnit unit(elm, id, attr, p4);
        m_array->push(unit);
        return id;
    }
    s32 idx = m_array->size();
    if (p4 >= 0) {
        idx = (p4 <= -1) ? m_array->size() : unkFindIndex(p4);
        if (idx < 0)
            return -1;
    }
    TermUnit unit(elm, id, attr, p4);
    m_array->insert(idx, unit);
    return id;
}

// NEAR-MISS: the original passes the priority to add() without sign-extending it.
// TODO: clean up -- the attribute mask pointer is read through an s16* cast; the flag should come from a typed argument.
int soTransitionTermGroup::addTerm(int unitId, u32 priority, soTransitionTerm* termIn, u16* attr) {
    if (unitId > -1 && searchTerm(&m_transitionTermInstanceManager, unitId) >= 0) {
        s32 idx = searchTerm(&m_transitionTermInstanceManager, unitId);
        soTransitionTerm* term = &m_transitionTermInstanceManager.m_array->at(idx).m_element;
        term->clearGeneralTerm();
        *term = *termIn;
        ((TermFlagBits*)term)->enabled = true;
        m_unitID = unitId;
    } else {
        soAttributeFlag flag(*(s16*)attr);
        m_unitID = m_transitionTermInstanceManager.add(*termIn, unitId, flag, priority);
    }
    return m_unitID;
}
