#include <types.h>
#include <snd/snd_system.h>

extern "C" s32 fn_8007AB34(sndHeapSys* heap, SndGroupID groupId);
extern "C" s32 fn_8007A324(sndHeapSys* heap, int heapLevel, SndGroupID groupId, s32 size);
extern "C" s32 fn_8007A5EC(sndHeapSys* heap, s32 slot, SndGroupID groupId, s32 size);
extern "C" nw4r::snd::SoundMemoryAllocatable* fn_8007A314(sndHeapSys* heap, s32 slot);
extern "C" void fn_8007A78C(sndHeapSys* heap, SndGroupID groupId, s32 slot);
extern "C" void fn_80079AA4(s32 unkB98, SndGroupID groupId, s32 slot, s32 result);

int sndSystem::loadSoundGroup(SndGroupID sndGroupId, int heapLevel, bool isRequest) {
    nw4r::snd::SoundArchive::GroupInfo groupInfo;
    s32 size;
    s32 slot;
    s32 result;

    if ((u8)_2D4[0x41A] == 0) {
        return -5;
    }
    m_dvdSoundArchive.detail_ReadGroupInfo(sndGroupId, &groupInfo);
    size = groupInfo.size + groupInfo.waveDataSize;
    if (size == 0) {
        return -3;
    }
    if (fn_8007AB34(m_sndHeapSys, sndGroupId) != 0) {
        return -1;
    }
    slot = fn_8007A324(m_sndHeapSys, heapLevel, sndGroupId, size);
    if (slot == 0x1C) {
        return -2;
    }
    result = fn_8007A5EC(m_sndHeapSys, slot, sndGroupId, size);
    if (result == -1) {
        return -2;
    }
    if (isRequest) {
        fn_80079AA4(*(s32*)&_2D4[0x8C4], sndGroupId, slot, result);
    } else {
        m_archivePlayer.LoadGroup((u32)sndGroupId, fn_8007A314(m_sndHeapSys, slot), 0x10000U);
        fn_8007A78C(m_sndHeapSys, sndGroupId, slot);
    }
    return result;
}

#include <snd/snd_system.h>
#include <gf/gf_error_check.h>
#include <gf/gf_heap_manager.h>

bool sndSystem::setupBuffers() {
    u32 playerSize = m_archivePlayer.GetRequiredMemSize(&m_dvdSoundArchive);
    u32 managerSize = m_3DManager.GetRequiredMemSize(&m_dvdSoundArchive);
    *(void**)&_2D4[0x420] = gfHeapManager::alloc(Heaps::Sound, playerSize);
    *(void**)&_2D4[0x424] = gfHeapManager::alloc(Heaps::Sound, 0xE6000);
    *(void**)&_2D4[0x428] = gfHeapManager::alloc(Heaps::Sound, managerSize);
    if (!m_archivePlayer.Setup(&m_dvdSoundArchive, *(void**)&_2D4[0x420], playerSize, *(void**)&_2D4[0x424], 0xE6000)) {
        gfErrorCheck::check();
        return false;
    }
    if (!m_3DManager.Setup(&m_dvdSoundArchive, *(void**)&_2D4[0x428], managerSize)) {
        gfErrorCheck::check();
        return false;
    }
    // HYPOTHESIS: 0x278 and 0x27C are not named in the header, so they are written at raw offsets
    *(s32*)((char*)this + 0x27C) = 0x20;
    *(void**)((char*)this + 0x278) = (char*)this + 0x294;
    return true;
}
