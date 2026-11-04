#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <nt/nt_etc_sbs.h>
#include <sr/sr_common.h>
#include <types.h>

class gfCryptoAES;
class nteFile;

// HYPOTHESIS: field offsets follow the constructor stores. Object size 0x3A0.
// HYPOTHESIS: nteWatchingErrorProc sits at 0x340 and nteWatchingEndProc at 0x344.
class nteWatching : public gfThread, public nteWatchingErrorProc, public nteWatchingEndProc {
public:
    u8 m_unk348;
    s32 m_unk34C;
    s32 m_unk350;
    Heaps::HeapType m_heap;
    s32 m_unk358;
    u8 m_unk35C;
    u8 m_unk35D;
    u16 m_unk35E;
    u8 m_unk360[7];
    u8 m_unk367[7];
    u8 m_unk36E[7];
    u8 m_unk375[7];
    u8 m_unk37C[7];
    u16 m_unk384;
    u8 m_unk386;
    u8 m_unk387;
    u8 m_unk388;
    s32 m_unk38C;
    u8 m_unk390;
    nteSBS* m_sbs;
    u8 m_unk398;
    gfCryptoAES* m_crypto;

    nteWatching(Heaps::HeapType heap);
    ~nteWatching();
    virtual void run();
    bool cancel();
    bool upload();
    bool download();
    void showError(s32 kind, s32 code);
    void notifySBSEndProc(s32 arg, s32 code);
    bool _makeDataForUL(void** data, u32* size);
    bool _storeData(nteFile* file);
};
static_assert(sizeof(nteWatching) == 0x3A0, "Class is the wrong size!");
static_assert(offsetof(nteWatching, m_unk350) == 0x350, "m_unk350");
static_assert(offsetof(nteWatching, m_unk388) == 0x388, "m_unk388");
static_assert(offsetof(nteWatching, m_unk38C) == 0x38C, "m_unk38C");
static_assert(offsetof(nteWatching, m_sbs) == 0x394, "m_sbs");
static_assert(offsetof(nteWatching, m_unk398) == 0x398, "m_unk398");
static_assert(offsetof(nteWatching, m_crypto) == 0x39C, "m_crypto");
