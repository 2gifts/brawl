#include <types.h>

#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <nt/nt_offline.h>

extern "C" void fn_800CF22C(muWifiInterfaceTask* self);
extern "C" void fn_801473C4(muWifiInterfaceTask* self);
extern "C" void fn_80145D24(s32 arg0, void* arg1, s32 arg2, s32 arg3, s32 arg4);

struct muWifiInterfaceTaskFields {
    char pad0[0x44];
    s32 unk44;
    s8 unk48;
    char pad49[0x27];
    char unk70[0xF5A];
    u8 unkFCA;
    char padFCB[0x7D];
    s32 unk1048;
    s32 unk104C;
};

struct NetworkFields {
    char pad0[0x38];
    s32 unk38;
};

struct ntFriendInfoFields {
    char pad0[0x608];
    u8 unk608;
};

void muWifiInterfaceTask::connect(bool arg0) {
    // MATCH-ONLY: declaration order picks the register assignment
    s32 offset;
    Network* network;
    s32 i;
    muWifiInterfaceTaskFields* fields = reinterpret_cast<muWifiInterfaceTaskFields*>(this);
    fields->unk48 = arg0;
    fields->unk44 = 1;
    fields->unk1048 = 0;
    fields->unk104C = 0;
    fn_801473C4(this);
    network = &g_Network;
    fields->unkFCA &= 0xFFFFFFEF;
    network->create(true);
    i = 0;
    offset = 0;
    do {
        // MATCH-ONLY: char* arithmetic keeps the loaded value as the left add operand
        ntFriendInfoFields* entry = reinterpret_cast<ntFriendInfoFields*>(reinterpret_cast<char*>(*reinterpret_cast<s32*>(ntFriendInfo::getInstance())) + offset);
        if (entry->unk608 != 0) {
            entry->unk608 = 2;
        }
        i += 1;
        offset += 0x89;
    } while (i < 0x40);
    fn_80145D24(reinterpret_cast<NetworkFields*>(network)->unk38, fields->unk70, 0x218, 0x138, 4);
    fn_800CF22C(this);
}

void netReport(const char* fmt, ...) {
}

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern "C" s32 fn_800C9A4C(u32 arg0) {
    if ((arg0 >= 0x4E86U && arg0 <= 0x4E8DU) || (arg0 >= 0x4E8FU && arg0 <= 0x5207U)) {
        return 1;
    }
    if (arg0 == 0x4E85U || (arg0 >= 0x59D8U && arg0 <= 0x5DBFU)) {
        return 2;
    }
    if (arg0 == 0x4E8EU) {
        return 3;
    }
    if (arg0 == 0x7148U) {
        return 7;
    }
    if (arg0 == 0x7149U) {
        return 8;
    }
    if (arg0 == 0x4E84U || (arg0 >= 0xC350U && arg0 <= 0xEA5FU)) {
        return 4;
    }
    if ((arg0 - 0x10000U) == 0x3A2EU) {
        return 5;
    }
    return 6;
}
