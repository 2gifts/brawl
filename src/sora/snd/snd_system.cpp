#include <types.h>
#include <snd/snd_system.h>
#include <gf/gf_error_check.h>
#include <gf/gf_heap_manager.h>

bool sndSystem::loadArchiveHeader() {
    u32 headerSize;
    void* header;

    if (!m_dvdSoundArchive.Open("/sound/smashbros_sound.brsar")) {
        gfErrorCheck::check();
        return false;
    }
    // HYPOTHESIS: 0x124 is not named in the header, so it is read at a raw offset
    headerSize = *(u32*)((char*)this + 0x124);
    header = gfHeapManager::alloc(Heaps::Sound, headerSize);
    *(void**)&_2D4[0x41C] = header;
    if (!m_dvdSoundArchive.LoadHeader(header, headerSize)) {
        gfErrorCheck::check();
        return false;
    }
    return true;
}
