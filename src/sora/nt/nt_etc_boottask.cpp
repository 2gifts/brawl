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
extern "C" void notify_Error__11nteBootTaskFiii(void*);

extern "C" void fn_80158630(void* self) {
    notify_Error__11nteBootTaskFiii(reinterpret_cast<u8*>(self) - 0x40);
}

// TODO: clean up, give variables real names
namespace jj_801582A4 {
extern "C" {
void __dt__11nteExchangeFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
extern char lbl_80462130[];
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_801582A4(void* self, int flag) {
    if (self) {
        *(void**)((char*)self + 0x3C) = lbl_80462130;
        *(void**)((char*)self + 0x40) = lbl_80462130 + 0x64;
        void* h = *(void**)((char*)self + 0x48);
        if (h) {
            __dt__11nteExchangeFv(h, 1);
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

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
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
void _requestJob__11nteExchangeFii(Target*, int, int);
long long getMyWiiID__15nteWiiConnect24Fv(Target*);
void setWiiID__9nteGlobalFUx(long long);
int check_getStatus__15nteWiiConnect24Fi(Target*, int);
void setEnableNWCMail__9nteGlobalFb(int);
void setEnableNWCDL__9nteGlobalFb(int);
int isEnable__14gfSysRecorder2Fi(int);
int isEnableNWCMail__9nteGlobalFv();
int fn_8014F584(Target*, int);
int isEnableNWCDL__9nteGlobalFv();
unsigned int isPermitService__9nteGlobalFv();
int dl_isRegisterDLTask__15nteWiiConnect24Fv(Target*);
void dl_registerDLTask__15nteWiiConnect24Fv(Target*);
void setPermitService__9nteGlobalFb(int);
void dl_deleteDLTask__15nteWiiConnect24Fv(Target*);
}
// TODO: clean up, give variables real names
extern "C" void fn_801583AC(Self* self) {
    switch (self->state) {
    case 0:
        break;
    case 1:
        _requestJob__11nteExchangeFii(self->target, 4, 0);
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
        setWiiID__9nteGlobalFUx(getMyWiiID__15nteWiiConnect24Fv(self->target));
        setEnableNWCMail__9nteGlobalFb(check_getStatus__15nteWiiConnect24Fi(self->target, 1));
        setEnableNWCDL__9nteGlobalFb(check_getStatus__15nteWiiConnect24Fi(self->target, 2));
        self->state = 4;
        break;
    case 4:
        if (isEnable__14gfSysRecorder2Fi(3) && isEnableNWCMail__9nteGlobalFv() && fn_8014F584(self->target, 0)) {
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
        if (isEnableNWCDL__9nteGlobalFv()) {
            if (isPermitService__9nteGlobalFv() == 1) {
                if (dl_isRegisterDLTask__15nteWiiConnect24Fv(self->target)) {
                    dl_registerDLTask__15nteWiiConnect24Fv(self->target);
                } else {
                    setPermitService__9nteGlobalFb(0);
                }
            } else if (dl_isRegisterDLTask__15nteWiiConnect24Fv(self->target)) {
                dl_deleteDLTask__15nteWiiConnect24Fv(self->target);
            }
        } else if (isPermitService__9nteGlobalFv() == 1) {
            setPermitService__9nteGlobalFb(0);
        }
        self->state = 0;
        self->done = 1;
        break;
    }
}
}

