#include <types.h>

// TODO: clean up, give variables real names
namespace jj_12F48 {
extern "C" void setPanelTask__10muProcMenuFP15muPanelCtrlTask(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x658) = value;
}
}

// TODO: clean up, give variables real names
namespace jj_12F50 {
extern "C" void init__10muProcMenuFPQ34nw4r3g3d8ScnGroupi(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x654) = value;
}
}

// TODO: clean up, give variables real names
namespace jj_12F58 {
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void vfn(void*, void*);
};

extern "C" void addCommonObject__10muMenuMainFv(void* self) {
    char* s = (char*)self;
    void* a = *(void**)(s + 0x7b4);
    void** p = (void**)((char*)*(void**)(s + 0x7bc) + 0x64);
    Obj* obj = *(Obj**)((char*)a + 0x48);
    for (int i = 0; i < 3; i++) {
        obj->vfn(*(void**)((char*)obj + 0xe4), *(void**)((char*)*p + 0x10));
        p++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_12FD0 {
extern "C" s32 proc__10muProcMenuFv(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_12FD8 {
extern "C" void exit__10muProcMenuFv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_12FDC {
extern "C" {
extern void* g_sndSystem;
extern char* g_gfApplication;
extern unsigned char lbl_2_rodata_4F0[];

void stopBGM__9sndSystemFi(void*, int);
unsigned char* getInstance__13gfFrameBufferFv();
char* getInstance__14gfSceneManagerFv();
void startKeepScreen__17gfKeepFrameBufferFv(void*);
}

extern "C" void exitScene__10muMenuMainFv(char* self, int idx) {
    switch (lbl_2_rodata_4F0[idx]) {
    case 0:
        break;
    case 1:
        stopBGM__9sndSystemFi(g_sndSystem, 0);
        break;
    case 2:
        stopBGM__9sndSystemFi(g_sndSystem, 0x1E);
        break;
    }

    unsigned char b28 = self[0xABF];
    unsigned char b29 = self[0xABE];
    unsigned char b30 = self[0xABD];
    unsigned char b31 = self[0xABC];

    unsigned char* mgr = getInstance__13gfFrameBufferFv();
    mgr[0x2C] = b31;
    mgr[0x2D] = b30;
    mgr[0x2E] = b29;
    mgr[0x2F] = b28;

    char* inst = getInstance__14gfSceneManagerFv();
    *(int*)(inst + 0x284) = idx;
    *(int*)(inst + 0x288) = 2;

    startKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
}
}

