#include <types.h>
#include <string.h>
#include <gf/gf_callback.h>
#include <gf/gf_error_manager.h>
#include <gf/gf_file_io.h>
#include <revolution/FA.h>
#include <revolution/NAND.h>
#include <revolution/NET/crc.h>
#include <revolution/OS.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/VF/d_vf.h>

extern "C" int fn_803EC0CC(const char* path);
extern "C" int fn_803EBE4C();
extern "C" int fn_803EC094(int drive);
extern "C" int fn_803EC0F8(int drive);
extern "C" int fn_803EC22C(int drive, int force);
extern "C" s32 fn_803C3B18(const char* path);
extern "C" s32 fn_803C3BE8(const char* path);
extern "C" s32 fn_803C3CF8(const char* path);
// HYPOTHESIS: the VF find-first/find-next calls fill the same find entry as FAFsfirst
extern "C" s32 fn_803C3DE0(FAEntryInfo* info, const char* pattern, int filter);
extern "C" s32 fn_803C3ED8(FAEntryInfo* info);
extern "C" s32 fn_803C4054(const char* drive);
extern "C" s32 fn_803C3048(const char* path, u32 size);
extern "C" s32 fn_803C31B4(const char* drive, const char* path);
extern "C" s32 NANDCreateDir(const char* path, u8 perm, u8 attr);
extern "C" DVDDiskID lbl_80494938;
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

void gfFileIO::setSDMountStatusCallback(MountStatusCB cb) {
    lbl_8059FFE8 = cb;
    cb(lbl_8059C52C);
}

void gfFileIO::FAInsertCallback() {
    lbl_8059C52C = 1;
    lbl_8059FFDC = true;
    bool isProtected = false;
    if (fn_803EC094('A') == 1) {
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
    if (fn_803EC094('A') == 1) {
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
        result = fn_803C3B18(filepath);
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
                result = fn_803C3B18(dir);
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
        result = fn_803EC0CC(filepath);
        if (result != 0) {
            result = fn_803EBE4C();
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
                    result = fn_803EC0CC(dir);
                    if (result != 0) {
                        result = fn_803EBE4C();
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
    if (gfFACreateDir(path) != 0 && fn_803EBE4C() != 0x11) {
        fn_803EBE4C();
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
    s32 ret = fn_803C3BE8(path);
    if (ret != 0) {
        if (ret != 2) {
            gfVFErrorCheck(ret);
            OSReport("VF check Dir: (%s)", request->m_pFilepath);
        }
    } else {
        fn_803C3BE8(current);
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
        if (fn_803EBE4C() != 2) {
            fn_803EBE4C();
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
    s32 ret = fn_803C3048(path, request->m_readLen);
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
    s32 ret = fn_803C31B4(drive, path);
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
    s32 ret = fn_803C4054(drive);
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
    int ret = fn_803EC0F8('A');
    if (ret != 0) {
        fn_803EBE4C();
        result = 0xD;
        int code = fn_803EBE4C();
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
        fn_803EBE4C();
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
        if (fn_803EBE4C() != 2) {
            fn_803EBE4C();
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
    s32 result = fn_803C3DE0((FAEntryInfo*)info, path, 0x20);
    if (result != 0) {
        if (result != 2) {
            gfVFErrorCheck(result);
        }
    } else {
        count = 1;
        while (result == 0) {
            result = fn_803C3ED8((FAEntryInfo*)info);
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
        if (fn_803EBE4C() != 2) {
            fn_803EBE4C();
        }
    } else {
        count = 1;
        while (result == 0) {
            result = FAFsnext((FAEntryInfo*)info);
            if (result != 0) {
                if (fn_803EBE4C() != 2) {
                    fn_803EBE4C();
                }
                break;
            }
            count++;
        }
    }
    return count;
}
