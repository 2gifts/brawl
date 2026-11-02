#ifndef GAMESPY_SERVERBROWSING_SB_INTERNAL_H
#define GAMESPY_SERVERBROWSING_SB_INTERNAL_H

// GameSpy SDK server browsing (sb_internal.h, sb_queryengine.c,
// sb_server.c). Field order and names follow the SDK's _SBServer,
// SBServerFIFO and _SBQueryEngine; the server list itself stays opaque. The
// functions are unnamed in our symbols; the SDK names are noted next to them.
#include <types.h>
#include <GameSpy/common/darray.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SBServer* SBServer;
typedef struct _SBQueryEngine SBQueryEngine;
typedef struct _SBServerList SBServerList;

struct _SBServer {
    u32 publicip;                 // 0x0
    u16 publicport;               // 0x4
    u32 privateip;                // 0x8
    u16 privateport;              // 0xC
    u32 icmpip;                   // 0x10
    u8 state;                     // 0x14
    u8 flags;                     // 0x15
    HashTable keyvals;            // 0x18
    u32 updatetime;               // 0x1C
    u32 querychallenge;           // 0x20
    SBServer next;                // 0x24
    u8 splitResponseBitmap;       // 0x28
};

typedef struct SBServerFIFO {
    SBServer first;   // 0x0
    SBServer last;    // 0x4
    int count;        // 0x8
} SBServerFIFO;

typedef void (*SBEngineCallbackFn)(SBQueryEngine* engine, int reason, SBServer server, void* instance);

#define MAX_QUERY_KEYS 20

struct _SBQueryEngine {
    int queryversion;                    // 0x0
    int maxupdates;                      // 0x4
    SBServerFIFO querylist;              // 0x8
    SBServerFIFO pendinglist;            // 0x14
    int querysock;                       // 0x20
    int unk24;
    u32 mypublicip;                      // 0x28
    u8 serverkeys[MAX_QUERY_KEYS];       // 0x2C
    int numserverkeys;                   // 0x40
    SBEngineCallbackFn ListCallback;     // 0x44
    void* instance;                      // 0x48
};

__static_assert(sizeof(SBQueryEngine) == 0x4C, "SBQueryEngine");

// a reference-counted string shared by all server lists
typedef struct SBRefString {
    const char* str;  // 0x0
    int refcount;     // 0x4
} SBRefString;

typedef struct SBKeyValuePair {
    const char* key;    // 0x0
    const char* value;  // 0x4
} SBKeyValuePair;

// the null server marker (SBNullServer)
extern SBServer lbl_805A1128;

// the shared reference string table (g_SBRefStrList)
extern HashTable lbl_805A1120;

typedef struct gs_in_addr {
    u32 s_addr;
} gs_in_addr;
char* SOInetNtoA(gs_in_addr in);

// the availability result (__GSIACResult)
extern int lbl_805A1048;

char* fn_8035D654(const char* s);  // goastrdup

int FIFORemove(SBServerFIFO* fifo, SBServer server);                              // FIFORemove
void QEStartQuery(SBQueryEngine* engine, SBServer server);                          // QEStartQuery
void SBQueryEngineInit(SBQueryEngine* engine, int maxupdates, int queryversion, int lanBrowse, SBEngineCallbackFn callback, void* instance);  // SBQueryEngineInit
void SBEngineHaltUpdates(SBQueryEngine* engine);                                           // SBQueryEngineClearQueue? (FIFO reset)
void SBEngineCleanup(SBQueryEngine* engine);                                           // SBEngineCleanup
void SBQueryEngineUpdateServer(SBQueryEngine* engine, SBServer server, int addfront, int querytype, int usequerychallenge);  // SBQueryEngineUpdateServer
void fn_80388120(SBQueryEngine* engine, int processAll);                           // ProcessIncomingReplies
void SBQueryEngineThink(SBQueryEngine* engine);                                           // SBQueryEngineThink
void SBQueryEngineAddQueryKey(SBQueryEngine* engine, u8 keyid);                                 // SBQueryEngineAddQueryKey
void SBQueryEngineRemoveServerFromFIFOs(SBQueryEngine* engine, SBServer server);                          // SBQueryEngineRemoveServerFromFIFOs
int RefStringHash(const void* elem, int numBuckets);                                 // RefStringHash
int RefStringCompare(const void* elem1, const void* elem2);                             // RefStringCompare
void RefStringFree(void* elem);                                                      // RefStringFree
HashTable SBRefStrHash(SBServerList* slist);                                        // SBRefStrHash
void SBRefStrHashCleanup(SBServerList* slist);                                             // SBRefStrHashCleanup
void SBServerFree(void* elem);                                                      // SBServerFree
char* SBServerGetPublicAddress(SBServer server);                                                // SBServerGetPublicAddress
u16 SBServerGetPublicQueryPort(SBServer server);                                                  // SBServerGetPublicQueryPort
int SBServerHasPrivateAddress(SBServer server);                                                  // SBServerHasPrivateAddress
char* SBServerGetPrivateAddress(SBServer server);                                                // SBServerGetPrivateAddress
u16 SBServerGetPrivateQueryPort(SBServer server);                                                  // SBServerGetPrivateQueryPort
const char* SBRefStr(SBServerList* slist, const char* str);                     // SBRefStr
void SBReleaseStr(SBServerList* slist, const char* str);                            // SBReleaseStr
void KeyValFree(void* elem);                                                      // KeyValFree
int KeyValCompareKey(const void* elem1, const void* elem2);                             // KeyValCompare
void SBServerSetPrivateAddr(SBServer server, u32 ip, u16 port);                               // SBServerSetPrivateAddr
int SBIsNullServer(SBServer server);                                                  // SBIsNullServer
int SBSendMessageToServer(void* sb, u32 ip, u16 port, const void* data, int len);           // SBSendMessageToServer
void SBSendNatNegotiateCookieToServer(void* sb, u32 ip, u16 port, int cookie);                          // SBSendNatNegotiateCookieToServer

#ifdef __cplusplus
}
#endif

#endif
