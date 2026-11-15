#include <ft/ft_audience_manager.h>
#include <sr/sr_common.h>
#include <types.h>

// TODO: activate (switches between the audience task and the null audience by its mode argument, which the shadow
// header does not declare yet).

ftAudienceManager* g_ftAudienceManager;

ftAudienceNull::~ftAudienceNull() { }

// The manager starts on the null audience; the audience task is created alongside.
void ftAudienceManager::initialize() {
    g_ftAudienceManager = new (Heaps::System) ftAudienceManager;
}

void ftAudienceManager::deactivate() {
    m_audience->deactivate();
    m_audience = &m_audienceNull;
}

void ftAudienceNull::checkCheerDefeat(int entryId) { }

bool ftAudienceNull::checkTumultFallSpecial(float, int entryId) {
    return false;
}

bool ftAudienceNull::checkTumultLanding(float, float, int entryId) {
    return false;
}

bool ftAudienceNull::checkTumultRange(float) {
    return false;
}

void ftAudienceNull::checkCheer(float, int entryId1, int entryId2) { }

void ftAudienceNull::reqCallFgmStopCheer() { }

float ftAudienceNull::correctCheerReaction(float reaction, float) {
    return reaction;
}

void ftAudienceNull::reqHandclapDead(float, bool) { }

void ftAudienceNull::deactivate() { }

void ftAudienceNull::activate() { }
