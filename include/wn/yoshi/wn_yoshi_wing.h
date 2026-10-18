#pragma once

#include <wn/weapon.h>

// Partial call-side view. Only the Weapon base and activation interface are
// recovered; do not instantiate this view or use it to determine article size.
class wnYoshiWing : public Weapon {
public:
    void activate(int founderTaskId, int resourceId, int team);
};
