#include <st_halberd/gr_halberd.h>

// MATCH-ONLY: deadstripped function that makes this unit emit Ground::setMdlIndex first (as in gr_famicom.cpp)
static void emitSetMdlIndex(grHalberd* ths, s32 mdlIndex) {
    ths->setMdlIndex(mdlIndex);
}

grHalberd::grHalberd(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_frameWork = NULL;
    m_stateWork = NULL;
    setupMelee();
}

grHalberd::~grHalberd() {
}
