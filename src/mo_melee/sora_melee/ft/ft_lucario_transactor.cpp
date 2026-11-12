#include <StaticAssert.h>
#include <ft/lucario/ft_lucario_transactor.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <wn/lucario/wn_lucario_aura_ball.h>

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

// Slot 13: shoots the aura ball (article 0xd).
bool ftLucarioTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd:
        if (wnLucarioAuraBall* weapon = dynamic_cast<wnLucarioAuraBall*>(article)) {
            return shootArticle(weapon, acc);
        }
        break;
    }
    return false;
}

// The map names a file-local routine applying the recoil for a charge level.
void set_recoil(soModuleAccesser* acc, int chargeLevel);

bool ftLucarioTransactor::shootArticle(wnLucarioAuraBall* weapon, soModuleAccesser* acc) {
    float lr = acc->getPostureModule().getLr();
    float scale = acc->getPostureModule().getScale();
    weapon->shoot(lr, acc->getWorkManageModule().getFloat(0x11000013), scale);
    set_recoil(acc, weapon->getChargeLevel());
    acc->getWorkManageModule().setInt(7, 0x10000042);
    return false;
}
