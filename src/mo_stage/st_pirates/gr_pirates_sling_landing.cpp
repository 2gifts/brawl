#include <gr/gr_pirates_sling.h>
#include <mt/mt_prng.h>

struct PiratesSlingLandingParams {
    u8 unk0[0x1C];
    float landingMin;
    float landingMax;
};

// A fighter landing on the raised sling shortens the time it stays up (to a random time that is not longer).
void grPiratesSling::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    PiratesSlingLandingParams* params = static_cast<PiratesSlingLandingParams*>(getStageData());
    if (params != NULL) {
        switch (m_state) {
        case State_Active:
            if (m_timer > params->landingMin) {
                float random = randf();
                float span = params->landingMax - params->landingMin;
                m_timer = params->landingMin + span * random;
            }
            break;
        }
    }
}
