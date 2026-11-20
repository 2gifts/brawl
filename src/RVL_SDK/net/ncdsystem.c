#include "revolution/ipc.h"
#include "revolution/os.h"

#define PACKED __attribute__((packed))
#define ALIGN(x) __attribute__((aligned(x)))
#define NULL 0
#define TRUE 1
#define FALSE 0

void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);



typedef s32 NCDErr;

enum {
    NCD_MODE_NONE = 0,
    NCD_MODE_WEP40,
    NCD_MODE_WEP104,
    NCD_MODE_INVALID3,
    NCD_MODE_WPA_PSK_TKIP,
    NCD_MODE_WPA2_PSK_TKIP,
    NCD_MODE_WPA_PSK_AES,
};
enum {
    NCD_LINKSTATUS_WORKING = 1,
    NCD_LINKSTATUS_NONE = 2,          // ?
    NCD_LINKSTATUS_WIRED = 3,  // ?
    NCD_LINKSTATUS_WIRELESS_DOWN = 4, // ?
    NCD_LINKSTATUS_WIRELESS_UP = 5,   // ?
};

enum {
    NCD_RESULT_SUCCESS = 0,
    NCD_RESULT_4 = -4,
    NCD_RESULT_FAILURE = -6,
    NCD_RESULT_FATAL_ERROR = -3,
    NCD_RESULT_INPROGRESS = -8,
};

enum {
    NCD_NWC24_PERMISSION_NONE = 0,
    NCD_NWC24_PERMISSION_SEND_MAIL = 1,
    NCD_NWC24_PERMISSION_RECEIVE_MAIL = 2,
    NCD_NWC24_PERMISSION_MESSAGE = (NCD_NWC24_PERMISSION_RECEIVE_MAIL | NCD_NWC24_PERMISSION_SEND_MAIL),
    NCD_NWC24_PERMISSION_DOWNLOAD = 4,
    NCD_NWC24_PERMISSION_ALL = 7
};

#define NCD_MAC_ADDRESS_LENGTH 6

#define NCD_CONNECT_TEST_FLAG 5
#define NCD_DHCP_FLAG 1
#define NCD_DNS_FLAG 2

typedef struct NCDProxyServerProfile {
    u8 mode;         // 0x00
    u8 authType;     // 0x01
    u8 reserved[2];  // 0x02

    char server[256];  // 0x04
    u16 port;          // 0x104

    char username[33];  // 0x106
    char password[33];  // 0x127
} PACKED NCDProxyServerProfile;

typedef struct NCDProxyProfile {
    NCDProxyServerProfile http;  // 0x00
    NCDProxyServerProfile ssl;   // 0x148

    struct {
        u32 isEnabled;       // 0x00
        char servers[1024];  // 0x04
    } noProxy;               // 0x290

    struct {
        u32 isEnabled;           // 0x00
        u32 isAutomaticEnabled;  // 0x04
        char automatic[256];     // 0x08
    } browser;                   // 0x694
} PACKED NCDProxyProfile;

typedef struct NCDIpAdjustProfile {
    s32 maxTransferUnit;    // 0x00
    s32 tcpRetransTimeout;  // 0x04
    s32 dhcpRetransCount;   // 0x08
} NCDIpAdjustProfile;

typedef struct NCDIpProfile {
    u8 addr[4];     // 0x00
    u8 netmask[4];  // 0x04
    u8 gateway[4];  // 0x08
    u8 dns1[4];     // 0x0C
    u8 dns2[4];     // 0x10
} NCDIpProfile;

typedef struct NCDIpConfig {
    BOOL useDhcp;   // 0x00
    BOOL useProxy;  // 0x04

    NCDIpProfile ip;  // 0x08

    NCDIpAdjustProfile adjust;  // 0x1C
    NCDProxyProfile proxy;      // 0x28
} NCDIpConfig;

typedef struct NCDWiredProfile {
    u8 reserved[4];  // 0x00
} NCDWiredProfile;

