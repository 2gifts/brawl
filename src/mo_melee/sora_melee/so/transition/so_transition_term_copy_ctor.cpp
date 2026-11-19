#define SO_TRANSITION_TERM_GROUP_UNIT
#include <so/transition/so_transition_module_impl.h>

// TODO: clean up -- the target kind is copied through (s16) because the original copies it with a signed halfword load;
// soTransitionTerm::m_targetKind probably has a signed type.
soTransitionTerm::soTransitionTerm(const soTransitionTerm& other) {
    m_flags = other.m_flags;
    m_targetKind = (s16)other.m_targetKind;
    m_generalTermIndex = other.m_generalTermIndex;
}
