#pragma once
#include <StaticAssert.h>
#include <ft/kirby/ft_kirby_copy_ability_process.h>

// Chomp search handling also used when Kirby copies Wario's neutral special.
// The native RTTI base list and complete vtable establish this interface.
class ftWarioTransactor : public ftKirbyCopyAbilityProcess {
public:
    ftWarioTransactor();
    virtual ~ftWarioTransactor();
    virtual void notifyEventCollisionSearch(soCollisionLog*, soModuleAccesser*);
};
static_assert(sizeof(ftWarioTransactor) == 4, "Wario transactor extent");
