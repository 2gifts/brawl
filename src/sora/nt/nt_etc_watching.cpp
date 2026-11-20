#include <gf/gf_thread.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_crypto_aes.h>
#include <nt/nt_etc_watching.h>
#include <nt/nt_net_report.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

nteWatching::nteWatching(Heaps::HeapType heap) : gfThread(this, 0xF, 0x2000, Heaps::Thread) {
    m_unk348 = 0;
    m_unk34C = 0;
    m_unk358 = 0;
    m_unk350 = 0x64;
    m_sbs = new (heap) nteSBS(1);
    m_sbs->unk50 = this;
    m_sbs->unk4C = this;
    m_heap = heap;
    m_unk398 = 0;
    m_unk38C = 0;
    m_unk390 = 0;
    m_crypto = NULL;
}

nteWatching::~nteWatching() {
    bool terminated = OSIsThreadTerminated(&m_thread) == true;
    if (!terminated) {
        m_unk398 = 1;
        OSResumeThread(&m_thread);
        bool running;
        while (running = OSIsThreadTerminated(&m_thread) == true, !running) {
            OSSleepTicks(OS_MSEC_TO_TICKS(1));
        }
    }
    if (m_crypto != NULL) {
        delete m_crypto;
        m_crypto = NULL;
    }
    netReport("SBS\x82\xCC\x89\xF0\x95\xFA\n");
    m_sbs->exit();
}

bool nteWatching::download() {
    bool terminated = OSIsThreadTerminated(&m_thread) == true;
    if (terminated) {
        return false;
    }
    if (m_unk350 != 0x64) {
        return false;
    }
    m_unk350 = 1;
    OSResumeThread(&m_thread);
    return true;
}

bool nteWatching::cancel() {
    bool terminated = OSIsThreadTerminated(&m_thread) == true;
    if (terminated) {
        return false;
    }
    if (m_unk350 == 0x64) {
        return false;
    }
    m_sbs->cancel();
    m_unk398 = 1;
    netReport("\x1b[31m [cancel] \x1b[00m\n");
    return true;
}

void nteWatching::showError(s32 kind, s32 code) {
    netReport("\x1b[31m nteWatching : Error = %d \x1b[00m\n");
    if (code == 6) {
        m_unk398 = 1;
    } else if (code == 7) {
        m_unk398 = 1;
    }
    if (kind != 0) {
        m_unk388 = 1;
    }
}

void nteWatching::notifySBSEndProc(s32 arg, s32 code) {
    s32 err = m_sbs->getErrorCode(code);
    if (err != 0) {
        m_unk38C = err;
        m_unk390 = 1;
        m_unk398 = 1;
    }
}

// TODO: clean up, give variables real names
extern "C" void showError__11nteWatchingFll(void*);

extern "C" void fn_801571F0(void* self) {
    showError__11nteWatchingFll(reinterpret_cast<u8*>(self) - 0x340);
}

extern "C" void notifySBSEndProc__11nteWatchingFll(void*);

// TODO: clean up, give variables real names
extern "C" void fn_801571F8(void* self) {
    notifySBSEndProc__11nteWatchingFll(reinterpret_cast<u8*>(self) - 0x344);
}

// TODO: clean up, give variables real names
namespace jj_80157048 {
typedef unsigned char u8;
typedef int s32;

struct Slot {
    s32 key;
    s32 cnt;
};

// TODO: clean up, give variables real names
extern "C" s32 fn_80157048(s32 unused, u8* data) {
    Slot slots[7];
    s32 i;
    s32 idx;
    u8 key;
    s32* p;
    s32 val;
    
    for (i = 0; i < 7; i++) {
        slots[i].key = -1;
        slots[i].cnt = 0;
    }
    
    for (i = 0; i < 7; i++) {
        if (data[0x25] <= 1) {
            key = data[0x24];
            p = (s32*)&slots[0].key;
            for (idx = 0; idx < 7; idx++) {
                val = *p;
                if (val < 0) {
                    slots[idx].key = key;
                    slots[idx].cnt = 1;
                    break;
                } else if ((s32)key == val) {
                    slots[idx].cnt++;
                    break;
                }
                p += 2;
            }
        }
        data += 0x2AC;
    }
    
    s32 ret = 0;
    for (i = 0; i < 7; i++) {
        if (slots[i].key >= 0 && ret < slots[i].cnt) {
            ret = slots[idx].cnt;
        }
    }
    return ret;
}
}

