#include <mu/mu_boot_network.h>
#include <stdio.h>
#include <types.h>
#include <nt/nt_etc_xchg.h>

void nteBootTask::notify_Error(int kind, int unk, int code) {
    char message[32];
    message[0] = '\0';
    if (code != 0) {
        sprintf(message, "ErrorCode:%d", code);
    }
}

void nteBootTask::startTask() {
    if (unk48 == NULL) {
        unk48 = new (unk44) nteExchange(unk44);
        if (unk48 == NULL) {
            m_finished = true;
        } else {
            unk5C = 1;
            m_finished = false;
        }
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_801585EC(void*);

extern "C" void fn_80158630(void* self) {
    fn_801585EC(reinterpret_cast<u8*>(self) - 0x40);
}

// TODO: clean up, give variables real names
namespace jj_801582A4 {
extern "C" {
void fn_8014E520(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
extern char lbl_80462130[];
}

extern "C" void* fn_801582A4(void* self, int flag) {
    if (self) {
        *(void**)((char*)self + 0x3C) = lbl_80462130;
        *(void**)((char*)self + 0x40) = lbl_80462130 + 0x64;
        void* h = *(void**)((char*)self + 0x48);
        if (h) {
            fn_8014E520(h, 1);
            *(void**)((char*)self + 0x48) = 0;
        }
        __dt__6gfTaskFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

namespace jj_8015820C {
extern "C" {
void* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(void*, const char*, int, int, int, bool);
extern char lbl_804620F8[];
extern char lbl_80462130[];
}

extern "C" void* fn_8015820C(void* self, int arg1) {
    unsigned char* p = (unsigned char*)self;
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(self, lbl_804620F8, 0x26, 0xE, 0xD, true);
    *(void**)(p + 0x3C) = lbl_80462130;
    *(void**)(p + 0x40) = lbl_80462130 + 0x64;
    *(int*)(p + 0x44) = arg1;
    *(int*)(p + 0x48) = 0;
    p[0x4C] = 0;
    *(int*)(p + 0x5C) = 0;
    *(int*)(p + 0x50) = 0;
    *(unsigned short*)(p + 0x54) = 0;
    *(unsigned short*)(p + 0x56) = 0;
    *(unsigned short*)(p + 0x58) = 0;
    p[0x2C] = p[0x2C] & ~2;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_801583AC {
struct Target {
    char pad[0x82];
    unsigned short a, b, c;
    char pad88[4];
    unsigned char ready;
    char pad8d[3];
    int result;
};
struct Self {
    char pad[0x48];
    Target* target;
    unsigned char done;
    char pad4d[3];
    int result;
    unsigned short a, b, c;
    char pad5a[2];
    unsigned int state;
};
extern "C" {
void fn_8014F490(Target*, int, int);
long long fn_8014CE68(Target*);
void fn_8014FEC8(long long);
int fn_8014D108(Target*, int);
void fn_801505B0(int);
void fn_801505DC(int);
int fn_800305B0(int);
int fn_80150594();
int fn_8014F584(Target*, int);
int fn_801505C0();
unsigned int fn_8014FDF0();
int fn_8014D794(Target*);
void fn_8014D854(Target*);
void fn_8014FE04(int);
void fn_8014DBDC(Target*);
}
extern "C" void fn_801583AC(Self* self) {
    switch (self->state) {
    case 0:
        break;
    case 1:
        fn_8014F490(self->target, 4, 0);
        self->state = 2;
        break;
    case 2:
        if (self->target->ready) {
            if (self->target->result == 0) {
                self->state = 3;
            } else {
                self->state = 0;
                self->done = 1;
            }
        }
        break;
    case 3:
        fn_8014FEC8(fn_8014CE68(self->target));
        fn_801505B0(fn_8014D108(self->target, 1));
        fn_801505DC(fn_8014D108(self->target, 2));
        self->state = 4;
        break;
    case 4:
        if (fn_800305B0(3) && fn_80150594() && fn_8014F584(self->target, 0)) {
            self->state = 5;
        } else {
            self->state = 6;
        }
        break;
    case 5:
        if (self->target->ready) {
            switch (self->target->result) {
            case 0: self->result = 0; break;
            case -1: self->result = 3; break;
            case -2: self->result = 1; break;
            case -3: self->result = 2; break;
            }
            if ((unsigned int)(self->result - 1) <= 1) {
                self->a = self->target->a;
                self->b = self->target->b;
                self->c = self->target->c;
            }
            self->state = 6;
        }
        break;
    case 6:
        if (fn_801505C0()) {
            if (fn_8014FDF0() == 1) {
                if (fn_8014D794(self->target)) {
                    fn_8014D854(self->target);
                } else {
                    fn_8014FE04(0);
                }
            } else if (fn_8014D794(self->target)) {
                fn_8014DBDC(self->target);
            }
        } else if (fn_8014FDF0() == 1) {
            fn_8014FE04(0);
        }
        self->state = 0;
        self->done = 1;
        break;
    }
}
}

