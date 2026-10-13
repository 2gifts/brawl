#include <types.h>
#include <string.h>
#include <gf/gf_file_io.h>
#include <revolution/FA.h>
#include <revolution/OS/OSMutex.h>

extern "C" int fn_803EC0CC(const char* path);
extern "C" int fn_803EBE4C();
extern "C" OSMutex lbl_80494910;
extern "C" char lbl_8059C598;

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

int gfFileIO::checkFileSD(gfFileIORequest* request) {
    gfFileIOLock lock(&lbl_80494910);
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

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_8059FFD8[0x4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8059FFDC[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8059FFE0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8059FFE4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8059FFE8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8059FFEC[0x4] __attribute__((aligned(4))) = { 0 };
