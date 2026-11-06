#pragma once
#include <so/status/so_status_module_impl.h>
#include <wn/wario/wn_wario_bike.h>
class wnWarioBikeStatusUniqProcessUtility : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessUtility* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessUtility() {}
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execStatus(soModuleAccesser*);
    virtual void execMapCorrection(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
};
class wnWarioBikeStatusUniqProcessStart : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessStart* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessStart() {}
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execFixPos(soModuleAccesser*);
};
class wnWarioBikeStatusUniqProcessDrive : public wnWarioBikeStatusUniqProcessUtility {
public:
    static wnWarioBikeStatusUniqProcessDrive* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessDrive() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};
class wnWarioBikeStatusUniqProcessWheelie : public wnWarioBikeStatusUniqProcessUtility {
public:
    static wnWarioBikeStatusUniqProcessWheelie* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessWheelie() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
};

// RTTI8494 lists only soStatusUniqProcess; TurnStart does not inherit Utility.
class wnWarioBikeStatusUniqProcessTurnStart : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessTurnStart* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessTurnStart() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

class wnWarioBikeStatusUniqProcessTurnLoop : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessTurnLoop* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessTurnLoop() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

class wnWarioBikeStatusUniqProcessTurnEnd : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessTurnEnd* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessTurnEnd() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};
// Native RTTI and vtables establish these source class bases and overrides.
class wnWarioBikeStatusUniqProcessTurnDown : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessTurnDown* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessTurnDown() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

class wnWarioBikeStatusUniqProcessEscape : public wnWarioBikeStatusUniqProcessUtility {
public:
    static wnWarioBikeStatusUniqProcessEscape* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessEscape() {}
    virtual void execFixPos(soModuleAccesser*);
};

class wnWarioBikeStatusUniqProcessBump : public wnWarioBikeStatusUniqProcessUtility {
public:
    static wnWarioBikeStatusUniqProcessBump* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessBump() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

// RTTI confirms Down derives directly from soStatusUniqProcess.
class wnWarioBikeStatusUniqProcessDown : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessDown* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessDown() {}
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

// Appeal and Item have native RTTI/vtable entries and distinct status TUs.
class wnWarioBikeStatusUniqProcessAppeal : public wnWarioBikeStatusUniqProcessUtility {
public:
    static wnWarioBikeStatusUniqProcessAppeal* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessAppeal() {}
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

class wnWarioBikeStatusUniqProcessItem : public soStatusUniqProcess {
public:
    static wnWarioBikeStatusUniqProcessItem* getInstance() __attribute__((never_inline));
    virtual ~wnWarioBikeStatusUniqProcessItem() {}
    virtual void initStatus(soModuleAccesser*);
};

// The bike constructor fetches each status process through its global instance (defined in the process units).
extern wnWarioBikeStatusUniqProcessStart g_wnWarioBikeStatusUniqProcessStart;
inline wnWarioBikeStatusUniqProcessStart* wnWarioBikeStatusUniqProcessStart::getInstance() { return &g_wnWarioBikeStatusUniqProcessStart; }
extern wnWarioBikeStatusUniqProcessDrive g_wnWarioBikeStatusUniqProcessDrive;
inline wnWarioBikeStatusUniqProcessDrive* wnWarioBikeStatusUniqProcessDrive::getInstance() { return &g_wnWarioBikeStatusUniqProcessDrive; }
extern wnWarioBikeStatusUniqProcessWheelie g_wnWarioBikeStatusUniqProcessWheelie;
inline wnWarioBikeStatusUniqProcessWheelie* wnWarioBikeStatusUniqProcessWheelie::getInstance() { return &g_wnWarioBikeStatusUniqProcessWheelie; }
extern wnWarioBikeStatusUniqProcessBump g_wnWarioBikeStatusUniqProcessBump;
inline wnWarioBikeStatusUniqProcessBump* wnWarioBikeStatusUniqProcessBump::getInstance() { return &g_wnWarioBikeStatusUniqProcessBump; }
extern wnWarioBikeStatusUniqProcessTurnStart g_wnWarioBikeStatusUniqProcessTurnStart;
inline wnWarioBikeStatusUniqProcessTurnStart* wnWarioBikeStatusUniqProcessTurnStart::getInstance() { return &g_wnWarioBikeStatusUniqProcessTurnStart; }
extern wnWarioBikeStatusUniqProcessTurnLoop g_wnWarioBikeStatusUniqProcessTurnLoop;
inline wnWarioBikeStatusUniqProcessTurnLoop* wnWarioBikeStatusUniqProcessTurnLoop::getInstance() { return &g_wnWarioBikeStatusUniqProcessTurnLoop; }
extern wnWarioBikeStatusUniqProcessTurnEnd g_wnWarioBikeStatusUniqProcessTurnEnd;
inline wnWarioBikeStatusUniqProcessTurnEnd* wnWarioBikeStatusUniqProcessTurnEnd::getInstance() { return &g_wnWarioBikeStatusUniqProcessTurnEnd; }
extern wnWarioBikeStatusUniqProcessTurnDown g_wnWarioBikeStatusUniqProcessTurnDown;
inline wnWarioBikeStatusUniqProcessTurnDown* wnWarioBikeStatusUniqProcessTurnDown::getInstance() { return &g_wnWarioBikeStatusUniqProcessTurnDown; }
extern wnWarioBikeStatusUniqProcessUtility g_wnWarioBikeStatusUniqProcessUtility;
inline wnWarioBikeStatusUniqProcessUtility* wnWarioBikeStatusUniqProcessUtility::getInstance() { return &g_wnWarioBikeStatusUniqProcessUtility; }
extern wnWarioBikeStatusUniqProcessEscape g_wnWarioBikeStatusUniqProcessEscape;
inline wnWarioBikeStatusUniqProcessEscape* wnWarioBikeStatusUniqProcessEscape::getInstance() { return &g_wnWarioBikeStatusUniqProcessEscape; }
extern wnWarioBikeStatusUniqProcessDown g_wnWarioBikeStatusUniqProcessDown;
inline wnWarioBikeStatusUniqProcessDown* wnWarioBikeStatusUniqProcessDown::getInstance() { return &g_wnWarioBikeStatusUniqProcessDown; }
extern wnWarioBikeStatusUniqProcessItem g_wnWarioBikeStatusUniqProcessItem;
inline wnWarioBikeStatusUniqProcessItem* wnWarioBikeStatusUniqProcessItem::getInstance() { return &g_wnWarioBikeStatusUniqProcessItem; }
extern wnWarioBikeStatusUniqProcessAppeal g_wnWarioBikeStatusUniqProcessAppeal;
inline wnWarioBikeStatusUniqProcessAppeal* wnWarioBikeStatusUniqProcessAppeal::getInstance() { return &g_wnWarioBikeStatusUniqProcessAppeal; }
