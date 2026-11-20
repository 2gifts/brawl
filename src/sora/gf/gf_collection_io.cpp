#include <gf/gf_file_io_handle.h>
#include <string.h>
#include <cstring>
#include <gf/gf_collection_io.h>
#include <gf/gf_file_io.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <nt/nt_etc_nwc24.h>
#include <revolution/OS/OSHardware.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

// HYPOTHESIS: stand-ins for callees that are not named yet; the map gives this unit's request workers.
// (GameGlobal::getEnableSaveLoad bit 3 enables VF save/load.)
extern "C" int fn_8003A23C(gfCollectionIO* io, gfCollectionIO::Request* request);
extern "C" int fn_8003A6F0(gfCollectionIO* io, gfCollectionIO::Request* request);
extern "C" int fn_8003AD48(gfCollectionIO* io, gfCollectionIO::Request* request);
extern "C" int fn_8003B760(gfCollectionIO* io, gfCollectionIO::Request* request);
extern "C" int fn_8003BBDC(gfCollectionIO* io, gfCollectionIO::Request* request);
extern "C" int fn_8003C000(gfCollectionIO* io, gfCollectionIO::Request* request);
extern "C" int fn_8003C4E4(gfCollectionIO* io, gfCollectionIO::Request* request, gfFileIOHandle* handle, bool wait);
extern "C" int fn_8003C934(gfCollectionIO* io, gfCollectionIO::Request* request);

// HYPOTHESIS: data names are the split's labels; the mutexes are gfMutex objects in the original.
gfCIOPermitRebootCallback lbl_804976D8;
OSMutex lbl_804976F0;
OSMutex lbl_80497718;
OSMutex lbl_80497740;
OSMutex lbl_80497768;
gfCollectionIO* lbl_80497780[10];
OSMutex* lbl_8059C6D4 = &lbl_804976F0;
OSMutex* lbl_8059C6D8 = &lbl_80497718;
u16 lbl_805A00A8;
u16 lbl_805A00AA;
const char* lbl_80423550[] = { "", "al", "rp", "st" };
const char* lbl_80423560[] = { "", "/al", "/rp", "/st" };
const char* lbl_804235B8[] = { "", "al_000000_delivery.bin", "rp_000000_delivery.bin", "st_000000_delivery.bin" };
const char* lbl_80423640[] = { "", "al_000001_sample.bin", "rp_000001_sample.bin", "st_000001_sample.bin",
                               "st_000002_sample.bin", "st_000003_sample.bin" };
const char* lbl_80423680[] = { "", "/al_*.bin", "/rp_*.bin", "/st_*.bin" };
u8 lbl_80423690[0x10] = { 0, 0, 0, 0, 0, 0, 0, 1 };

// HYPOTHESIS: the gfFileIO::SDMountStatus parameter is unused.
extern "C" void fn_80038890(int status) {
    for (u16 i = 0; i < 10; i++) {
        if (lbl_80497780[i]) {
            lbl_80497780[i]->m_sdChangeCount++;
            return;
        }
    }
}

void gfCollectionIO_DSThread::run() {
    if (m_owner) {
        OSLockMutex(&lbl_80497740);
        {
            gfFileIOHandle handle;
            fn_8003C4E4(m_owner, nullptr, &handle, true);
            while (!handle.isReady()) {
                OSSleepTicks(OS_MSEC_TO_TICKS(1));
            }
        }
        OSUnlockMutex(&lbl_80497740);
    }
}

gfCollectionIO::gfCollectionIO(Heaps::HeapType heap)
    : gfTask("gfCollectionIO", Category_None, 0xE, 6, true), m_heap(heap), m_thread(nullptr) {
    m_wc24 = new (heap) nteWiiConnect24(heap);
    m_isReady = true;
    m_result = 0;
    m_fileName[0] = 0;
    m_request.m_kind = Kind_None;
    unkEC = 0;
    unkE4 = 0;
    unkE8 = 0;
    m_dsState = 0;
    m_sdChangeCount = 0;
    m_dsThread = nullptr;
    int i;
    gfCollectionIO** slot;
    for (i = 0, slot = lbl_80497780; i < 10; slot++, i++) {
        if (!*slot) {
            lbl_80497780[i] = this;
            break;
        }
    }
    gfFileIO::setSDMountStatusCallback(fn_80038890);
    m_sdChangeCount++;
    unk2C_b1 = false;
}

// MATCH-ONLY: every thread test keeps OSIsThreadTerminated(...) == TRUE in a bool first, as an
// inline bool accessor would; the comma form does the same inside the wait loops.
gfCollectionIO::~gfCollectionIO() {
    for (int i = 0; i < 10; i++) {
        if (lbl_80497780[i] == this) {
            lbl_80497780[i] = nullptr;
            break;
        }
    }
    if (m_dsThread) {
        bool terminated;
        while (terminated = OSIsThreadTerminated(&m_dsThread->m_thread) == TRUE, !terminated) {
            OSSleepTicks(OS_MSEC_TO_TICKS(1));
        }
        delete m_dsThread;
        m_dsThread = nullptr;
    }
    if (m_thread) {
        bool terminated;
        while (terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE, !terminated) {
            OSSleepTicks(OS_MSEC_TO_TICKS(1));
        }
        delete m_thread;
        m_thread = nullptr;
    }
    if (m_wc24) {
        delete m_wc24;
        m_wc24 = nullptr;
    }
    OSLockMutex(lbl_8059C6D4);
    if (lbl_805A00A8) {
        lbl_805A00A8--;
        if (m_vfHandle.unmountVF(0)) {
            while (!m_vfHandle.isReady()) {
                OSSleepTicks(OS_MSEC_TO_TICKS(1));
            }
            m_vfHandle.getReturnStatus();
            lbl_805A00A8 = 0;
        }
    }
    OSUnlockMutex(lbl_8059C6D4);
    OSLockMutex(lbl_8059C6D8);
    if (lbl_805A00AA) {
        lbl_805A00AA--;
        if (m_sdHandle.unmountSD()) {
            while (!m_sdHandle.isReady()) {
                OSSleepTicks(OS_MSEC_TO_TICKS(1));
            }
            m_sdHandle.getReturnStatus();
            lbl_805A00AA = 0;
        }
    }
    OSUnlockMutex(lbl_8059C6D8);
}

