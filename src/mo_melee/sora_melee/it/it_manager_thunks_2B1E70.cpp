// The two vtable adjustor thunks of itManager's second base sit in a separate text range of the original it_manager unit.
// TODO: clean up - this unit only re-compiles it_manager.cpp so the thunks emit; the real fix is merging the text ranges into one unit.
#include "../src/mo_melee/sora_melee/it/it_manager.cpp"
