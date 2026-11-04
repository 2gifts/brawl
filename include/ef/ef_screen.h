#pragma once

// Shadows the BrawlHeaders declaration to expose the verified flash request.
#include <GX.h>
#include <types.h>

class efScreenHandle;

class efScreen {
    char _spacer[0x1DC];

public:
    // HYPOTHESIS: the screen-fill listener; stMelee installs its StageScreenNotify here and Stage::closeStage clears it.
    void* m_notify;

private:
    char _1E0[0xC];

public:
    int requestFill(float, int, int, GXColor*);
    void requestFill(efScreenHandle handle, float frames, GXColor* color);
    int requestFlash(float frames, int layer, u8 alpha, u32 cycles, GXColor* color);
    static efScreen* getInstance();
};

extern efScreen* g_efScreen;

inline efScreen* efScreen::getInstance() { return g_efScreen; }
