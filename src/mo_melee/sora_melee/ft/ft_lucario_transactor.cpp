#include <StaticAssert.h>
#include <ft/lucario/ft_lucario_transactor.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftLucarioTransactor::ftLucarioTransactor() { }
ftLucarioTransactor::~ftLucarioTransactor() { }

// The map names a file-local routine that updates the aura ball effects.
void update_auraball_effect(soModuleAccesser* moduleAccesser);

// Leaving the fighter resets the aura ball state as init does.
void ftLucarioTransactor::exit(soModuleAccesser* moduleAccesser) {
    init(moduleAccesser);
}

void ftLucarioTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isEstimate() == true) {
        update_auraball_effect(moduleAccesser);
    }
}