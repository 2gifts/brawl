#include <StaticAssert.h>
#include <ft/koopa/ft_koopa_transactor.h>
#include <so/so_module_accesser.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftKoopaTransactor::ftKoopaTransactor() { }
ftKoopaTransactor::~ftKoopaTransactor() { }

void ftKoopaTransactor::exit(soModuleAccesser* moduleAccesser) { }

void ftKoopaTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateSpecialNTransact(moduleAccesser);
}
