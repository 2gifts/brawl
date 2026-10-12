#pragma once
#include <so/status/so_status_module_impl.h>
class soModuleAccesser;
class ftSnakeStatusUniqProcessFinalCommon {
public:
    static void setCameraOffsetZ(soModuleAccesser*, float);
    static void exitStatusCommon(soModuleAccesser*, int);
};
class ftSnakeStatusUniqProcessFinalEntry : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessFinalEntry() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
class ftSnakeStatusUniqProcessFinalSet : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessFinalSet() {}
    virtual void execFixPos(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
class ftSnakeStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessFinalEnd() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftSnakeStatusUniqProcessFinalEntry g_ftSnakeStatusUniqProcessFinalEntry;
extern ftSnakeStatusUniqProcessFinalSet g_ftSnakeStatusUniqProcessFinalSet;
extern ftSnakeStatusUniqProcessFinalEnd g_ftSnakeStatusUniqProcessFinalEnd;