void gfCollectionIO::run() {
    OSLockMutex(&lbl_80497740);
    while (!m_handle.isReady()) {
        OSSleepTicks(OS_MSEC_TO_TICKS(1));
    }
    m_result = -1;
    switch (m_request.m_kind) {
    case Kind_NewFileName:
        m_result = fn_8003A23C(this, &m_request);
        break;
    case Kind_Save:
        lbl_804976D8.m_canReturn = false;
        m_result = fn_8003A6F0(this, &m_request);
        lbl_804976D8.m_canReturn = true;
        break;
    case Kind_Load:
        m_result = fn_8003AD48(this, &m_request);
        break;
    case Kind_List:
        m_result = fn_8003B760(this, &m_request);
        break;
    case Kind_ListCount:
        m_result = fn_8003BBDC(this, &m_request);
        break;
    case Kind_Summary:
        m_result = fn_8003C000(this, &m_request);
        break;
    case Kind_DriveStatus:
        m_result = fn_8003C4E4(this, &m_request, nullptr, true);
        break;
    case Kind_Delete:
        lbl_804976D8.m_canReturn = false;
        m_result = fn_8003C934(this, &m_request);
        lbl_804976D8.m_canReturn = true;
        break;
    }
    while (!m_handle.isReady()) {
        OSSleepTicks(OS_MSEC_TO_TICKS(1));
    }
    m_isReady = true;
    OSUnlockMutex(&lbl_80497740);
}

void gfCollectionIO::processDefault() {
    switch (m_dsState) {
    case 0:
        if (m_sdChangeCount != 0) {
            m_sdChangeCount = 0;
            m_dsState = 1;
        }
        break;
    case 1:
        m_dsThread = new (m_heap) gfCollectionIO_DSThread(this, m_heap);
        m_dsState++;
        break;
    case 2:
        bool terminated = OSIsThreadTerminated(&m_dsThread->m_thread) == TRUE;
        if (terminated) {
            delete m_dsThread;
            m_dsThread = nullptr;
            m_dsState = 0;
        }
        break;
    }
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (terminated) {
            delete m_thread;
            m_thread = nullptr;
        }
    }
}

bool gfCollectionIO::isDropReady() {
    if (!m_isReady) {
        return false;
    }
    if (m_dsState != 0 || m_sdChangeCount != 0) {
        return false;
    }
    if (m_dsThread) {
        bool terminated = OSIsThreadTerminated(&m_dsThread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
    }
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
    }
    return true;
}

