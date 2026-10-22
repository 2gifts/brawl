#pragma once
#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_system_callback.h>
#include <gf/gf_task.h>
#include <gf/gf_thread.h>
#include <sr/sr_common.h>
#include <types.h>

class gfCollectionIO;
// HYPOTHESIS: opaque; only its constructor and destructor are used here.
class nteWiiConnect24;

class gfCollectionIO_DSThread : public gfThread {
public:
    gfCollectionIO* m_owner; // 0x340
    u8 unk344[0x4];

    gfCollectionIO_DSThread(gfCollectionIO* owner, Heaps::HeapType heap)
        : gfThread(this, 0xF, 0x1000, heap), m_owner(owner) {
        OSResumeThread(&m_thread);
    }
    virtual void run();
};
static_assert(sizeof(gfCollectionIO_DSThread) == 0x348, "Class is the wrong size!");

class gfCIOPermitRebootCallback : public gfReturnStatusCallback {
public:
    bool m_canReturn; // 0x08

    virtual bool returnStatus() const;
};
static_assert(sizeof(gfCIOPermitRebootCallback) == 0xC, "Class is the wrong size!");

class gfCollectionIO : public gfRunnable, public gfTask {
public:
    enum Kind {
        Kind_None,
        Kind_NewFileName,
        Kind_Save,
        Kind_Load,
        Kind_List,
        Kind_ListCount,
        Kind_Summary,
        Kind_DriveStatus,
        Kind_Delete,
    };

    enum Device {
        Device_VF,
        Device_SD,
    };

    // HYPOTHESIS: local type name; the layout is what every request function fills in.
    struct Request {
        int m_kind;              // 0x00
        int m_device;            // 0x04
        int m_fileType;          // 0x08
        char m_fileName[0x80];   // 0x0C
        void* m_data;            // 0x8C
        int m_size;              // 0x90
        bool unk94;              // 0x94
    };

    gfFileIOHandle m_handle;     // 0x44
    gfFileIOHandle m_sdHandle;   // 0x48
    gfFileIOHandle m_vfHandle;   // 0x4C
    Heaps::HeapType m_heap;      // 0x50
    nteWiiConnect24* m_wc24;     // 0x54
    gfThread* m_thread;          // 0x58
    bool m_isReady;              // 0x5C
    int m_result;                // 0x60
    char m_fileName[0x80];       // 0x64
    int unkE4;
    int unkE8;
    int unkEC;
    Request m_request;           // 0xF0
    gfCollectionIO_DSThread* m_dsThread; // 0x188
    int m_dsState;               // 0x18C
    int m_sdChangeCount;         // 0x190

    gfCollectionIO(Heaps::HeapType heap);
    virtual ~gfCollectionIO();
    virtual void run();
    virtual void processDefault();

    bool isDropReady();
    // HYPOTHESIS: parameter types from the callers (file type, file name, data, size, flag).
    bool requestNewFileNameVF(int fileType);
    bool requestNewFileNameSD(int fileType);
    bool requestSaveVF(int fileType, const char* fileName, void* data, int size, bool unk);
    bool requestSaveSD(int fileType, const char* fileName, void* data, int size, bool unk);
    bool requestLoadVF(int fileType, const char* fileName);
    bool requestLoadSD(int fileType, const char* fileName);
    bool requestListVF(int fileType);
    bool requestListSD(int fileType);
    bool requestListCountVF(int fileType);
    bool requestListCountSD(int fileType);
    bool requestSummaryVF(int fileType, const char* fileName);
    bool requestSummarySD(int fileType, const char* fileName);
    bool requestDeleteVF(int fileType, const char* fileName);
    bool requestDeleteSD(int fileType, const char* fileName);
    static const char* getDeliveryFileName(int fileType);
    static const char* getSampleFileName(int fileType, int index);
    static bool isEnableVFSaveLoad();
};
static_assert(sizeof(gfCollectionIO) == 0x194, "Class is the wrong size!");

// HYPOTHESIS: layouts only as far as the destructors show them.
class gfCollSummaryCommon {
public:
    u8 unk0[0x50];

    virtual ~gfCollSummaryCommon() { }
};

class gfCollSummaryStageEdit : public gfCollSummaryCommon {
public:
    u8 unk54[0x2C];
    void* unk80;

    virtual ~gfCollSummaryStageEdit();
};

class gfCollSummaryReplay : public gfCollSummaryCommon {
public:
    u8 unk54[0x70];
    void* unkC4;

    virtual ~gfCollSummaryReplay();
};

class gfCollSummarySnapShot : public gfCollSummaryCommon {
public:
    u8 unk54[0x8];
    void* unk5C;

    virtual ~gfCollSummarySnapShot();
};
