#include <gr/gr_pirates.h>

// MATCH-ONLY: the original emits the weak Ground::setMdlIndex before the constructor, as if a function using it came
// first and was dropped; this unused static helper reproduces that position (a source-level cause was not found).
static void unusedSetMdlIndex(grPirates* ground) {
    ground->setMdlIndex(0);
}

grPirates::grPirates(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_ctrlFrame = NULL;
    unk15C = NULL;
    setupMelee();
}

grPirates::~grPirates() { }
