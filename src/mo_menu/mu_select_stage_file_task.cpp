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
