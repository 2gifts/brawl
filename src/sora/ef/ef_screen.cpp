#include <types.h>
#include <ef/ef_screen.h>
#include <ef/ef_screen_handle.h>
#include <sr/sr_common.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
efScreen* g_efScreen = 0;

void efScreen::create() {
    if (!g_efScreen) {
        g_efScreen = new (Heaps::System) efScreen();
    }
}

void efScreen::destroy() {
    if (g_efScreen) {
        delete g_efScreen;
    }
}

void efScreen::cancelFill(efScreenHandle* handle, float frames) {
    efScreenFillSlot& slot = m_fills[handle->unk0];
    slot.m_active = 1;
    slot.m_progress = 0.0f;
    slot.m_frames = frames;
    slot.m_current[0] = slot.m_to[0];
    slot.m_current[1] = slot.m_to[1];
    slot.m_current[2] = slot.m_to[2];
    slot.m_current[3] = slot.m_to[3];
    slot.m_from[0] = slot.m_to[0];
    slot.m_from[1] = slot.m_to[1];
    slot.m_from[2] = slot.m_to[2];
    slot.m_from[3] = 0;
}
