#pragma once
// Local BrawlHeaders shadow: input-product, stop and exit callbacks, local packet size API and
// the AID/user-id accessors used by the online menu task.

#include <StaticAssert.h>
#include <nt/d_net_connect.h>
#include <nt/etc/nt_etc_dwc.h>
#include <DWC.h>
#include <NHTTP.h>
#include <types.h>

struct NetworkUserData;

class NtMatching;

class NtShared {
public:
    typedef void (*ntPutProductDataCallback_t)(void**, u32*);
    static void ntSetPutProductDataCallback(ntPutProductDataCallback_t cb);
    static void ntDecodeCmd(u8 aid, u8* buffer, int size);
    typedef void (*ntConsumedDataCallback_t)(const u16*);
    static void ntSetConsumedDataCallback(ntConsumedDataCallback_t cb) {
        NtConsumedDataCallback = cb;
    }
    // HYPOTHESIS: the parameter lists come from the nt_sync call sites of the stored pointers;
    // the storage names follow NtConsumedDataCallback.
    typedef void (*ntStopCallback_t)(u16, u8);
    // HYPOTHESIS: the original calls the setters out of line.
    static void ntSetStopCallback(ntStopCallback_t cb) __attribute__((never_inline));
    typedef void (*ntExitCallback_t)(u8, u16, u8);
    static void ntSetExitCallback(ntExitCallback_t cb) __attribute__((never_inline));
    // HYPOTHESIS: the parameter lists come from the register use in st_operator_network.
    static void ntStartSync(NetworkUserData* data);
    static bool ntRequestStop(NetworkUserData* data, u8 arg);
    static bool ntRequestExit(NetworkUserData* data, u8 arg);
private:
    static ntPutProductDataCallback_t NtPutProductDataCallback;
    static ntConsumedDataCallback_t NtConsumedDataCallback;
    static ntStopCallback_t NtStopCallback;
    static ntExitCallback_t NtExitCallback;
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
    union {
        char unk0[0x3440];
        struct {
            char _padA8[0xA8];
            int unkA8;
            char _padAC[0x134];
            int unk1E0;
        };
    };
    u64 m_userId;
};

class Network {
public:
    virtual ~Network();
    void create(bool);
    // HYPOTHESIS: matching arguments are word-sized integers in the menu callers.
    bool retryMatching(int, int);
    bool disconnectAll(bool);
    static void alarmCallback(Network* network);
    void shutdown();
    void setLocalDataSize(u16 size);
    // HYPOTHESIS: the original calls these out of line; their weak copies are
    // emitted in mu_wifi_interface.o.
    static u8 getMyAID() __attribute__((never_inline));
    static u8 getParentAID() __attribute__((never_inline));

private:
    friend class ntReturnStatusCallback;
    // HYPOTHESIS: callback state, countdown and matching arguments occupy these words.
    union {
        char _0x4[0x2C];
        struct {
            char unk4[0x10];
            s32 unk14;
            s32 unk18;
            int unk1C;
            int unk20;
            int unk24;
            char unk28[8];
        };
    };

public:
    // HYPOTHESIS: the field is the NtMatching manager, whose leading bytes hold the user data.
    union {
        NetworkUserData* m_userData;
        NtMatching* m_matching;
    };

    // HYPOTHESIS: a second pointer to the per-session data, read by the stop, exit and sync requests.
    union {
        char _0x34[0xC];
        struct {
            NetworkUserData* unk34;
            char _0x38[8];
        };
    };
};

inline u8 Network::getMyAID() {
    return lbl_8059E742;
}

inline u8 Network::getParentAID() {
    return lbl_8059E745;
}
static_assert(sizeof(Network) == 0x40, "Class is wrong size!");

extern Network g_Network;
