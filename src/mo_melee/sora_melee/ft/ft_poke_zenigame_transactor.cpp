#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/poke/ft_poke_zenigame_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/wn_weapon_builder.h>

// The water article (its header needs the weapon builder chain): only its type is used here.
class wnPokeZenigameWater : public Weapon { };

// TODO: activeArticle1 (the water gun start).

ftPokeZenigameTransactor::ftPokeZenigameTransactor() { }
ftPokeZenigameTransactor::~ftPokeZenigameTransactor() { }

// Work int 0x10000040 is cleared when the Pokemon comes out.
void ftPokeZenigameTransactor::init(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
}

void ftPokeZenigameTransactor::initTransact(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
}

void ftPokeZenigameTransactor::processUpdate(soModuleAccesser* moduleAccesser) { }

void ftPokeZenigameTransactor::processUpdateTransact(soModuleAccesser* moduleAccesser) { }

// The water gun is the only article the transactor starts (with Zenigame's Kirby resource).
bool ftPokeZenigameTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    wnPokeZenigameWater* water = dynamic_cast<wnPokeZenigameWater*>(article);
    ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
    return activeArticle1(water, acc, resources.getKirbyResId(Fighter_PokeZenigame));
}
