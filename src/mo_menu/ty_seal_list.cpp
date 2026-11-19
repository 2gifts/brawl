#include <types.h>

#include <mu/mu_msg.h>
#include <mu/menu.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_file_io_handle.h>
#include <stdio.h>
#include <revolution/OS/OSError.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
// TODO: clean up, give variables real names
namespace jj_EEBC {
inline void clearWords(int* p, int* e) { do { *p=0; ++p; } while (p<e); }
extern "C" {
void* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(void*, const char*, int, int, int, bool);
void fn_800B96E8(void*, int);
void fn_2_3BC(void*);
void __dt__14gfFileIOHandleFv(void*);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void* create__16muObjProcessTaskFQ25Heaps8HeapType(int);
void* create__8MenuRootFPCciPCc(const char*, int, const char*);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__21tySealListDataManagerFb(void*, int);
MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(unsigned long, HeapType, unsigned long);
extern const char lbl_2_data_1608[], lbl_2_data_1618[];
extern char lbl_2_data_18E8[];
extern const char* lbl_2_data_16C8[];
extern void* lbl_2_bss_40;
void* __ct__14tySealListTaskFv(void* self) {
    const char** files;
    char* b=(char*)self;
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(self,lbl_2_data_1608,0x12,0xe,6,true);
    *(int*)(b+0x44)=0; *(void**)(b+0x3c)=lbl_2_data_18E8;
    *(int*)(b+0x48)=0; *(int*)(b+0x58)=0; b[0x5c]=0;
    fn_800B96E8(b+0x60,0xff);
    b[0x5164]=0; *(int*)(b+0x52d4)=0; *(int*)(b+0x52d8)=0;
    clearWords((int*)(b+0x52d8),(int*)(b+0x5358));
    *(int*)(b+0x5358)=0;
    __construct_array(b+0x5364,fn_2_3BC,__dt__14gfFileIOHandleFv,4,4);
    __construct_array(b+0x5394,fn_2_3BC,__dt__14gfFileIOHandleFv,4,32);
    b[0x5414]=0; b[0x2c]&=~2; *(int*)(b+0x31f4)=0;
    char* q=b;
    for(int i=0;i<25;i++){
        *(int*)(q+0x4194)=0;
        *(int*)(q+0x4198)=0;
        *(int*)(q+0x419c)=0;
        *(int*)(q+0x41a0)=0;
        *(int*)(q+0x41a4)=0;
        *(int*)(q+0x41a8)=0;
        *(int*)(q+0x41ac)=0;
        *(int*)(q+0x41b0)=0;
        *(int*)(q+0x41b4)=0;
        *(int*)(q+0x41b8)=0;
        *(int*)(q+0x41bc)=0;
        *(int*)(q+0x41c0)=0;
        *(int*)(q+0x41c4)=0;
        *(int*)(q+0x41c8)=0;
        *(int*)(q+0x41cc)=0;
        *(int*)(q+0x41d0)=0;
        *(int*)(q+0x41d4)=0;
        *(int*)(q+0x41d8)=0;
        *(int*)(q+0x41dc)=0;
        *(int*)(q+0x41e0)=0;
        *(int*)(q+0x41e4)=0;
        *(int*)(q+0x41e8)=0;
        *(int*)(q+0x41ec)=0;
        *(int*)(q+0x41f0)=0;
        *(int*)(q+0x41f4)=0;
        *(int*)(q+0x41f8)=0;
        *(int*)(q+0x41fc)=0;
        *(int*)(q+0x4200)=0;
        *(int*)(q+0x4204)=0;
        *(int*)(q+0x4208)=0;
        *(int*)(q+0x420c)=0;
        *(int*)(q+0x4210)=0;
        *(int*)(q+0x4214)=0;
        *(int*)(q+0x4218)=0;
        *(int*)(q+0x421c)=0;
        *(int*)(q+0x4220)=0;
        *(int*)(q+0x4224)=0;
        *(int*)(q+0x4228)=0;
        *(int*)(q+0x422c)=0;
        *(int*)(q+0x4230)=0;
        q+=0xa0;
    }
    char* p=(char*)create__16muObjProcessTaskFQ25Heaps8HeapType(0x2a);
    *(void**)(b+0x52c4)=p; *(void**)(p+0x40)=b+0x5168; *(int*)(p+0x44)=0x55;
    *(void**)(b+0x40)=create__8MenuRootFPCciPCc(lbl_2_data_1608,0x55,lbl_2_data_1618);
    void* obj=__nw__FUlQ25Heaps8HeapType(0x3eb4,0x2a);
    if(obj) obj=__ct__21tySealListDataManagerFb(obj,1);
    *(void**)(b+0x4c)=obj;
    {
        char* base=b; int count=0;
        while(count<80){
            *(int*)(base+0x5168)=0; count+=40;
            *(int*)(base+0x516c)=0;
            *(int*)(base+0x5170)=0;
            *(int*)(base+0x5174)=0;
            *(int*)(base+0x5178)=0;
            *(int*)(base+0x517c)=0;
            *(int*)(base+0x5180)=0;
            *(int*)(base+0x5184)=0;
            *(int*)(base+0x5188)=0;
            *(int*)(base+0x518c)=0;
            *(int*)(base+0x5190)=0;
            *(int*)(base+0x5194)=0;
            *(int*)(base+0x5198)=0;
            *(int*)(base+0x519c)=0;
            *(int*)(base+0x51a0)=0;
            *(int*)(base+0x51a4)=0;
            *(int*)(base+0x51a8)=0;
            *(int*)(base+0x51ac)=0;
            *(int*)(base+0x51b0)=0;
            *(int*)(base+0x51b4)=0;
            *(int*)(base+0x51b8)=0;
            *(int*)(base+0x51bc)=0;
            *(int*)(base+0x51c0)=0;
            *(int*)(base+0x51c4)=0;
            *(int*)(base+0x51c8)=0;
            *(int*)(base+0x51cc)=0;
            *(int*)(base+0x51d0)=0;
            *(int*)(base+0x51d4)=0;
            *(int*)(base+0x51d8)=0;
            *(int*)(base+0x51dc)=0;
            *(int*)(base+0x51e0)=0;
            *(int*)(base+0x51e4)=0;
            *(int*)(base+0x51e8)=0;
            *(int*)(base+0x51ec)=0;
            *(int*)(base+0x51f0)=0;
            *(int*)(base+0x51f4)=0;
            *(int*)(base+0x51f8)=0;
            *(int*)(base+0x51fc)=0;
            *(int*)(base+0x5200)=0;
            *(int*)(base+0x5204)=0;
            base+=0xa0;
        }
        int* tail=(int*)b+count;
        files=lbl_2_data_16C8;
        tail[0x145a]=0;
        tail[0x145b]=0;
        tail[0x145c]=0;
        tail[0x145d]=0;
        tail[0x145e]=0;
    }
    *(int*)(b+0x52bc)=0; *(int*)(b+0x52c0)=0; *(int*)(b+0x52d4)=0;
    *(int*)(b+0x5358)=0; *(int*)(b+0x52d8)=0;
    for(int i=0;i<31;i++) ((int*)(b+0x52dc))[i]=0;
    for(int i=0;i<32;i++) b[0x5374+i]=1;
    for(int i=0;i<6;i++) b[0x535c+i]=0;
    const char* names[4]={files[0],files[1],files[2],files[3]};
    unsigned int index=0; const char** name=names; int offset=0;
    do{
        ((gfFileIOHandle*)(b+0x5364+offset))->readRequest(*name,(HeapType)0x2b,0,0);
        ++name; ++index; offset+=4;
    }while(index<4);
    muMenu::loadMenuSound();
    MuMsg* msg=create__5MuMsgFUlQ25Heaps8HeapTypeUl(1,(HeapType)0x2a,0x2b);
    *(MuMsg**)(b+0x52c8)=msg; msg->allocMsgBuf(0x100,10);
    msg=create__5MuMsgFUlQ25Heaps8HeapTypeUl(2,(HeapType)0x2a,0x2b);
    *(MuMsg**)(b+0x52d0)=msg; msg->allocMsgBuf(0x80,3);
    fn_800B96E8(b+0x60,0xf0);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
    lbl_2_bss_40=gfHeapManager::alloc((HeapType)0x2a,0x6400);
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_FB18 {
extern "C" {
void fn_800AEC70(void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void* getBuffer__14gfFileIOHandleCFv(void*);
void free__13gfHeapManagerFPv(void*);
void release__14gfFileIOHandleFv(void*);
void __dt__5MuMsgFv(void*, int);
void fn_800AD87C(void*, int);
void exit__6gfTaskFv(void*);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
extern char lbl_2_data_18E8[];
extern void* lbl_2_bss_40;
}

class gfPad5CProbe {
public:
    char pad[0x5C];
};

class gfVCallProbe : public gfPad5CProbe {
public:
    virtual void f(int);
};

extern "C" {

void* __dt__14tySealListTaskFv(void* self, int arg1) {
    char* s = (char*)self;

    if (self != 0) {
        unsigned int i;
        char* p;
        char* q;

        *(void**)(s + 0x3C) = lbl_2_data_18E8;
        fn_800AEC70(*(void**)(s + 0x4C));

        p = s;
        i = 0;
        do {
            void* o = *(void**)(p + 0x52BC);
            if (o != 0) {
                Destroy__Q34nw4r3g3d6G3dObjFv(o);
                *(void**)(p + 0x52BC) = 0;
            }
            i++;
            p += 4;
        } while (i < 2);

        p = s;
        i = 0;
        do {
            void* o = *(void**)(p + 0x5168);
            if (o != 0) {
                if (o != 0) {
                    ((gfVCallProbe*)o)->f(1);
                }
                *(void**)(p + 0x5168) = 0;
            }
            i++;
            p += 4;
        } while (i < 0x55);

        q = s + 0x5364;
        i = 0;
        do {
            void* b = getBuffer__14gfFileIOHandleCFv(q);
            free__13gfHeapManagerFPv(b);
            release__14gfFileIOHandleFv(q);
            i++;
            q += 4;
        } while (i < 4);

        p = s;
        i = 0;
        do {
            void* o = *(void**)(p + 0x52D8);
            if (o != 0) {
                free__13gfHeapManagerFPv(o);
                *(void**)(p + 0x52D8) = 0;
            }
            i++;
            p += 4;
        } while (i < 0x20);

        if (lbl_2_bss_40 != 0) {
            free__13gfHeapManagerFPv(lbl_2_bss_40);
            lbl_2_bss_40 = 0;
        }

        __dt__5MuMsgFv(*(void**)(s + 0x52C8), 1);
        __dt__5MuMsgFv(*(void**)(s + 0x52D0), 1);
        fn_800AD87C(*(void**)(s + 0x4C), 1);
        exit__6gfTaskFv(*(void**)(s + 0x52C4));
        *(void**)(s + 0x52C4) = 0;
        exit__6gfTaskFv(*(void**)(s + 0x40));
        *(void**)(s + 0x40) = 0;

        __destroy_arr(s + 0x5394, __dt__14gfFileIOHandleFv, 4, 0x20);
        __destroy_arr(s + 0x5364, __dt__14gfFileIOHandleFv, 4, 4);

        __dt__6gfTaskFv(s, 0);
        if (arg1 > 0) {
            __dl__FPv(s);
        }
    }

    return self;
}

}
}

// TODO: clean up, give variables real names
namespace jj_10EC0 {

extern "C" {
extern char lbl_2_data_1398[];
extern char* lbl_2_data_878[];

int numberPrint__14tySealListTaskFv(void* self, char* out, unsigned int value, int width) {
    char buffer[128];
    const char* formats = lbl_2_data_1398;
    int digits;
    unsigned int place;
    int result;
    unsigned int i;
    unsigned int count;
    result = 0;
    place = 1;

    sprintf(out, formats + 0x408);
    digits = 0;

loop:
    if (value >= place) {
        unsigned int next = place * 10;
        result = sprintf(buffer, formats + 0x40c,
                         lbl_2_data_878[(value % next) / place], out);
        sprintf(out, formats + 0x414, buffer);
        place = next;
        digits++;
        goto loop;
    }

    if (digits < width) {
        count = width - digits;
        for (i = 0; i < count; i++) {
            result = sprintf(buffer, formats + 0x40c, lbl_2_data_878[11], out);
            sprintf(out, formats + 0x414, buffer);
        }
    }

    return result;
}
}
}

// TODO: clean up, give variables real names
namespace jj_114FC {
class MuMsg;

struct Sub {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13(void* a);
};

struct Mid {
    char pad[0x48];
    Sub* sub;
};

struct Arg {
    char pad[0xc];
    void* val;
};

struct Self {
    char pad0[0x40];
    Mid* mid;
    char pad44[0x517c - 0x44];
    Arg* a517c[5];
    char pad5190[0x5198 - 0x5190];
    Arg* a5198;
    Arg* a519c[8];
    Arg* a51bc[32];
    Arg* a523c[32];
    Sub* s52bc;
    char pad52c0[0x52c8 - 0x52c0];
    MuMsg* msg52c8;
    char pad52cc[4];
    MuMsg* msg52d0;
    char pad52d4[4];
    void* buf52d8[32];
    char pad5358[0x5414 - 0x5358];
    unsigned char flag5414;
};

extern "C" void printf__5MuMsgFUlPCce(MuMsg*, unsigned long, const char*, ...);
extern "C" void free__13gfHeapManagerFPv(void*);
extern "C" char lbl_2_data_17A0[];

extern "C" void clearSealPage__14tySealListTaskFv(Self* self) {
    int i;
    for (i = 6; i < 10; i++) {
        printf__5MuMsgFUlPCce(self->msg52c8, i, lbl_2_data_17A0);
    }
    printf__5MuMsgFUlPCce(self->msg52d0, 1, lbl_2_data_17A0);
    printf__5MuMsgFUlPCce(self->msg52d0, 2, lbl_2_data_17A0);
    for (i = 0; i < 5; i++) {
        self->s52bc->f13(self->a517c[i]->val);
    }
    for (i = 0; i < 32; i++) {
        self->mid->sub->f13(self->a51bc[i]->val);
        if (self->buf52d8[i]) {
            free__13gfHeapManagerFPv(self->buf52d8[i]);
            self->buf52d8[i] = 0;
        }
        self->mid->sub->f13(self->a523c[i]->val);
    }
    for (i = 0; i < 8; i++) {
        self->mid->sub->f13(self->a519c[i]->val);
    }
    self->mid->sub->f13(self->a5198->val);
    printf__5MuMsgFUlPCce(self->msg52d0, 0, lbl_2_data_17A0);
    self->flag5414 = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_116AC {
extern "C" u32 getSealList__21tySealListDataManagerFUlUlPPv(s32*, u32, u32, s32*);
extern "C" char lbl_2_data_17BC[];
extern "C" char lbl_2_data_17C8[];
struct Entry { char pad[8]; const char* name; };
struct Object {
 char pad0[0x4c]; s32* source; char pad50[0xb8]; u32 index;
 Entry* entries[0xc3a]; u32 ids[0x3e8]; u32 counts[0x478];
 u8 flags[0x20]; gfFileIOHandle handles[0x20]; u8 loaded;
};
extern "C" void loadPage__14tySealListTaskFv(Object* obj) {
 char path[64];
 if (obj->loaded) { OSReport(lbl_2_data_17BC); return; }
 getSealList__21tySealListDataManagerFUlUlPPv(obj->source, 0x20, obj->ids[obj->index], (s32*)obj->entries);
 unsigned n = 0;
 unsigned j;
 unsigned k = 0;
 for (; n < 2; n++) {
  for (j = 0; j < obj->counts[obj->index + n]; j++) {
   sprintf(path, lbl_2_data_17C8, obj->entries[k]->name);
   obj->handles[k].readRequest(path, (HeapType)0x2b, 0, 0);
   obj->flags[k] = 0;
   k++;
  }
 }
 obj->loaded = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_12C6C {

extern "C" void fn_800B9984(void*);

struct MenuMain {
    char padding[0x68];
    unsigned int current;
    unsigned int unknown;
    unsigned int pressed;
};

extern "C" void ctrlSetup__14tySealListTaskFv(MenuMain* self, gfPadStatus* output) {
    fn_800B9984((char*)self + 0x60);
    output->m_buttonsPressedThisFrame2.bits |= self->pressed;
    output->m_buttonsCurrentFrame2.bits |= self->current;

    for (int i = 0; i < 8; ++i) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error < 0)
            continue;

        switch (status.m_controllerType) {
        case gfPadType::WIIMOTE:
            if (status.m_buttonsCurrentFrame2.bits & 0x400000) {
                output->m_buttonsCurrentFrame2.bits = 0;
                output->m_buttonsPressedThisFrame2.bits = 0;

                if (status.m_buttonsPressedThisFrame.bits & 0x200)
                    output->m_buttonsPressedThisFrame.bits ^= 0x200;

                if ((status.m_buttonsPressedThisFrame2.bits & 0x10000) ||
                    (status.m_buttonsPressedThisFrame2.bits & 1)) {
                    output->m_buttonsPressedThisFrame2.bits |= 0x40;
                } else if ((status.m_buttonsPressedThisFrame2.bits & 0x20000) ||
                           (status.m_buttonsPressedThisFrame2.bits & 2)) {
                    output->m_buttonsPressedThisFrame2.bits |= 0x20;
                }

                if ((status.m_buttonsCurrentFrame2.bits & 0x10000) ||
                    (status.m_buttonsCurrentFrame2.bits & 1)) {
                    output->m_buttonsCurrentFrame2.bits |= 0x40;
                } else if ((status.m_buttonsCurrentFrame2.bits & 0x20000) ||
                           (status.m_buttonsCurrentFrame2.bits & 2)) {
                    output->m_buttonsCurrentFrame2.bits |= 0x20;
                }
            }
            break;

        case gfPadType::NUNCHUK:
            if (status.m_buttonsCurrentFrame2.bits & 0x40000)
                output->m_buttonsPressedThisFrame2.bits &= ~0x400;

            if ((status.m_buttonsPressedThisFrame2.bits & 0x10000) ||
                (status.m_buttonsPressedThisFrame2.bits & 0x80000)) {
                output->m_buttonsPressedThisFrame2.bits |= 0x40;
            } else if ((status.m_buttonsPressedThisFrame2.bits & 0x20000) ||
                       (status.m_buttonsPressedThisFrame2.bits & 0x1000)) {
                output->m_buttonsPressedThisFrame2.bits |= 0x20;
            }

            if ((status.m_buttonsCurrentFrame2.bits & 0x10000) ||
                (status.m_buttonsCurrentFrame2.bits & 0x80000)) {
                output->m_buttonsCurrentFrame2.bits |= 0x40;
            } else if ((status.m_buttonsCurrentFrame2.bits & 0x20000) ||
                       (status.m_buttonsCurrentFrame2.bits & 0x1000)) {
                output->m_buttonsCurrentFrame2.bits |= 0x20;
            }
            break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_12E60 {

struct MenuMain {
    char pad0[0x108];
    int unk108;
    char pad10C[0x200];
    unsigned int unk30C;
    unsigned char b310;
    char pad311[0x5164 - 0x311];
    unsigned char b5164;
};

extern "C" void ctrlSetup__14tySealListTaskFv(MenuMain* self, gfPadStatus* output);

extern "C" int isSkipOff__14tySealListTaskFv(MenuMain* self) {
    gfPadStatus status;
    if (self->b5164 == 0) {
        return 0;
    }
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    ctrlSetup__14tySealListTaskFv(self, &status);
    int v = self->unk108;
    if (v == 0) {
        goto ret1;
    }
    if (self->b310 != 0 && self->unk30C <= (unsigned)(v + 2)) {
    ret1:
        return 1;
    }
    if (status.m_buttonsCurrentFrame2.bits & 0x20) {
        return 0;
    }
    if (status.m_buttonsCurrentFrame2.bits & 0x40) {
        return 0;
    }
    if (status.m_buttonsCurrentFrame2.bits & 0x400) {
        return 0;
    }
    return (status.m_buttonsCurrentFrame2.bits & 0x800) == 0;
}
}

