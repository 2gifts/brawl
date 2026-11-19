#include <ip/ip_switch.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <nt/nt_report.h>
#include <revolution/OS/OSError.h>
#include <st/operator/st_operator_network.h>
#include <types.h>
#include <ip/ip_network_producer.h>

extern "C" {
// HYPOTHESIS: muWifiInterfaceTask::getNetworkError (map name), the error code with its detail.
int fn_800CBCB0(muWifiInterfaceTask* task, int* detail);
}

// HYPOTHESIS: the operator that receives the network stop and exit callbacks.
static stOperatorNetwork* unk_bss_operator;

stOperatorNetwork::~stOperatorNetwork() {
}

bool NtBase::isNetCondOnline() {
    return gUnk805a03d8 >= 3 && gUnk805a03d8 <= 6;
}

void netReport(const char* fmt, ...) {
}

extern int lbl_27_data_52B78;
extern char lbl_27_data_52E10[0x28];

void stOperatorNetworkMelee::setPrepareLoadStageComplete() {
    unk6C = lbl_27_data_52B78;
    unk70 = 0;
    if (m_isOnline) {
        muWifiInterfaceTask* task = g_muWifiInterfaceTask;
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        task->m_aidInfos[aid].unk56_b6 = true;
        netReport(lbl_27_data_52E10);
        task->sendAIDInfo(false, 0xFE);
        ipNetworkProducer* producer = ipNetworkProducer::getInstance();
        producer->unk0 = 0;
    }
}

void stNetworkOperatorStopCallback(u16 frame, u8 result) {
    if (unk_bss_operator != NULL) {
        unk_bss_operator->stopControllerCompleteCallback(result);
    }
}

void stNetworkOperatorAbortCallback(u8 aid, u16 frame, u8 result) {
    if (unk_bss_operator != NULL) {
        unk_bss_operator->abortControllerCompleteCallback(aid, result);
    }
}

void stOperatorNetworkMelee::processBegin() {
    if (m_isOnline) {
        unk64 = fn_800CBCB0(g_muWifiInterfaceTask, &unk68);
        if (unk64 != 0) {
            unk48 = false;
            m_isOnline = false;
            if (unk64 <= 6) {
                m_errorValue = 2;
                OSReport("stOperatorNetworkMelee::processBegin() ERROR_CODE_FATAL !!!");
            } else {
                m_errorValue = 1;
                OSReport("stOperatorNetworkMelee::processBegin() ERROR_CODE_MATCHING_BROKEN !!!");
            }
            NtShared::ntSetStopCallback(NULL);
            NtShared::ntSetExitCallback(NULL);
        }
    }
}

void stOperatorNetworkMelee::checkNoContest() {
    if (!m_isNoContest && m_isOnline && unk_bss_operator != NULL) {
        m_isNoContest = unk5C == 2;
    }
}

void stOperatorNetworkMelee::setNullPadForNetworkOut(bool isNull) {
    ipSwitch::getInstance()->m_flags.nullNetworkOutput = isNull;
}

bool NtBase::isParent() {
    return lbl_8059E742 == lbl_8059E745 && lbl_8059E742 != 0xFF;
}

bool stOperatorNetworkMelee::isSync() {
    if (m_isOnline) {
        if (NtBase::isParent()) {
            NtShared::ntStartSync(g_Network.unk34);
            return true;
        }
        bool result = false;
        NetworkUserData* data = g_Network.unk34;
        if (data->unkA8 == 6) {
            if (data->unk1E0 == 0) {
                result = true;
            }
        }
        return result;
    }
    return true;
}

void stOperatorNetworkMelee::startController() {
    if (m_isOnline) {
        ipSwitch::getInstance()->setSource(1);
        unk_bss_operator = this;
        NtShared::ntSetStopCallback(stNetworkOperatorStopCallback);
        NtShared::ntSetExitCallback(stNetworkOperatorAbortCallback);
    }
}

void stOperatorNetworkMelee::stopControllerAsync(int arg) {
    if (m_isOnline) {
        if (unk_bss_operator != NULL) {
            unk54 = arg;
            if (!NtShared::ntRequestStop(g_Network.unk34, arg)) {
                unk5C = arg;
                unk4A = true;
                NtShared::ntSetStopCallback(NULL);
            }
        }
    }
}

