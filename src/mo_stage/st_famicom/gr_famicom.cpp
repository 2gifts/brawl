#include <types.h>

#include <st_famicom/gr_famicom.h>

// MATCH-ONLY: deadstripped function that makes this unit emit Ground::setMdlIndex first (as in gr_dxbigblue.cpp)
static void emitSetMdlIndex(grFamicom* ths, s32 mdlIndex) {
    ths->setMdlIndex(mdlIndex);
}

grFamicom::grFamicom(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grFamicom::~grFamicom() {
}
