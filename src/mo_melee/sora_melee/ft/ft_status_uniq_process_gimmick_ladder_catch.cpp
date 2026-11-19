#include <types.h>

// TODO: clean up, give variables real names
namespace jj_177B2C {
extern "C" void initLadderCatch__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserP24soGimmickLadderEventArgs(void) {}
}

// TODO: clean up, give variables real names
namespace jj_177B30 {
extern "C" void adjustLadderMovement__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserff(void*, void*, float, float);
extern "C" void correctX__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserfi(void*, void*, float, int);
extern "C" void correctY__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserffi(void*, void*, float, float, int);

extern "C" void execLadderCatch__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserP24soGimmickLadderEventArgs(void* a, void* b, float* c) {
    adjustLadderMovement__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserff(a, b, c[2], c[3]);
    correctX__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserfi(a, b, c[2], 1);
    correctY__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserffi(a, b, c[3], c[6], 1);
}
}

// TODO: clean up, give variables real names
namespace jj_177BA8 {
extern "C" void fixPosLadderCatch__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserP24soGimmickLadderEventArgs(void) {}
}

// TODO: clean up, give variables real names
namespace jj_177BAC {
extern "C" void exitLadderCatch__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesser(void) {}
}

