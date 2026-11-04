#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_match.h>
#include <nt/nt_net_report.h>
#include <nt/nt_tr.h>
#include <types.h>

// HYPOTHESIS: the matching manager instance (the map leaves it unnamed; nt_match.cpp types it as its own view).
extern NtMatching* lbl_805A0434;

NtTr::NtTr() : unk47DC(0) {
    lbl_805A03A8 = this;
    unk20 = 0;
    unk24 = 0;
    unk28 = 0;
    unk2C = 0;
    unk3C = 0;
}

NtTr::~NtTr() {
    lbl_805A03A8 = 0;
}

void NtTr::closeAID(u8 aid) {
    unk44[aid].init();
    u32 mask = ~(1 << aid);
    unk1C &= mask;
    unk38 &= mask;
    unk4808 &= mask;
    lbl_805A0434->watchResetNodeMissMatch();
    g_muWifiInterfaceTask->closeAIDWifi(aid);
}

extern u32 g_NtAIDBitmap;
extern u8 lbl_8059E742;
extern char lbl_8045EE20[0x44];
extern char lbl_8045EEA4[0x2B];
extern char lbl_8045EED0[0x2EC];

bool NtTr::refSendCmd(u8 aid, void** command, u16 size, u16* sequence, u8 type) {
    __typeof__(unk44[0].unk1CData[0])* packet;
    u16 packetSize = size + 0x10;
    __typeof__(unk44[0])* queue = &unk44[aid];
    if (!(g_NtAIDBitmap & (1 << aid)))
        return false;
    if (3 - ((queue->unk18 + 4 - queue->unk14) % 4) < 1)
        packet = 0;
    else
        packet = &queue->unk1CData[queue->unk18];
    if (!packet) {
        netReport(lbl_8045EE20);
        return false;
    }
    packet->unk0 = 1;
    packet->unk4 = ++queue->unk10;
    if (sequence)
        *sequence = packet->unk4;
    packet->unk6 = 9;
    packet->unk7 = 0xF7;
    packet->unkC = packetSize;
    packet->unkA = type;
    packet->unkE = packet->unk4;
    *command = packet->unk16;
    return true;
}

bool NtTr::isSendQueueEmpty() {
    unsigned int aid;
    for (aid = 0; aid < 4; ++aid) {
        u8 destination = aid;
        if (aid != lbl_8059E742 && (g_NtAIDBitmap & (1 << destination))) {
            if (lbl_805A03A8->unk44[aid].unk14 != lbl_805A03A8->unk44[aid].unk18)
                break;
        }
    }
    return aid == 4;
}

void NtTr::broadcastMyShortComment() {
    unk3E_b6 = 1;
}

void NtTr::broadcastMyName() {
    unk3E_b5 = 1;
}

bool NtTr::ntTrRecvQue::stock(u8 aid, u8 type, const void* command, u32 size) {
    if (size > 0x384) {
        netReport(lbl_8045EEA4, size);
        return false;
    }
    __typeof__(unk10[0])* packet = static_cast<__typeof__(unk10[0])*>(unk8);
    unk8 = packet->unk4;
    if (!packet) {
        netReport(lbl_8045EED0, aid);
        return false;
    }
    packet->unk8Data = aid;
    packet->unk9 = type;
    memcpy(packet->unkA, command, size);
    if (unk0)
        static_cast<__typeof__(unk10[0])*>(unk0)->unk0 = packet;
    packet->unk4 = unk0;
    packet->unk0 = 0;
    unk0 = packet;
    if (!unk4)
        unk4 = packet;
    return true;
}
