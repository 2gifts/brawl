#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/lucario/ft_lucario.h>
#include <so/so_module_accesser.h>

// ftLucario::effect_suspend (the map name); src/mo_fighter/ft_lucario/ft_lucario.cpp calls it by this stand-in name.
extern "C" void fn_118_98F4(u8* p);

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

void ftLucarioStatusUniqProcessSpecialLwSplit::initStatus(soModuleAccesser* moduleAccesser) {
    ftLucario& fighter = dynamic_cast<ftLucario&>(*moduleAccesser->m_stageObject);
    fn_118_98F4((u8*)&fighter);
}

void ftLucarioStatusUniqProcessSpecialLwAppear::initStatus(soModuleAccesser*) { }

ftLucarioStatusUniqProcessSpecialLw g_ftLucarioStatusUniqProcessSpecialLw;
ftLucarioStatusUniqProcessSpecialLwSplit g_ftLucarioStatusUniqProcessSpecialLwSplit;
ftLucarioStatusUniqProcessSpecialLwAppear g_ftLucarioStatusUniqProcessSpecialLwAppear;
