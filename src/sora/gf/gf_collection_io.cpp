#include <cstring>
#include <gf/gf_collection_io.h>
#include <gf/gf_file_io.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <revolution/OS/OSHardware.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

// HYPOTHESIS: stand-ins for callees that are not named yet; the map gives
// a GameGlobal flags getter (bit 3 enables VF save/load),
// the nteWiiConnect24 constructor and destructor, and this unit's request workers.
extern "C" u8 fn_8004EAD4(GameGlobal* global);
extern "C" nteWiiConnect24* fn_8014CD60(nteWiiConnect24* wc24, Heaps::HeapType heap);
extern "C" void fn_8014CD7C(nteWiiConnect24* wc24, int del);
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
    // MATCH-ONLY: stand-in for new (heap) nteWiiConnect24(heap) until its constructor is named.
    nteWiiConnect24* wc24 = static_cast<nteWiiConnect24*>(operator new(0x14, heap));
    if (wc24) {
        wc24 = fn_8014CD60(wc24, heap);
    }
    m_wc24 = wc24;
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
        // MATCH-ONLY: stand-in for delete m_wc24 until its destructor is named.
        fn_8014CD7C(m_wc24, 1);
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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
    if (!((fn_8004EAD4(g_GameGlobal) >> 3) & 1)) {
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

const char* gfCollectionIO::getDeliveryFileName(int fileType) {
    return lbl_804235B8[fileType];
}

const char* gfCollectionIO::getSampleFileName(int fileType, int index) {
    return lbl_80423640[fileType + index];
}

bool gfCollectionIO::isEnableVFSaveLoad() {
    return (fn_8004EAD4(g_GameGlobal) >> 3) & 1;
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