bool gfCollectionIO::requestNewFileNameVF(int fileType) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        unkE8 = 0;
        unkE4 = 0;
        return true;
    }
    Request request;
    request.m_kind = Kind_NewFileName;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    request.m_fileName[0] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestNewFileNameSD(int fileType) {
    Request request;
    request.m_kind = Kind_NewFileName;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    request.m_fileName[0] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestSaveVF(int fileType, const char* fileName, void* data, int size, bool unk) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        unkE8 = 0;
        unkE4 = 0;
        return true;
    }
    Request request;
    request.m_kind = Kind_Save;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    request.m_data = data;
    request.m_size = size;
    request.unk94 = unk;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestSaveSD(int fileType, const char* fileName, void* data, int size, bool unk) {
    Request request;
    request.m_kind = Kind_Save;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    request.m_data = data;
    request.m_size = size;
    request.unk94 = unk;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestLoadVF(int fileType, const char* fileName) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        unkE8 = 0;
        unkE4 = 0;
        return true;
    }
    Request request;
    request.m_kind = Kind_Load;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestLoadSD(int fileType, const char* fileName) {
    Request request;
    request.m_kind = Kind_Load;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestListVF(int fileType) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        unkE8 = 0;
        unkE4 = 0;
        return true;
    }
    Request request;
    request.m_kind = Kind_List;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestListSD(int fileType) {
    Request request;
    request.m_kind = Kind_List;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestListCountVF(int fileType) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        unkE8 = 0;
        unkE4 = 0;
        return true;
    }
    Request request;
    request.m_kind = Kind_ListCount;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestListCountSD(int fileType) {
    Request request;
    request.m_kind = Kind_ListCount;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestSummaryVF(int fileType, const char* fileName) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        return true;
    }
    Request request;
    request.m_kind = Kind_Summary;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestSummarySD(int fileType, const char* fileName) {
    Request request;
    request.m_kind = Kind_Summary;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestDeleteVF(int fileType, const char* fileName) {
    if (!((g_GameGlobal->getEnableSaveLoad() >> 3) & 1)) {
        m_result = 8;
        m_isReady = true;
        return true;
    }
    Request request;
    request.m_kind = Kind_Delete;
    request.m_device = Device_VF;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

bool gfCollectionIO::requestDeleteSD(int fileType, const char* fileName) {
    Request request;
    request.m_kind = Kind_Delete;
    request.m_device = Device_SD;
    request.m_fileType = fileType;
    strncpy(request.m_fileName, fileName, sizeof(request.m_fileName));
    request.m_fileName[sizeof(request.m_fileName) - 1] = 0;
    if (m_thread) {
        bool terminated = OSIsThreadTerminated(&m_thread->m_thread) == TRUE;
        if (!terminated) {
            return false;
        }
        delete m_thread;
        m_thread = nullptr;
    }
    m_thread = new (m_heap) gfThread(this, 0xF, 0x1800, m_heap);
    if (!m_thread) {
        return false;
    }
    m_isReady = false;
    unkE4 = 0;
    unkE8 = 0;
    memcpy(&m_request, &request, sizeof(request));
    OSResumeThread(&m_thread->m_thread);
    return true;
}

void gfCollectionIO::getBasePathVF(int fileType, char* path) {
    strcpy(path, "vf0:");
    strcat(path, lbl_80423560[fileType]);
}

const char* gfCollectionIO::getDeliveryFileName(int fileType) {
    return lbl_804235B8[fileType];
}

const char* gfCollectionIO::getSampleFileName(int fileType, int index) {
    return lbl_80423640[fileType + index];
}

bool gfCollectionIO::isEnableVFSaveLoad() {
    return (g_GameGlobal->getEnableSaveLoad() >> 3) & 1;
}

gfCollSummaryStageEdit::~gfCollSummaryStageEdit() {
    if (unk80) {
        gfHeapManager::free(unk80);
    }
}

gfCollSummaryReplay::~gfCollSummaryReplay() {
    if (unkC4) {
        gfHeapManager::free(unkC4);
    }
}

gfCollSummarySnapShot::~gfCollSummarySnapShot() {
    if (unk5C) {
        gfHeapManager::free(unk5C);
    }
}

bool gfCIOPermitRebootCallback::returnStatus() const {
    return m_canReturn;
}

// TODO: clean up, give variables real names
namespace jj_8003C4A4 {
extern "C" void __dl__FPv(void*);

extern "C" {

// TODO: clean up, give variables real names
void* fn_8003C4A4(void* ptr, int flag) {
    if (ptr != 0 && flag > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}

}
}

// TODO: clean up, give variables real names
namespace jj_800390C8 {
extern "C" int OSIsThreadTerminated(void* thread);

struct Obj {
    unsigned char pad0[0x58];
    void* t58;
    unsigned char flag5c;
    unsigned char pad1[0x188 - 0x5D];
    void* t188;
    int f18c;
    int f190;
};

// TODO: clean up, give variables real names
extern "C" int fn_800390C8(Obj* __restrict self) {
    int r;
    if (self->flag5c == 0) {
        return 0;
    }
    if (self->f18c != 0 || self->f190 != 0) {
        return 0;
    }
    if (self->t188 != 0) {
        r = (OSIsThreadTerminated((char*)self->t188 + 8) - 1) == 0;
        if (!r) {
            return 0;
        }
    }
    if (self->t58 != 0) {
        r = (OSIsThreadTerminated((char*)self->t58 + 8) - 1) == 0;
        if (!r) {
            return 0;
        }
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_8003B420 {

extern "C" {
u8 getEnableSaveLoad__10GameGlobalFv(void* a);
u64 getTimeStampFileDeliv__9nteGlobalFv(void);
u8 getCollectionLifetime__9nteGlobalFv(void);
int getCollectionVFName__14gfSysRecorder2FPc(char* buf);
}
extern "C" void* g_GameGlobal;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" u16 lbl_805A00A8;
extern "C" char lbl_8059C6DC[5];
extern "C" char lbl_8059C708[2];
extern "C" const char* lbl_80423560[];
extern "C" const char* lbl_804235B8[];

struct Args {
    int unk0;
    int unk4;
    int index;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" s32 fn_8003B420(u8* self, Args* arg) {
    char path[0x40];
    char buf[0x40];
    int ok;

    if (!((getEnableSaveLoad__10GameGlobalFv(g_GameGlobal) >> 3) & 1)) {
        return 0;
    }
    int result = 0;
    s64 t0 = (s64)getTimeStampFileDeliv__9nteGlobalFv();
    s64 t1 = OSGetTime();
    u64 h = getCollectionLifetime__9nteGlobalFv();
    u64 freq = *(u32*)0x800000F8 / 4;
    if (t0 + (s64)((s64)h * 3600LL * freq) < t1) {
        int idx = arg->index;
        strcpy(path, lbl_8059C6DC);
        strcat(path, lbl_80423560[idx]);
        strcat(path, lbl_8059C708);
        strcat(path, lbl_804235B8[arg->index]);
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4C))->mountVF(0, buf))) {
            while (!((gfFileIOHandle*)(self + 0x4C))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x4C))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D4);
                ok = 0;
            } else {
                goto vf_inc;
            }
        } else {
vf_inc:
            lbl_805A00A8 = lbl_805A00A8 + 1;
            OSUnlockMutex(lbl_8059C6D4);
            ok = 1;
        }
        if (ok != 0) {
            while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x44))->checkFileRequest(path)) {
                while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() == 0 && ((gfFileIOHandle*)(self + 0x44))->del(path)) {
                    while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() == 0) {
                        result = 1;
                    }
                }
            }
            OSLockMutex(lbl_8059C6D4);
            if (lbl_805A00A8 != 0) {
                lbl_805A00A8 = lbl_805A00A8 - 1;
                if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4C))->unmountVF(0)) {
                    while (!((gfFileIOHandle*)(self + 0x4C))->isReady()) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    ((gfFileIOHandle*)(self + 0x4C))->getReturnStatus();
                    lbl_805A00A8 = 0;
                }
            }
            OSUnlockMutex(lbl_8059C6D4);
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8003CFC0 {
extern "C" void __dt__14gfCollectionIOFv(void*);

extern "C" void fn_8003CFC0(void* self) {
    __dt__14gfCollectionIOFv(reinterpret_cast<u8*>(self) - 0x4);
}
}

