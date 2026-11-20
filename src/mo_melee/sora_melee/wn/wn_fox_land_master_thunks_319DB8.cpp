// The soAnimCmdControlUnit instance-manager thunks and the vtable adjustor thunks of wnFoxLandMaster sit in a separate text range of the original wn_fox_land_master unit.
// TODO: clean up - this unit only re-compiles wn_fox_land_master.cpp so these weak functions emit; the real fix is merging the text ranges into one unit.
#include "../src/mo_melee/sora_melee/wn/wn_fox_land_master.cpp"
