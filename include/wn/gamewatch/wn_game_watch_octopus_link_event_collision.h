#pragma once
#include <so/link/so_link_event_presenter.h>

struct wnGameWatchOctopusLinkEventCollision : soLinkEventArgs {
    float m_power;
    // HYPOTHESIS: this flag distinguishes hit callbacks from accumulated attacks.
    bool unkC;
    wnGameWatchOctopusLinkEventCollision(int eventKind, float power, bool eventFlag);
};
static_assert(sizeof(wnGameWatchOctopusLinkEventCollision) == 0x10, "Octopus collision event extent");
