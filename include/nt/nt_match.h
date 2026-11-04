#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <revolution/OS/OSThread.h>

// Local shadow of BrawlHeaders' nt/nt_match.h: adds NtMatching (watchResetNodeMissMatch and thSaveUserDataNow; main.dol 0x80147434 ..., names from the sora map).

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
    static void ntFree(int, void*);
};
// TODO size assertion

class NtMatching {
public:
    // HYPOTHESIS: matching arguments are word-sized integers in the menu callers.
    bool retryMatching(int, int);
    // HYPOTHESIS: the matching manager NtTr resets (only the methods its callers use are declared).
    void watchResetNodeMissMatch();
    // HYPOTHESIS: starts the save of the user data (the name of the Wi-Fi user) in a thread, when the network state
    // allows it.
    static void thSaveUserDataNow();
    bool deleteFriend(int index);
    void freeDisplayOrder();
    static void updateServerCallback(int error, int isChanged, void* param);
    static void deleteFriendListCallback(int deletedIndex, int srcIndex, void* param);
    static int getAvailableFriendListIndex(u64 friendKey);
    static void udpRecvCallback(u8 aid, u8* buffer, int size);
    static void newClientCallback(int index, void* param);
    static void changetFriendStatusCallback(int index, u8 status, const char* statusString, void* param);
    // HYPOTHESIS: the final argument selects updates to the local display order.
    bool deleteDisplayOrder(int index, bool updateLocal);

    void requestDisconnectAIDBitmap(u8 bitmap);
    bool isSaveFriendInfoDone();
    u8 getCloseAIDBitmap();
    void setSequenceWatch(int sequence, int state);
    u64 getRegisterFriendCode(int index);
    void shutdown();
    bool isNetShutdownErrorOccured();
    void resetWork();
    static void setErrorStatus(s32 error);

    union {
        struct {
            u8 unk0[0x12];
            u8 unk12[0x40];
            union {
                u8 unk52[0x33BF];
                struct {
                    u8 _pad52[0x337A];
                    u8 unk33CC;
                };
            };
            bool unk3411;
            bool unk3412;
            u8 unk3413[0xE];
            s8 unk3421;
            u8 unk3422[0x11A];
            void* unk353C;
        };
        // HYPOTHESIS: typed state and thread fields occupy the existing padding.
        struct {
            u8 _pad0[0x11];
            u8 unk11;
            u8 _pad12[0x8E];
            OSThread unkA0;
            u8 _pad3B8[0x3015];
            u8 unk33CD;
            u8 unk33CE;
            u8 _pad33CF[0x26];
            u8 unk33F5;
            u8 unk33F6;
            u8 _pad33F7[0xD];
            u32 unk3404;
            u8 _pad3408[0x28];
            int unk3430;
            int unk3434;
            u32 unk3438;
            u8 _pad343C[0x6E];
            u8 unk34AA;
            u8 unk34AB;
        };
    };
};
