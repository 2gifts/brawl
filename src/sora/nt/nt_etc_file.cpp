#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <if/if_snap_save.h>
#include <nt/nt_etc_file.h>
#include <nt/nt_etc_global.h>
#include <sr/sr_getappname.h>
#include <string.h>
#include <types.h>

extern "C" u32 NETCalcCRC32(const void* data, u32 len);

u8 nteSection::getDataType() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 4;
    }
    return data->m_dataType;
}

u8 nteSection::getStateType() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 4;
    }
    return data->m_stateType;
}

s64 nteSection::getCreated() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 0;
    }
    return data->m_created;
}

bool nteSection::setOwnState(s64 created, u32 state) {
    GameGlobal* global;
    nteSectionData* data = m_data;
    if (data == NULL) {
        return false;
    }
    if (state >= 4) {
        return false;
    }
    data->m_wiiId = nteGlobal::getWiiID();
    global = g_GameGlobal;
    data->m_region = (global->getDistributionType() << 5) | global->getLanguage();
    data->m_stateType = state;
    data->m_created = created;
    data->m_crc = 0xDEADBEEF;
    data->m_crc = NETCalcCRC32(data, data->m_dataSize + 0x20);
    return true;
}

nteFile::nteFile(Heaps::HeapType heap) {
    m_heap = heap;
    m_count = 0;
}

nteFile::~nteFile() {
    for (u32 i = 0; i < m_count; i++) {
        if (m_entries[i].m_data != NULL) {
            gfHeapManager::free(m_entries[i].m_data);
            m_entries[i].m_data = NULL;
        }
    }
}

u32 nteFile::getDataFileSize() {
    u32 size = m_count * 12 + 8;
    for (u32 i = 0; i < m_count; i++) {
        size += m_entries[i].m_dataSize;
        size += 0x20;
    }
    u32 rem = size & 0xF;
    if (rem != 0) {
        size += 0x10 - rem;
    }
    return size;
}

nteFileDataPhoto::~nteFileDataPhoto() {
    if (unkC_ptr != NULL) {
        gfHeapManager::free(unkC_ptr);
    }
}

void nteFileDataPhoto::setData(void* data, u32 size) {
    if (data == NULL || size == 0) {
        return;
    }
    if (unk10 != size) {
        u8* buffer = static_cast<u8*>(gfHeapManager::alloc(unk4, size + 0x34, 32));
        if (buffer != NULL) {
            if (unkC_ptr != NULL) {
                memcpy(buffer, unkC_ptr, 0x34);
            } else {
                memset(buffer, 0, 0x34);
            }
            memcpy(buffer + 0x34, data, size);
        }
        if (unkC_ptr != NULL) {
            gfHeapManager::free(unkC_ptr);
        }
        unkC_ptr = buffer;
        if (buffer != NULL) {
            unk10 = size;
        } else {
            unk10 = 0;
        }
    } else if (unkC_ptr != NULL) {
        memcpy(unkC_ptr + 0x34, data, size);
    }
}

nteFileDataStage::nteFileDataStage(Heaps::HeapType heap) {
    unkC = static_cast<u8*>(gfHeapManager::alloc(heap, 0x34, 32));
    if (unkC != NULL) {
        memset(unkC, 0, 0x34);
    }
    unk10 = 0;
    unk0 = 3;
    unk4 = heap;
}

nteFileDataStage::~nteFileDataStage() {
    if (unkC != NULL) {
        gfHeapManager::free(unkC);
    }
}

void nteFileDataStage::setData(void* data, u32 size) {
    if (data == NULL || size == 0) {
        return;
    }
    if (unk10 != size) {
        u8* buffer = static_cast<u8*>(gfHeapManager::alloc(unk4, size + 0x34, 32));
        if (buffer != NULL) {
            if (unkC != NULL) {
                memcpy(buffer, unkC, 0x34);
            } else {
                memset(buffer, 0, 0x34);
            }
            memcpy(buffer + 0x34, data, size);
        }
        if (unkC != NULL) {
            gfHeapManager::free(unkC);
        }
        unkC = buffer;
        if (buffer != NULL) {
            unk10 = size;
        } else {
            unk10 = 0;
        }
    } else if (unkC != NULL) {
        memcpy(unkC + 0x34, data, size);
    }
}

nteFileDataSetting::~nteFileDataSetting() {
    if (unkC != NULL) {
        gfHeapManager::free(unkC);
    }
}

u64 nteSection::getWiiId() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 0;
    }
    return data->m_wiiId;
}
