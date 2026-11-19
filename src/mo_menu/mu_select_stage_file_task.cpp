#include <gf/gf_collection_io.h>
#include <gf/gf_task.h>
#include <types.h>

// The task of the stage selection that lists and loads the custom stage files (module sora_menu_sel_stage). The class is
// only described by the members the decompiled functions touch.
/// Function names come from the sora_menu_sel_stage map.

// gfCollectionIO::isDropReady()
extern "C" int fn_800390C8(gfCollectionIO* io);
// 0x10 bytes of the game data: the first one is non-zero while the device of the files can not be used
extern u8 lbl_80423690[0x10];

// HYPOTHESIS: one listed file (the name, then flags).
struct muSelectStageFileInfo {
    char m_name[0x17];
    u8 m_delivery : 1; // 0x17
    u8 m_flags : 7;
};

// HYPOTHESIS: the list of the files of one device.
struct muSelectStageFileList {
    int m_revision;                  // 0x00
    int m_numFile;                   // 0x04
    muSelectStageFileInfo* m_files;  // 0x08
    int m_unkC;
};

// HYPOTHESIS: the data of one stage file that is loaded.
struct muSelectStageFileData {
    int m_state;  // 0x00
    int m_device; // 0x04 the device of the file (0: the NAND, 1: the SD card)
    int m_index;  // 0x08 the index of the file in the list of the device
    u8 m_unkC[0x58 - 0xC];
};

class muSelectStageFileTask : public gfTask {
public:
    int m_state;                     // 0x40
    int m_curData;                   // 0x44
    u8 m_unk48[0x68 - 0x48];
    gfCollectionIO* m_io;            // 0x68
    u8 m_unk6C[0x70 - 0x6C];
    muSelectStageFileData m_data[9]; // 0x70
    int m_editDevice;                // 0x388
    int m_editIndex;                 // 0x38C
    int m_editState;                 // 0x390

    void exit();
    int getFileListRevesion(int device);
    int getNumFile(int device);
    int isDeliveryData(int device, int index);
    const char* getFileName(int device, int index);
    void deleteFileData(int index);
    muSelectStageFileData* getFileData(int index);
    int isDropReady();
    int mainStepDevNotReady();
    void initStepNANDListing();
    void initStepSDListing();
    void deleteAllFileData();
    void cancelAllLoadFileData();
    void initStepLoading();
    void initStepEditDataLoading();

    muSelectStageFileList* getList(int device) {
        return (muSelectStageFileList*)((u8*)this + 0x4C) + device;
    }
};

void muSelectStageFileTask::exit() {
    if (m_io != NULL) {
        m_io->exit();
        m_io = NULL;
    }
    gfTask::exit();
}

int muSelectStageFileTask::getFileListRevesion(int device) {
    return getList(device)->m_revision;
}

int muSelectStageFileTask::getNumFile(int device) {
    return getList(device)->m_numFile;
}

int muSelectStageFileTask::isDeliveryData(int device, int index) {
    return getList(device)->m_files[index].m_delivery == 1;
}

const char* muSelectStageFileTask::getFileName(int device, int index) {
    return getList(device)->m_files[index].m_name;
}

void muSelectStageFileTask::deleteFileData(int index) {
    if (m_data[index].m_state == 2) {
        m_data[index].m_state = 5;
    } else {
        m_data[index].m_state = 0;
    }
}

muSelectStageFileData* muSelectStageFileTask::getFileData(int index) {
    return &m_data[index];
}

int muSelectStageFileTask::isDropReady() {
    if (m_state != 5) {
        return 0;
    }
    return fn_800390C8(m_io);
}

int muSelectStageFileTask::mainStepDevNotReady() {
    if (lbl_80423690[0] != 0) {
        return 5;
    }
    return m_state;
}

void muSelectStageFileTask::initStepNANDListing() {
    m_io->requestListVF(3);
}

void muSelectStageFileTask::initStepSDListing() {
    m_io->requestListSD(3);
}

void muSelectStageFileTask::deleteAllFileData() {
    for (int i = 0; i < 9; i++) {
        deleteFileData(i);
    }
}

void muSelectStageFileTask::cancelAllLoadFileData() {
    for (int i = 1; i < 9; i++) {
        if (m_data[i].m_state == 1) {
            m_data[i].m_state = 0;
        }
    }
}

void muSelectStageFileTask::initStepLoading() {
    muSelectStageFileData* data = &m_data[m_curData];
    data->m_state = 2;
    int device = data->m_device;
    const char* name = getFileName(device, data->m_index);
    if (device == 1) {
        m_io->requestSummarySD(3, name);
    } else {
        m_io->requestSummaryVF(3, name);
    }
}

void muSelectStageFileTask::initStepEditDataLoading() {
    int device = m_editDevice;
    const char* name = getFileName(device, m_editIndex);
    if (device == 1) {
        m_io->requestLoadSD(3, name);
    } else {
        m_io->requestLoadVF(3, name);
    }
    m_editState = 2;
}

