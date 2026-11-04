#pragma once

// Local shadow of BrawlHeaders' nt/nt_offline.h: adds ntFriendInfo::getSaveData and ntFriendInfoSave.

#include <StaticAssert.h>
#include <types.h>

// TODO: ntFriendInfoSave?
class UnkFriendInfo {
    struct UnkClass1 {
        u8 unk0;
        u8 unk1;
        u8 unk2[0x2];
    };

    // TODO: First 8 bytes likely hold the 16-digit Wii Number
    struct UnkClass2 {
        u8 unk0;
        u8 unk1;
        u8 unk2;
        u8 unk3;
        u8 unk4;
        u8 unk5;
        u8 unk6;
        u8 unk7;
        u8 unk8[0x81];
    };
public:
    static const u32 FriendRosterLimit = 64;

    u8 unk0[0x350];
    UnkClass1 unk350[FriendRosterLimit];
    u8 unk450[0x163];
    UnkClass2 unk5B3[FriendRosterLimit];
};
// TODO size assertion

class ntFriendInfo {

public:
    UnkFriendInfo* getFriendInfo();
    static ntFriendInfo* getInstance();
    // HYPOTHESIS: the data of the friends for the Wi-Fi part of the save (muWifiCnctWndTask copies it to the buffer of
    // gfSysRecorder2); size is the size of the buffer on entry and of the data on return.
    bool getSaveData(void** data, int* size);
    void setDirtyFlag(bool dirty);
};
// TODO size assertion

// The save of the friend data (the name comes from the text that muWifiCnctWndTask prints while it waits for it).
class ntFriendInfoSave {
public:
    // HYPOTHESIS: the number of saves that are running (ntFriendInfo adds one when it starts one).
    static int s_savingCount;

    static bool isNotSaving() { return s_savingCount == 0; }
};

class NtOffline {
public:
    NtOffline();
    virtual ~NtOffline();
    u32 getFriendIndexHaveWiiID(u64 id);

    u32 unk4;
};
// TODO size assertion
