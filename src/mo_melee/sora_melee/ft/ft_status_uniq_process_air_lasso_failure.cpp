#include <so/status/so_status_module_impl.h>
#include <types.h>

// Air lasso failure: no own status methods in the REL; only the destructor is defined. Unlike the other lasso
// statuses it derives straight from soStatusUniqProcess (its RTTI base array has a single entry).
class ftStatusUniqProcessAirLassoFailure : public soStatusUniqProcess {
public:
    ftStatusUniqProcessAirLassoFailure() { }
    virtual ~ftStatusUniqProcessAirLassoFailure() { }
};

ftStatusUniqProcessAirLassoFailure g_ftStatusUniqProcessAirLassoFailure;
