#pragma once

// Build configuration of R.O.B.'s Robo Beam, for the unit that owns its constructor (wn_robot_beam.cpp defines
// WN_ROBOT_BEAM_BUILDER). Other units see the opaque storage of wnRobotBeam in wn_robot_beam.h.
// HYPOTHESIS: the motion count, from the sibling articles' configurations.
#include <wn/wn_module_accesser_builder.h>

class wnRobotBeamModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 2, 0, soCollisionAttackModuleImpl, 1,
                                               false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0>
        AnimCmdModuleSubBuildConfig;
};