namespace jj_8003CFC8 {
extern "C" void processDefault__14gfCollectionIOFv(void*);

// TODO: clean up, give variables real names
extern "C" void fn_8003CFC8(void* self) {
    processDefault__14gfCollectionIOFv(reinterpret_cast<u8*>(self) - 0x4);
}
}

#include <sr/sr_getappname.h>
// TODO: clean up, give variables real names
namespace jj_8003A6F0 {

extern "C" {
int getCollectionVFName__14gfSysRecorder2FPc(char*);
void __ct__11gfCryptoESPFv(void*);
int encrypt__11gfCryptoESPFPCvUlQ25Heaps8HeapType(void*, unsigned int, unsigned int, int);
void __dt__11gfCryptoESPFv(void*, int);
int fn_8003C4E4(void*, int, int, int);
int sprintf(char*, const char*, ...);
}
extern "C" OSMutex lbl_80497768;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" OSMutex* lbl_8059C6D8;
extern "C" u16 lbl_805A00A8;
extern "C" u16 lbl_805A00AA;
extern "C" char lbl_8059C6DC[5];
extern "C" char lbl_8059C6E4[4];
extern "C" char lbl_8059C700[6];
extern "C" char MOD_PATCH_DIR[];
extern "C" const char* lbl_80423560[];

struct Args {
    int unk0;
    int mode;
    int index;
    char name[0x80];
    void* data;
    int length;
    u8 flag;
};

extern "C" int fn_8003A6F0(u8* self, Args* arg) {
    int result = 3;
    u32 locked = (u32)(-arg->mode | arg->mode) >> 31;
    int vf;
    int sd;
    char path[0x20];
    char buf2[0x40];
    char buf[0x40];
    char thread[0x371];

    if (locked != 0) OSLockMutex(&lbl_80497768);
    vf = 0;
    sd = 0;
    if (arg->mode == 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4c))->mountVF(0, buf))) {
            while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x4c))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D4);
                vf = 0;
            } else {
                goto vf_inc;
            }
        } else {
vf_inc:
            lbl_805A00A8 = lbl_805A00A8 + 1;
            OSUnlockMutex(lbl_8059C6D4);
            vf = 1;
        }
        if (lbl_805A00A8 == 0) goto cleanup;
        {
            int index = arg->index;
            strcpy(path, lbl_8059C6DC);
            strcat(path, lbl_80423560[index]);
        }
    } else {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->mountSD()) {
            while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x48))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D8);
                sd = 0;
            } else {
                goto sd_inc;
            }
        } else {
sd_inc:
            lbl_805A00AA = lbl_805A00AA + 1;
            OSUnlockMutex(lbl_8059C6D8);
            sd = 1;
        }
        if (lbl_805A00AA == 0) goto cleanup;
        {
            int index = arg->index;
            strcpy(path, lbl_8059C6E4);
            strcat(path, MOD_PATCH_DIR);
            strcat(path, srGetAppGamename());
            strcat(path, lbl_80423560[index]);
        }
    }
    if (!((gfFileIOHandle*)(self + 0x44))->checkDirRequest(path)) {
        result = 3;
    } else {
        while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
            OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
        }
        if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() == 0x12) {
            if (((gfFileIOHandle*)(self + 0x44))->createDirRequest(path)) {
                while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() != 0) goto cleanup;
            } else {
                goto cleanup;
            }
        }
        sprintf(buf2, lbl_8059C700, path, arg->name);
        if (arg->flag != 0 && ((gfFileIOHandle*)(self + 0x44))->checkFileRequest(buf2)) {
            while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() == 0) {
                if (((gfFileIOHandle*)(self + 0x44))->del(buf2)) {
                    while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() != 0) goto cleanup;
                } else {
                    goto cleanup;
                }
            }
        }
        if (arg->mode == 0) {
            if (((gfFileIOHandle*)(self + 0x44))->writeRequest(buf2, arg->data, arg->length, 0)) {
                goto final_loop;
            } else {
                goto cleanup;
            }
        } else {
            __ct__11gfCryptoESPFv(thread);
            *(u8*)(thread + 0x370) = 1;
            if (encrypt__11gfCryptoESPFPCvUlQ25Heaps8HeapType(thread, (unsigned int)arg->data, (unsigned int)arg->length, *(int*)(self + 0x50)) != 0) {
                while (*(u8*)(thread + 0x360) == 0) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                if (*(int*)(thread + 0x364) != 0) {
                    __dt__11gfCryptoESPFv(thread, -1);
                    goto cleanup;
                }
                if (((gfFileIOHandle*)(self + 0x44))->writeRequest(buf2, *(void**)(thread + 0x368), *(int*)(thread + 0x36c), 0) == 0) {
                    __dt__11gfCryptoESPFv(thread, -1);
                    goto cleanup;
                } else {
                    goto d054_c;
                }
            } else {
                __dt__11gfCryptoESPFv(thread, -1);
                goto cleanup;
            }
d054_c:
            __dt__11gfCryptoESPFv(thread, -1);
        }
final_loop:
        while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
            OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
        }
        if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() == 0) {
            result = 0;
            fn_8003C4E4(self, 0, 0, 1);
        }
    }
