#pragma once

// The parts the Pokemon Trainer's three Pokemon (PokeZenigame, PokeFushigisou, PokeLizardon) share: the party stamina and
// the entry event presenter every one of them owns, and the sora_melee helpers their fighter classes call.

#include <ft/ft_fighter_builder.h>
#include <so/event/so_event_presenter.h>
#include <types.h>

class ftOwner;

// HYPOTHESIS: the Pokemon Trainer's party state; both modules are constructed in sora_melee from the first extend parameter block.
class ftPokemonStaminaModule {
    u8 unk0[0xC];
public:
    // HYPOTHESIS: unkC / unk14 are copied into the work floats 0x11000013 / 0x11000014 at the start.
    float unkC;
    float unk10;
    float unk14;
    ftPokemonStaminaModule(const void* extendParam, soModuleAccesser* moduleAccesser);
    ~ftPokemonStaminaModule() { }
};

// HYPOTHESIS: the entry event presenter is the first base (the fighter walks the observer list at the address of the module).
class ftPokemonEventManageModule : public soEventPresenter<ftEntryEventObserver> {
    u8 unkC[0x1C - 0xC];
public:
    ftPokemonEventManageModule(s32 entryId, soModuleAccesser* moduleAccesser, ftPokemonStaminaModule* stamina);
    virtual ~ftPokemonEventManageModule() { }
    // HYPOTHESIS: inline presenter helpers in the style of the trainer's ftEntryEventPresenter: each tells every observer.
    void notifyEventPokemonStart(int taskId) {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonStart(taskId);
        }
    }
    void notifyEventPokemonCollect() {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonCollect();
        }
    }
    void notifyEventPokemonChangeCancel() {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonChangeCancel();
        }
    }
};

// HYPOTHESIS: parameter types of the sora_melee Pokemon helpers (unnamed there; the trailing names are the module map's).
extern "C" {
float fn_27_112BEC(ftOwner* owner);                                                                   // ftOwner::getStaminaMaxAdd
void fn_27_368988(ftPokemonStaminaModule* stamina, float staminaMaxAdd);                              // ftPokemonStaminaModule::activate
void fn_27_3689A4(ftPokemonStaminaModule* stamina);                                                   // ftPokemonStaminaModule::initStamina
void fn_27_368AAC(ftPokemonStaminaModule* stamina);                                                   // ftPokemonStaminaModule::decStaminaAttack
void fn_27_368AE8(ftPokemonStaminaModule* stamina);                                                   // ftPokemonStaminaModule::incStaminaDead
void fn_27_368B30(ftPokemonStaminaModule* stamina);                                                   // ftPokemonStaminaModule::update
void fn_27_3769FC(ftPokemonEventManageModule* eventManage);                                           // ftPokemonEventManageModule::activate
void fn_27_376A58(ftPokemonEventManageModule* eventManage);                                           // ftPokemonEventManageModule::deactivate
void fn_27_376A80(ftPokemonEventManageModule* eventManage, int statusKind, int prevStatusKind, soStatusData* statusData); // ::onChangeStatus
void fn_27_377094(ftPokemonEventManageModule* eventManage);                                           // ftPokemonEventManageModule::onAttack
void fn_27_3772A8(ftPokemonEventManageModule* eventManage);                                           // ftPokemonEventManageModule::onInflict
void fn_27_3774AC(ftPokemonEventManageModule* eventManage);                                           // ftPokemonEventManageModule::onDamage
void fn_27_3776C0(ftPokemonEventManageModule* eventManage);                                           // ::notifyEventPokemonTrainerUpdate
bool fn_27_3776C8(ftPokemonEventManageModule* eventManage, Vec3f* pos, float* lr);                    // ::notifyEventPokemonRequestChange
void fn_27_37794C(soModuleAccesser* moduleAccesser, int kind, int statusKind);                        // ftPokemonUtil::changeCommon
void fn_27_377B30(soModuleAccesser* moduleAccesser, void* eventArg);                                  // ftPokemonUtil::finalEndCommon
}

// HYPOTHESIS: the block the change hands over from the previous Pokemon (position, facing and damage).
struct ftPokemonChangeWork {
    Vec3f m_pos;
    float m_lr;
    u8 unk10[0x5C - 0x10];
    float m_damage;
};
