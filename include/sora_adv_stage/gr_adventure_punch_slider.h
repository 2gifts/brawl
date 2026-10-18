#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono_punch_slider.h>
#include <memory.h>
#include <types.h>

// The adventure stage's punch slider: the shared grYakumonoPunchSlider with no additions of its own.
class grAdventurePunchSlider : public grYakumonoPunchSlider {
public:
    grAdventurePunchSlider(const char* taskName) : grYakumonoPunchSlider(taskName) { }
    virtual ~grAdventurePunchSlider();

    static grAdventurePunchSlider* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventurePunchSlider) == 0x1C4, "Class is wrong size!");
