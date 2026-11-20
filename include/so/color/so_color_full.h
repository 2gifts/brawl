#pragma once

// Full layout of the color flash/light/blend classes for their own units (so_color_flash.cpp, so_color_light.cpp,
// so_color_blend.cpp). so/color/so_color_flash.h and friends (BrawlHeaders) keep the opaque copies the rest of the code
// sees; a unit must not include both.
// TODO: clean up: merge into the BrawlHeaders shadows once the link module users are checked (compiler ICE risk in the
// Pokemon Trainer units).

#include <StaticAssert.h>
#include <revolution/GX/GXTypes.h>
#include <types.h>

// HYPOTHESIS: the RGBA value of a color flash (floats 0..255).
struct soColorFlashValue {
    float m_r;
    float m_g;
    float m_b;
    float m_a;

    soColorFlashValue() { }
    soColorFlashValue(const soColorFlashValue& other) : m_r(other.m_r), m_g(other.m_g), m_b(other.m_b), m_a(other.m_a) { }
    soColorFlashValue operator+(const soColorFlashValue& other) const;
};

// A flashing color: the current value, its per-frame speed towards a target and a per-channel scale; the scaled value is
// kept as a GXColor for the renderer.
// HYPOTHESIS: field and method names (the map leaves the methods unnamed).
class soColorFlash {
public:
    GXColor m_color;            // 0x00
    soColorFlashValue m_value;  // 0x04
    soColorFlashValue m_speed;  // 0x14
    soColorFlashValue m_scale;  // 0x24
    bool m_isEnable;            // 0x34
    bool m_isUpdate;            // 0x35

    soColorFlash();
    virtual ~soColorFlash();
    virtual void reset();
    void set(const soColorFlashValue& value);
    void update();
    void stop();
    void setTarget(const soColorFlashValue& target, float frames);
};
static_assert(sizeof(soColorFlash) == 0x3C, "Class is the wrong size!");

// The color flash with a light direction or intensity on top.
// HYPOTHESIS: names of the two floats and the flag.
class soColorLight : public soColorFlash {
public:
    float m_light0; // 0x3C
    float m_light1; // 0x40
    bool m_isLight; // 0x44

    soColorLight();
    virtual ~soColorLight();
    virtual void reset();
};
static_assert(sizeof(soColorLight) == 0x48, "Class is the wrong size!");

class soColorBlend {
public:
    soColorFlash m_flash; // 0x00
    soColorLight m_light; // 0x3C
    int m_priority;       // 0x84 HYPOTHESIS: reset to 50

    soColorBlend();
    ~soColorBlend();
    void reset();
    bool isEnable();
    void stop();
    void update();
};
static_assert(sizeof(soColorBlend) == 0x88, "Class is the wrong size!");
