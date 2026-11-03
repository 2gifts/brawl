// Local BrawlHeaders shadow: adds save-data fields and operations.
#pragma once

#include <StaticAssert.h>
#include <types.h>

struct DWCstAccUserData;
typedef struct DWCstAccUserData DWCAccUserData;

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

    union {
        u8 unk0[0x350];
        struct {
            u16 unk0_0;
            u16 unk2;
            u16 unk4;
            u8 unk6;
            u8 unk7[9];
            u8 unk10[0x340];
        };
    };
    UnkClass1 unk350[FriendRosterLimit];
    u8 unk450[0x163];
    UnkClass2 unk5B3[FriendRosterLimit];
};
// TODO size assertion

class ntFriendInfo {

public:
    UnkFriendInfo* unk0;
    u32 unk4;
    bool unk8;
    bool unk9;

    void setSaveData(const void* data, u32 size);
    bool isValid(const DWCAccUserData* data);
    UnkFriendInfo* getFriendInfo();
    static ntFriendInfo* getInstance();
};
// TODO size assertion

class NtOffline {
public:
    NtOffline();
    virtual ~NtOffline();
    u32 getFriendIndexHaveWiiID(u64 id);

    u32 unk4;
};
// TODO size assertion
