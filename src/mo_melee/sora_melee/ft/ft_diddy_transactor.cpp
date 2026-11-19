#include <StaticAssert.h>
#include <ft/diddy/ft_diddy_transactor.h>
#include <ft/diddy/ft_diddy_article_pools.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftDiddyTransactor::ftDiddyTransactor() { }
ftDiddyTransactor::~ftDiddyTransactor() { }

// The popgun is article 0xd; it is started with Diddy's Kirby resource id.
bool ftDiddyTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnDiddyGun* weapon = dynamic_cast<wnDiddyGun*>(article);
        if (weapon == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(weapon, acc, resources.getKirbyResId(Fighter_Diddy));
    }
    return false;
}

// TODO: clean up, give variables real names
namespace jj_364150 {
struct Msg { int id; unsigned char flag; };
struct Module {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void call(int, Msg*, int);
};
struct Other {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void call(int, int);
};
struct Modules {
    char pad[0x54];
    Module* status;
    char pad58[0x2c];
    Other* other;
};
struct Accesser {
    char pad[0xd8];
    Modules* modules;
};
extern "C" unsigned char g_ftKirbyCopyAbilityIdConverter[];
extern "C" int convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(void*, unsigned int, int, void*, unsigned int);
extern "C" void fn_27_364150(void*, int status, void*, void*, Accesser* accesser) {
    switch (convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(g_ftKirbyCopyAbilityIdConverter, 0, status, accesser, 1)) {
    case 0x10c:
    case 0x112:
    case 0x117:
    case 0x118:
        return;
    case 0x119: {
        Msg msg;
        msg.id = 0x838;
        msg.flag = 0;
        accesser->modules->status->call(1, &msg, 0);
        break;
    }
    case 0x11a: {
        Msg msg;
        msg.id = 0x839;
        msg.flag = 0;
        accesser->modules->status->call(1, &msg, 0);
        break;
    }
    default:
        accesser->modules->other->call(13, 0);
        break;
    }
}
}

