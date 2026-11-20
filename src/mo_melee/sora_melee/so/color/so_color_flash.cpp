#include <so/color/so_color_full.h>

// The scaled value as a renderer color.
static inline GXColor makeColor(const soColorFlashValue& value, const soColorFlashValue& scale) {
    GXColor color = {(u8)(value.m_r * scale.m_r), (u8)(value.m_g * scale.m_g), (u8)(value.m_b * scale.m_b),
                     (u8)(value.m_a * scale.m_a)};
    return color;
}

soColorFlash::soColorFlash() {
    reset();
}

soColorFlash::~soColorFlash() { }

void soColorFlash::reset() {
    m_isEnable = false;
    m_isUpdate = false;
    m_scale.m_r = 1.0f;
    m_scale.m_g = 1.0f;
    m_scale.m_b = 1.0f;
    m_scale.m_a = 1.0f;
}

void soColorFlash::set(const soColorFlashValue& value) {
    m_isEnable = true;
    m_value = value;
    m_color = makeColor(value, m_scale);
    m_speed.m_r = 0.0f;
    m_speed.m_g = 0.0f;
    m_speed.m_b = 0.0f;
    m_speed.m_a = 0.0f;
}

void soColorFlash::update() {
    if (m_isUpdate == true) {
        soColorFlashValue value = m_speed + m_value;
        m_value = value;
        m_color = makeColor(m_value, m_scale);
    }
}

#pragma auto_inline off
soColorFlashValue soColorFlashValue::operator+(const soColorFlashValue& other) const {
    soColorFlashValue result;
    result.m_r = m_r + other.m_r;
    result.m_g = m_g + other.m_g;
    result.m_b = m_b + other.m_b;
    result.m_a = m_a + other.m_a;
    return result;
}
#pragma auto_inline reset

void soColorFlash::stop() {
    m_isEnable = false;
    m_isUpdate = false;
}

soColorFlashValue soColorFlashCalcSpeed(const soColorFlashValue& from, const soColorFlashValue& to, float frames);

void soColorFlash::setTarget(const soColorFlashValue& target, float frames) {
    m_speed = soColorFlashCalcSpeed(m_value, target, frames);
    m_isUpdate = true;
}

// The per-frame step from one value to another (rounded to the nearest step).
// HYPOTHESIS: name (unnamed in the map).
#pragma auto_inline off
// NONMATCHING: the original reads the start value per component, after each store.
soColorFlashValue soColorFlashCalcSpeed(const soColorFlashValue& from, const soColorFlashValue& to, float frames) {
    float inv = 1.0f / frames;
    soColorFlashValue speed;
    speed.m_r = inv * ((0.5f + to.m_r) - from.m_r);
    speed.m_g = inv * ((0.5f + to.m_g) - from.m_g);
    speed.m_b = inv * ((0.5f + to.m_b) - from.m_b);
    speed.m_a = inv * ((0.5f + to.m_a) - from.m_a);
    return speed;
}
#pragma auto_inline reset
