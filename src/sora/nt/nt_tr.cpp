#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_match.h>
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