typedef struct NCDPrivacy {
    u16 mode;        // 0x00
    u8 reserved[2];  // 0x02
    union {
        struct {
            u16 keyId;  // 0x00
            union {
                u16 option;      // 0x02
                u8 reserved[2];  // 0x02
            };
            u8 key[4][5];  // 0x04
        } wep40;
        struct {
            u16 keyId;  // 0x00
            union {
                u16 option;      // 0x02
                u8 reserved[2];  // 0x02
            };
            u8 key[4][13];  // 0x04
        } wep104;
        struct {
            u16 keyLen;      // 0x00
            u8 reserved[2];  // 0x02
            u8 key[64];      // 0x04
        } tkip;
        struct {
            u16 keyLen;      // 0x00
            u8 reserved[2];  // 0x02
            u8 key[64];      // 0x04
        } aes;
    };
} PACKED NCDPrivacy;

typedef struct NCDApConfig {
    u8 ssid[32];     // 0x00
    u16 ssidLength;  // 0x20

    u8 reserved[2];  // 0x22

    NCDPrivacy privacy;  // 0x24
} PACKED NCDApConfig;

typedef struct NCDRakuApConfig {
    NCDApConfig cfg;
    u32 unk_0x6c;
} NCDRakuApConfig;

typedef struct NCDUsbapConfig {
    u16 nickname[11];  // 0x00
    u8 reserved[2];    // 0x16
} PACKED NCDUsbapConfig;

typedef struct NCDAossConfig {
    struct {
        u8 ssid[32];     // 0x00
        u16 ssidLength;  // 0x20
        u16 keyId;       // 0x22
        u8 key[4][5];    // 0x24
    } wep40;             // 0x00
    struct {
        u8 ssid[32];     // 0x00
        u16 ssidLength;  // 0x20
        u16 keyId;       // 0x22
        u8 key[4][13];   // 0x24
    } wep104;            // 0x38
    struct {
        u8 ssid[32];     // 0x00
        u16 ssidLength;  // 0x20
        u16 keyLen;      // 0x22
        u8 key[64];      // 0x24
    } tkip;              // 0x90
    struct {
        u8 ssid[32];     // 0x00
        u16 ssidLength;  // 0x20
        u16 keyLen;      // 0x22
        u8 key[64];      // 0x24
    } aes;               // 0xF4
} PACKED NCDAossConfig;

typedef struct NCDWirelessProfile {
    u16 rateset;      // 0x00
    u8 configMethod;  // 0x02
    u8 retryLimit;    // 0x03
    union {
        NCDApConfig manual;
        NCDUsbapConfig usbap;
        NCDAossConfig aoss;
        NCDApConfig rakuraku;
    } config;  // 0x04
} PACKED NCDWirelessProfile;

typedef struct NCDIfConfig {
    u8 selectedMedia;  // 0x00
    u8 linkTimeout;    // 0x01
    union {
        NCDWiredProfile wired;
        NCDWirelessProfile wireless;
    } netif;  // 0x02
} NCDIfConfig;

typedef struct NCDProfile {
    u8 flags;                   // 0x00
    u8 reserved[3];             // 0x01
    NCDIpProfile ip;            // 0x04
    NCDIpAdjustProfile adjust;  // 0x18
    NCDProxyProfile proxy;      // 0x24
    union {
        NCDWiredProfile wired;
        NCDWirelessProfile wireless;
    } netif;  // 0x7C0
} NCDProfile;

typedef struct NCDConfig {
    u32 version;             // 0x00
    u8 selectedMedia;        // 0x04
    u8 nwc24Permission;      // 0x05
    u8 linkTimeout;          // 0x06
    u8 reserved[1];          // 0x07
    NCDProfile profiles[3];  // 0x08
} PACKED NCDConfig;


#define NCD_IPC_HEAP_SIZE 0x1B60
#define NCD_MAX_PROFILE 3

