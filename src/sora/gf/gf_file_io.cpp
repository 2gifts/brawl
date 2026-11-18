#include <gf/gf_heap_manager.h>
#include <gf/gf_error_check.h>
#include <gm/gm_global.h>
#include <revolution/FA/FARemove.h>
#include <revolution/OS/OSError.h>
#include <gf/gf_callback.h>
#include <gf/gf_error_manager.h>
#include <gf/gf_file_io.h>
#include <gf/gf_memory_pool.h>
#include <revolution/FA.h>
#include <revolution/NAND.h>
#include <revolution/NET/crc.h>
#include <revolution/OS.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/VF/d_vf.h>
#include <string.h>
#include <types.h>
#include <FAT/fa_api.h>

extern "C" int fn_803EC22C(int drive, int force);
extern "C" s32 VFCreateDir(const char* path);
extern "C" s32 VFChangeDir(const char* path);
extern "C" s32 fn_803C3CF8(const char* path);
// HYPOTHESIS: the VF find-first/find-next calls fill the same find entry as FAFsfirst
extern "C" s32 VFFileSearchFirst(FAEntryInfo* info, const char* pattern, int filter);
extern "C" s32 VFFileSearchNext(FAEntryInfo* info);
extern "C" s32 VFFormatDrive(const char* drive);
extern "C" s32 VFCreateSystemFileNANDFlash(const char* path, u32 size);
extern "C" s32 VFMountDriveNANDFlash(const char* drive, const char* path);
extern "C" s32 NANDCreateDir(const char* path, u8 perm, u8 attr);
extern "C" DVDDiskID lbl_80494938;
extern "C" const char* lbl_80421198[];
extern "C" char lbl_8059C584;
extern "C" char lbl_8059C594;
extern "C" char lbl_8059C59C;
extern "C" char lbl_8059C5A0;
extern "C" char lbl_8059C5A4;
extern "C" VFFile VFCreateFile(const char* path, u32 attr);
extern "C" s32 NANDReadDir(const char* path, char* names, u32* count);
extern "C" gfFileIO::FS_TYPE lbl_8059C528;
extern "C" int lbl_8059C52C;
extern "C" char lbl_8059C550;
extern "C" char lbl_8059C558;
extern "C" char lbl_8059C560;
extern "C" char lbl_8059C568;
extern "C" char lbl_8059C570;
extern "C" char lbl_8059C57C;
extern "C" char lbl_8059C58C;
extern "C" char lbl_8059C598;
extern "C" char lbl_8059C5B0;

// HYPOTHESIS: gfMutex (name from the map) wraps one OSMutex
class gfMutex {
public:
    gfMutex() { OSInitMutex(&m_mutex); }
    ~gfMutex() { }

    OSMutex m_mutex;
};

// HYPOTHESIS: scoped mutex guard; the class name is a guess
class gfFileIOLock {
public:
    gfFileIOLock(OSMutex* mutex) : m_mutex(mutex), m_isLocked(1) {
        if (m_isLocked) {
            OSLockMutex(m_mutex);
        }
    }

    ~gfFileIOLock() {
        if (m_isLocked) {
            OSUnlockMutex(m_mutex);
        }
    }

private:
    OSMutex* m_mutex;
    // MATCH-ONLY: 8-bit bitfield reproduces the original flag byte read-modify-write
    u8 m_isLocked : 8;
};

gfMutex lbl_80494910;

// HYPOTHESIS: zero-initialized .sbss of this unit, sizes and order from the original layout; untyped where no use is recovered yet.
unsigned char lbl_8059FFD8[0x4] __attribute__((aligned(8))) = { 0 };
bool lbl_8059FFDC __attribute__((aligned(4))) = 0;
unsigned char lbl_8059FFE0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8059FFE4[0x4] __attribute__((aligned(4))) = { 0 };
gfFileIO::MountStatusCB lbl_8059FFE8 = 0;
gfCallBackList lbl_8059FFEC;

const DVDDiskID* gfFileIO::getDVDDiskId() {
    return &lbl_80494938;
}

void gfFileIO::setFatalErrorCallback(ErrorCallback* cb) {
    lbl_8059FFEC.add(cb);
}

void gfFileIO::server() {
    if (OSGetConsoleType() & 0xF0000000) {
        bool isLocked = OSTryLockMutex(&lbl_80494910.m_mutex) == TRUE;
        if (isLocked) {
            OSUnlockMutex(&lbl_80494910.m_mutex);
        }
    }
}

// the original callers call this out of line
#pragma dont_inline on
gfFileIO::FS_TYPE gfFileIO::checkFSType(const char* filepath) {
    if (strncmp(filepath, &lbl_8059C550, 4) == 0) {
        return MCS;
    }
    if (strncmp(filepath, &lbl_8059C558, 4) == 0) {
        return DVD;
    }
    if (strncmp(filepath, &lbl_8059C560, 5) == 0) {
        return NAND;
    }
    if (strncmp(filepath, &lbl_8059C568, 3) == 0) {
        return SD;
    }
    if (filepath[0] == 'v' && filepath[1] == 'f') {
        if (filepath[2] >= '0' && filepath[2] <= '9') {
            return VF;
        }
        return lbl_8059C528;
    }
    return lbl_8059C528;
}
#pragma dont_inline reset

void gfFileIO::gfNandErrorCheck(s32 result, int quiet) {
    switch (result) {
    case NAND_RESULT_OK:
        return;
    case NAND_RESULT_ALLOC_FAILED:
        if (!quiet) {
            gfErrorManager::getInstance()->notifyError(8, 0);
        }
        break;
    case NAND_RESULT_BUSY:
        if (!quiet) {
            gfErrorManager::getInstance()->notifyError(8, 0);
        }
        break;
    case NAND_RESULT_CORRUPT:
        gfErrorManager::getInstance()->notifyError(5, 0);
        break;
    case NAND_RESULT_MAXBLOCKS:
        gfErrorManager::getInstance()->notifyError(6, 0);
        break;
    case NAND_RESULT_MAXFILES:
        gfErrorManager::getInstance()->notifyError(7, 0);
        break;
    case NAND_RESULT_ACCESS:
        return;
    case NAND_RESULT_ECC_CRIT:
    case NAND_RESULT_EXISTS:
        return;
    case NAND_RESULT_INVALID:
        return;
    case NAND_RESULT_MAXFD:
        return;
    case NAND_RESULT_NOEXISTS:
    case NAND_RESULT_NOTEMPTY:
    case NAND_RESULT_OPENFD:
    case NAND_RESULT_AUTHENTICATION:
    case NAND_RESULT_MAXDEPTH:
        return;
    case NAND_RESULT_UNKNOWN:
        return;
    case NAND_RESULT_FATAL_ERROR:
        return;
    }
}

#pragma dont_inline on
void gfFileIO::gfVFErrorCheck(s32 error) { }
#pragma dont_inline reset

// the original callers call this out of line
#pragma dont_inline on
void gfFileIO::gfNANDSetTmpPath(char* buf, const char* filepath) {
    char* hex = buf + 5;
    strcpy(buf, &lbl_8059C570);
    u16 crc = NETCalcCRC16(filepath, strlen(filepath));
    for (int i = 0; i < 4; i++) {
        int digit = (crc >> ((3 - i) * 4)) & 0xF;
        if (digit < 10) {
            hex[i] = digit + '0';
        } else {
            hex[i] = digit + 'a' - 10;
        }
    }
    hex[4] = 0;
}
#pragma dont_inline reset

void gfFileIO::setSDMountStatusCallback(MountStatusCB cb) {
    lbl_8059FFE8 = cb;
    cb(lbl_8059C52C);
}

void gfFileIO::FAInsertCallback() {
    lbl_8059C52C = 1;
    lbl_8059FFDC = true;
    bool isProtected = false;
    if (FAIsWriteProtected('A') == 1) {
        isProtected = true;
    }
    if (isProtected) {
        lbl_8059C52C = 3;
    }
    if (lbl_8059FFE8) {
        lbl_8059FFE8(lbl_8059C52C);
    }
}

