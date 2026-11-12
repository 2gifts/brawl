#include <StaticAssert.h>
#include <ft/samus/ft_samus_transactor.h>
#include <so/so_module_accesser.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftSamusTransactor::ftSamusTransactor() { }
ftSamusTransactor::~ftSamusTransactor() { }

void ftSamusTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateSpecialNTransact(moduleAccesser);
}