#include <gf/gf_heap_manager.h>
#include <string.h>
#include <cstring>
// TODO: clean up, give variables real names
namespace jj_7600 {

extern "C" {
extern const char lbl_11_data_668[];
extern char lbl_11_data_688[];

struct Counts {
    int a;
    int b;
};

extern Counts lbl_11_rodata_248;

void* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(
    void*, const char*, gfTask::Category, int, int, bool);
void* __ct__14gfCollectionIOFQ25Heaps8HeapType(void*, int);
}

extern "C" void* __ct__21muSelectStageFileTaskFv(void* self) {
    char* p = (char*)self;

    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(
        self, lbl_11_data_668, gfTask::Category_Menu, 15, 8, true);

    *(void**)(p + 0x3C) = lbl_11_data_688;
    *(int*)(p + 0x40) = 0;
    *(int*)(p + 0x44) = 0;
    *(int*)(p + 0x68) = 0;
    *(char*)(p + 0x398) = 0;

    Counts counts = lbl_11_rodata_248;
    *(unsigned char*)(p + 0x2C) &= ~2;
    memset(p + 0x48, 0, 0x20);

    int i = 0;
    int* cur = &counts.a;
    int offset = 0;
    do {
        void* ptr = gfHeapManager::alloc(
            (Heaps::HeapType)0x2A, *cur * 0x18);
        ++i;
        *(void**)(p + offset + 0x54) = ptr;
        ++cur;
        offset += 0x10;
    } while (i < 2);

    memset(p + 0x70, 0, 0x318);

    offset = 0;
    i = 0;
    do {
        void* ptr = gfHeapManager::alloc(
            (Heaps::HeapType)0x2A, 0x12C00, 0x20);
        ++offset;
        *(void**)(p + i + 0x80) = ptr;
        i += 0x58;
    } while (offset < 9);

    memset(p + 0x388, 0, 0x10);

    void* obj = operator new(0x194, (Heaps::HeapType)0x2A);
    if (obj) {
        obj = __ct__14gfCollectionIOFQ25Heaps8HeapType(obj, 0x2A);
    }
    *(void**)(p + 0x68) = obj;

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_775C {
typedef unsigned char u8;

extern "C" {
extern u8 lbl_11_data_688[];

void free__13gfHeapManagerFPv(void*);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
}

extern "C" void* __dt__21muSelectStageFileTaskFv(u8* self, int deleting)
{
    if (self) {
        *reinterpret_cast<void**>(self + 0x3C) = lbl_11_data_688;

        u8* cursor = self;
        for (int i = 0; i < 9; ++i, cursor += 0x58) {
            void* heap = *reinterpret_cast<void**>(cursor + 0x80);
            if (heap) {
                free__13gfHeapManagerFPv(heap);
            }
        }

        cursor = self;
        for (int i = 0; i < 2; ++i, cursor += 0x10) {
            void* heap = *reinterpret_cast<void**>(cursor + 0x54);
            if (heap) {
                free__13gfHeapManagerFPv(heap);
            }
        }

        __dt__6gfTaskFv(self, 0);

        if (deleting > 0) {
            __dl__FPv(self);
        }
    }

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_7870 {
extern "C" unsigned char lbl_80423690[];

extern "C" unsigned char* fn_11_7870() {
    return lbl_80423690;
}
}

// TODO: clean up, give variables real names
namespace jj_7BE4 {
extern "C" int getDeliveryFileName__14gfCollectionIOFi(int);
extern "C" int makeFileList__21muSelectStageFileTaskFv(void* self, int type, unsigned int count, const char* strings) {
    char* base = (char*)self + type * 16;
    int offset;
    char* entry;
    int accepted;
    unsigned int i;
    int found;
    accepted = 0;
    found = 0;
    i = 0;
    offset = 0;
    while (i < count && accepted < 1000) {
        int len = strlen(strings);
        if (len <= 22) {
            entry = *(char**)(base + 0x54) + offset;
            strcpy(entry, strings);
            entry[0x17] &= ~0x80;
            if (type == 0 && found == 0 && strcmp(strings, (const char*)getDeliveryFileName__14gfCollectionIOFi(3)) == 0) {
                found = 1;
                entry[0x17] |= 0x80;
            }
            offset += 24;
            accepted++;
        }
        i++;
        strings += len + 1;
    }
    return accepted;
}
}

// TODO: clean up, give variables real names
namespace jj_7CF0 {
#pragma scheduling off

typedef int s32;

struct StageEntry {
    s32 active;
    s32 arg1;
    s32 arg2;
    char padding[0x4C];
};

extern "C" s32 loadFileData__21muSelectStageFileTaskFv(void* arg0, s32 arg1, s32 arg2) {
    StageEntry* entry = (StageEntry*)((char*)arg0 + 0xC8);
    s32 result = 1;

    if (entry->active != 0) {
        s32 active2 = (++entry)->active;
        result = 2;
        if (active2 != 0) {
            s32 active3 = (++entry)->active;
            result = 3;
            if (active3 != 0) {
                s32 active4 = (++entry)->active;
                result = 4;
                if (active4 != 0) {
                    s32 active5 = (++entry)->active;
                    result = 5;
                    if (active5 != 0) {
                        s32 active6 = (++entry)->active;
                        result = 6;
                        if (active6 != 0) {
                            s32 active7 = (++entry)->active;
                            result = 7;
                            if (active7 != 0) {
                                s32 active8 = (++entry)->active;
                                result = 8;
                                if (active8 != 0) {
                                    result = 9;
                                    ++entry;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (result >= 9)
        return -1;

    entry->active = 1;
    entry->arg1 = arg1;
    entry->arg2 = arg2;
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_85E8 {
extern "C" s32 MuStageTblAcces_GetForceExistFlag__14MuStageTblPageFv(void) {
    return 0x0;
}
}