void gfFileIO::FAEjectCallback() {
    lbl_8059C52C = 2;
    lbl_8059FFDC = false;
    if (lbl_8059FFE8) {
        lbl_8059FFE8(lbl_8059C52C);
    }
}

bool gfFileIO::checkSDWriteProtected() {
    bool isProtected = false;
    if (FAIsWriteProtected('A') == 1) {
        isProtected = true;
    }
    return isProtected;
}

bool gfFileIO::checkSDInserted() {
    return lbl_8059FFDC;
}

int gfFileIO::gfVFCreateDir(const char* filepath)
{
    char dir[0x80];
    int result;
    u32 i;
    u32 end;
    u32 j;

    if (strlen(filepath) < 0x7F) {
        result = VFCreateDir(filepath);
        if (result != 0 && result != 0x11) {
            end = strlen(filepath);
            if (filepath[end - 1] == '/') {
                end -= 1;
            }
            i = 3;
            while (i < end) {
                strcpy(dir, filepath);
                for (j = i; j < end; j++) {
                    i = j + 1;
                    if (dir[j] == '/') {
                        dir[j] = 0;
                        break;
                    }
                }
                result = VFCreateDir(dir);
                if (result != 0) {
                    if (result != 0x11) {
                        break;
                    }
                }
            }
        }
    }
    return result;
}

int gfFileIO::gfFACreateDir(const char* filepath)
{
    char dir[0x80];
    int result;
    u32 i;
    u32 end;
    u32 j;

    if (strlen(filepath) < 0x7F) {
        result = FAMkdir(filepath);
        if (result != 0) {
            result = FAErrnum();
            if (result != 0x11) {
                end = strlen(filepath);
                if (filepath[end - 1] == '/') {
                    end -= 1;
                }
                i = 3;
                while (i < end) {
                    strcpy(dir, filepath);
                    for (j = i; j < end; j++) {
                        i = j + 1;
                        if (dir[j] == '/') {
                            dir[j] = 0;
                            break;
                        }
                    }
                    result = FAMkdir(dir);
                    if (result != 0) {
                        result = FAErrnum();
                        if (result != 0x11) {
                            break;
                        }
                    }
                }
            }
        }
    }
    return result;
}

int gfFileIO::createDirNAND(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char home[0x80];
    char* filepath = request->m_pFilepath;
    int result = 5;

    path[0] = 0;
    if (strncmp(filepath, &lbl_8059C57C, 4) == 0) {
        strcat(path, filepath);
    } else {
        NANDGetHomeDir(home);
        strncpy(path, home, 0x40);
        strcat(path, filepath);
    }
    s32 ret = NANDCreateDir(path, 0x34, 0);
    if (ret != NAND_RESULT_OK && ret != NAND_RESULT_EXISTS) {
        gfNandErrorCheck(ret, 1);
        OSReport("NAND Create Dir: (%s)\n", path);
    } else {
        result = 0;
    }
    return result;
}

int gfFileIO::createDirVF(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    int result = 5;
    char* filepath = request->m_pFilepath;
    u8 drive = request->m_vfUnk;

    strcpy(path, &lbl_8059C58C);
    path[0] = drive + 'B';
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    int ret = gfVFCreateDir(path);
    if (ret != 0 && ret != 0x11) {
        gfVFErrorCheck(ret);
        OSReport("VF Create Dir: (%s)", path);
    } else {
        result = 0;
    }
    return result;
}

int gfFileIO::createDirSD(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char* filepath = request->m_pFilepath;
    int result = 5;

    strcpy(path, &lbl_8059C598);
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    if (gfFACreateDir(path) != 0 && FAErrnum() != 0x11) {
        FAErrnum();
        OSReport("SD Create Dir: (%s)", path);
    } else {
        result = 0;
    }
    return result;
}

int gfFileIO::checkDirVF(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char current[0x80];
    u8 drive = request->m_vfUnk;
    char* filepath = request->m_pFilepath;
    int result = 0x12;

    strcpy(path, &lbl_8059C58C);
    path[0] = drive + 'B';
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    strcpy(current, &lbl_8059C5B0);
    current[0] += request->m_vfUnk;
    s32 ret = VFChangeDir(path);
    if (ret != 0) {
        if (ret != 2) {
            gfVFErrorCheck(ret);
            OSReport("VF check Dir: (%s)", request->m_pFilepath);
        }
    } else {
        VFChangeDir(current);
        result = 0;
    }
    return result;
}

int gfFileIO::checkDirSD(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char* filepath = request->m_pFilepath;
    int result = 0x12;

    strcpy(path, &lbl_8059C598);
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    FAHandle* dir;
    bool isOpen = false;
    dir = FAOpendir(path);
    if (dir == NULL) {
        if (FAErrnum() != 2) {
            FAErrnum();
            OSReport("SD check Dir: (%s)", request->m_pFilepath);
        }
    } else {
        isOpen = true;
        result = 0;
    }
    if (isOpen) {
        FAClosedir(dir);
    }
    return result;
}

