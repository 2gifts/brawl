#define SO_GENERAL_TERM_MANAGER_IMPL
#include <memory.h>
#include <so/transition/so_transition_module_impl.h>
#include <types.h>

soGeneralTermManager g_soGeneralTermManager;

// HYPOTHESIS: the free-list setup is an inline helper of the pool (the arguments are loaded before the stores).
static inline void soGeneralTermManagerSetPool(soGeneralTermManager* manager, soGeneralTerm* terms, s16* indices, int num) {
    manager->_unk08 = 0;
    manager->m_generalTerms = terms;
    manager->m_indices = indices;
    manager->m_numGeneralTerms = num;
    for (s16 i = 0; i < manager->m_numGeneralTerms - 1; i++) {
        manager->m_indices[i] = i + 1;
    }
    manager->m_indices[(s16)(manager->m_numGeneralTerms - 1)] = -1;
}

void soGeneralTermManager::initialize(int num, Heaps::HeapType heapType) {
    if (m_generalTerms2 != NULL || m_indices2 != NULL) {
        return;
    }
    m_generalTerms2 = new (heapType) soGeneralTerm[num];
    m_indices2 = new (heapType) s16[num];
    soGeneralTermManagerSetPool(this, m_generalTerms2, m_indices2, num);
}
