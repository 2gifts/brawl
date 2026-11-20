#include <so/color/so_color_full.h>

soColorBlend::soColorBlend() { }

soColorBlend::~soColorBlend() { }

void soColorBlend::reset() {
    m_priority = 50;
    m_flash.reset();
    m_light.reset();
}

bool soColorBlend::isEnable() {
    if (m_flash.m_isEnable == true || m_light.m_isEnable == true) {
        return true;
    }
    return false;
}

void soColorBlend::stop() {
    m_flash.stop();
    m_light.stop();
}

void soColorBlend::update() {
    m_flash.update();
    m_light.update();
}
