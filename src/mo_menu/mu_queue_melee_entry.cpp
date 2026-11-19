#include <types.h>

// The entry screen of the queue melee (module sora_menu_qm). Only a part of its functions is decompiled.
/// Function names come from the sora_menu_qm map.

// HYPOTHESIS: the display of one player of the queue.
class muQueueMeleePlayer {
public:
    ~muQueueMeleePlayer();
};

muQueueMeleePlayer::~muQueueMeleePlayer() { }

// TODO: clean up, give variables real names
namespace jj_46E0 {
extern "C" {
void fn_800B96E8(void* p, int v);

void* fn_4_46E0(void* self) {
    fn_800B96E8((char*)self + 4, 0xff);
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_6844 {
typedef unsigned char u8;

class Counter {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void method(int);

    u8 pad[0xE0];
    int count;
};

class Entry2 {
    u8 pad[0x5C];

public:
    virtual void method(int);
};

extern "C" {
extern u8 lbl_4_data_1058[];

void free__13gfHeapManagerFPv(void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void* getBuffer__14gfFileIOHandleCFv(const void*);
void release__14gfFileIOHandleFv(void*);
void __dt__5MuMsgFv(void*, int);
void exit__6gfTaskFv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__18muQueueMeleePlayerFv(void*, int);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* __dt__21muQueueMeleeEntryTaskFv(u8* object, int deleting) {
    if (object) {
        *(void**)(object + 0x3C) = lbl_4_data_1058;

        Counter* obj = *(Counter**)(*(u8**)(object + 0x40) + 0x48);
        int n;
        while ((n = obj->count) != 0) {
            if (n != 0)
                obj->method(n - 1);
        }

        if (*(void**)(object + 0x1210))
            free__13gfHeapManagerFPv(*(void**)(object + 0x1210));
        if (*(void**)(object + 0x1214))
            free__13gfHeapManagerFPv(*(void**)(object + 0x1214));

        u8* cursor = object;
        for (unsigned int i = 0; i < 0x21; ++i, cursor += 4) {
            void* entry = *(void**)(cursor + 0x1308);
            if (entry) {
                Destroy__Q34nw4r3g3d6G3dObjFv(entry);
                *(void**)(cursor + 0x1308) = 0;
            }
        }

        unsigned int i2 = 0;
        u8* cursor2 = object;
        for (; i2 < 0x3C; ++i2, cursor2 += 4) {
            Entry2* entry = *(Entry2**)(cursor2 + 0x1218);
            if (entry) {
                if (entry) {
                    entry->method(1);
                }
                *(void**)(cursor2 + 0x1218) = 0;
            }
        }

        __dt__5MuMsgFv(*(void**)(object + 0x4C), 1);
        free__13gfHeapManagerFPv(getBuffer__14gfFileIOHandleCFv(object + 0x1394));
        release__14gfFileIOHandleFv(object + 0x1394);
        __dt__5MuMsgFv(*(void**)(object + 0x50), 1);
        exit__6gfTaskFv(*(void**)(object + 0x138C));
        *(void**)(object + 0x138C) = 0;
        exit__6gfTaskFv(*(void**)(object + 0x40));
        *(void**)(object + 0x40) = 0;

        __destroy_arr(object + 0x1394, __dt__14gfFileIOHandleFv, 4, 3);
        __destroy_arr(object + 0x54, __dt__18muQueueMeleePlayerFv, 0xE8, 0x10);
        __dt__6gfTaskFv(object, 0);

        if (deleting > 0)
            __dl__FPv(object);
    }

    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_8B84 {
extern "C" {
void* getInstance__16CameraControllerFv(void*);
void changeInput__16CameraControllerFQ216CameraController4Kind(void*, int);
int isReady__14gfFileIOHandleCFv(void*);
void* getBuffer__14gfFileIOHandleCFv(void*);
void Init__Q34nw4r3g3d7ResFileFPv(void*);
void release__14gfFileIOHandleFv(void*);
void playBGM__9sndSystemFiUlb(void*, int, unsigned long, int);
void endKeepScreen__17gfKeepFrameBufferFv(void*);
void initMdlData__21muQueueMeleeEntryTaskFv(void*, void*);
extern void* g_sndSystem;
extern char* g_gfApplication;
extern void* lbl_805A01D8;
}

extern "C" void fn_4_8B84(void* arg0) {
    char* self = (char*)arg0;
    char* p30;
    int* p29;
    char* p28;
    unsigned int idx;
    void* lbl;

    if (*(unsigned char*)(self + 0x1393) == 0) {
        void* obj = *(void**)(self + 0x40);
        if (*(unsigned char*)((char*)obj + 0x1A4) == 0) {
            return;
        }
        changeInput__16CameraControllerFQ216CameraController4Kind(getInstance__16CameraControllerFv(obj), 9);
        *(unsigned char*)(self + 0x1393) = 1;
    }

    p30 = self;
    p29 = (int*)(self + 0x1394);
    idx = 0;
    do {
        p28 = self + idx;
        if (*(unsigned char*)(p28 + 0x1390) == 0 && *(int*)(p30 + 0x1394) != 0) {
            if (isReady__14gfFileIOHandleCFv(p29) == 0) {
                return;
            }
            *(unsigned char*)(p28 + 0x1390) = 1;
        }
        p30 += 4;
        idx++;
        p29++;
    } while (idx < 3);

    *(void**)(self + 0x1210) = getBuffer__14gfFileIOHandleCFv(self + 0x1398);
    Init__Q34nw4r3g3d7ResFileFPv(self + 0x1210);
    release__14gfFileIOHandleFv(self + 0x1398);

    *(void**)(self + 0x1214) = getBuffer__14gfFileIOHandleCFv(self + 0x139C);
    Init__Q34nw4r3g3d7ResFileFPv(self + 0x1214);
    release__14gfFileIOHandleFv(self + 0x139C);

    *(unsigned char*)(self + 0x13A0) = 1;
    playBGM__9sndSystemFiUlb(g_sndSystem, 0x2701, 0, 1);
    lbl = lbl_805A01D8;
    *(int*)((char*)lbl + 0x174) = 0x2D;
    initMdlData__21muQueueMeleeEntryTaskFv(arg0, lbl);
    endKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
}
}