// TODO: clean up - the unit's .bss/.sbss are untyped lbl_ placeholders addressed through macros instead of named statics
unsigned char lbl_80536580[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805365A0[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805365C0[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805A1138[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A113C[0x4] __attribute__((aligned(4))) = { 0 };
#define ncdMutex (*(OSMutex*)(lbl_80536580))
#define ncdCommonResult ((s32*)(lbl_805365A0))
#define ncdCommonVector ((IPCIOVector*)lbl_805365C0)
#define ncdInitialized (*(u32*)lbl_805A1138)
#define ncdCommonBuffer (*(NCDConfig**)lbl_805A113C)
extern const char* __NCDVersion;

void LockRight(void);
NCDErr ExecConfigCommand(const char* name, NCDConfig* config, u32 command);

NCDErr NCDGetCurrentIfConfig(NCDIfConfig* ifConfig) {
    NCDErr err = 0;
    NCDConfig* config;
    NCDProfile* profile;
    s32 idx;

    if (ifConfig == (void*)NULL) {
        return -3;
    }

    LockRight();

    err = ExecConfigCommand("NCDGetCurrentIfConfig", 0, 3);
    if (err == 0) {
        config = ncdCommonBuffer;
        idx = ncdCommonResult[1];

        if (idx < 0 || idx >= NCD_MAX_PROFILE) {
            err = -7;
        } else {
            profile = &config->profiles[idx];

            if (profile->flags & 1) {
                ifConfig->selectedMedia = 2;
                memcpy(&ifConfig->netif, &profile->netif,
                       sizeof(NCDWiredProfile));
            } else {
                ifConfig->selectedMedia = 1;
                memcpy(&ifConfig->netif, &profile->netif,
                       sizeof(NCDWirelessProfile));
            }

            ifConfig->linkTimeout = config->linkTimeout;
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}

NCDErr NCDGetCurrentIpConfig(NCDIpConfig* ipConfig) {
    NCDErr err = 0;
    NCDConfig* config;
    s32 idx;

    if (ipConfig == (void*)NULL) {
        return -3;
    }

    LockRight();

    err = ExecConfigCommand("NCDGetCurrentIpConfig", 0, 3);
    if (err == 0) {
        config = ncdCommonBuffer;
        idx = ncdCommonResult[1];

        if (idx < 0 || idx >= NCD_MAX_PROFILE) {
            err = -7;
        } else {
            memcpy(&ipConfig->adjust, &config->profiles[idx].adjust,
                   sizeof(NCDIpAdjustProfile));

            if (config->profiles[idx].flags & 0x6) {
                ipConfig->useDhcp = TRUE;
                memcpy(&ipConfig->ip, &config->profiles[idx].ip,
                       sizeof(NCDIpProfile));
            } else {
                ipConfig->useDhcp = FALSE;
                memcpy(&ipConfig->ip, &config->profiles[idx].ip,
                       sizeof(NCDIpProfile));
            }

            if (config->profiles[idx].flags & 0x10) {
                ipConfig->useProxy = TRUE;
                memcpy(&ipConfig->proxy, &config->profiles[idx].proxy,
                       sizeof(NCDProxyProfile));
            } else {
                ipConfig->useProxy = FALSE;
                memset(&ipConfig->proxy, 0, sizeof(NCDProxyProfile));
            }
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}

NCDErr NCDGetLinkStatus(void) {
    NCDErr err;
    s32 fd;

    if (OSGetCurrentThread() == NULL) {
        return -5;
    }

    LockRight();

    fd = IOS_Open("/dev/net/ncd/manage", IPC_OPEN_NONE);
    if (fd < 0) {
        if (fd == IPC_RESULT_NOEXISTS_INTERNAL) {
            err = -8;
        } else {
            err = -2;
        }
    } else {
        ncdCommonVector[0].base = ncdCommonResult;
        ncdCommonVector[0].length = 0x20;

        if (IOS_Ioctlv(fd, 7, 0, 1, ncdCommonVector) < 0) {
            err = -2;
        } else {
            err = ncdCommonResult[0];
            if (err == 0) {
                err = ncdCommonResult[1];
                err = err >= 0 ? err : -1;
            }
        }

        if (IOS_Close(fd) < 0) {
            err = -1;
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}

NCDErr NCDiGetWirelessMacAddress(u8* macAddr) {
    NCDErr err = 0;
    s32 fd;

    if (macAddr == (void*)NULL) {
        return -3;
    }
    if (OSGetCurrentThread() == NULL) {
        return -5;
    }

    LockRight();

    fd = IOS_Open("/dev/net/ncd/manage", IPC_OPEN_NONE);
    if (fd < 0) {
        if (fd == IPC_RESULT_NOEXISTS_INTERNAL) {
            err = -8;
        } else {
            err = -2;
        }
    } else {
        ncdCommonVector[0].base = ncdCommonResult;
        ncdCommonVector[0].length = 0x20;
        ncdCommonVector[1].base = ncdCommonBuffer;
        ncdCommonVector[1].length = NCD_MAC_ADDRESS_LENGTH;

        if (IOS_Ioctlv(fd, 8, 0, 2, ncdCommonVector) < 0) {
            err = -2;
        } else {
            err = ncdCommonResult[0];
            if (err == 0) {
                memcpy(macAddr, ncdCommonBuffer, NCD_MAC_ADDRESS_LENGTH);
            }
        }

        if (IOS_Close(fd) < 0) {
            err = -1;
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}

NCDErr NCDiGetEnabledConfigList(u32* list0, u32* list1, u32* list2) {
    NCDErr err;
    u32 mask0 = 0;
    u32 mask1 = 0;
    u32 mask2 = 0;
    NCDConfig* config;
    u8 flags;
    int i;
    NCDProfile* p;

    LockRight();

    err = ExecConfigCommand("NCDiGetEnabledConfigList", 0, 3);
    if (err == 0) {
        config = ncdCommonBuffer;
        for (i = 0; i < 3; i++) {
            p = &config->profiles[i];
            flags = p->flags;
            if (flags & 0x80) {
                if (flags & 1) {
                    mask0 |= 1 << i;
                } else {
                    if (p->netif.wireless.configMethod != 1) {
                        mask1 |= 1 << i;
                    }
                    if (p->netif.wireless.configMethod == 1) {
                        mask2 |= 1 << i;
                    }
                }
            }
        }
    }

    OSUnlockMutex(&ncdMutex);

    if (list0 != NULL) {
        *list0 = mask0;
    }
    if (list1 != NULL) {
        *list1 = mask1;
    }
    if (list2 != NULL) {
        *list2 = mask2;
    }

    return err;
}

NCDErr ExecConfigCommand(const char* name, NCDConfig* config, u32 command) {
    NCDErr err = 0;
    s32 fd;

    if (OSGetCurrentThread() == NULL) {
        return -5;
    }

    LockRight();

    fd = IOS_Open("/dev/net/ncd/manage", IPC_OPEN_NONE);
    if (fd < 0) {
        if (fd == IPC_RESULT_NOEXISTS_INTERNAL) {
            err = -8;
        } else {
            err = -2;
        }
    } else {
        ncdCommonVector[0].base = ncdCommonBuffer;
        ncdCommonVector[0].length = sizeof(NCDConfig);
        ncdCommonVector[1].base = ncdCommonResult;
        ncdCommonVector[1].length = 0x20;

        switch (command) {
        case 3:
        case 5:
            if (IOS_Ioctlv(fd, command, 0, 2, ncdCommonVector) < 0) {
                err = -2;
            } else {
                err = ncdCommonResult[0];
                if (err == 0 && config != (void*)NULL) {
                    memcpy(config, ncdCommonBuffer, sizeof(NCDConfig));
                }
            }
            break;
        case 4:
        case 6:
            if (config != (void*)NULL) {
                memcpy(ncdCommonBuffer, config, sizeof(NCDConfig));
            }
            if (IOS_Ioctlv(fd, command, 1, 1, ncdCommonVector) < 0) {
                err = -2;
            } else {
                err = ncdCommonResult[0];
            }
            break;
        }

        if (IOS_Close(fd) < 0) {
            err = -1;
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}

void LockRight(void) {
    BOOL enabled;
    void* lo;

    enabled = OSDisableInterrupts();

    if (!(ncdInitialized & 1)) {
        OSRegisterVersion(__NCDVersion);
        OSInitMutex(&ncdMutex);

        lo = (void*)OSRoundUp32B(IPCGetBufferLo());

        if ((u32)IPCGetBufferHi() - (u32)lo < NCD_IPC_HEAP_SIZE) {
            OSPanic("ncdsystem.c", 1457,
                    "Could not reserve heap for NCD library from IPC arena");
        }

        IPCSetBufferLo((void*)((u32)lo + NCD_IPC_HEAP_SIZE));
        ncdCommonBuffer = (NCDConfig*)lo;

        memset(ncdCommonBuffer, 0, NCD_IPC_HEAP_SIZE);
        memset(ncdCommonResult, 0, 0x20);
        memset(ncdCommonVector, 0, sizeof(ncdCommonVector));

        ncdInitialized |= 1;
    }

    OSRestoreInterrupts(enabled);
    OSLockMutex(&ncdMutex);
}
