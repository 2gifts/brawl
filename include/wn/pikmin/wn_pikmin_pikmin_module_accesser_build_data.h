#pragma once

#include <wn/wn_module_accesser_build_data.h>

// Build data of the Pikmin article: the shared article build data plus one extra anim-command list.
// HYPOTHESIS: the extra value is the seventh constructor argument of the base (stored again at +0x40).
class wnPikminPikminModuleAccesserBuildData : public wnModuleAccesserBuildData {
public:
    const void* m_animCmdData; // +0x40: returned for anim-command group 1, thread 0

    wnPikminPikminModuleAccesserBuildData(s32 kind, const void* data, Heaps::HeapType instHeap,
                                          Heaps::HeapType nwModelInstHeap, Heaps::HeapType nwMotionInstHeap,
                                          u8 resGroupNo, s32 unk10, soParamAccesser* paramAccesser, s32 unk18, s32 unk1C,
                                          s32 unk2C, s32 unk30, s32 unk34, s32 unk38, s32 unk3C) :
        wnModuleAccesserBuildData(kind, data, instHeap, nwModelInstHeap, nwMotionInstHeap, resGroupNo, unk10,
                                  paramAccesser, unk18, unk1C, unk2C, unk30, unk34, unk38, unk3C),
        m_animCmdData(reinterpret_cast<const void*>(unk10)) { }
    virtual void* getAnimCmdData(u32 eventThread, s32 p2) const;
};

class wnPikminPikminUtility {
public:
    static bool isOwnerConditionGround(int condition);
    static void tryAttachGround(soModuleAccesser* moduleAccesser);
    static float getTargetDist(soModuleAccesser* moduleAccesser, int mode);
    static float getOwnerDist(soModuleAccesser* moduleAccesser, int mode);
    static float getDist(Vec2f* from, Vec2f* to, int mode);
};
