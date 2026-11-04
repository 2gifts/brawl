#include <ef/ef_screen.h>
#include <ef/ef_screen_handle.h>
#include <so/effect/so_effect_screen.h>
#include <so/so_array.h>
#include <types.h>

soEffectScreen::soEffectScreen(soArray<efScreenHandle>* handles) : m_handles(handles) { }

soEffectScreen::~soEffectScreen() { }

// efScreen::cancelFill (the map name); src/mo_stage/st_dxzebes/st_dxzebes.cpp calls it by this stand-in name.
extern "C" void fn_8005E3C8(efScreen* screen, efScreenHandle* handle, float frames);

void soEffectScreen::fill(int index, int frames, const GXColor& color, int layer, int priority) {
    GXColor updateSource;
    if (index == -1) {
        GXColor fillColor = {color.r, color.g, color.b, color.a};
        efScreenHandle handle;
        handle.unk0 = (u32)g_efScreen->requestFill(0.0f, layer, priority, &fillColor) >> 24;
        fn_8005E3C8(g_efScreen, &handle, frames);
    } else if (m_handles->at(index).isValid() == true) {
        updateSource.r = color.r;
        updateSource.g = color.g;
        updateSource.b = color.b;
        updateSource.a = color.a;
        efScreenHandle handle = m_handles->at(index);
        GXColor updateColor = updateSource;
        g_efScreen->requestFill(handle, frames, &updateColor);
    } else {
        GXColor fillColor = {color.r, color.g, color.b, color.a};
        u8 handle = (u32)g_efScreen->requestFill(frames, layer, priority, &fillColor) >> 24;
        efScreenHandle& storedHandle = m_handles->at(index);
        storedHandle.unk0 = handle;
    }
}

void soEffectScreen::clear(int index, int frames) {
    if (m_handles->at(index).isValid() == true) {
        efScreenHandle handle = m_handles->at(index);
        fn_8005E3C8(g_efScreen, &handle, frames);
        efScreenHandle& storedHandle = m_handles->at(index);
        storedHandle.unk0 = -1;
    }
}

void soEffectScreen::clearAll() {
    for (int index = 0; index < m_handles->size(); ++index) {
        if (m_handles->at(index).isValid() == true) {
            efScreenHandle handle = m_handles->at(index);
            fn_8005E3C8(g_efScreen, &handle, 0.0f);
            m_handles->at(index).unk0 = -1;
        }
    }
}
