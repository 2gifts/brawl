#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLucarioStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialLwSplit : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialLwSplit() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialLwAppear : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialLwAppear() { }
    virtual void initStatus(soModuleAccesser*);
};

void ftLucarioStatusUniqProcessSpecialLw::initStatus(soModuleAccesser*) { }

void ftLucarioStatusUniqProcessSpecialLwSplit::initStatus(soModuleAccesser*) { }

void ftLucarioStatusUniqProcessSpecialLwAppear::initStatus(soModuleAccesser*) { }

ftLucarioStatusUniqProcessSpecialLw g_ftLucarioStatusUniqProcessSpecialLw;
ftLucarioStatusUniqProcessSpecialLwSplit g_ftLucarioStatusUniqProcessSpecialLwSplit;
ftLucarioStatusUniqProcessSpecialLwAppear g_ftLucarioStatusUniqProcessSpecialLwAppear;
