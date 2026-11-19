#include <types.h>

// TODO: clean up, give variables real names
namespace jj_350 {
extern "C" void fn_4_350(int* p) {
    *p = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_35C {
extern "C" {
extern char lbl_4_data_560[];
extern char lbl_4_data_568[];
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
}
struct Scene {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void remove(int);
    char pad[0xE0];
    int count;
};
struct Object { char pad[0xc]; void* model; };
struct Menu {
    char pad[0x2d4];
    Object* a;
    Object* b;
    Object* c;
    Object* d;
    char pad2[8];
    Object* e;
    char pad3[0x68];
    Scene* scene;
    void* x;
    void* y;
    void* z;
};
extern "C" void pushbackPanel__20muQueueMeleeRuleTaskFv(Menu* self, int mode) {
    switch (mode) {
    case 1: {
        Scene* scene = self->scene;
        while (scene->count != 0) {
            if (scene->count != 0) scene->remove(scene->count - 1);
        }
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->a->model, lbl_4_data_560);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->y, lbl_4_data_568);
        break;
    }
    case 2: {
        Scene* scene = self->scene;
        while (scene->count != 0) {
            if (scene->count != 0) scene->remove(scene->count - 1);
        }
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->b->model, lbl_4_data_560);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->c->model, lbl_4_data_560);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->y, lbl_4_data_568);
        break;
    }
    case 3: {
        Scene* scene = self->scene;
        while (scene->count != 0) {
            if (scene->count != 0) scene->remove(scene->count - 1);
        }
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->b->model, lbl_4_data_560);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->c->model, lbl_4_data_560);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->d->model, lbl_4_data_568);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->x, lbl_4_data_568);
        break;
    }
    case 4: {
        Scene* scene = self->scene;
        while (scene->count != 0) {
            if (scene->count != 0) scene->remove(scene->count - 1);
        }
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->e->model, lbl_4_data_560);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scene, self->z, lbl_4_data_568);
        break;
    }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2128 {
typedef unsigned char u8;

class MenuObject {
    u8 padding[0x5C];
public:
    virtual void method(int);
};

class Obj3B0 {
    u8 padding[0x3C];
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
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
};

extern "C" {
extern u8 lbl_4_data_774[];

void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void free__13gfHeapManagerFPv(void*);
void* getBuffer__14gfFileIOHandleCFv(const void*);
void release__14gfFileIOHandleFv(void*);
void __dt__5MuMsgFv(void*, int);
void exit__6gfTaskFv(void*);
void __dt__9gfArchiveFv(void*, int);
void __dt__14gfFileIOHandleFv(void*, int);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* __dt__20muQueueMeleeRuleTaskFv(u8* object, int deleting) {
    if (object) {
        *reinterpret_cast<void**>(object + 0x3C) = lbl_4_data_774;

        u8* cursor = object;
        for (unsigned int i = 0; i < 6; ++i, cursor += 4) {
            void* entry = *reinterpret_cast<void**>(cursor + 0x358);
            if (entry) {
                Destroy__Q34nw4r3g3d6G3dObjFv(entry);
                *reinterpret_cast<void**>(cursor + 0x358) = 0;
            }
        }

        cursor = object;
        for (unsigned int i = 0; i < 0x23; ++i, cursor += 4) {
            MenuObject* entry = *reinterpret_cast<MenuObject**>(cursor + 0x2CC);
            if (entry) {
                if (entry) {
                    entry->method(1);
                }
                *reinterpret_cast<void**>(cursor + 0x2CC) = 0;
            }
        }

        __dt__9gfArchiveFv(*(void**)(object + 0x3B4), 1);
        __dt__9gfArchiveFv(*(void**)(object + 0x3B8), 1);

        free__13gfHeapManagerFPv(getBuffer__14gfFileIOHandleCFv(object + 0x3C4));
        release__14gfFileIOHandleFv(object + 0x3C4);
        free__13gfHeapManagerFPv(getBuffer__14gfFileIOHandleCFv(object + 0x3C8));
        release__14gfFileIOHandleFv(object + 0x3C8);
        free__13gfHeapManagerFPv(getBuffer__14gfFileIOHandleCFv(object + 0x3CC));
        release__14gfFileIOHandleFv(object + 0x3CC);
        free__13gfHeapManagerFPv(getBuffer__14gfFileIOHandleCFv(object + 0x3D0));
        release__14gfFileIOHandleFv(object + 0x3D0);

        __dt__5MuMsgFv(*(void**)(object + 0xFC), 1);
        __dt__5MuMsgFv(*(void**)(object + 0x100), 1);

        exit__6gfTaskFv(*(void**)(object + 0x370));
        *(void**)(object + 0x370) = 0;
        exit__6gfTaskFv(*(void**)(object + 0x40));
        *(void**)(object + 0x40) = 0;

        void* temp398 = *(void**)(object + 0x398);
        if (temp398) {
            exit__6gfTaskFv(temp398);
            *(void**)(object + 0x398) = 0;
        }

        Obj3B0* obj3B0 = *(Obj3B0**)(object + 0x3B0);
        if (obj3B0) {
            obj3B0->vf23();
            *(void**)(object + 0x3B0) = 0;
        }

        void* temp394 = *(void**)(object + 0x394);
        if (temp394) {
            exit__6gfTaskFv(temp394);
            *(void**)(object + 0x394) = 0;
        }

        __destroy_arr(object + 0x3C4, __dt__14gfFileIOHandleFv, 4, 4);
        __dt__6gfTaskFv(object, 0);

        if (deleting > 0)
            __dl__FPv(object);
    }
    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_32B0 {
extern "C" {
void* getMsgData__5MuMsgFUlPPv(void* self, unsigned long index, void** size);
void* memcpy(void* dest, const void* src, unsigned long size);
void* memset(void* dest, int c, unsigned long n);
void printData__5MuMsgFUlPCvi(void* msg, unsigned long index, char* buffer, unsigned long len);
}

struct Obj {
    char pad[0xFC];
    void* msg;
};

extern "C" void setRulePlayerNum__20muQueueMeleeRuleTaskFv(Obj* self, unsigned long a, int b, int c, int d)
{
    char buf[0x80];
    unsigned long size;
    unsigned long pos;
    void* data;

    memset(buf, 0, 0x80);
    data = getMsgData__5MuMsgFUlPPv(self->msg, c + 0x1F, (void**)&size);
    memcpy(buf, data, size);
    pos = size;
    data = getMsgData__5MuMsgFUlPPv(self->msg, 0x2B, (void**)&size);
    memcpy(buf + pos, data, size);
    pos += size;
    if (d >= 10) {
        data = getMsgData__5MuMsgFUlPPv(self->msg, d / 10 + 0x1F, (void**)&size);
        memcpy(buf + pos, data, size);
        pos += size;
    }
    data = getMsgData__5MuMsgFUlPPv(self->msg, d % 10 + 0x1F, (void**)&size);
    memcpy(buf + pos, data, size);
    pos += size;
    if (b == 1) {
        data = getMsgData__5MuMsgFUlPPv(self->msg, 0x12, (void**)&size);
    } else {
        data = getMsgData__5MuMsgFUlPPv(self->msg, 0x13, (void**)&size);
    }
    memcpy(buf + pos, data, size);
    printData__5MuMsgFUlPCvi(self->msg, a, buf, pos + size);
}
}

