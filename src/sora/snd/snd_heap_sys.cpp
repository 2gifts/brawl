#include <snd/snd_system.h>
#include <types.h>

nw4r::snd::SoundMemoryAllocatable* sndHeapSys::getHeapP(s32 slot) {
    return &m_heapArr[slot];
}

void sndHeapSys::saveState(SndGroupID groupId, s32 slot) {
    m_heapArr[slot].SaveState();
}

s32 sndHeapSys::isLoaded(SndGroupID groupId) {
    for (u32 i = 0; i < 30; i++) {
        sndHeapGroup* group = (i < 30) ? &m_groups[i] : NULL;
        if (group != NULL && group->m_isLoaded != 0 && group->m_groupId == groupId) {
            return 1;
        }
    }
    return 0;
}

// The first of the levels 3 to 6 that no loaded group uses; 0x1C when all are taken.
int sndHeapSys::findFreeLevel() {
    int result;
    for (int level = 3; level <= 6; level++) {
        result = level;
        for (u32 i = 0; i < 30; i++) {
            bool used = false;
            if (m_groups[i].m_isLoaded != 0 && level == m_groups[i].m_level) {
                used = true;
            }
            if (used) {
                result = 0x1C;
                break;
            }
        }
        if (result != 0x1C) {
            break;
        }
    }
    return result;
}
