#include <so/stop/so_stop_module_impl.h>

// The fighter stop module: hit-stop frames come from the fighter's damage constants.
// TODO: getHitStopFrame(float power, float mul) and setHitStop(float, int) override the base slot that our soStopModule
// view declares without arguments (getHitStopFrame()), so they stay in the module blob until that view is fixed.
class ftStopModuleImpl : public soStopModuleImpl {
public:
    ftStopModuleImpl(soModuleAccesser* moduleAccesser);
    virtual ~ftStopModuleImpl();
};

ftStopModuleImpl::ftStopModuleImpl(soModuleAccesser* moduleAccesser) : soStopModuleImpl(moduleAccesser) { }

ftStopModuleImpl::~ftStopModuleImpl() { }
