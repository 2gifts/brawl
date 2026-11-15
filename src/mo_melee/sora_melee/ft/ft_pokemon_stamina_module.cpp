#include <ft/poke/ft_pokemon_stamina_module.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>

u8 g_ftPokemonStaminaSystem = true;

ftPokemonStaminaModule::ftPokemonStaminaModule(const void* extendParam, soModuleAccesser* moduleAccesser) :
    m_moduleAccesser(moduleAccesser),
    m_param(static_cast<const ftPokemonStaminaParam*>(extendParam)),
    m_frame(0),
    unkC(m_param->m_max),
    unk10(m_param->m_max) { }

void ftPokemonStaminaModule::activate(float bonus) {
    unk10 = m_param->m_max + bonus;
    unk14 = m_param->unk24;
}

void ftPokemonStaminaModule::initStamina() {
    m_frame = 0;
    unkC = unk10;
}

// Once a second: drains while the Pokemon is active, regenerates otherwise.
void ftPokemonStaminaModule::decStaminaUpdate() {
    if (g_ftPokemonStaminaSystem) {
        if (m_frame == 0) {
            if (m_moduleAccesser->getStageObject().isActive()) {
                if (unkC > 0.0f) {
                    unkC -= m_param->m_drain;
                    if (unkC < 0.0f) {
                        unkC = 0.0f;
                    }
                }
            } else if (unkC < unk10) {
                unkC += m_param->m_regen;
                if (unkC > unk10) {
                    unkC = unk10;
                }
            }
        }
        m_frame = (m_frame + 1) % 60;
    }
}

void ftPokemonStaminaModule::decStaminaAttack() {
    if (g_ftPokemonStaminaSystem) {
        unkC -= m_param->m_attackCost;
        if (unkC < 0.0f) {
            unkC = 0.0f;
        }
    }
}

void ftPokemonStaminaModule::incStaminaDead() {
    if (g_ftPokemonStaminaSystem) {
        float max = unk10;
        unkC = max - (max - unkC) * m_param->m_deadKeep;
        if (unkC < 0.0f) {
            unkC = 0.0f;
        }
    }
}

// Tired Pokemon hit softer: the attack power multiplier falls from 1 towards the minimum below the tired threshold.
void ftPokemonStaminaModule::update() {
    if (!g_ftPokemonStaminaSystem) {
        m_moduleAccesser->getCollisionAttackModule().setPowerMul(1.0f);
        return;
    }
    float rate = 1.0f; // HYPOTHESIS: the attack power multiplier
    if (unkC < m_param->m_tiredBelow) {
        float minRate = m_param->m_minPower;
        rate = minRate + (1.0f - minRate) * (unkC / unk10);
    }
    m_moduleAccesser->getCollisionAttackModule().setPowerMul(rate);
    m_moduleAccesser->getWorkManageModule().setFloat(unkC, 0x11000013);
}