cleanup:
    if (vf != 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 != 0) {
            lbl_805A00A8 = lbl_805A00A8 - 1;
            if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4c))->unmountVF(0)) {
                while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x4c))->getReturnStatus();
                lbl_805A00A8 = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D4);
    }
    if (sd != 0) {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA != 0) {
            lbl_805A00AA = lbl_805A00AA - 1;
            if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->unmountSD()) {
                while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x48))->getReturnStatus();
                lbl_805A00AA = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D8);
    }
    if (locked != 0) OSUnlockMutex(&lbl_80497768);
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8003AD48 {

extern "C" {
int getCollectionVFName__14gfSysRecorder2FPc(char*);
void* __ct__11gfCryptoESPFv(void*);
int decrypt__11gfCryptoESPFPCvUlQ25Heaps8HeapType(void*, void*, unsigned int, int);
void* __dt__11gfCryptoESPFv(void*, int);
void fn_80153488(void*);
void fn_80153498(void*, int);
int fn_80153504(void*, void*);
int getDataType__10nteSectionFv(void*);
}
extern "C" OSMutex lbl_80497768;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" OSMutex* lbl_8059C6D8;
extern "C" u16 lbl_805A00A8;
extern "C" u16 lbl_805A00AA;
extern "C" char lbl_8059C6DC[5];
extern "C" char lbl_8059C6E4[4];
extern "C" char lbl_8059C708[2];
extern "C" char MOD_PATCH_DIR[];
extern "C" const char* lbl_80423560[];

struct Args { int unk0; int mode; int index; char name[1]; };

extern "C" int fn_8003AD48(u8* self, Args* arg) {
    char path[0x40];
    char buf[0x40];
    char thread[0x380];
    char sp8[8];
    int result = 4;
    bool locked;
    int vf;
    int sd;

    *(int*)(self + 0xEC) = 0;
    locked = arg->mode != 0;
    if (locked) OSLockMutex(&lbl_80497768);
    vf = 0;
    sd = 0;
    if (arg->mode == 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4c))->mountVF(0, buf))) {
            while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x4c))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D4);
                vf = 0;
            } else {
                goto vf_inc;
            }
        } else {
vf_inc:
            lbl_805A00A8 = lbl_805A00A8 + 1;
            OSUnlockMutex(lbl_8059C6D4);
            vf = 1;
        }
        if (lbl_805A00A8 == 0) goto cleanup;
        int index = arg->index;
        strcpy(path, lbl_8059C6DC);
        strcat(path, lbl_80423560[index]);
    } else {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->mountSD()) {
            while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x48))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D8);
                sd = 0;
            } else {
                goto sd_inc;
            }
        } else {
sd_inc:
            lbl_805A00AA = lbl_805A00AA + 1;
            OSUnlockMutex(lbl_8059C6D8);
            sd = 1;
        }
        if (lbl_805A00AA == 0) goto cleanup;
        int index = arg->index;
        strcpy(path, lbl_8059C6E4);
        strcat(path, MOD_PATCH_DIR);
        strcat(path, srGetAppGamename());
        strcat(path, lbl_80423560[index]);
    }
    strcat(path, lbl_8059C708);
    strcat(path, arg->name);

    if (((gfFileIOHandle*)(self + 0x44))->checkFileRequest(path)) {
        while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
            OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
        }
        if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() != 0) {
            *(void**)(self + 0xE4) = 0;
            *(int*)(self + 0xE8) = 0;
            goto cleanup;
        }
        if ((u32)((gfFileIOHandle*)(self + 0x44))->getSize() > 0x64000) {
            *(void**)(self + 0xE4) = 0;
            *(int*)(self + 0xE8) = 0;
            goto cleanup;
        }
        if (((gfFileIOHandle*)(self + 0x44))->readRequest(path, *(Heaps::HeapType*)(self + 0x50), 0, 0)) {
            while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() != 0) {
                *(void**)(self + 0xE4) = 0;
                *(int*)(self + 0xE8) = 0;
                goto cleanup;
            }
            if (arg->mode == 0) {
                *(void**)(self + 0xE4) = ((gfFileIOHandle*)(self + 0x44))->getBuffer();
                *(int*)(self + 0xE8) = ((gfFileIOHandle*)(self + 0x44))->getSize();
                goto block_48;
            }
            __ct__11gfCryptoESPFv(thread);
            {
                unsigned int size = ((gfFileIOHandle*)(self + 0x44))->getSize();
                if (decrypt__11gfCryptoESPFPCvUlQ25Heaps8HeapType(thread, ((gfFileIOHandle*)(self + 0x44))->getBuffer(), size, *(int*)(self + 0x50)) != 0) {
                    while (*(u8*)(thread + 0x360) == 0) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    if (*(int*)(thread + 0x364) != 0) {
                        gfHeapManager::free(((gfFileIOHandle*)(self + 0x44))->getBuffer());
                        *(void**)(self + 0xE4) = 0;
                        *(int*)(self + 0xE8) = 0;
                        __dt__11gfCryptoESPFv(thread, -1);
                        goto cleanup;
                    }
                    gfHeapManager::free(((gfFileIOHandle*)(self + 0x44))->getBuffer());
                    *(void**)(self + 0xE4) = *(void**)(thread + 0x368);
                    *(int*)(self + 0xE8) = *(int*)(thread + 0x36C);
                    goto thread_done;
                }
                gfHeapManager::free(((gfFileIOHandle*)(self + 0x44))->getBuffer());
                *(void**)(self + 0xE4) = 0;
                *(int*)(self + 0xE8) = 0;
                __dt__11gfCryptoESPFv(thread, -1);
                goto cleanup;
            }
        }
        goto cleanup;
    }
    goto cleanup;

thread_done:
    __dt__11gfCryptoESPFv(thread, -1);