int gfFileIO::createVFSystemFile(gfFileIORequest* request) {
    char name[0x80];
    char path[0x80];
    char home[0x80];
    int result;
    char* filepath = request->m_pFilepath;
    u32 len = strlen(filepath);
    char* p = filepath;
    for (u32 i = 0; i < len; i++) {
        if (*p == ':') {
            strncpy(name, p + 1, 0x80);
            goto copied;
        }
        p++;
    }
    strncpy(name, filepath, 0x80);
copied:
    path[0] = 0;
    if (strncmp(name, &lbl_8059C57C, 4) == 0) {
        strcat(path, name);
    } else {
        NANDGetHomeDir(home);
        strncpy(path, home, 0x40);
        strcat(path, name);
    }
    s32 ret = VFCreateSystemFileNANDFlash(path, request->m_readLen);
    if (ret != 0) {
        OSReport("createVFSystemFile() Error! path(%s) size(%d) err=%d", path, request->m_readLen, ret);
        gfVFErrorCheck(ret);
        result = 8;
    } else {
        result = 0;
    }
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::mountVFSystemFile(gfFileIORequest* request) {
    char name[0x80];
    char path[0x80];
    char home[0x80];
    char drive[] = "B";
    int result;
    char* filepath = request->m_pFilepath;
    u32 len = strlen(filepath);
    char* p = filepath;
    for (u32 i = 0; i < len; i++) {
        if (*p == ':') {
            strncpy(name, p + 1, 0x80);
            goto copied;
        }
        p++;
    }
    strncpy(name, filepath, 0x80);
copied:
    path[0] = 0;
    if (strncmp(name, &lbl_8059C57C, 4) == 0) {
        strcat(path, name);
    } else {
        NANDGetHomeDir(home);
        strncpy(path, home, 0x40);
        strcat(path, name);
    }
    drive[0] += request->m_vfUnk;
    s32 ret = VFMountDriveNANDFlash(drive, path);
    if (ret != 0) {
        OSReport("mountVFSystemFile() Error! drive(%s) path(%s) err=%d", drive, path, ret);
        gfVFErrorCheck(ret);
        if (ret == 0xB005) {
            result = 0xA;
        } else {
            result = 9;
        }
    } else {
        result = 0;
    }
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::unmountVFSystemFile(gfFileIORequest* request) {
    int result;
    char drive[] = "B";
    // MATCH-ONLY: the original sign-extends the drive index here (not in formatVFSystemFile)
    drive[0] += (char)request->m_vfUnk;
    s32 ret = VFUnmountDrive(drive);
    if (ret != 0) {
        OSReport("unmountVFSystemFile() Error! drive(%s) err=%d", drive, ret);
        gfVFErrorCheck(ret);
        result = 0xB;
    } else {
        result = 0;
    }
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::formatVFSystemFile(gfFileIORequest* request) {
    int result;
    char drive[] = "B";
    drive[0] += request->m_vfUnk;
    s32 ret = VFFormatDrive(drive);
    if (ret != 0) {
        OSReport("formatVFSystemFile() Error! drive(%s) err=%d", drive, ret);
        gfVFErrorCheck(ret);
        result = 0xC;
    } else {
        result = 0;
    }
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::mountSD(gfFileIORequest* request) {
    int result;
    int ret = FAMount('A');
    if (ret != 0) {
        FAErrnum();
        result = 0xD;
        int code = FAErrnum();
        if (code == 5) {
            result = 0xE;
        }
        OSReport("mountSD() Error! err=%d errcode=%d", ret, code);
    } else {
        result = 0;
    }
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::unmountSD(gfFileIORequest* request) {
    int result;
    int ret = fn_803EC22C('A', 0);
    if (ret != 0) {
        FAErrnum();
        result = 0xF;
        OSReport("unmountSD() Error! err=%d", ret);
    } else {
        result = 0;
    }
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::checkFileNAND(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char home[0x80];
    NANDFileInfo info;
    char* filepath = request->m_pFilepath;
    int result = 0x11;

    path[0] = 0;
    if (strncmp(filepath, &lbl_8059C57C, 4) == 0) {
        strcat(path, filepath);
    } else {
        NANDGetHomeDir(home);
        strncpy(path, home, 0x40);
        strcat(path, filepath);
    }
    s32 ret = NANDOpen(path, &info, 1);
    if (ret != NAND_RESULT_OK) {
        if (ret != NAND_RESULT_NOEXISTS) {
            OSReport("checkFileNAND() ret=%d\n", ret);
            gfNandErrorCheck(ret, 0);
        }
    } else {
        NANDClose(&info);
        result = 0;
    }
    return result;
}

int gfFileIO::checkFileVF(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    u8 drive = request->m_vfUnk;
    char* filepath = request->m_pFilepath;
    int result = 0x11;

    strcpy(path, &lbl_8059C58C);
    path[0] = drive + 'B';
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    s32 size = fn_803C3CF8(path);
    if (size < 0) {
        s32 error = VFGetLastError();
        if (error != 2) {
            gfVFErrorCheck(error);
        }
    } else {
        request->m_readLen = size;
        result = 0;
    }
    return result;
}

int gfFileIO::checkFileSD(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    FAStat stat;
    char path[128];
    char* filepath;
    int result;

    filepath = request->m_pFilepath;
    request->m_readLen = 0;
    result = 0x11;
    strcpy(path, &lbl_8059C598);
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    if (FAFstat(path, &stat) != 0) {
        if (FAErrnum() != 2) {
            FAErrnum();
        }
    } else {
        result = 0;
        request->m_readLen = stat.filesize;
    }
    return result;
}

int gfFileIO::getFileNumVF(const char* path) {
    // HYPOTHESIS: the find entry is 0x450 bytes (frame size); OpenRVL's FAEntryInfo is 0x454
    char info[0x450];
    int count = -1;
    s32 result = VFFileSearchFirst((FAEntryInfo*)info, path, 0x20);
    if (result != 0) {
        if (result != 2) {
            gfVFErrorCheck(result);
        }
    } else {
        count = 1;
        while (result == 0) {
            result = VFFileSearchNext((FAEntryInfo*)info);
            if (result != 0) {
                if (result != 2) {
                    gfVFErrorCheck(result);
                }
                break;
            }
            count++;
        }
    }
    return count;
}

int gfFileIO::getFileNumSD(const char* path) {
    // HYPOTHESIS: the find entry is 0x450 bytes (frame size); OpenRVL's FAEntryInfo is 0x454
    char info[0x450];
    int count = -1;
    int result = FAFsfirst(path, 0x20, (FAEntryInfo*)info);
    if (result != 0) {
        if (FAErrnum() != 2) {
            FAErrnum();
        }
    } else {
        count = 1;
        while (result == 0) {
            result = FAFsnext((FAEntryInfo*)info);
            if (result != 0) {
                if (FAErrnum() != 2) {
                    FAErrnum();
                }
                break;
            }
            count++;
        }
    }
    return count;
}

int gfFileIO::writeVFFile(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    bool isOpen;
    int result;
    char* filepath = request->m_pFilepath;
    int length = request->m_readLen;
    void* buffer = request->m_dest;
    isOpen = false;
    result = 1;
    u8 drive = request->m_vfUnk;
    strcpy(path, &lbl_8059C58C);
    path[0] = drive + 'B';
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    VFFile file = VFOpenFile(filepath, &lbl_8059C5A0, 0);
    if (file || (file = VFCreateFile(filepath, 0))) {
        isOpen = true;
        if (VFWriteFile(file, buffer, (length + 31) & ~31) == 0) {
            OSReport(lbl_80421198[4], &lbl_8059C594, filepath, length, buffer);
            request->m_readLen = length;
            request->m_dest = buffer;
            result = 0;
        } else {
            OSReport(lbl_80421198[7], &lbl_8059C594, filepath);
            result = 3;
        }
    }
    if (isOpen) {
        VFCloseFile(file);
    } else {
        OSReport(lbl_80421198[5], &lbl_8059C594, filepath);
    }
    return result;
}

int gfFileIO::writeSDFile(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    bool isOpen;
    int result;
    char* filepath = request->m_pFilepath;
    int length = request->m_readLen;
    int offset = request->m_offset;
    void* buffer = request->m_dest;
    isOpen = false;
    result = 1;
    strcpy(path, &lbl_8059C598);
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    FAHandle* file;
    if (offset != 0) {
        file = FAFopen(path, &lbl_8059C5A4);
    } else {
        file = FAFopen(path, &lbl_8059C5A0);
    }
    if (file) {
        isOpen = true;
        u32 size = (length + 31) & ~31;
        if (offset != 0) {
            FAFseek(file, offset, 0);
        }
        u32 written = FAFwrite(buffer, 1, size, file);
        if (written == size) {
            OSReport(lbl_80421198[4], &lbl_8059C59C, filepath, length, buffer);
            request->m_readLen = length;
            request->m_dest = buffer;
            result = 0;
        } else {
            OSReport(lbl_80421198[7], &lbl_8059C59C, filepath);
            result = 3;
        }
    }
    if (isOpen) {
        FAFclose(file);
    } else {
        OSReport(lbl_80421198[5], &lbl_8059C59C, filepath);
    }
    return result;
}

int gfFileIO::readDirNAND(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char home[0x80];
    u32 count;
    char* filepath = request->m_pFilepath;
    int result = 5;
    path[0] = 0;
    if (strncmp(filepath, &lbl_8059C57C, 4) == 0) {
        strcat(path, filepath);
    } else {
        NANDGetHomeDir(home);
        strncpy(path, home, 0x40);
        strcat(path, filepath);
    }
    s32 ret = NANDReadDir(path, NULL, &count);
    if (ret != 0) {
        gfNandErrorCheck(ret, 0);
        OSReport("NAND Create Dir: (%s)", path);
    } else {
        u32 size = (count * 13 + 31) & ~31;
        if (size) {
            char* buffer = (char*)gfMemoryPool::alloc(const_cast<void*>(request->m_heap), size, 32);
            if (!buffer) {
                OSReport(lbl_80421198[2], &lbl_8059C584, filepath, size);
                result = 4;
            } else {
                ret = NANDReadDir(path, buffer, &count);
                if (ret != 0) {
                    gfNandErrorCheck(ret, 0);
                    OSReport("NAND Create Dir: (%s)", path);
                } else {
                    result = 0;
                }
            }
        }
    }
    return result;
}


// HYPOTHESIS: checkFile(path) builds a bare request on the stack (no constructor run) with only the cached flags cleared
int gfFileIO::checkFile(const char* filepath) {
    bool result = false;
    char buf[0x20];
    ((gfFileIORequest*)buf)->m_pFilepath = (char*)filepath;
    ((gfFileIORequest*)buf)->unk16_b7 = 0;
    ((gfFileIORequest*)buf)->m_isCached = false;
    if (checkFile((gfFileIORequest*)buf) == 0) {
        result = true;
    }
    return result;
}

extern "C" int lbl_8059C530;

// HYPOTHESIS: stand-in name; stores the language that selects the localized file name suffix
extern "C" void fn_8001B784(int language) {
    lbl_8059C530 = language;
}

// HYPOTHESIS: stack copy of the first 0x18 bytes of a request, as findfirst* and checkFile build it
struct gfFileIORequestHead {
    char* m_pFilepath;
    int m_words[5];
};

// HYPOTHESIS: findfirstSD/findfirstVF are defined below under their fn_ names
extern "C" int fn_8001FC98(gfFileIORequest* request);
extern "C" int fn_8001F988(gfFileIORequest* request);

int gfFileIO::findfirstFile(gfFileIORequest* request) {
    char name[0x80];
    int result;
    FS_TYPE type = checkFSType(request->m_pFilepath);
    char* filepath = request->m_pFilepath;
    u32 len = strlen(filepath);
    char* p = filepath;
    for (u32 i = 0; i < len; i++) {
        if (*p == ':') {
            strncpy(name, p + 1, 0x80);
            goto copied;
        }
        p++;
    }
    strncpy(name, filepath, 0x80);
copied:
    gfFileIORequestHead head;
    gfFileIORequestHead* src = (gfFileIORequestHead*)request;
    head.m_pFilepath = src->m_pFilepath;
    head.m_words[0] = src->m_words[0];
    head.m_words[1] = src->m_words[1];
    head.m_words[2] = src->m_words[2];
    head.m_words[3] = src->m_words[3];
    head.m_words[4] = src->m_words[4];
    result = 0x11;
    head.m_pFilepath = name;
    switch (type) {
    case SD:
        result = fn_8001FC98((gfFileIORequest*)&head);
        break;
    case VF:
        ((gfFileIORequest*)&head)->m_vfUnk = request->m_pFilepath[2] - '0';
        result = fn_8001F988((gfFileIORequest*)&head);
        break;
    }
    request->m_dest = ((gfFileIORequest*)&head)->m_dest;
    request->m_readLen = ((gfFileIORequest*)&head)->m_readLen;
    request->m_returnStatus = result;
    return result;
}

int gfFileIO::writeNandCacheFileCRC(gfFileIORequest* request) {
    gfFileIORequestHead head;
    gfFileIORequestHead* src = (gfFileIORequestHead*)request;
    char tmpPath[0x80];
    head.m_pFilepath = src->m_pFilepath;
    head.m_words[0] = src->m_words[0];
    head.m_words[1] = src->m_words[1];
    head.m_words[2] = src->m_words[2];
    head.m_words[3] = src->m_words[3];
    head.m_words[4] = src->m_words[4];
    gfNANDSetTmpPath(tmpPath, request->m_pFilepath);
    head.m_pFilepath = tmpPath;
    return writeNandCacheFile((gfFileIORequest*)&head);
}

extern "C" int fn_8001FC98(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    char* filepath = request->m_pFilepath;
    int result = 0x11;

    strcpy(path, &lbl_8059C598);
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    int count = gfFileIO::getFileNumSD(path);
    if (count >= 0) {
        if (count == 0) {
            result = 0;
        } else {
            char* buffer;
            u32 size = count << 7;
            buffer = (char*)gfMemoryPool::alloc(const_cast<void*>(request->m_heap), size, 32);
            if (!buffer) {
                OSReport(lbl_80421198[2], &lbl_8059C59C, request->m_pFilepath, size);
                result = 4;
            } else {
                request->m_dest = buffer;
                gfFileIO::getFindFirstFileSD(path, buffer, size, 0x80);
                request->m_dest = buffer;
                request->m_readLen = count;
                result = 0;
            }
        }
    }
    return result;
}

extern "C" int fn_8001F988(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910.m_mutex);
    char path[0x80];
    u8 drive = request->m_vfUnk;
    char* filepath = request->m_pFilepath;
    int result = 0x11;

    strcpy(path, &lbl_8059C58C);
    path[0] = drive + 'B';
    if (*filepath == '/') {
        path[2] = 0;
    }
    strncat(path, filepath, 0x80);
    int count = gfFileIO::getFileNumVF(path);
    if (count >= 0) {
        if (count == 0) {
            result = 0;
        } else {
            char* buffer;
            u32 size = count << 7;
            buffer = (char*)gfMemoryPool::alloc(const_cast<void*>(request->m_heap), size, 32);
            if (!buffer) {
                OSReport(lbl_80421198[2], &lbl_8059C594, request->m_pFilepath, size);
                result = 4;
            } else {
                request->m_dest = buffer;
                gfFileIO::getFindFirstFileVF(path, buffer, size, 0x80);
                request->m_dest = buffer;
                request->m_readLen = count;
                result = 0;
            }
        }
    }
    return result;
}

// TODO: clean up, give variables real names
namespace jj_800207FC {
extern "C" {
void OSReport(const char* fmt, ...);
void OSSleepTicks(u64 ticks);
void fn_8001B8E0(int, int);
void fn_8001B9EC(void* dest, const char* src);
s32 fn_8020A870(void* p);
extern char lbl_804215C8[];
}

extern "C" s32 fn_800207FC(u8* arg0) {
    char buf[0x74];
    s32 retry;
    s32 status;
    const char** list;
    s32 result = 0;
    u32 count;
    u32 i = 0;

    count = *(u32*)(arg0 + 0x8);
    list = *(const char***)(arg0 + 0xC);

    for (; i < count; i++) {
        fn_8001B9EC(buf, *list);
        do {
            status = fn_8020A870(buf);
            retry = 0;
            if ((u32)(status + 3) <= 1) {
                OSSleepTicks((u64)((*(u32*)0x800000F8 >> 2) / 1000) << 4);
                retry = 1;
            } else if (status != -0xC) {
                fn_8001B8E0(status, 0);
            }
        } while (retry != 0);

        if (status == 0) {
            result = 0;
        } else {
            OSReport(lbl_804215C8, status);
        }
        list++;
    }
    return result;
}
}

namespace jj_8001BBB8 {
struct Work {
    void* first;
    void* second;
    unsigned short count;
    unsigned short count2;
    int value;
    int value2;
};

struct State {
    int value;
    Work* work;
    unsigned char bytes[8];
};

extern "C" {
void fn_8001BB1C();
void fn_8001BB8C();
void fn_803EC154(int, void (*)(), void (*)());
void fn_803EBFCC(int);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
void fn_803EBC54(int, int, int, State*);
int fn_803EC094(int);
void fn_801FC318(void*, const void*, const void*, int, int);

extern State lbl_80494928;
extern unsigned char lbl_80494938[];
extern int lbl_8059C52C;
extern unsigned char lbl_8059FFDC;
extern const unsigned char lbl_805A14B0[8];
extern const unsigned char lbl_805A14B8[8];

void fn_8001BBB8() {
    fn_803EC154(0, fn_8001BB1C, fn_8001BB8C);
    fn_803EBFCC(0x10000);

    lbl_80494928.value = 0;
    lbl_80494928.work = 0;

    Work* work =
        (Work*)alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(1, 0x1194);
    work->first = (char*)work + 0x20;
    work->second = (char*)work + 0x160;
    work->count = 4;
    work->count2 = 4;
    work->value = 1;
    work->value2 = 1;
    lbl_80494928.work = work;

    fn_803EBC54(0, 0, 0, &lbl_80494928);

    if (lbl_80494928.bytes[1] & 0x10) {
        lbl_8059C52C = 1;
        int enabled = 0;
        lbl_8059FFDC = 1;

        if (fn_803EC094(0x41) == 1) {
            enabled = 1;
        }
        if (enabled) {
            lbl_8059C52C = 3;
        }
    }

    fn_801FC318(lbl_80494938, lbl_805A14B0, lbl_805A14B8, 0, 2);
}
}
}

namespace jj_8001CFA4 {
extern "C" {
void OSLockMutex(void*);
void OSUnlockMutex(void*);
int fn_8001D134(void*);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
unsigned long strlen(const char*);
extern char lbl_80494910[0x18];
extern char lbl_8059C570[6];
}

struct S {
    const char* p;
    int a, b, c, d;
    unsigned r : 8;
    unsigned x : 8;
    unsigned y : 16;
};

struct Lock {
    void* mutex;
    bool locked : 8;

    Lock(void* m, bool l = true) {
        mutex = m;
        locked = l;
        if (locked) OSLockMutex(mutex);
    }

    void release() {
        if (locked) OSUnlockMutex(mutex);
    }
};

extern "C" int fn_8001CFA4(S* arg) {
    Lock lock(lbl_80494910);
    S local;
    char buf2[0x80];
    char buf1[0x80];

    local = *arg;

    const char* s = arg->p;
    const char* p = &s[strlen(s) - 1];
    do {
        if (*(p - 1) == '/') break;
        p--;
    } while (p != s);
    strcpy(buf2, p);

    strcpy(buf1, lbl_8059C570);
    strcat(buf1, buf2);
    local.p = buf1;

    int result = fn_8001D134(&local);
    if (result == 0) {
        arg->c = local.c;
        arg->b = local.b;
        arg->r = result;
    }

    Lock* lp = &lock;
    lp->release();
    return result;
}
}

namespace jj_8001FB64 {


extern "C" {

int fn_803EBE4C(void);
extern char lbl_8042153C[];

int fn_8001FB64(const char* pattern, unsigned int dest, int unused, int size) {
    int result;
    char* output = (char*)dest;
    char buffer[0x444];

    if ((result = FAFsfirst(pattern, 0x20, (FAEntryInfo*)buffer)) != 0) {
        if (fn_803EBE4C() != 2) {
            fn_803EBE4C();
        }
    } else {
        {
            char* name = buffer + 0x23a;
            if (*name == 0) {
                name = buffer + 0x22d;
            }

            unsigned int length = strlen(name);
            if (size == 0 || length < (unsigned int)size) {
                strcpy(output, name);
                *(output += length) = 0;
                ++output;
            }
        }

        while (result == 0) {
            result = FAFsnext((FAEntryInfo*)buffer);
            if (result == 0) {
                char* name = buffer + 0x23a;
                if (*name == 0) {
                    name = buffer + 0x22d;
                }

                unsigned int length = strlen(name);
                strcpy(output, name);
                *(output += length) = 0;
                ++output;
            } else {
                if (fn_803EBE4C() != 2) {
                    fn_803EBE4C();
                    OSReport(lbl_8042153C, pattern);
                }
                break;
            }
        }
    }

    return 0;
}

}
}

namespace jj_8001D8E8 {
extern "C" {
unsigned int strlen(const char*);
char* strncpy(char*, const char*, unsigned int);
int fn_8001B7F8(const char*);
int fn_8001DA24(char**);
int fn_8001DC28(char**);
int fn_8001DB34(char**);
}
struct A { const char* p; int a, b, c, d; unsigned r : 8; unsigned x : 8; unsigned y : 16; };
struct B { unsigned char pad[0x15]; unsigned char x : 8; };
extern "C" int fn_8001D8E8(A* self) {
    char buf[128];
    int t = fn_8001B7F8(self->p);
    const char* s = self->p;
    unsigned int n = strlen(s);
    const char* q = s;
    for (unsigned int i = 0; i < n; i++) {
        if (*q == ':') { strncpy(buf, q + 1, 128); goto done; }
        q++;
    }
    strncpy(buf, s, 128);
done:
    A l;
    ((int*)&l)[0] = ((int*)self)[0];
    ((int*)&l)[1] = ((int*)self)[1];
    ((int*)&l)[2] = ((int*)self)[2];
    ((int*)&l)[3] = ((int*)self)[3];
    ((int*)&l)[4] = ((int*)self)[4];
    ((int*)&l)[5] = ((int*)self)[5];
    l.p = buf;
    int r = 5;
    switch (t) {
    case 0: r = 5; break;
    case 2: r = fn_8001DA24((char**)&l); break;
    case 3: r = fn_8001DC28((char**)&l); break;
    case 4:
        ((B*)&l)->x = ((const unsigned char*)self->p)[2] - '0';
        r = fn_8001DB34((char**)&l);
        break;
    }
    return r;
}
}

namespace jj_8001F84C {
extern "C" {
int fn_803C3DE0(int*, int, int);
void fn_8001B9E8(void);
int fn_803C3ED8(int*);
extern const char lbl_80421520[];
int fn_8001F84C(int arg0, unsigned int dest, int unused, int size) {
    char buffer[0x444];
    int status;
    int result;
    char* output = (char*)dest;
    result = -1;
    status = fn_803C3DE0((int*)buffer, arg0, 0x20);
    if (status != 0) {
        if (status != 2) fn_8001B9E8();
    } else {
        {
            char* name = buffer + 0x23A;
            if (*name == 0) name = buffer + 0x22D;
            unsigned int length = strlen(name);
            if (size == 0 || length < (unsigned int)size) {
                strcpy(output, name);
                *(output += length) = 0;
                ++output;
            }
        }
        result = 1;
        for (; status == 0; ++result) {
            status = fn_803C3ED8((int*)buffer);
            if (status == 0) {
                char* name = buffer + 0x23A;
                if (*name == 0) name = buffer + 0x22D;
                unsigned int length = strlen(name);
                strcpy(output, name);
                *(output += length) = 0;
                ++output;
            } else {
                if (status != 2) {
                    fn_8001B9E8();
                    OSReport(lbl_80421520, arg0);
                }
                break;
            }
        }
    }
    return result;
}
}
}

// TODO: clean up, give variables real names
namespace jj_800208E8 {
extern "C" {
void OSLockMutex(void*);
void OSUnlockMutex(void*);
void OSReport(const char*, ...);
void fn_8020D38C(char*);
s32 fn_8020B5A0(char*, char*);
s32 fn_8020B834(char*, char*);
unsigned int strlen(const char*);
char* strncpy(char*, const char*, unsigned int);
char* strcat(char*, const char*);
int strncmp(const char*, const char*, unsigned int);
extern char lbl_80494910[0x18];
extern char lbl_8059C57C[5];
extern char lbl_804215D8[];
extern char lbl_804215F8[];
struct Lock {
    void* mutex;
    bool locked : 8;
    Lock(void* m, bool l = true) { mutex = m; locked = l; if (locked) OSLockMutex(mutex); }
    void release() { if (locked) OSUnlockMutex(mutex); }
};
}
extern "C" int fn_800208E8(const char* name, int arg1) {
    if (name == 0 || arg1 == 0)
        return 0;
    char buf_name[0x80];
    char buf_path[0x80];
    char buf_tmp[0x80];
    Lock lock(lbl_80494910);
    char arg2[8];
    unsigned int len = strlen(name);
    const char* p = name;
    unsigned int i;
    for (i = 0; i < len; i++) {
        if (*p == ':') {
            strncpy(buf_name, p + 1, 0x80);
            goto copied;
        }
        p++;
    }
    strncpy(buf_name, name, 0x80);
copied:
    buf_path[0] = 0;
    if (strncmp(buf_name, lbl_8059C57C, 4) == 0) {
        strcat(buf_path, buf_name);
    } else {
        fn_8020D38C(buf_tmp);
        strncpy(buf_path, buf_tmp, 0x40);
        strcat(buf_path, buf_name);
    }
    int r = fn_8020B5A0(buf_path, arg2);
    if (r != 0) {
        OSReport(lbl_804215D8, r, buf_path);
        Lock* lp = &lock;
        lp->release();
        return 0;
    }
    arg2[7] = (char)arg1;
    r = fn_8020B834(buf_path, arg2);
    if (r != 0) {
        OSReport(lbl_804215F8, r, buf_path);
        Lock* lp = &lock;
        lp->release();
        return 0;
    }
    Lock* lp = &lock;
    lp->release();
    return 1;
}
}

namespace jj_8001EE94 {

extern "C" {
unsigned int strlen(const char*);
char* strncpy(char*, const char*, int);
void OSReport(const char*, ...);
int fn_8001B7F8(const char*);
int fn_8001B8E0(int, int);
int fn_8020DA08(int*, int*);
int fn_803EBF9C(int, void*);
int fn_803EBE4C(void);
int fn_803C3FD0(signed char*);
int fn_803C3F44(void);
int fn_8001B9E8(void);
extern char lbl_80420FC0[];
extern unsigned char lbl_8059C5B4;
extern unsigned char lbl_8059C5B5;
}

struct Flags {
    unsigned r : 8;
    unsigned x : 8;
    unsigned y : 16;
};

struct S {
    const char* p;
    int a, b, c, d;
    Flags flags;
};

struct B {
    unsigned char pad[0x15];
    unsigned char x : 8;
};

extern "C" int fn_8001EE94(S* self) {
    int out2;
    int out1;
    int data[4];
    S local;
    char text[128];
    char* base = lbl_80420FC0;
    int type = fn_8001B7F8(self->p);
    const char* s = self->p;
    unsigned int n = strlen(s);
    const char* q = s;
    for (unsigned int i = 0; i < n; i++) {
        if (*q == ':') {
            strncpy(text, q + 1, 128);
            goto copied;
        }
        q++;
    }
    strncpy(text, s, 128);
copied:
    local.flags = self->flags;
    int result = self->b;
    int status = 5;
    switch (type) {
    case 1:
        break;
    case 2: {
        status = 0x10;
        int rv = fn_8020DA08(&out1, &out2);
        if (rv != 0) {
            fn_8001B8E0(rv, 0);
            OSReport(base + 0x4F8);
        } else {
            status = 0;
            result = out1;
            ((B*)&local)->x = out2;
        }
        break;
    }
    case 3: {
        status = 0x10;
        if (fn_803EBF9C(0x41, data) != 0) {
            fn_803EBE4C();
            OSReport(base + 0x510);
        } else {
            status = 0;
            result = (unsigned)(data[1] * data[3] * data[2]) >> 17;
        }
        break;
    }
    case 4: {
        signed char bytes[2];
        status = 0x10;
        int delta = (unsigned char)self->p[2] - '0';
        ((B*)&local)->x = delta;
        bytes[0] = lbl_8059C5B4 + ((B*)&local)->x;
        bytes[1] = lbl_8059C5B5;
        int rv = fn_803C3FD0(bytes);
        if (rv < 0) {
            fn_803C3F44();
            fn_8001B9E8();
            OSReport(base + 0x528);
        } else {
            status = 0;
            result = rv / 131072;
        }
        break;
    }
    }
    self->flags.r = status;
    self->b = result;
    ((B*)self)->x = ((B*)&local)->x;
    return status;
}
}

namespace jj_8002035C {
typedef unsigned int u32;
typedef unsigned long long u64;

extern "C" {
extern void OSReport(const char *, ...);
extern void OSSleepTicks(u64);
extern int fn_8001B8E0(int, int);
extern void fn_8001B9EC(void *, int);
extern int fn_8020A710(void *, int, int);
extern int fn_8020B9BC(void *, int, int);
extern char lbl_80420FC0[];

int fn_8002035C(int arg0, int arg1) {
    char sp8[0x7C];
    char *base;
    int ret;
    int r25;
    int flag;
    int r26;
    u32 q;

    base = lbl_80420FC0;
    fn_8001B9EC(sp8, arg0);
    ret = 0;
    OSReport(base + 0x598, sp8);
    do {
        r25 = fn_8020B9BC(sp8, arg1, 2);
        flag = 0;
        if ((u32)r25 + 3 <= 1) {
            q = (*(u32 *)0x800000F8 / 4) / 1000;
            OSSleepTicks((u64)q << 4);
            flag = 1;
        } else if (r25 != -0xC) {
            fn_8001B8E0(r25, 0);
        }
    } while (flag != 0);

    if (r25 == 0) {
        ret = 1;
    } else {
        do {
            r26 = fn_8020A710(sp8, 0x34, 2);
            flag = 0;
            if ((u32)r26 + 3 <= 1) {
                q = (*(u32 *)0x800000F8 / 4) / 1000;
                OSSleepTicks((u64)q << 4);
                flag = 1;
            } else if (r26 != -0xC) {
                fn_8001B8E0(r26, 0);
            }
        } while (flag != 0);

        if (r26 != 0) {
            OSReport(base + 0x5B0, r26);
        } else {
            do {
                r25 = fn_8020B9BC(sp8, arg1, 2);
                flag = 0;
                if ((u32)r25 + 3 <= 1) {
                    q = (*(u32 *)0x800000F8 / 4) / 1000;
                    OSSleepTicks((u64)q << 4);
                    flag = 1;
                } else if (r25 != -0xC) {
                    fn_8001B8E0(r25, 0);
                }
            } while (flag != 0);

            if (r25 != 0) {
                OSReport(base + 0x2D0, r25);
            } else {
                ret = 1;
            }
        }
    }
    return ret;
}
}
}

namespace jj_8001FDDC {


extern "C" {
int fn_8001B7F8(const char*);
int fn_8001FAD8(const char*);
int fn_8001F7C4(const char*);
extern OSMutex lbl_80494910;
extern char lbl_8059C58C[4];
extern char lbl_8059C598[4];
}

struct Flags {
    unsigned int res : 8;
    unsigned int drive : 8;
    unsigned int rest : 16;
};

struct Arg {
    const char* path;
    int pad4;
    int r8;
    int rC;
    int pad10;
    Flags flags;
};

struct Drive {
    unsigned char pad[0x15];
    unsigned char value : 8;
};

struct Lock {
    OSMutex* mutex;
    bool locked : 8;

    Lock(OSMutex* m, bool l = true) {
        mutex = m;
        locked = l;
        if (locked)
            OSLockMutex(mutex);
    }

    void release() {
        if (locked)
            OSUnlockMutex(mutex);
    }
};

extern "C" int fn_8001FDDC(Arg* arg) {
    char name[0x80];
    char path[0x80];
    Arg local;

    int type = fn_8001B7F8(arg->path);
    const char* p = arg->path;
    unsigned int len = strlen(p);
    const char* q = p;

    for (unsigned int i = 0; i < len; i++, q++) {
        if (*q == ':') {
            strncpy(name, q + 1, 0x80);
            goto done;
        }
    }
    strncpy(name, p, 0x80);

done:
    local.flags = arg->flags;
    char* n = name;
    int result = 0x11;
    int r = 0;

    switch (type) {
    case 3: {
        Lock lock(&lbl_80494910);
        strcpy(path, lbl_8059C598);
        if (n[0] == '/')
            path[2] = 0;
        strncat(path, n, 0x80);
        r = fn_8001FAD8(path);
        Lock* lp = &lock;
        lp->release();
        break;
    }
    case 4: {
        ((Drive*)&local)->value = arg->path[2] - '0';
        Lock lock(&lbl_80494910);
        strcpy(path, lbl_8059C58C);
        path[0] = ((Drive*)&local)->value + 'B';
        if (n[0] == '/')
            path[2] = 0;
        strncat(path, n, 0x80);
        r = fn_8001F7C4(path);
        Lock* lp = &lock;
        lp->release();
        break;
    }
    }

    if (r >= 0)
        result = 0;
    if (r < 0)
        r = 0;

    arg->r8 = r;
    arg->rC = 0;
    arg->flags.res = result;
    return result;
}
}

namespace jj_8001C9EC {

extern "C" {
void OSReport(const char*, ...);
char* strcpy(char*, const char*);
char* strncat(char*, const char*, unsigned long);
int fn_803C374C(const char*, const char*, int);
int fn_803C3CB8(int);
void fn_803C3828(int);
void fn_803C3920(int, void*, int, unsigned int*);
extern OSMutex lbl_80494910;
extern char lbl_8059C58C[4];
extern char lbl_8059C590[2];
extern char lbl_8059C594[3];
extern const char* lbl_80421198[10];
}

struct Arg {
    const char* path;
    unsigned char pad4[4];
    int size;
    void* data;
    void* pool;
    unsigned char pad14;
    unsigned char drive;
};

struct Lock {
    OSMutex* mutex;
    bool locked : 8;
    Lock(OSMutex* m, bool l = true) {
        mutex = m;
        locked = l;
        if (locked) OSLockMutex(mutex);
    }
    void release() {
        if (locked) OSUnlockMutex(mutex);
    }
};

extern "C" int fn_8001C9EC(Arg* arg) {
    Lock lock(&lbl_80494910);
    char path[0x80];
    void* p;
    unsigned char drive;
    int result;
    int found;
    const char* name;
    void* pool;
    int sz;
    int h;
    unsigned int outSize;

    name = arg->path;
    pool = arg->pool;
    drive = arg->drive;
    result = 1;
    found = 0;

    strcpy(path, lbl_8059C58C);
    path[0] = drive + 0x42;
    if (*name == '/') path[2] = 0;
    strncat(path, name, 0x80);
    h = fn_803C374C(name, lbl_8059C590, 0);
    if (h != 0) {
        found = 1;
        sz = fn_803C3CB8(h);
        if (sz > 0) {
            unsigned long aligned = (sz + 0x1F) & ~0x1F;
            if (aligned != 0) {
                p = gfMemoryPool::alloc(pool, aligned, 0x20);
                if (p == 0) {
                    OSReport(lbl_80421198[2], lbl_8059C594, name, sz);
                    result = 4;
                } else {
                    fn_803C3920(h, p, sz, &outSize);
                    if (outSize >= (unsigned int)sz) {
                        int frame;
                        if (g_GameGlobal != 0) {
                            frame = (int)g_GameGlobal->getGameFrame();
                        } else {
                            frame = 0;
                        }
                        OSReport(lbl_80421198[0], lbl_8059C594, name, outSize, p, frame);
                        arg->size = sz;
                        result = 0;
                        arg->data = p;
                    } else {
                        OSReport(lbl_80421198[3], lbl_8059C594, name);
                        gfHeapManager::free(p);
                        result = 2;
                        gfErrorCheck::check();
                    }
                }
            }
        }
    }
    if (found) {
        fn_803C3828(h);
    } else {
        OSReport(lbl_80421198[1], lbl_8059C594, name);
    }
    Lock* lp = &lock;
    lp->release();
    return result;
}
}

namespace jj_8001E2F8 {
extern "C" {
void OSLockMutex(void*);
void OSUnlockMutex(void*);
void OSReport(const char*, ...);
int fn_8001B7F8(const char*);
int fn_8001B8E0(int, int);
int fn_8001E510(void*, int);
int fn_8001E630(void*);
int fn_8020AD88(char*, int, int*);
void fn_8020D38C(char*);
char* strcat(char*, const char*);
unsigned int strlen(const char*);
int strncmp(const char*, const char*, unsigned int);
char* strncpy(char*, const char*, int);
extern char lbl_80421340[0x15];
extern char lbl_80494910[0x18];
extern char lbl_8059C57C[5];
}
struct S {
    const char* p;
    int a, b, c, d;
    unsigned r : 8;
    unsigned x : 8;
    unsigned y : 16;
};
struct B { unsigned char pad[0x15]; unsigned char x : 8; };
struct Guard {
    void* mutex;
    unsigned char flag : 8;
};

extern "C" int fn_8001E2F8(S* self) {
    int out;
    int r;
    Guard g;
    Guard* p = &g;
    S l;
    char buf[128];
    char s176[128];
    char s30[128];
    int t = fn_8001B7F8(self->p);
    const char* s = self->p;
    unsigned int n = strlen(s);
    const char* q = s;
    unsigned int i;
    for (i = 0; i < n; i++) {
        if (*q == ':') {
            strncpy(buf, q + 1, 128);
            goto done;
        }
        q++;
    }
    strncpy(buf, s, 128);
done:
    r = 5;
    ((int*)&l)[0] = ((int*)self)[0];
    ((int*)&l)[1] = ((int*)self)[1];
    ((int*)&l)[2] = ((int*)self)[2];
    ((int*)&l)[3] = ((int*)self)[3];
    ((int*)&l)[4] = ((int*)self)[4];
    ((int*)&l)[5] = ((int*)self)[5];
    l.p = buf;
    switch (t) {
    case 2: {
        p->flag = 1;
        p->mutex = lbl_80494910;
        if (p->flag != 0) OSLockMutex(p->mutex);
        const char* name = l.p;
        s30[0] = 0;
        int result = 0x12;
        if (strncmp(name, lbl_8059C57C, 4) == 0) {
            strcat(s30, name);
        } else {
            fn_8020D38C(s176);
            strncpy(s30, s176, 0x40);
            strcat(s30, name);
        }
        int rv = fn_8020AD88(s30, 0, &out);
        if (rv != 0) {
            fn_8001B8E0(rv, 0);
            OSReport(lbl_80421340, s30);
        } else {
            result = 0;
        }
        if (p->flag != 0) OSUnlockMutex(p->mutex);
        r = result;
        break;
    }
    case 3:
        r = fn_8001E630(&l);
        break;
    case 4: {
        int d = ((const unsigned char*)self->p)[2] - '0';
        ((B*)&l)->x = d;
        r = fn_8001E510(&l, d);
        break;
    }
    }
    self->r = r;
    return r;
}
}

namespace jj_8001DD0C {
extern "C" {
int fn_8001B7F8(const char*);
extern OSMutex lbl_80494910;
extern char lbl_8059C58C[4];
extern char lbl_8059C598[4];
extern char lbl_8059C5A8[5];
}
struct S {
    const char* p;
    int a, b, c, d;
    unsigned r : 8;
    unsigned x : 8;
    unsigned y : 16;
};
struct B { unsigned char pad[0x15]; unsigned char x : 8; };
extern "C" int fn_8001DF68(S*);
extern "C" int fn_8001E10C(char*, S*);
struct Lock {
    OSMutex* mutex;
    bool locked : 8;
    Lock(OSMutex* m, bool l = true) {
        mutex = m;
        locked = l;
        if (locked) OSLockMutex(mutex);
    }
    void release() {
        if (locked) OSUnlockMutex(mutex);
    }
};
extern "C" void fn_8001DD0C(S* self) {
    char buf[128];
    char sdPath[128];
    char vfPath[128];
    int t = fn_8001B7F8(self->p);
    const char* s = self->p;
    unsigned int n = strlen(s);
    const char* q = s;
    for (unsigned int i = 0; i < n; i++) {
        if (*q == ':') {
            strncpy(buf, q + 1, 128);
            goto done;
        }
        q++;
    }
    strncpy(buf, s, 128);
done:
    S l;
    ((int*)&l)[0] = ((int*)self)[0];
    ((int*)&l)[1] = ((int*)self)[1];
    ((int*)&l)[2] = ((int*)self)[2];
    ((int*)&l)[3] = ((int*)self)[3];
    ((int*)&l)[4] = ((int*)self)[4];
    ((int*)&l)[5] = ((int*)self)[5];
    l.p = buf;
    int r = 5;
    switch (t) {
    case 1: break;
    case 2: r = fn_8001DF68(&l); break;
    case 3: {
        Lock lock(&lbl_80494910);
        const char* path = l.p;
        strcpy(sdPath, lbl_8059C598);
        if (path[0] == '/') sdPath[2] = 0;
        strncat(sdPath, path, 128);
        strncat(sdPath, lbl_8059C5A8, 128);
        int result = fn_8001E10C(sdPath, &l);
        Lock* lp = &lock;
        lp->release();
        r = result;
        break;
    }
    case 4: {
        ((B*)&l)->x = ((const unsigned char*)self->p)[2] - '0';
        Lock lock(&lbl_80494910);
        unsigned char drive = ((B*)&l)->x;
        const char* path = l.p;
        strcpy(vfPath, lbl_8059C58C);
        vfPath[0] = drive + 'B';
        if (path[0] == '/') vfPath[2] = 0;
        strncat(vfPath, path, 128);
        strncat(vfPath, lbl_8059C5A8, 128);
        int result = fn_8001E10C(vfPath, &l);
        Lock* lp = &lock;
        lp->release();
        r = result;
        break;
    }
    }
    self->r = r;
}
}

namespace jj_8001EC10 {

extern "C" {
int fn_8001B7F8(const char*);
void fn_8001B9E8(int);
int fn_8020A870(const char*);
void fn_8020D38C(char*);
int fn_803C3A48(const char*);
void fn_803EBE4C();
extern OSMutex lbl_80494910;
extern char lbl_8059C57C[5];
extern char lbl_8059C58C[4];
extern char lbl_8059C598[4];
extern char lbl_80421490[];
}

struct Flags {
    unsigned r : 8;
    unsigned x : 8;
    unsigned y : 16;
};
struct S {
    const char* p;
    int a, b, c, d;
    Flags flags;
};
struct B {
    unsigned char pad[0x15];
    unsigned char x : 8;
};
struct Lock {
    OSMutex* mutex;
    bool locked : 8;
    Lock(OSMutex* m, bool l = true) {
        mutex = m;
        locked = l;
        if (locked) OSLockMutex(mutex);
    }
    void release() {
        if (locked) OSUnlockMutex(mutex);
    }
};

extern "C" int fn_8001EC10(S* self) {
    char name[128];
    char tmp[128];
    char nandPath[128];
    char vfPath[128];
    char sdPath[128];
    S local;
    int type = fn_8001B7F8(self->p);
    const char* s = self->p;
    unsigned int n = strlen(s);
    const char* q = s;
    for (unsigned int i = 0; i < n; i++) {
        if (*q == ':') {
            strncpy(name, q + 1, 128);
            goto copied;
        }
        q++;
    }
    strncpy(name, s, 128);
copied:
    local.flags = self->flags;
    const char* path = name;
    int status = 7;
    switch (type) {
    case 2: {
        Lock lock(&lbl_80494910);
        nandPath[0] = 0;
        if (strncmp(path, lbl_8059C57C, 4) == 0) {
            strcat(nandPath, path);
        } else {
            fn_8020D38C(tmp);
            strncpy(nandPath, tmp, 64);
            strcat(nandPath, path);
        }
        type = fn_8020A870(nandPath);
        if (type != 0 && type != -12) {
            OSReport(lbl_80421490, type, nandPath);
            fn_8001B9E8(type);
            status = 7;
        } else {
            status = 0;
        }
        Lock* lp = &lock;
        lp->release();
        break;
    }
    case 4: {
        ((B*)&local)->x = (unsigned char)self->p[2] - '0';
        strcpy(vfPath, lbl_8059C58C);
        vfPath[0] = ((B*)&local)->x + 'B';
        if (path[0] == '/') vfPath[2] = 0;
        strncat(vfPath, path, 128);
        int rv = fn_803C3A48(vfPath);
        if (rv != 0) {
            fn_8001B9E8(rv);
            status = 7;
        } else {
            status = 0;
        }
        break;
    }
    case 3: {
        status = 7;
        strcpy(sdPath, lbl_8059C598);
        if (path[0] == '/') sdPath[2] = 0;
        strncat(sdPath, path, 128);
        if (FARemove(sdPath) != 0) {
            fn_803EBE4C();
        } else {
            status = 0;
        }
        break;
    }
    }
    self->flags.r = status;
    return status;
}
}

namespace jj_8001F0D4 {
extern "C" {
unsigned int strlen(const char*);
char* strncpy(char*, const char*, unsigned int);
char* strcat(char*, const char*);
int fn_8001B7F8(const char*);
int fn_8001F390(void*);
int fn_8001F4B0(void*);
int fn_801F5F08(const char*);
int fn_801F6210(int, void*);
extern int lbl_8059C530;
extern char* lbl_804211C0[];
extern char lbl_8059C56C[4];
}
struct S {
    const char* p;
    int a, b, c, d;
    unsigned r : 8;
    unsigned x : 8;
    unsigned y : 16;
};
struct B { unsigned char pad[0x15]; unsigned char x : 8; };
extern "C" int fn_8001F0D4(S* self) {
    char name[128];
    char localized[128];
    char path[128];
    int info[16];
    int type = fn_8001B7F8(self->p);
    const char* s = self->p;
    unsigned int n = strlen(s);
    const char* q = s;
    for (unsigned int i = 0; i < n; i++) {
        if (*q == ':') {
            strncpy(name, q + 1, 128);
            goto copied;
        }
        q++;
    }
    strncpy(name, s, 128);
copied:
    S local;
    ((int*)&local)[0] = ((int*)self)[0];
    ((int*)&local)[1] = ((int*)self)[1];
    ((int*)&local)[2] = ((int*)self)[2];
    ((int*)&local)[3] = ((int*)self)[3];
    ((int*)&local)[4] = ((int*)self)[4];
    ((int*)&local)[5] = ((int*)self)[5];
    local.p = name;
    local.b = 0;
    int status = 17;
    if (type == 2) goto nand;
    if (type < 2) {
        if (type == 0) goto common;
        if (type >= 0) goto common;
        goto done;
    }
    if (type == 4) goto vf;
    if (type >= 4) goto done;
    goto sd;
common: {
        status = 17;
        int size;
        int length;
        const char* p = local.p;
        fn_8001B7F8(p);
        unsigned int len = strlen(p);
        const char* t = p;
        for (unsigned int i = 0; i < len; i++) {
            if (*t == ':') {
                strncpy(path, t + 1, 128);
                goto copied2;
            }
            t++;
        }
        strncpy(path, p, 128);
copied2:
        length = strlen(path);
        const char* end = path + length;
        for (int i = length; i != 0; i--) {
            if (*end == '.') {
                strncpy(localized, path, length);
                localized[length] = 0;
                strcat(localized, lbl_804211C0[lbl_8059C530]);
                strcat(localized, path + length);
                goto localized_done;
            }
            end--;
            length--;
        }
        strncpy(localized, path, 128);
        strcat(localized, lbl_8059C56C);
localized_done:
        size = -1;
        int file = fn_801F5F08(localized);
        if (file < 0) file = fn_801F5F08(path);
        if (file >= 0) {
            fn_801F6210(file, info);
            size = (info[13] + 31) & ~31;
        }
        if (size >= 0) status = 0;
        goto done;
    }
    nand: status = fn_8001F390(&local); goto done;
    sd: status = gfFileIO::checkFileSD((gfFileIORequest*)&local); goto done;
    vf:
        ((B*)&local)->x = ((const unsigned char*)self->p)[2] - '0';
        status = fn_8001F4B0(&local);
        goto done;
    done:
    self->r = status;
    if (status == 0) self->b = local.b;
    return status;
}
}

