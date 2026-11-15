#include <gm/gm_global.h>
#include <snd/snd_system.h>
#include <st/loader/st_loader_stage_se.h>
#include <st/stage.h>
#include <types.h>

// A stage id (or the upper bytes of one) and the sound group of the stages with that id.
struct stLoaderSeGroupEntry {
    u32 m_stageId;
    int m_groupId;
};

// HYPOTHESIS: the sound group of every melee stage kind (0x3A entries), and the Subspace stage tables (ended by an id of
// -1); defined in this unit, not transcribed yet.
extern const int lbl_27_data_4DB80[0x3A];
extern const stLoaderSeGroupEntry lbl_27_rodata_3A30[];
extern const stLoaderSeGroupEntry lbl_27_rodata_3CD0[];

// BYTES: the derived loaders call these constructors and destructors out of line.
#pragma dont_inline on
stLoaderStageSe::stLoaderStageSe() : stLoader("StLoaderStageSe") {
    m_groupHandle = -1;
    m_groupId = -1;
    m_heapLevel = 1;
}

stLoaderStageSe::~stLoaderStageSe() { }
#pragma dont_inline reset

int stLoaderStageSe::buildSoundGroup() {
    u16 stageKind = *reinterpret_cast<u16*>(reinterpret_cast<u8*>(&m_globalModeMelee->m_meleeInitData) + 0x12);
    if (stageKind > 0x39) {
        return -1;
    }
    return lbl_27_data_4DB80[stageKind];
}

void stLoaderStageSe::processBegin() {
    if ((u8)_0x45[1] == 0) {
        return;
    }
    switch (m_state) {
    case 0:
        if ((u8)_0x45[2] == 1) {
            m_state = 6;
        }
        m_groupId = buildSoundGroup();
        if ((u32)m_groupId != 0xFFFFFFFF) {
            m_state = 1;
        }
        break;
    case 1:
        m_groupHandle = g_sndSystem->loadSoundGroup(static_cast<SndGroupID>(m_groupId), m_heapLevel, true);
        m_state = 2;
        break;
    case 2:
        if (g_sndSystem->isFinishLoad1(m_groupId)) {
            m_state = 3;
        }
        break;
    case 3:
        if (g_Stage != NULL) {
            reinterpret_cast<int*>(g_Stage)[0xB8 / 4] = m_groupHandle;
        }
        if ((u8)_0x45[2] == 1 || (u32)m_groupId != (u32)buildSoundGroup()) {
            m_state = 5;
        }
        break;
    case 5:
        g_sndSystem->freeGroup(m_groupHandle, m_heapLevel);
        m_groupHandle = -1;
        if ((u8)_0x45[2] == 1) {
            m_state = 6;
        } else {
            m_state = 0;
        }
        break;
    }
}

bool stLoaderStageSe::isLoad() {
    if ((u32)buildSoundGroup() == 0xFFFFFFFF) {
        return true;
    }
    return m_state == 3;
}

stLoaderStageSeAdventure::stLoaderStageSeAdventure() {
    m_heapLevel = 0x18;
}

stLoaderStageSeAdventure::~stLoaderStageSeAdventure() { }

int stLoaderStageSeAdventure::buildSoundGroup() {
    u32 stageId = g_GameGlobal->m_advData->unk624;
    u32 area = stageId & 0xFFFF0000;
    if (area == 0x2A000000) {
        u32 key = stageId & 0xFFFFFF00;
        const stLoaderSeGroupEntry* entry = lbl_27_rodata_3CD0;
        for (int i = 0; entry->m_stageId != 0xFFFFFFFF; entry++, i++) {
            if (key == (entry->m_stageId & 0xFFFFFF00)) {
                return lbl_27_rodata_3CD0[i].m_groupId;
            }
        }
        return -1;
    }
    const stLoaderSeGroupEntry* entry = lbl_27_rodata_3A30;
    for (int i = 0; entry->m_stageId != 0xFFFFFFFF; entry++, i++) {
        if (area == (entry->m_stageId & 0xFFFF0000)) {
            if ((entry->m_stageId & 0xFFFF) == 0) {
                return lbl_27_rodata_3A30[i].m_groupId;
            }
            if (stageId == entry->m_stageId) {
                return lbl_27_rodata_3A30[i].m_groupId;
            }
        }
    }
    return -1;
}

stLoaderCommonSeAdventure::stLoaderCommonSeAdventure() {
    m_heapLevel = 0x17;
}

stLoaderCommonSeAdventure::~stLoaderCommonSeAdventure() { }

int stLoaderCommonSeAdventure::buildSoundGroup() {
    return 0xD9;
}

#pragma dont_inline on
stLoaderPokemonSe::stLoaderPokemonSe() : stLoader("StLoaderPokemonSe") {
    m_groupHandle = -1;
    m_groupId = -1;
    m_heapLevel = 2;
}

stLoaderPokemonSe::~stLoaderPokemonSe() { }
#pragma dont_inline reset

int stLoaderPokemonSe::buildSoundGroup() {
    return 0x77;
}

void stLoaderPokemonSe::processBegin() {
    if ((u8)_0x45[1] == 0) {
        return;
    }
    switch (m_state) {
    case 0:
        if ((u8)_0x45[2] == 1) {
            m_state = 6;
        }
        m_groupId = buildSoundGroup();
        if ((u32)m_groupId != 0xFFFFFFFF) {
            m_state = 1;
        }
        break;
    case 1:
        m_groupHandle = g_sndSystem->loadSoundGroup(static_cast<SndGroupID>(m_groupId), m_heapLevel, true);
        m_state = 2;
        break;
    case 2:
        if (g_sndSystem->isFinishLoad1(m_groupId)) {
            m_state = 3;
        }
        break;
    case 3:
        if ((u8)_0x45[2] == 1 || (u32)m_groupId != (u32)buildSoundGroup()) {
            m_state = 5;
        }
        break;
    case 5:
        g_sndSystem->freeGroup(m_groupHandle, m_heapLevel);
        m_groupHandle = -1;
        if ((u8)_0x45[2] == 1) {
            m_state = 6;
        } else {
            m_state = 0;
        }
        break;
    }
}