block_48:
    fn_80153488(sp8);
    if (fn_80153504(sp8, *(void**)(self + 0xE4)) == 0) {
        if (*(void**)(self + 0xE4) != 0) {
            gfHeapManager::free(*(void**)(self + 0xE4));
        }
        *(void**)(self + 0xE4) = 0;
        result = 6;
        *(int*)(self + 0xE8) = 0;
        fn_80153498(sp8, -1);
    } else {
        int kind = 0;
        switch (getDataType__10nteSectionFv(sp8)) {
        case 1:
            kind = 1;
            break;
        case 2:
            kind = 2;
            break;
        case 3:
            kind = 3;
            break;
        }
        if (kind == 0 || arg->index != kind) {
            if (*(void**)(self + 0xE4) != 0) {
                gfHeapManager::free(*(void**)(self + 0xE4));
            }
            *(void**)(self + 0xE4) = 0;
            result = 6;
            *(int*)(self + 0xE8) = 0;
            fn_80153498(sp8, -1);
        } else {
            fn_80153498(sp8, -1);
            switch (arg->index) {
            case 1:
                *(int*)(self + 0xEC) = 1;
                break;
            case 2:
                *(int*)(self + 0xEC) = 2;
                break;
            case 3:
                *(int*)(self + 0xEC) = 3;
                break;
            }
            result = 0;
        }
    }

cleanup:
    if (vf != 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 != 0) {
            lbl_805A00A8 = lbl_805A00A8 - 1;
            if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4c))->unmountVF(0)) {
                while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x4c))->getReturnStatus();
                lbl_805A00A8 = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D4);
    }
    if (sd != 0) {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA != 0) {
            lbl_805A00AA = lbl_805A00AA - 1;
            if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->unmountSD()) {
                while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x48))->getReturnStatus();
                lbl_805A00AA = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D8);
    }
    if (locked) OSUnlockMutex(&lbl_80497768);
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8003B760 {

extern "C" {
void fn_8003B420(void*);
int getCollectionVFName__14gfSysRecorder2FPc(char*);
}
extern "C" OSMutex lbl_80497768;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" OSMutex* lbl_8059C6D8;
extern "C" u16 lbl_805A00A8;
extern "C" u16 lbl_805A00AA;
extern "C" char lbl_8059C6DC[5];
extern "C" char lbl_8059C6E4[4];
extern "C" char MOD_PATCH_DIR[];
extern "C" const char* lbl_80423560[];
extern "C" const char* lbl_80423680[];

struct Args { int unk0; int mode; int index; };

extern "C" int fn_8003B760(u8* self, Args* arg) {
    char path[0x40];
    char buf[0x40];
    int result = 4;
    fn_8003B420(self);
    bool locked = arg->mode != 0;
    if (locked) OSLockMutex(&lbl_80497768);
    int vf = 0;
    int sd = 0;
    if (arg->mode == 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4c))->mountVF(0, buf))) {
            while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x4c))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D4);
                vf = 0;
            } else {
                goto vf_inc;
            }
        } else {
vf_inc:
            lbl_805A00A8 = lbl_805A00A8 + 1;
            OSUnlockMutex(lbl_8059C6D4);
            vf = 1;
        }
        if (lbl_805A00A8 == 0) goto cleanup;
    } else {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->mountSD()) {
            while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x48))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D8);
                sd = 0;
            } else {
                goto sd_inc;
            }
        } else {
sd_inc:
            lbl_805A00AA = lbl_805A00AA + 1;
            OSUnlockMutex(lbl_8059C6D8);
            sd = 1;
        }
        if (lbl_805A00AA == 0) goto cleanup;
    }
    {
        int index = arg->index;
        if (arg->mode == 0) {
            strcpy(path, lbl_8059C6DC);
            strcat(path, lbl_80423560[index]);
        } else {
            strcpy(path, lbl_8059C6E4);
            strcat(path, MOD_PATCH_DIR);
            strcat(path, srGetAppGamename());
            strcat(path, lbl_80423560[index]);
        }
        strcat(path, lbl_80423680[index]);
        if (!((gfFileIOHandle*)(self + 0x44))->findfirstRequest(path, *(HeapType*)(self + 0x50))) {
            *(void**)(self + 0xe4) = 0;
            *(int*)(self + 0xe8) = 0;
        } else {
            while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            switch (((gfFileIOHandle*)(self + 0x44))->getReturnStatus()) {
            case 0:
                *(void**)(self + 0xe4) = ((gfFileIOHandle*)(self + 0x44))->getBuffer();
                *(int*)(self + 0xe8) = ((gfFileIOHandle*)(self + 0x44))->getSize();
                break;
            case 17:
                *(void**)(self + 0xe4) = 0;
                *(int*)(self + 0xe8) = 0;
                break;
            default:
                *(void**)(self + 0xe4) = 0;
                *(int*)(self + 0xe8) = 0;
                goto failed;
            }
            result = 0;
            *(int*)(self + 0xec) = 7;
failed:
            if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() != 0 &&
                ((gfFileIOHandle*)(self + 0x44))->getBuffer() != 0) {
                gfHeapManager::free(((gfFileIOHandle*)(self + 0x44))->getBuffer());
            }
        }
    }
