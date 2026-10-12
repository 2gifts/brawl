#include <types.h>
#include <string.h>
#include <gf/gf_file_io.h>
#include <revolution/FA.h>
#include <revolution/OS/OSMutex.h>

extern "C" OSMutex lbl_80494910;
extern "C" char lbl_8059C598;
extern "C" int fn_803EBE4C();

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
