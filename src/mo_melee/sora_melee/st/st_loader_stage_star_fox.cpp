#include <cstring>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <st/loader/st_loader_stage_star_fox.h>
#include <types.h>

extern "C" bool fn_27_23ABA0(stLoader* loader);

// MATCH-ONLY: the native code reads the 16-bit stage kind unsigned, not through the signed enum bitfield.
static inline u32 getStageKind(gmGlobalModeMelee* melee) {
    return *reinterpret_cast<u16*>(reinterpret_cast<u8*>(&melee->m_meleeInitData) + 0x12);
}

stLoaderStageStarFox::stLoaderStageStarFox() {
    m_partIndex = 0;
    m_loadedStageKind = 0;
    m_isLocked = false;
    m_table[0] = 0;
    m_table[1] = 1;
    m_table[2] = 2;
    m_table[3] = 3;
}

stLoaderStageStarFox::~stLoaderStageStarFox() {
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

void stLoaderStageStarFox::buildFileName() {
    static const char* s_starFoxParts[] = { "ASTEROID", "BATTLESHIP", "CORNERIA", "SPACE" };
    static const char* s_emblemParts[] = { "00", "01", "02" };

    memset(m_fileName, 0, sizeof(m_fileName));
    if (getStageKind(m_globalModeMelee) == 0x13) {
        strcpy(m_fileName, "/STAGE/MELEE/STGSTARFOX_");
        strcat(m_fileName, s_starFoxParts[m_partIndex]);
    } else {
        strcpy(m_fileName, "/STAGE/MELEE/STGEMBLEM_");
        strcat(m_fileName, s_emblemParts[m_partIndex]);
    }
    strcat(m_fileName, ".PAC");
    m_loadedStageKind = getStageKind(m_globalModeMelee);
}

void stLoaderStageStarFox::setStage(Stage* stage) {
    m_stage = stage;
}

// Shuffles the four parts (only a four part table is shuffled), never starting with the part that ended the last round.
void stLoaderStageStarFox::shuffuleTable(int num) {
    m_tableIndex = 0;
    int last = m_table[3];
    m_tableNum = num;
    if (num == 4) {
        for (int i = 0; i < 32; i++) {
            int a = randi(4);
            int b = randi(4);
            u8 tmp = m_table[a];
            m_table[a] = m_table[b];
            m_table[b] = tmp;
        }
        u8 first = m_table[0];
        if (last == first) {
            m_table[0] = m_table[1];
            m_table[1] = first;
        }
    }
}

bool stLoaderStageStarFox::isLoad() {
    return fn_27_23ABA0(this);
}
