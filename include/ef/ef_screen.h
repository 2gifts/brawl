#pragma once

// Shadows the BrawlHeaders declaration to expose the verified flash request.
#include <GX.h>
#include <types.h>

class efScreenHandle;

// HYPOTHESIS: one fill slot of the screen, 0x20 bytes, starting at efScreen+0xA4.
struct efScreenFillSlot {
    u8 m_active : 8;
    u8 _1[3];
    float m_frames;
    float m_progress;
    u8 m_current[4];
    u8 m_from[4];
    u8 m_to[4];
    u8 _18[8];
};

class efScreen {
    union {
        char _spacer[0x1DC];
        struct {
            char _pad[0xA4];
            efScreenFillSlot m_fills[9];
        };
    };

public:
    // HYPOTHESIS: the screen-fill listener; stMelee installs its StageScreenNotify here and Stage::closeStage clears it.
    void* m_notify;

private:
    char _1E0[0xC];

public:
    // HYPOTHESIS: construction resets the screen state before publishing the singleton.
    efScreen() { reset(); }
    ~efScreen();
    void reset();
    static void create();
    static void destroy();
    void cancelFill(efScreenHandle* handle, float frames);
    int requestFill(float, int, int, GXColor*);
    void requestFill(efScreenHandle handle, float frames, GXColor* color);
    int requestFlash(float frames, int layer, u8 alpha, u32 cycles, GXColor* color);
    static efScreen* getInstance();
};

extern efScreen* g_efScreen;

inline efScreen::~efScreen() { g_efScreen = 0; }

inline efScreen* efScreen::getInstance() { return g_efScreen; }
