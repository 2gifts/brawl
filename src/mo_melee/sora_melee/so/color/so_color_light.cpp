#include <so/color/so_color_full.h>

soColorLight::soColorLight() { }

soColorLight::~soColorLight() { }

void soColorLight::reset() {
    soColorFlash::reset();
    m_isLight = false;
    m_light1 = 0.0f;
    m_light0 = 0.0f;
}
