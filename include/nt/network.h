#pragma once
// Local BrawlHeaders shadow: input-product callback, local packet size API and
// the AID/user-id accessors used by the online menu task.

#include <StaticAssert.h>
#include <nt/d_net_connect.h>
#include <nt/etc/nt_etc_dwc.h>
#include <DWC.h>
#include <NHTTP.h>
#include <types.h>

class NtShared {
public:
    typedef void (*ntPutProductDataCallback_t)(void**, u32*);
    static void ntSetPutProductDataCallback(ntPutProductDataCallback_t cb);
    typedef void (*ntConsumedDataCallback_t)(const u16*);
    static void ntSetConsumedDataCallback(ntConsumedDataCallback_t cb) {
        NtConsumedDataCallback = cb;
    }
private:
    static ntPutProductDataCallback_t NtPutProductDataCallback;
    static ntConsumedDataCallback_t NtConsumedDataCallback;
};
// TODO size

class ntResetCallback {
    virtual void userProc();
    virtual ~ntResetCallback();
};

class ntReturnStatusCallback {
    virtual bool returnStatus();
};

class ntShutdown : public ntReturnStatusCallback {
public:
    virtual bool returnStatus();
};

// HYPOTHESIS: own and parent AIDs of the current session (0xFF when unset).
extern u8 lbl_8059E742;
extern u8 lbl_8059E745;

// HYPOTHESIS: per-session DWC user data; only the user id is known.
struct NetworkUserData {
    char unk0[0x3440];
    u64 m_userId;
};

class Network {
public:
    virtual ~Network();
    void create(bool);
    void setLocalDataSize(u16 size);
    // HYPOTHESIS: the original calls these out of line; their weak copies are
    // emitted in mu_wifi_interface.o.
    static u8 getMyAID() __attribute__((never_inline));
    static u8 getParentAID() __attribute__((never_inline));

private:
    char _0x4[0x2C];

public:
    NetworkUserData* m_userData;

private:
    char _0x34[0xC];
};

inline u8 Network::getMyAID() {
    return lbl_8059E742;
}

inline u8 Network::getParentAID() {
    return lbl_8059E745;
}
static_assert(sizeof(Network) == 0x40, "Class is wrong size!");

extern Network g_Network;
