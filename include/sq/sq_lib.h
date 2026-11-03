#pragma once

#include <gm/gm_lib.h>

// HYPOTHESIS: character kinds and color indices follow the callers' integer registers.
gmCharacterKind sqGetPatchCHKind(gmCharacterKind kind);
int sqGetPatchInfoColor(gmCharacterKind kind, int color, int player);
void sqWriteMetamorphosisHumanFighter();