bool stOperatorNetworkMelee::isStopControllerComplete() {
    if (m_isOnline) {
        if (unk_bss_operator != NULL) {
            if (!unk4A) {
                return false;
            }
            NtShared::ntSetStopCallback(NULL);
            unk48 = false;
            m_isOnline = false;
        }
    }
    ipSwitch::getInstance()->setSource(0);
    return true;
}

void stOperatorNetworkMelee::abortControllerAsync(int arg) {
    if (m_isOnline) {
        if (unk_bss_operator != NULL) {
            unk58 = arg;
            if (!NtShared::ntRequestExit(g_Network.unk34, arg)) {
                unk58 = arg;
                unk4B = true;
                NtShared::ntSetExitCallback(NULL);
            }
        }
    }
}

bool stOperatorNetworkMelee::isAbortControllerComplete() {
    if (m_isOnline) {
        if (unk_bss_operator != NULL) {
            checkNoContest();
            if (!unk4B && !m_isNoContest) {
                return false;
            }
            NtShared::ntSetExitCallback(NULL);
            unk48 = false;
            m_isOnline = false;
        }
    }
    ipSwitch::getInstance()->setSource(0);
    return true;
}

void NtShared::ntSetStopCallback(ntStopCallback_t cb) {
    NtStopCallback = cb;
}

void NtShared::ntSetExitCallback(ntExitCallback_t cb) {
    NtExitCallback = cb;
}

