#pragma once

class soModuleAccesser;

// Egg Roll's shared behavior is also called by its hit callbacks.
class ftYoshiStatusUniqProcessSpecialSUtility {
public:
    static bool checkLife(soModuleAccesser*);
    static bool checkCancel(soModuleAccesser*);
    static void setBodyChange(soModuleAccesser*);
    static void setBodyScale(soModuleAccesser*);
    static void resetYoshiSpecialS(soModuleAccesser*);
    static void resetYoshiSpecialS2(soModuleAccesser*);
    static void audioDash(soModuleAccesser*);
    static void procHit(soModuleAccesser*);
    static void procHitWall(soModuleAccesser*, int);
    static bool getFlick(soModuleAccesser*);
    static void setRot(soModuleAccesser*);
    static void setAttack(soModuleAccesser*);
    static void setPower(soModuleAccesser*);
};
