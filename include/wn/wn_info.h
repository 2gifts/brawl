#pragma once

#include <types.h>

// Per-article-kind facts that the generic article code asks for (the article kinds are the Weapon sub kinds).
// HYPOTHESIS: the parameter is the article sub kind; names come from the symbol map.
class wnInfo {
public:
    static int getOwnerDeactiveTreatType(s32 kind);
    static bool isFinal(s32 kind);
    static bool isInhaled(s32 kind);
    static bool isSpy(s32 kind);
    static bool isDark(s32 kind);
    static bool isLoupe(s32 kind);
};
