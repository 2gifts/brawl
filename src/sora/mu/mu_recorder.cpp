#include <mu/mu_collection.h>
#include <mu/mu_recorder.h>
#include <string.h>

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

void muRecorder::saveAdv(u32 slot) {
    m_slot = slot;
    m_request = Request_SaveAdv;
}

void muRecorder::loadAdv(u32 slot) {
    m_slot = slot;
    m_request = Request_LoadAdv;
}
