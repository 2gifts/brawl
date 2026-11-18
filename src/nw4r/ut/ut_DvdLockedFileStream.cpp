#include <types.h>
#include <nw4r/ut.h>

#include <revolution/OS.h>

namespace nw4r {
namespace ut {

NW4R_UT_RTTI_DEF_DERIVED(DvdLockedFileStream, DvdFileStream);

bool DvdLockedFileStream::sInitialized = false;
OSMutex DvdLockedFileStream::sMutex;
OSThreadQueue DvdLockedFileStream::sThreadQueue;

void DvdLockedFileStream::InitMutex_() {
    BOOL enabled = OSDisableInterrupts();

    if (!sInitialized) {
        OSInitMutex(&sMutex);
        OSInitThreadQueue(&sThreadQueue);
        sInitialized = true;
    }

    OSRestoreInterrupts(enabled);
}

DvdLockedFileStream::DvdLockedFileStream(s32 entrynum)
    : DvdFileStream(entrynum), mCancelFlag(false) {
    InitMutex_();
}

DvdLockedFileStream::DvdLockedFileStream(const DVDFileInfo* pInfo, bool close)
    : DvdFileStream(pInfo, close), mCancelFlag(false) {
    InitMutex_();
}

DvdLockedFileStream::~DvdLockedFileStream() {}

s32 DvdLockedFileStream::Read(void* pDst, u32 size) {
    OSLockMutex(&sMutex);
    s32 result = DvdFileStream::Read(pDst, size);
    OSUnlockMutex(&sMutex);
    return result;
}

s32 DvdLockedFileStream::Peek(void* pDst, u32 size) {
    OSLockMutex(&sMutex);
    s32 result = DvdFileStream::Peek(pDst, size);
    OSUnlockMutex(&sMutex);
    return result;
}

} // namespace ut
} // namespace nw4r

// TODO: clean up, give variables real names
namespace jj_8015D4CC {
#pragma peephole off
extern "C" void fn_8015D00C(void*);
extern "C" void fn_8015D4CC(void* self) {
 fn_8015D00C(self);
 *((signed char*)self + 111) = 0;
}
}

namespace jj_8015D6A8 {
extern "C" unsigned int OSDisableInterrupts(void);
extern "C" void OSRestoreInterrupts(unsigned int status);
extern "C" void fn_801E187C(void* p);
extern "C" void fn_8015D29C(void* arg);
extern int lbl_805A0498;

extern "C" void fn_8015D6A8(void* arg0) {
    unsigned int status = OSDisableInterrupts();
    *(unsigned char*)((char*)arg0 + 0x6F) = 1;
    fn_801E187C(&lbl_805A0498);
    OSRestoreInterrupts(status);
    fn_8015D29C(arg0);
}
}

