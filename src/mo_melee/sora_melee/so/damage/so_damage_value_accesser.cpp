#include <so/damage/so_damage_value_accesser.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/damage/so_damage_transactor_actor.h>
#include <so/so_module_accesser.h>
#include <types.h>

static const int DamageStatusKindNum = 22;

// The damage transactor's value parameter is the table of the 22 damage status kinds.
// MATCH-ONLY: the original passes no accesser to getDamageValueParam (its argument register is left as it is).
static inline const int* getDamageStatusKindTable(const soModuleAccesser* moduleAccesser) {
    soModuleAccesser* unused;
    return (const int*)moduleAccesser->getDamageModule().getTransactor()->getDamageValueParam(unused);
}

// Map name soDamageValueAccesser::getConstantStatusKind: the status kind of a damage status index.
// TODO: clean up: keeps the placeholder name because the fighter RELs (ft_diddy) and ft_damage_transactor_impl.cpp call it
// by that name.
// MATCH-ONLY: the const parameter lets the accesser load move ahead of the prologue, as in the original.
extern "C" int fn_27_61334(const soModuleAccesser* moduleAccesser, int index) {
    const int* table = getDamageStatusKindTable(moduleAccesser);
    if (table != NULL && index >= 0 && index < DamageStatusKindNum) {
        return table[index];
    }
    return 0;
}

// The damage status index of a status kind (0 when the kind is not a damage status).
// NONMATCHING: the original loads the module enumeration before saving the link register, which MWCC only does for a
// const accesser parameter (fn_27_61334 above takes one); the mangled name says the parameter is not const.
int soDamageValueAccesser::getConstantDamageStatusKind(soModuleAccesser* moduleAccesser, int statusKind) {
    const int* table = getDamageStatusKindTable(moduleAccesser);
    if (table != NULL) {
        for (int i = 0; i < DamageStatusKindNum; i++) {
            if (statusKind == table[i]) {
                return i;
            }
        }
        return 0;
    }
    return 0;
}
