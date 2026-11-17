#include <types.h>

// HYPOTHESIS: a pair that the figure menus keep two of in every unit (an id that is 0xFF while it is not used, and a
// state); the pairs are made when the unit starts.
struct muFigPair {
    int m_id;
    int m_state;

    muFigPair(int id, int state) : m_id(id), m_state(state) { }
};

static muFigPair s_pair0(0xFF, 0);
static muFigPair s_pair1(0xFF, 1);
