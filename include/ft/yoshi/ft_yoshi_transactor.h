#pragma once
#include <ft/kirby/ft_kirby_copy_ability_process.h>
class wnYoshiTamago;

// Activates Egg Lay's shell for Yoshi and Kirby's copied neutral special.
class ftYoshiTransactor : public ftKirbyCopyAbilityProcess {
public:
    ftYoshiTransactor();
    virtual ~ftYoshiTransactor();
    virtual bool activeArticle(soArticle*, soModuleAccesser*);
    bool activeArticle1(wnYoshiTamago*, soModuleAccesser*, u32 resourceId);
};
static_assert(sizeof(ftYoshiTransactor) == 4, "Yoshi transactor extent");
