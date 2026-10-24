#pragma once

// A parameter value of an enemy that grows with its level: the Enemy::getValue* / setValue* helpers read it as
// m_base + m_rate * the enemy's value float ValueFloat_LevelScale (they skip the product when m_rate is 0). The random
// variants read two of them in a row, the lower and the upper bound.
// HYPOTHESIS: the field roles come from those helpers; the enemies keep these values in their constant parameters.

#include <types.h>

struct emScaledValue {
    float m_base;
    float m_rate;
};

// HYPOTHESIS: a value of the enemy that scales its constants, such as its level; the scaled values grow with it.
static const int ValueFloat_LevelScale = 0x1100000c;