cleanup:
    if (vf != 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 != 0) {
            lbl_805A00A8 = lbl_805A00A8 - 1;
            if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4c))->unmountVF(0)) {
                while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x4c))->getReturnStatus();
                lbl_805A00A8 = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D4);
    }
    if (sd != 0) {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA != 0) {
            lbl_805A00AA = lbl_805A00AA - 1;
            if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->unmountSD()) {
                while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x48))->getReturnStatus();
                lbl_805A00AA = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D8);
    }
    if (locked) OSUnlockMutex(&lbl_80497768);
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8003BBDC {

extern "C" {
void fn_8003B420(void*);
int getCollectionVFName__14gfSysRecorder2FPc(char*);
}
extern "C" OSMutex lbl_80497768;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" OSMutex* lbl_8059C6D8;
extern "C" u16 lbl_805A00A8;
extern "C" u16 lbl_805A00AA;
extern "C" char lbl_8059C6DC[5];
extern "C" char lbl_8059C6E4[4];
extern "C" char MOD_PATCH_DIR[];
extern "C" const char* lbl_80423560[];
extern "C" const char* lbl_80423680[];

struct Args { int unk0; int mode; int index; };

extern "C" int fn_8003BBDC(u8* self, Args* arg) {
    char path[0x40];
    char buf[0x40];
    int result = 4;
    bool locked;
    int vf;
    int sd;

    fn_8003B420(self);
    locked = arg->mode != 0;
    if (locked) OSLockMutex(&lbl_80497768);
    vf = 0;
    sd = 0;
    if (arg->mode == 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4c))->mountVF(0, buf))) {
            while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x4c))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D4);
                vf = 0;
            } else {
                goto vf_inc;
            }
        } else {
vf_inc:
            lbl_805A00A8 = lbl_805A00A8 + 1;
            OSUnlockMutex(lbl_8059C6D4);
            vf = 1;
        }
        if (lbl_805A00A8 == 0) goto cleanup;
    } else {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->mountSD()) {
            while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x48))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D8);
                sd = 0;
            } else {
                goto sd_inc;
            }
        } else {
sd_inc:
            lbl_805A00AA = lbl_805A00AA + 1;
            OSUnlockMutex(lbl_8059C6D8);
            sd = 1;
        }
        if (lbl_805A00AA == 0) goto cleanup;
    }
    {
        int index = arg->index;
        if (arg->mode == 0) {
            strcpy(path, lbl_8059C6DC);
            strcat(path, lbl_80423560[index]);
        } else {
            strcpy(path, lbl_8059C6E4);
            strcat(path, MOD_PATCH_DIR);
            strcat(path, srGetAppGamename());
            strcat(path, lbl_80423560[index]);
        }
        strcat(path, lbl_80423680[index]);
        if (((gfFileIOHandle*)(self + 0x44))->findFileCountRequest(path)) {
            while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            switch (((gfFileIOHandle*)(self + 0x44))->getReturnStatus()) {
            case 0:
                *(int*)(self + 0xe4) = 0;
                *(int*)(self + 0xe8) = ((gfFileIOHandle*)(self + 0x44))->getSize();
                goto block_35;
            case 17:
                *(int*)(self + 0xe4) = 0;
                *(int*)(self + 0xe8) = 0;
block_35:
                result = 0;
                *(int*)(self + 0xec) = 8;
                break;
            }
        }
    }
