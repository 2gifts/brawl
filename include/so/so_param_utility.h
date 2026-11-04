#pragma once

class soParamUtility {
public:
    // HYPOTHESIS: table type follows wnPikminPikmin::getWaitAnimTable; the second argument is an integer.
    static int getRandomMotionKind(const void* table, int kind);
};
