#include <st_homerun/gr_homerun.h>

// MATCH-ONLY: deadstripped function that makes this unit emit Ground::setMdlIndex first (as in gr_famicom.cpp)
static void emitSetMdlIndex(grHomerun* ths, s32 mdlIndex) {
    ths->setMdlIndex(mdlIndex);
}

grHomerun::grHomerun(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grHomerun::~grHomerun() { }
