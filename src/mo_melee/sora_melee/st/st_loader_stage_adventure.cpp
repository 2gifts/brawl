#include <cstdio>
#include <cstring>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <st/loader/st_loader_stage_adventure.h>
#include <types.h>

// MATCH-ONLY: the native code reads the 16-bit stage kind unsigned, not through the signed enum bitfield.
static inline u32 getStageKind(gmGlobalModeMelee* melee) {
    return *reinterpret_cast<u16*>(reinterpret_cast<u8*>(&melee->m_meleeInitData) + 0x12);
}

stLoaderStageAdventure::stLoaderStageAdventure() {
    m_advStageId = -1;
    m_advData = g_GameGlobal->m_advData;
    unk98 = 1;
}

stLoaderStageAdventure::~stLoaderStageAdventure() {
    if (m_archive) {
        delete m_archive;
    }
    if (m_secondaryArchive) {
        delete m_secondaryArchive;
    }
    if (m_fileArchive) {
        delete m_fileArchive;
    }
    if (m_effectArchive) {
        delete m_effectArchive;
    }
    if (m_secondaryData) {
        gfHeapManager::free(m_secondaryData);
    }
    if (m_fileData) {
        gfHeapManager::free(m_fileData);
    }
    if (m_effectData) {
        gfHeapManager::free(m_effectData);
    }
    m_archive = nullptr;
    m_secondaryArchive = nullptr;
    m_fileArchive = nullptr;
    m_effectArchive = nullptr;
    m_secondaryData = nullptr;
    m_fileData = nullptr;
    m_effectData = nullptr;
}

// The file is /STAGE/ADVENTURE/<a><b><c>[<d>].PAC from the bytes of the stage id; the extra arguments are in the
// original call.
void stLoaderStageAdventure::buildFileName() {
    m_effectResourceId = getStageKind(m_globalModeMelee) + 0x31;
    m_stageKind = getStageKind(m_globalModeMelee);
    m_subStageKind = m_globalModeMelee->m_meleeInitData.m_subStageKind;
    // HYPOTHESIS: the word the header calls m_itemGenKind holds the adventure stage id here.
    m_advStageId = m_advData->m_itemGenKind;
    u32 a = (m_advStageId >> 24) & 0xFF;
    u32 b = (m_advStageId >> 16) & 0xFF;
    u32 c = (m_advStageId >> 8) & 0xFF;
    u32 d = m_advStageId & 0xFF;
    memset(m_fileName, 0, sizeof(m_fileName));
    if (d == 0) {
        sprintf(m_fileName, "/STAGE/ADVENTURE/%02d%02d%02d.PAC", a, b, c, a, b, c);
    } else {
        sprintf(m_fileName, "/STAGE/ADVENTURE/%02d%02d%02d%c.PAC", a, b, c, (s8)(d + 0x60), a, b, c, (s8)(d + 0x60));
    }
}

bool stLoaderStageAdventure::isRemove() {
    if ((u8)_0x45[2] == 1 || m_stageKind != getStageKind(m_globalModeMelee) ||
        m_subStageKind != m_globalModeMelee->m_meleeInitData.m_subStageKind || m_advStageId != m_advData->m_itemGenKind)
    {
        return true;
    }
    return false;
}
