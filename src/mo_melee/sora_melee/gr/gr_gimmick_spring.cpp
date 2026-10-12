#include <gr/gr_gimmick_spring.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

bool grGimmickSpring::setSleepArea(bool unk1) {
    bool result = grYakumono::setSleepArea(unk1);
    if (unk1 == 1) {
        soGimmickEventArgs_Disable args;
        m_yakumono->presentEventGimmick(&args, -1);
    }
    return result;
}


bool grGimmickSpring::disableArea() {
    bool result = grYakumono::disableArea();
    soGimmickEventArgs_Disable args;
    m_yakumono->presentEventGimmick(&args, -1);
    return result;
}


struct grGimmickSpringPosArgs : soGimmickSpringEventArgs {
    inline grGimmickSpringPosArgs() : soGimmickSpringEventArgs(Gimmick::Spring_Event_Pos, 0) {}
};

void grGimmickSpring::presentPosEvent() {
    grGimmickSpringPosArgs args;
    getTopNode(&args.m_topPos);
    m_yakumono->presentEventGimmick(&args, -1);
}