cleanup:
    if (vf != 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 != 0) {
            lbl_805A00A8 = lbl_805A00A8 - 1;
            if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4c))->unmountVF(0)) {
                while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x4c))->getReturnStatus();
                lbl_805A00A8 = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D4);
    }
    if (sd != 0) {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA != 0) {
            lbl_805A00AA = lbl_805A00AA - 1;
            if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->unmountSD()) {
                while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x48))->getReturnStatus();
                lbl_805A00AA = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D8);
    }
    if (locked) OSUnlockMutex(&lbl_80497768);
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8003C4E4 {

extern "C" {
u8 getEnableSaveLoad__10GameGlobalFv(void* a);
int getCollectionVFName__14gfSysRecorder2FPc(char* buf);
int checkSDInserted__8gfFileIOFv(void);
int checkSDWriteProtected__8gfFileIOFv(void);
}

extern "C" void* g_GameGlobal;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" OSMutex* lbl_8059C6D8;
extern "C" OSMutex lbl_80497768;
extern "C" u16 lbl_805A00A8;
extern "C" u16 lbl_805A00AA;
extern "C" char lbl_8059C714;
extern "C" char lbl_8059C71C;

struct FileState {
    u8 unk0;
    u8 pad1[3];
    s32 unk4;
    s32 unk8;
    s32 unkC;
};
extern "C" FileState lbl_80423690;

extern "C" s32 fn_8003C4E4(u8* self, void* unused, gfFileIOHandle* arg2, u32 mode) {
    char buf[0x40];
    s32 size;
    s32 state;

    size = 0;
    state = 1;
    if (arg2 == 0) {
        arg2 = (gfFileIOHandle*)(self + 0x44);
    }
    if ((getEnableSaveLoad__10GameGlobalFv(g_GameGlobal) >> 3) & 1) {
        if (lbl_80423690.unkC == 0 || mode == 1) {
            s32 ok;
            OSLockMutex(lbl_8059C6D4);
            if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4C))->mountVF(0, buf))) {
                while (!((gfFileIOHandle*)(self + 0x4C))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                if (((gfFileIOHandle*)(self + 0x4C))->getReturnStatus() != 0) {
                    OSUnlockMutex(lbl_8059C6D4);
                    ok = 0;
                } else {
                    goto vf_inc;
                }
            } else {
vf_inc:
                lbl_805A00A8 = lbl_805A00A8 + 1;
                OSUnlockMutex(lbl_8059C6D4);
                ok = 1;
            }
            if (ok != 0) {
                if (arg2->checkFreeSize(&lbl_8059C714)) {
                    while (!arg2->isReady()) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    if (arg2->getReturnStatus() == 0) {
                        lbl_80423690.unkC = arg2->getSize();
                    }
                }
                OSLockMutex(lbl_8059C6D4);
                if (lbl_805A00A8 != 0) {
                    lbl_805A00A8 = lbl_805A00A8 - 1;
                    if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4C))->unmountVF(0)) {
                        while (!((gfFileIOHandle*)(self + 0x4C))->isReady()) {
                            OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                        }
                        ((gfFileIOHandle*)(self + 0x4C))->getReturnStatus();
                        lbl_805A00A8 = 0;
                    }
                }
                OSUnlockMutex(lbl_8059C6D4);
            }
        }
    } else {
        lbl_80423690.unkC = 0;
    }
    if (checkSDInserted__8gfFileIOFv() != 0) {
        if (lbl_80423690.unk4 != (state = 0) || mode == 1) {
            s32 ok2;
            OSLockMutex(&lbl_80497768);
            OSLockMutex(lbl_8059C6D8);
            if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->mountSD()) {
                while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                if (((gfFileIOHandle*)(self + 0x48))->getReturnStatus() != 0) {
                    OSUnlockMutex(lbl_8059C6D8);
                    ok2 = 0;
                } else {
                    goto sd_inc;
                }
            } else {
sd_inc:
                lbl_805A00AA = lbl_805A00AA + 1;
                OSUnlockMutex(lbl_8059C6D8);
                ok2 = 1;
            }
            if (ok2 != 0) {
                if (checkSDWriteProtected__8gfFileIOFv() != 0) {
                    state = 2;
                }
                if (arg2->checkFreeSize(&lbl_8059C71C)) {
                    while (!arg2->isReady()) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    if (arg2->getReturnStatus() == 0) {
                        size = arg2->getSize();
                    }
                }
                OSLockMutex(lbl_8059C6D8);
                if (lbl_805A00AA != 0) {
                    lbl_805A00AA = lbl_805A00AA - 1;
                    if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->unmountSD()) {
                        while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                            OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                        }
                        ((gfFileIOHandle*)(self + 0x48))->getReturnStatus();
                        lbl_805A00AA = 0;
                    }
                }
                OSUnlockMutex(lbl_8059C6D8);
            } else {
                state = 3;
            }
            OSUnlockMutex(&lbl_80497768);
        }
    }
    if (lbl_80423690.unk4 != state || mode == 1) {
        lbl_80423690.unk8 = size;
        lbl_80423690.unk4 = state;
    }
    lbl_80423690.unk0 = 1;
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_8003C934 {

extern "C" {
int getCollectionVFName__14gfSysRecorder2FPc(char*);
int fn_8003C4E4(void*, int, int, int);
}
extern "C" OSMutex lbl_80497768;
extern "C" OSMutex* lbl_8059C6D4;
extern "C" OSMutex* lbl_8059C6D8;
extern "C" u16 lbl_805A00A8;
extern "C" u16 lbl_805A00AA;
extern "C" char lbl_8059C6DC[5];
extern "C" char lbl_8059C6E4[4];
extern "C" char lbl_8059C708[2];
extern "C" char MOD_PATCH_DIR[];
extern "C" const char* lbl_80423560[];

struct Args { int unk0; int mode; int index; char name[1]; };

extern "C" int fn_8003C934(u8* self, Args* arg) {
    char path[0x40];
    char buf[0x40];
    int result = 7;
    if (arg->mode == 0) {
        OSLockMutex(lbl_8059C6D4);
        int ok;
        if (lbl_805A00A8 == 0 && (getCollectionVFName__14gfSysRecorder2FPc(buf), ((gfFileIOHandle*)(self + 0x4c))->mountVF(0, buf))) {
            while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x4c))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D4);
                ok = 0;
            } else {
                goto vf_inc;
            }
        } else {
vf_inc:
            lbl_805A00A8 = lbl_805A00A8 + 1;
            OSUnlockMutex(lbl_8059C6D4);
            ok = 1;
        }
        if (ok == 0) return 7;
        int index = arg->index;
        strcpy(path, lbl_8059C6DC);
        strcat(path, lbl_80423560[index]);
    } else {
        OSLockMutex(&lbl_80497768);
        OSLockMutex(lbl_8059C6D8);
        int ok;
        if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->mountSD()) {
            while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            if (((gfFileIOHandle*)(self + 0x48))->getReturnStatus() != 0) {
                OSUnlockMutex(lbl_8059C6D8);
                ok = 0;
            } else {
                goto sd_inc;
            }
        } else {
sd_inc:
            lbl_805A00AA = lbl_805A00AA + 1;
            OSUnlockMutex(lbl_8059C6D8);
            ok = 1;
        }
        if (ok == 0) {
            OSUnlockMutex(&lbl_80497768);
            return 7;
        }
        int index = arg->index;
        strcpy(path, lbl_8059C6E4);
        strcat(path, MOD_PATCH_DIR);
        strcat(path, srGetAppGamename());
        strcat(path, lbl_80423560[index]);
    }
    strcat(path, lbl_8059C708);
    strcat(path, arg->name);
    if (((gfFileIOHandle*)(self + 0x44))->del(path)) {
        while (!((gfFileIOHandle*)(self + 0x44))->isReady()) {
            OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
        }
        if (((gfFileIOHandle*)(self + 0x44))->getReturnStatus() == 0) {
            result = 0;
            fn_8003C4E4(self, 0, 0, 1);
        }
    }
    if (arg->mode == 0) {
        OSLockMutex(lbl_8059C6D4);
        if (lbl_805A00A8 != 0) {
            lbl_805A00A8 = lbl_805A00A8 - 1;
            if (lbl_805A00A8 == 0 && ((gfFileIOHandle*)(self + 0x4c))->unmountVF(0)) {
                while (!((gfFileIOHandle*)(self + 0x4c))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x4c))->getReturnStatus();
                lbl_805A00A8 = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D4);
    } else {
        OSLockMutex(lbl_8059C6D8);
        if (lbl_805A00AA != 0) {
            lbl_805A00AA = lbl_805A00AA - 1;
            if (lbl_805A00AA == 0 && ((gfFileIOHandle*)(self + 0x48))->unmountSD()) {
                while (!((gfFileIOHandle*)(self + 0x48))->isReady()) {
                    OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                }
                ((gfFileIOHandle*)(self + 0x48))->getReturnStatus();
                lbl_805A00AA = 0;
            }
        }
        OSUnlockMutex(lbl_8059C6D8);
        OSUnlockMutex(&lbl_80497768);
    }
    return result;
}
}

