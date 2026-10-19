#pragma once

#include <types.h>

class soModuleAccesser;
class StageObject;
class wnPikminPikmin;
class wnPikminDolfin;
class wnPikminChain;

// Helpers of the Pikmin fighter shared by the fighter and its status processes: the condition that describes how Olimar moves
// (stored in every owner info the Pikmin of the line follow), the option byte stored next to it and the activation of the
// three articles (Pikmin, End of Day Dolfin, Pikmin Chain).
// HYPOTHESIS: the class name is the one of the module map; the names of the conditions are not known.
class ftPikminUtility {
public:
    static int getPikminOwnerCondition(soModuleAccesser* moduleAccesser, float speed);
    static bool activeArticle(wnPikminPikmin* article, soModuleAccesser* moduleAccesser);
    static bool activeArticle1(wnPikminDolfin* article, soModuleAccesser* moduleAccesser);
    static bool activeArticle2(wnPikminChain* article, soModuleAccesser* moduleAccesser);
    static void unRefLogGroup(soModuleAccesser* moduleAccesser);
    static void getPikminOwnerOptionInfo(u8* option, soModuleAccesser* moduleAccesser);
    static void reducePikminAllHelper(StageObject* stageObject);
    static bool isHideOrderOwnerCondition(int statusKind);
};