namespace jj_80155C50 {

extern "C" {
extern char lbl_80461718[];
int fn_80155F54(void* self, void** out1, int* out2);
void* __ct__11gfCryptoAESFv();
void* __dt__11gfCryptoAESFv(void* self, int flag);
int encrypt__11gfCryptoAESFPCvUlQ25Heaps8HeapType(void* self, void* a, int b, int c);
void netReport__FPCce(const char* fmt, ...);
void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
}

inline bool terminated(u8* thread) {
    return OSIsThreadTerminated((OSThread*)(thread + 8)) == 1;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80155C50(void* arg0) {
    char* base = lbl_80461718;
    u8* self = (u8*)arg0;
    void* heap;
    int r;

    if (terminated(self)) {
        return 0;
    }
    if (*(int*)(self + 0x350) != 0x64) {
        return 0;
    }
    netReport__FPCce(base + 0x2F4);
    if (*(void**)(self + 0x39c) == 0) {
        void* spC = 0;
        int sp8 = 0;
        if (fn_80155F54(self, &spC, &sp8) != 0) {
            heap = __nw__FUlQ25Heaps8HeapType(0x388, *(int*)(self + 0x354));
            if (heap != 0) {
                heap = __ct__11gfCryptoAESFv();
            }
            *(void**)(self + 0x39c) = heap;
            netReport__FPCce(base + 0x30C, heap);
            heap = *(void**)(self + 0x39c);
            if (heap != 0) {
                *(u8*)((u8*)heap + 0x380) = 1;
                r = encrypt__11gfCryptoAESFPCvUlQ25Heaps8HeapType(*(void**)(self + 0x39c), spC, sp8, *(int*)(self + 0x354));
                if (r != 0) {
                    netReport__FPCce(base + 0x33C, spC, sp8, *(int*)(self + 0x354));
                    while (*(u8*)((u8*)*(void**)(self + 0x39c) + 0x370) == 0) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                }
                netReport__FPCce(base + 0x390);
                if (*(int*)((u8*)*(void**)(self + 0x39c) + 0x374) != 0) {
                    netReport__FPCce(base + 0x3B4);
                    __dt__11gfCryptoAESFv(*(void**)(self + 0x39c), 1);
                    *(void**)(self + 0x39c) = 0;
                }
                if (spC != 0) {
                    gfHeapManager::free(spC);
                }
            }
        }
    }
    *(int*)(self + 0x350) = 0;
    OSResumeThread((OSThread*)(self + 8));
    return 1;
}
}

namespace jj_80156164 {
extern "C" {

extern unsigned char lbl_80461718[];

struct SwObj {
    int type;
    int pad4;
    void** vt;
    int unkC;
    int unk10;
};

struct SwInst {
    char pad[0x18];
    int unk18;
};

struct Class_80454F64_base { char pad[0x8]; };
class Class_80454F64 : public Class_80454F64_base {
public:
    virtual void __dt__15cmPhotoCallBackFv(int);
};

struct Class_80461670_base { char pad[0x8]; };
class Class_80461670 : public Class_80461670_base {
public:
    virtual void __dt__18nteFileDataSettingFv();
};

struct Class_80461684_base { char pad[0x8]; };
class Class_80461684 : public Class_80461684_base {
public:
    virtual void __dt__16nteFileDataStageFv();
};

struct Class_80461698_base { char pad[0x8]; };
class Class_80461698 : public Class_80461698_base {
public:
    virtual void fn_80152BDC();
};

struct Class_804616AC_base { char pad[0x8]; };
class Class_804616AC : public Class_804616AC_base {
public:
    virtual void __dt__16nteFileDataPhotoFv();
};

struct Class_80466494_base { char pad[0x8]; };
class Class_80466494 : public Class_80466494_base {
public:
    virtual void fn_801B48D8();
};

struct Class_80466B80_base { char pad[0x8]; };
class Class_80466B80 : public Class_80466B80_base {
public:
    virtual void __dt__Q34nw4r3snd18SoundArchivePlayerFv();
};

struct Class_80466D44_base { char pad[0x8]; };
class Class_80466D44 : public Class_80466D44_base {
public:
    virtual void __dt__Q44nw4r3snd6detail5VoiceFv();
};

struct Class_80488350_base { char pad[0x8]; };
class Class_80488350 : public Class_80488350_base {
public:
    virtual void __dt__16hkNullContactMgrFv();
};

unsigned char getSections_Direct__7nteFileFPCUc(void*);
void* getSectionData_Direct__7nteFileFPUcUl(void*, int);
void fn_80153488(void*);
void fn_80153498(void*, int);
int fn_80153504(void*, int);
SwObj* fn_80153818(void*, int);
void* getInstance__8ipSwitchFv(void);
int setBufferData__13ipKeyRecorderFPCvUl(int, void*, int);
void netReport__FPCce(const char*, ...);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_80156164(void* self, void* arg1) {
    char buf[0x10];
    char* base = (char*)lbl_80461718;
    int q;
    SwObj* obj;
    unsigned int ok;
    int v;
    int a;
    int t;

    t = getSections_Direct__7nteFileFPCUc(arg1);
    if ((unsigned char)t != 1) {
        netReport__FPCce((const char*)(base + 0x484), t);
        return 0;
    }

    fn_80153488(buf);
    v = (int)getSectionData_Direct__7nteFileFPUcUl(arg1, 0);
    if (fn_80153504(buf, v) == 0) {
        netReport__FPCce((const char*)(base + 0x4C8));
        fn_80153498(buf, -1);
        return 0;
    }

    obj = fn_80153818(buf, *(int*)((char*)self + 0x354));
    if (obj == 0) {
        netReport__FPCce((const char*)(base + 0x500));
        fn_80153498(buf, -1);
        return 0;
    }

    ok = 0;
    t = obj->type;
    if (t != 2) {
        netReport__FPCce((const char*)(base + 0x514), t);
    } else {
        q = obj->unkC + 0x34;
        a = obj->unk10;
        {
            SwInst* inst = (SwInst*)getInstance__8ipSwitchFv();
            if (setBufferData__13ipKeyRecorderFPCvUl(inst->unk18, (void*)q, a) != 0) {
                ok = 1;
            } else {
                netReport__FPCce((const char*)(base + 0x544));
            }
        }

        if (ok == 1) {
            char* s = (char*)self;
            char* p = (char*)q + 0x18;
            unsigned char i;
            s[0x35C] = ((unsigned char)p[0x8] >> 2) & 0x3F;
            s[0x35D] = ((unsigned char)p[0x9] >> 2) & 7;
            *(unsigned short*)&s[0x35E] = *(unsigned short*)&p[0x1A];
            s[0x386] = p[0x13];

            for (i = 0; i < 7; i++) {
                s[0x360 + i] = p[0x98 + i * 0x5C];
                s[0x367 + i] = p[0x9E + i * 0x5C];
                s[0x36E + i] = p[0xA3 + i * 0x5C];
                s[0x375 + i] = p[0x99 + i * 0x5C];
                s[0x37C + i] = p[0x9A + i * 0x5C];
            }

            *(unsigned short*)&s[0x384] = 0;
        }
    }

    if (obj != 0) {
        ((Class_80454F64*)obj)->__dt__15cmPhotoCallBackFv(1);
    }

    fn_80153498(buf, -1);
    return ok;
}

}
}

namespace jj_80155F54 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

extern "C" {
    void* getInstance__8ipSwitchFv(void);
    void netReport__FPCce(const char*, ...);
    void getBufferData__13ipKeyRecorderFPPvPUl(void*, void**, int*);
    void clearOnamae__13ipKeyRecorderFP14ipReplayHeader(int);
    u8 getUploadSizeLimit__9nteGlobalFv(void);
    void __ct__17nteFileDataReplayFQ25Heaps8HeapType(void*, int);
    void fn_80152BDC(void*, int);
    void setData__17nteFileDataReplayFPvUl(void*, int, int);
    void fn_80153488(void*);
    void fn_80153498(void*, int);
    void fn_80153610(void*, void*, u64, int, int, int);
    void __ct__7nteFileFQ25Heaps8HeapType(void*, int);
    void __dt__7nteFileFv(void*, int);
    void fn_80153EC4(void*, void*);
    int fn_80153810(void*);
    int getDataFileSize__7nteFileFv(void*);
    int addSection__7nteFileFP14nteSectionData(void*, int);
    void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUll(int, int, int);
    u64 OSGetTime(void);
    extern char lbl_80461718[];
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80155F54(void* arg0, void** arg1, int* arg2) {
    const char* base = lbl_80461718;
    int spC = 0;
    int sp8 = 0;
    char sp10[8];
    char sp18[0x14];
    char sp2C[0xB4];
    u8* inst;
    u16 lim;
    int type;
    void* heap;

    inst = (u8*)getInstance__8ipSwitchFv();
    if (((u8*)*(void**)(inst + 0x18))[0x3B] != 0) {
        netReport__FPCce(base + 0x408);
        return 0;
    }
    inst = (u8*)getInstance__8ipSwitchFv();
    getBufferData__13ipKeyRecorderFPPvPUl(*(void**)(inst + 0x18), (void**)&spC, &sp8);
    clearOnamae__13ipKeyRecorderFP14ipReplayHeader(spC);
    lim = (u16)((u8)getUploadSizeLimit__9nteGlobalFv() * 0xE10 + 0x168);
    if (*(u16*)(*(u8**)(*(u8**)((u8*)getInstance__8ipSwitchFv() + 0x18) + 0x24) + 6) > lim) {
        netReport__FPCce(base + 0x438);
        return 0;
    }
    __ct__17nteFileDataReplayFQ25Heaps8HeapType(sp18, 0x2A);
    setData__17nteFileDataReplayFPvUl(sp18, spC, sp8);
    fn_80153488(sp10);
    {
        u64 t = OSGetTime();
        fn_80153610(sp10, sp18, t, 0x2A, 0, 0);
    }
    __ct__7nteFileFQ25Heaps8HeapType(sp2C, 0x2A);
    if (addSection__7nteFileFP14nteSectionData(sp2C, fn_80153810(sp10)) == 0) {
        netReport__FPCce(base + 0x458);
        __dt__7nteFileFv(sp2C, -1);
        fn_80153498(sp10, -1);
        fn_80152BDC(sp18, -1);
        return 0;
    }
    type = getDataFileSize__7nteFileFv(sp2C);
    heap = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUll(0x2A, type, 0x20);
    if (heap == 0) {
        netReport__FPCce(base + 0x470);
        __dt__7nteFileFv(sp2C, -1);
        fn_80153498(sp10, -1);
        fn_80152BDC(sp18, -1);
        return 0;
    }
    fn_80153EC4(sp2C, heap);
    *arg2 = type;
    *arg1 = heap;
    __dt__7nteFileFv(sp2C, -1);
    fn_80153498(sp10, -1);
    fn_80152BDC(sp18, -1);
    return 1;
}
}

