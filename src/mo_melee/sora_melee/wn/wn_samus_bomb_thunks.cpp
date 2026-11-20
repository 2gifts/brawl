// The vtable adjustor thunks of wnSamusBomb sit in a separate text range of the original unit.
// TODO: clean up - this unit only re-compiles wn_samus_bomb.cpp so the thunks emit; the real fix is merging the two text ranges into one unit.
#include "../src/mo_melee/sora_melee/wn/wn_samus_bomb.cpp"
