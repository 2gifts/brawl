#include <StaticAssert.h>
#include <ft/ness/ft_ness_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/ness/wn_ness_pkflash.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftNessTransactor::ftNessTransactor() { }
ftNessTransactor::~ftNessTransactor() { }

bool ftNessTransactor::shootArticle1(wnNessPKFlash* weapon, soModuleAccesser* acc) {
    weapon->shoot();
    return false;
}