// TODO: clean up, give variables real names
namespace jj_254AC8 {
extern "C" {
void __ct__10stOperatorFPCc(void*, const char*);
int isNetCondOnline__6NtBaseFv();
extern char lbl_27_data_52B7C[];
extern char lbl_27_data_52C28[];
extern int lbl_27_data_52B78;
extern char g_Network[];
extern void* g_muWifiInterfaceTask;
extern int lbl_27_bss_59C0;
}

extern "C" void* fn_27_254AC8(void* p) {
    char* self = (char*)p;
    __ct__10stOperatorFPCc(p, lbl_27_data_52B7C);
    *(void**)(self + 0x3C) = lbl_27_data_52C28;
    self[0x48] = 0;
    self[0x49] = 0;
    self[0x4A] = 0;
    self[0x4B] = 0;
    self[0x4C] = 0;
    self[0x4D] = 0;
    *(int*)(self + 0x50) = 0;
    *(int*)(self + 0x54) = 0;
    *(int*)(self + 0x58) = 0;
    *(int*)(self + 0x5C) = 0;
    *(int*)(self + 0x60) = 0;
    *(int*)(self + 0x64) = 0;
    *(int*)(self + 0x68) = 0;
    *(int*)(self + 0x6C) = lbl_27_data_52B78;
    *(int*)(self + 0x70) = 0;

    bool online = g_Network != 0 && g_muWifiInterfaceTask != 0;
    self[0x48] = online;
    bool active = online && isNetCondOnline__6NtBaseFv() != 0;
    self[0x49] = active;

    *(int*)(self + 0x6C) = lbl_27_data_52B78;
    *(int*)(self + 0x70) = 0;
    lbl_27_bss_59C0 = 0;
    self[0x2C] &= ~2;
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_254C44 {
extern "C" {
extern unsigned int lbl_27_bss_59C0;
extern char g_Network[];
extern char lbl_27_data_52C28[];

void ntSetStopCallback__8NtSharedFPFUsUc_v(int);
void ntSetExitCallback__8NtSharedFPFUcUsUc_v(void (*)(void*));
void* __dt__10stOperatorFv(void*, int);
void __dl__FPv(void*);

void* fn_27_254C44(void* self, int flag) {
    if (self) {
        *(void**)((char*)self + 0x3C) = lbl_27_data_52C28;
        lbl_27_bss_59C0 = 0;
        if (g_Network) {
            ntSetStopCallback__8NtSharedFPFUsUc_v(0);
            ntSetExitCallback__8NtSharedFPFUcUsUc_v(0);
        }
        if (self)
            __dt__10stOperatorFv(self, 0);
        if (flag > 0)
            __dl__FPv(self);
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_254CF4 {
extern "C" void ntSetStopCallback__8NtSharedFPFUsUc_v(int);
extern "C" void ntSetExitCallback__8NtSharedFPFUcUsUc_v(void (*)(void*));
extern "C" int fn_800CBCB0(int, int*);
extern "C" int g_muWifiInterfaceTask;
extern "C" char lbl_27_data_52B94[];
extern "C" char lbl_27_data_52BD0[];
extern "C" void OSReport(const char* fmt, ...);

extern "C" void fn_27_254CF4(void* arg0) {
    unsigned char* self = (unsigned char*)arg0;
    int result;

    if (self[0x49] != 0) {
        result = fn_800CBCB0(g_muWifiInterfaceTask, (int*)(self + 0x68));
        *(int*)(self + 0x64) = result;
        if (result != 0) {
            self[0x48] = 0;
            self[0x49] = 0;
            if (result <= 6) {
                *(int*)(self + 0x50) = 2;
                OSReport(lbl_27_data_52B94);
            } else {
                *(int*)(self + 0x50) = 1;
                OSReport(lbl_27_data_52BD0);
            }
            ntSetStopCallback__8NtSharedFPFUsUc_v(0);
            ntSetExitCallback__8NtSharedFPFUcUsUc_v(0);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_254ED0 {
extern "C" {
int isPreparedLoadAllStage__19muWifiInterfaceTaskFv(void*);
int testSendAIDInfoDone__19muWifiInterfaceTaskFv(void*);
void fn_8013FDD4(void*);
extern char lbl_27_data_52B78[];
extern char g_Network[];
extern void* g_muWifiInterfaceTask;
}

extern "C" int fn_27_254ED0(void* p) {
    unsigned char* self = (unsigned char*)p;
    char* d = lbl_27_data_52B78;
    if (self[0x49] != 0) {
        switch (*(int*)(self + 0x70)) {
        case 0:
            *(int*)(self + 0x6C) = *(int*)(d + 0xA0);
            *(int*)(self + 0x70) = 0x98;
            goto c1;
        l9c:
            *(int*)(self + 0x6C) = *(int*)(d + 0xA4);
            *(int*)(self + 0x70) = 0x9C;
            break;
        case 0x98:
        case 0x9C:
        c1:
            if (isPreparedLoadAllStage__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask) == 0) goto l9c;
            goto c2;
        la1:
            *(int*)(self + 0x6C) = *(int*)(d + 0xA8);
            *(int*)(self + 0x70) = 0xA1;
            break;
        case 0xA1:
        c2:
            if (testSendAIDInfoDone__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask) == 0) goto la1;
            fn_8013FDD4(*(void**)(g_Network + 0x34));
            *(int*)(self + 0x6C) = *(int*)(d + 0xAC);
            *(int*)(self + 0x70) = 0xA6;
            break;
        }
        return *(int*)(self + 0x6C) == 2;
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_2550F4 {

struct fn_27_flags {
    u8 _0 : 4;
    u8 m_bit : 1;
    u8 _1 : 3;
};

extern "C" void fn_27_2550F4(void* unused, u32 flag) {
    fn_27_flags* f = (fn_27_flags*)((u8*)ipSwitch::getInstance() + 0x24);
    f->m_bit = flag;
}
}

// TODO: clean up, give variables real names
namespace jj_255348 {
extern "C" unsigned char lbl_8059E742;

typedef unsigned char u8;
typedef unsigned int u32;

extern "C" void fn_27_255348(u8* self, u32 p4, u32 p5) {
    if (self[0x49] == 0) {
        return;
    }
    if (self[0x4B] != 0) {
        return;
    }
    if (p4 != lbl_8059E742) {
        return;
    }
    *(u32*)(self + 0x60) = p5;
    self[0x4B] = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_2553C8 {
extern "C" void fn_27_2553C8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2553CC {
extern "C" s32 fn_27_2553CC(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2553D4 {
extern "C" s32 fn_27_2553D4(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2553DC {
extern "C" s32 fn_27_2553DC(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2553E4 {
extern "C" s32 fn_27_2553E4(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2553EC {
extern "C" void fn_27_2553EC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2553F0 {
extern "C" s32 fn_27_2553F0(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_2553F8 {
extern "C" void fn_27_2553F8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2553FC {
extern "C" s32 fn_27_2553FC(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_255404 {
extern "C" void fn_27_255404(void) {}
}

// TODO: clean up, give variables real names
namespace jj_255408 {
extern "C" s32 fn_27_255408(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_255410 {
extern "C" s32 fn_27_255410(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_255418 {
extern "C" void fn_27_255418(void) {}
}

// TODO: clean up, give variables real names
namespace jj_25541C {
extern "C" void fn_27_25541C(void) {}
}

