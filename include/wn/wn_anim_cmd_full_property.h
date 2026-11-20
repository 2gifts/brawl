#pragma once
// The "full property" configuration of an article's animation command threads: like the default configuration
// (wn/wn_module_accesser_builder.h) with one more value per thread, the capacity of that thread's interpreter stack.
// The uniq interpreter thread 4 takes the stack capacity of thread 0. HYPOTHESIS: the two unnamed values per thread.
#include <wn/wn_module_accesser_builder.h>

#ifndef WN_ANIM_CMD_FULL_PROPERTY_CONFIG
#define WN_ANIM_CMD_FULL_PROPERTY_CONFIG
template <int E0, int C0, int A0, int B0, int S0, int E1, int C1, int A1, int B1, int S1, int E2, int C2, int A2, int B2, int S2, int E3,
          int C3, int A3, int B3, int S3>
class wnAnimCmdModuleSubBuildConfigFullProperty {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, C0, A0, B0, 0, 0, S0> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, C1, A1, B1, 1, 0, S1> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, C2, A2, B2, 1, 0, S2> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, C3, A3, B3, 1, 0, S3> Unit3Config;
#ifdef WN_ANIM_CMD_UNIQ_STACK_OF_UNIT0 // opt-in (the Pokemon seed): the uniq thread takes the stack capacity of thread 0
    typedef soAnimCmdControlUnitBuildConfigUniq<4, S0> Unit4Config;
#else
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 2> Unit4Config;
#endif
    enum { Unit0Enabled = E0, Unit1Enabled = E1, Unit2Enabled = E2, Unit3Enabled = E3, Unit4Enabled = 1 };
};
#endif
