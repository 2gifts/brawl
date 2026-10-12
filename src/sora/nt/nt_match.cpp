#include <mu/wifi/mu_wifi_interface.h>
#include <types.h>

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
    static void ntFree(int, void*);
};

struct OSMutex;
extern "C" void OSLockMutex(OSMutex* mutex);
extern "C" void OSUnlockMutex(OSMutex* mutex);
extern OSMutex lbl_8049EC78;

extern "C" void fn_80025F2C(void* heap, void* ptr);

struct unk805A0434 {
    u8 unk0[0x353C];
    void* unk353C;
};
extern unk805A0434* lbl_805A0434;

void DWCAllocType::ntFree(int arg0, void* arg1) {
    if (arg1 != 0) {
        OSLockMutex(&lbl_8049EC78);
        fn_80025F2C(lbl_805A0434->unk353C, arg1);
        OSUnlockMutex(&lbl_8049EC78);
    }
}


void netReport0(const char* fmt, ...) {
}
