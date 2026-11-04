#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>

struct DWCAccUserData;
struct DWCDate;
struct DWCTime;

// HYPOTHESIS: nteWatchingErrorProc is the one-slot error callback at nteWatching offset 0x340.
class nteWatchingErrorProc {
public:
    virtual void showError(s32 kind, s32 code) = 0;
};

// HYPOTHESIS: nteWatchingEndProc is the one-slot completion callback at nteWatching offset 0x344.
class nteWatchingEndProc {
public:
    virtual void notifySBSEndProc(s32 arg, s32 code) = 0;
};

// HYPOTHESIS: field roles follow the constructor stores and the debug strings in finishSBS.
class nteSBS : public gfTask {
public:
    s32 unk40;
    s32 unk44;
    bool _fInitialized;
    bool unk49;
    // HYPOTHESIS: the completion and error listeners
    nteWatchingEndProc* unk4C;
    nteWatchingErrorProc* unk50;
    s32 unk54;
    s32 unk58;
    u8 unk5C;
    s32 unk60[6];

    nteSBS(u8 flag);
    virtual ~nteSBS();
    bool startupSBS(DWCAccUserData* userData, const char* gameName, const char* secretKey, int param);
    bool finishSBS();
    bool download(void* buf, int size, DWCDate* date, DWCTime* time);
    bool downloadWinCount(void* out);
    void cancel();
    int getErrorCode(u32 kind);
    s64 cnvTime(DWCDate* date, DWCTime* time);
    s64 getTime();
    void updateBattleInfo(int index, s32 first, s32 second, s32 third, s32 fourth, s32 value);
    bool upload(int kind, const void* data, int size);

    // HYPOTHESIS: true while a request is active or a proc id is set
    bool isNowInProgress() const {
        bool inProgress = false;
        if (unk40 == 1 || unk44 != 0)
            inProgress = true;
        return inProgress;
    }
};
static_assert(sizeof(nteSBS) == 0x78, "Class is wrong size!");
