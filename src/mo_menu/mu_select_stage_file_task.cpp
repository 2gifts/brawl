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
    int m_state; // 0x00
    u8 m_unk4[0x58 - 0x4];
};

class muSelectStageFileTask : public gfTask {
public:
    int m_state;                     // 0x40
    u8 m_unk44[0x68 - 0x44];
    gfCollectionIO* m_io;            // 0x68
    u8 m_unk6C[0x70 - 0x6C];
    muSelectStageFileData m_data[4]; // 0x70

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
