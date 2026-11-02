#ifndef GAMESPY_GHTTP_GHTTPCONNECTION_H
#define GAMESPY_GHTTP_GHTTPCONNECTION_H

// GameSpy SDK HTTP (ghttpConnection.h, ghttpBuffer.h, ghttpPost.h,
// ghttpEncryption.h). Field order and names follow the SDK; fields whose name
// is not evidenced by the matched code are unkN. The functions are unnamed in
// our symbols; the SDK names are noted next to the placeholders.
#include <types.h>
#include <GameSpy/common/darray.h>
#include <GameSpy/common/gsXML.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int GHTTPBool;
typedef int GHTTPRequest;
typedef int GHTTPResult;
typedef int GHTTPState;
typedef int GHTTPByteCount;
typedef int GHTTPEncryptionEngine;

typedef struct GHIConnection GHIConnection;
typedef struct GHIEncryptor GHIEncryptor;

typedef void (*ghttpProgressCallback)(GHTTPRequest request, GHTTPState state, const char* buffer, GHTTPByteCount bufferLen,
                                      GHTTPByteCount bytesReceived, GHTTPByteCount totalSize, void* param);
typedef GHTTPBool (*ghttpCompletedCallback)(GHTTPRequest request, GHTTPResult result, char* buffer, GHTTPByteCount bufferLen, void* param);

typedef int (*GHTTPEncryptorInitFunc)(GHIConnection* connection, GHIEncryptor* encryptor);
typedef int (*GHTTPEncryptorCleanupFunc)(GHIConnection* connection, GHIEncryptor* encryptor);
typedef int (*GHTTPEncryptorEncryptFunc)(GHIConnection* connection, GHIEncryptor* encryptor, const char* plainText, int plainTextLength,
                                         char* encryptedBuffer, int* encryptedLength);
typedef int (*GHTTPEncryptorDecryptFunc)(GHIConnection* connection, GHIEncryptor* encryptor, const char* encryptedData, int* encryptedLength,
                                         char* decryptedBuffer, int* decryptedLength);

typedef enum GHIEncryptionResult {
    GHIEncryptionResult_None,
    GHIEncryptionResult_Success,
    GHIEncryptionResult_BufferTooSmall,
    GHIEncryptionResult_Error
} GHIEncryptionResult;

struct GHIEncryptor {
    void* mInterface;                         // 0x0
    GHTTPEncryptionEngine mEngine;            // 0x4
    GHTTPBool mInitialized;                   // 0x8
    GHTTPBool mSessionEstablished;            // 0xC
    GHTTPEncryptorInitFunc mInitFunc;         // 0x10
    GHTTPEncryptorCleanupFunc mCleanupFunc;   // 0x14
    GHTTPEncryptorEncryptFunc mEncryptFunc;   // 0x18
    GHTTPEncryptorDecryptFunc mDecryptFunc;   // 0x1C
};

typedef struct GHIBuffer {
    GHIConnection* connection;  // 0x0
    char* data;                 // 0x4
    int size;                   // 0x8
    int len;                    // 0xC
    int pos;                    // 0x10
    int sizeIncrement;          // 0x14
    GHTTPBool fixed;            // 0x18
    GHTTPBool dontFree;         // 0x1C
    GHTTPBool readOnly;         // 0x20
} GHIBuffer;

typedef struct GHIPost {
    DArray data;            // 0x0
    void* callback;         // 0x4
    void* param;            // 0x8
    GHTTPBool hasFiles;     // 0xC
    GHTTPBool hasSoap;      // 0x10
    GHTTPBool useDIME;      // 0x14
    GHTTPBool autoFree;     // 0x18
} GHIPost;

typedef enum GHIPostDataType {
    GHIString,
    GHIFileDisk,
    GHIFileMemory,
    GHIXmlData
} GHIPostDataType;

typedef struct GHIPostData {
    GHIPostDataType type;   // 0x0
    char* name;             // 0x4
    union {
        char* string;                                                   // 0x8
        struct { char* filename; char* reportFilename; char* contentType; } fileDisk;     // 0x8
        struct { const char* data; int len; char* reportFilename; char* contentType; } fileMemory; // 0x8
        GSXmlStreamWriter xml;                                          // 0x8
    } data;
} GHIPostData;

// the progress of one post data entry while it is sent
typedef struct GHIPostDataState {
    GHIPostData* data;      // 0x0
    int pos;                // 0x4
    void* file;             // 0x8
    int len;                // 0xC
} GHIPostDataState;

struct GHIConnection {
    GHTTPBool inUse;                            // 0x0
    GHTTPRequest request;                       // 0x4
    int uniqueID;                               // 0x8
    int type;                                   // 0xC
    GHTTPState state;                           // 0x10
    char* URL;                                  // 0x14
    char* serverAddress;                        // 0x18
    u32 serverIP;                               // 0x1C
    u16 serverPort;                             // 0x20
    char* requestPath;                          // 0x24
    int protocol;                               // 0x28
    char* sendHeaders;                          // 0x2C
    void* saveFile;                             // 0x30
    GHTTPBool blocking;                         // 0x34
    GHTTPBool persistConnection;                // 0x38
    GHTTPResult result;                         // 0x3C
    ghttpProgressCallback progressCallback;     // 0x40
    ghttpCompletedCallback completedCallback;   // 0x44
    void* callbackParam;                        // 0x48
    int socket;                                 // 0x4C
    int socketError;                            // 0x50
    GHIBuffer sendBuffer;                       // 0x54
    GHIBuffer encodeBuffer;                     // 0x78
    GHIBuffer recvBuffer;                       // 0x9C
    GHIBuffer decodeBuffer;                     // 0xC0
    GHIBuffer getFileBuffer;                    // 0xE4
    GHTTPBool userBufferSupplied;               // 0x108
    int statusMajorVersion;                     // 0x10C
    int statusMinorVersion;                     // 0x110
    int statusCode;                             // 0x114
    int statusStringIndex;                      // 0x118
    int headerStringIndex;                      // 0x11C
    GHTTPBool completed;                        // 0x120
    GHTTPByteCount fileBytesReceived;           // 0x124
    GHTTPByteCount totalSize;                   // 0x128
    char* redirectURL;                          // 0x12C
    int redirectCount;                          // 0x130
    GHTTPBool chunkedTransfer;                  // 0x134
    char chunkHeader[11];                       // 0x138
    int chunkHeaderLen;                         // 0x144
    int chunkBytesLeft;                         // 0x148
    int chunkReadingState;                      // 0x14C
    GHTTPBool processing;                       // 0x150
    GHTTPBool connectionClosed;                 // 0x154
    GHTTPBool throttle;                         // 0x158
    u32 lastThrottleRecv;                       // 0x15C
    GHIPost* post;                              // 0x160
    int postingStateIndex;                      // 0x164
    int unk168;
    GHTTPByteCount postBytesSent;               // 0x16C
    int unk170;
    int unk174;
    int unk178;
    GHTTPBool postWaitForUserResponse;          // 0x17C
    GHTTPBool postComplete;                     // 0x180
    int unk184;
    char* proxyOverrideServer;                  // 0x188
    u16 proxyOverridePort;                      // 0x18C
    GHIEncryptor encryptor;                     // 0x190
    void* handleThread;                         // 0x1B0
};

__static_assert(sizeof(GHIBuffer) == 0x24, "GHIBuffer");
__static_assert(sizeof(GHIPost) == 0x1C, "GHIPost");
__static_assert(sizeof(GHIPostData) == 0x18, "GHIPostData");
__static_assert(sizeof(GHIEncryptor) == 0x20, "GHIEncryptor");
__static_assert(offsetof(GHIConnection, encryptor) == 0x190, "GHIConnection.encryptor");

// ghttp globals
extern u16 lbl_805A1060;            // ghiProxyPort
extern char* lbl_805A1064;          // ghiProxyAddress
extern int lbl_805A106C;            // ghiNumConnections
extern int lbl_805A1070;            // ghiConnectionsLen
extern GHIConnection** lbl_805A1074;  // ghiConnections
extern int lbl_805A1078;            // ghiReferenceCount
extern int lbl_8059FA50;            // ghiThrottleBufferSize
extern int lbl_8059FA54;            // ghiThrottleTimeDelay
extern char lbl_8059FA40[3];        // ": "
extern char lbl_8059FA44[3];        // CRLF
extern char lbl_8059FA48[3];        // "%d"
extern char lbl_804905F8[9];        // "https://"
extern u8 lbl_80490604[0x31];       // SSL MAC pad 1
extern u8 lbl_80490638[0x31];       // SSL MAC pad 2
extern u8 lbl_8059FA6C[4];          // SSL record content type

// platform helpers
char* fn_8035D654(const char* s);                          // goastrdup
u32 fn_8035C620(const char* name);                         // inet_addr
int fn_8035D364(const char* hostname, void** handle);      // gsiStartResolvingHostname
int fn_8035C384(int s, const void* addr, int len);         // connect
int fn_8035CD04(int s, int blocking);                      // SetSockBlocking
void SetReceiveBufferSize(int s, int size);                         // SetReceiveBufferSize

// MSL stdio of this build (ftell comes from <stdio.h>)
int fread(void* buffer, int size, int count, void* file);
int fseek(void* file, long offset, int origin);
void rewind(void* file);

// md5 and rc4
void RC4Encrypt(void* rc4, const u8* src, u8* dst, int len);  // RC4Encrypt

GHTTPBool ghiResizeBuffer(GHIBuffer* buffer, int sizeIncrement);                                          // ghiResizeBuffer
GHTTPBool ghiInitBuffer(GHIConnection* connection, GHIBuffer* buffer, int initialSize, int sizeIncrement); // ghiInitBuffer
GHTTPBool ghiInitFixedBuffer(GHIConnection* connection, GHIBuffer* buffer, char* userBuffer, int userBufferLen); // ghiInitFixedBuffer
GHTTPBool ghiAppendDataToBuffer(GHIBuffer* buffer, const char* data, int dataLen);                              // ghiAppendDataToBuffer
GHTTPBool ghiEncryptDataToBuffer(GHIBuffer* buffer, const char* data, int dataLen);                              // ghiEncodeDataIntoBuffer
GHTTPBool ghiAppendHeaderToBuffer(GHIBuffer* buffer, const char* name, const char* value);                        // ghiAppendHeaderToBuffer
GHTTPBool ghiAppendIntToBuffer(GHIBuffer* buffer, int i);                                                     // ghiAppendIntToBuffer
void ghiResetBuffer(GHIBuffer* buffer);                                                                 // ghiResetBuffer
GHTTPBool ghiSendBufferedData(GHIConnection* connection);                                                    // ghiSendBufferedData
GHTTPBool ghiReadDataFromBufferFixed(GHIBuffer* buffer, char* data, int len);                                       // ghiReadDataFromBuffer
void ghiCallCompletedCallback(GHIConnection* connection);                                                         // ghiCallCompletedCallback
void ghiCallProgressCallback(GHIConnection* connection, const char* buffer, GHTTPByteCount bufferLen);            // ghiCallProgressCallback
void ghiCallPostCallback(GHIConnection* connection);                                                         // ghiCallPostCallback
void ghiCreateLock(void);                                                                              // ghiCreateLock
void ghiLock(void);                                                                              // ghiLock
void ghiUnlock(void);                                                                              // ghiUnlock
GHTTPBool ghiDecryptReceivedData(GHIConnection* connection);                                                    // ghiDecryptReceivedData
int ghiDoSend(GHIConnection* connection, const char* buffer, int len);                             // ghiTrySendThenBuffer
int ghiTrySendThenBuffer(GHIConnection* connection, const char* buffer, int len);                             // ghiSendPostData
GHIConnection* ghiNewConnection(void);                                                                    // ghiNewConnection
GHTTPBool ghiFreeConnection(GHIConnection* connection);                                                    // ghiFreeConnection
GHIConnection* ghiRequestToConnection(GHTTPRequest request);                                                    // ghiRequestToConnection
void ghiEnumConnections(GHTTPBool (*callback)(GHIConnection*));                                             // ghiEnumConnections
void ghiRedirectConnection(GHIConnection* connection);                                                         // ghiRedirectConnection
GHTTPBool ghttpSetRequestEncryptionEngine(GHTTPRequest request, GHTTPEncryptionEngine engine);                           // ghttpSetRequestEncryptionEngine
int ghiEncryptorSslInitFunc(GHIConnection* connection, GHIEncryptor* encryptor);                                 // ghiEncryptorSslInitFunc
int ghiEncryptorSslCleanupFunc(GHIConnection* connection, GHIEncryptor* encryptor);                                 // ghiEncryptorSslCleanupFunc
int ghiEncryptorSslEncryptFunc(GHIConnection* connection, GHIEncryptor* encryptor, const char* plainText, int plainTextLength,
                char* encryptedBuffer, int* encryptedLength);                                        // ghiEncryptorSslEncryptFunc
int ghiEncryptorSslDecryptFunc(GHIConnection* connection, GHIEncryptor* encryptor, const char* encryptedData, int* encryptedLength,
                char* decryptedBuffer, int* decryptedLength);                                        // ghiEncryptorSslDecryptFunc
void ghiHandleStatus(GHIConnection* connection);                                                         // ghiHandleComplete
GHTTPBool ghiProcessConnection(GHIConnection* connection);                                                    // ghiProcessConnection
void ghttpStartup(void);                                                                              // ghttpStartup
GHTTPRequest ghttpGetExA(const char* URL, const char* headers, char* buffer, int bufferSize, GHIPost* post, GHTTPBool throttle,
                         GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void* param);  // ghttpGetEx
GHTTPRequest ghttpPostA(const char* URL, GHIPost* post, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void* param);  // ghttpPost
GHTTPRequest ghttpPostExA(const char* URL, const char* headers, GHIPost* post, GHTTPBool throttle, GHTTPBool blocking,
                         ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void* param);  // ghttpPostEx
void ghttpThink(void);                                                                              // ghttpThink
GHTTPBool ghttpRequestThink(GHTTPRequest request);                                                         // ghttpRequestThink
void ghttpCancelRequest(GHTTPRequest request);                                                              // ghttpCancelRequest
void ghttpSetMaxRecvTime(GHTTPRequest request, int value);                                                   // sets connection unk184
GHIPost* ghttpNewPost(void);                                                                          // ghttpNewPost
void ghttpPostSetAutoFree(void* post, GHTTPBool autoFree);                                                    // ghttpPostSetAutoFree
void ghttpFreePost(GHIPost* post);                                                                     // ghttpFreePost
void ghttpPostAddXml(void* post, GSXmlStreamWriter xml);                                                 // ghttpPostAddXml
void ghiPostDataFree(void* elem);                                                                        // ghiFreePostData
GHIPost* ghiNewPost(void);                                                                          // ghiNewPost
void ghiIsPostAutoFree(GHIPost* post, GHTTPBool autoFree);                                                 // ghiPostSetAutoFree
void ghiFreePost(GHIPost* post);                                                                     // ghiFreePost
GHTTPBool ghiPostStateInit(GHIPostDataState* state);                                                      // ghiPostDataStateInit
GHTTPBool ghiPostInitState(GHIConnection* connection);                                                    // ghiPostInitState
int ghiPostCleanupState(GHIConnection* connection);                                                          // ghiPostCleanupState
int ghiPostFileDiskStateDoPosting(GHIPostDataState* state, GHIConnection* connection);                                 // ghiPostFileDiskStateDoPosting
int ghiPostDoPosting(GHIConnection* connection);                                                          // ghiPostDoPosting
GHTTPBool ghiParseURL(GHIConnection* connection);                                                    // ghiParseURL
void ghiDoSocketInit(GHIConnection* connection);                                                         // ghiDoSocketInit
void ghiDoHostLookup(GHIConnection* connection);                                                         // ghiDoHostLookup
void ghiDoLookupPending(GHIConnection* connection);                                                         // ghiDoLookupPending
void ghiDoConnecting(GHIConnection* connection);                                                         // ghiDoConnecting
void ghiDoSecuringSession(GHIConnection* connection);                                                         // ghiDoSecuringSession
void ghiDoSendingRequest(GHIConnection* connection);                                                         // ghiDoSendingRequest
void ghiDoPosting(GHIConnection* connection);                                                         // ghiDoPosting
void ghiDoWaiting(GHIConnection* connection);                                                         // ghiDoWaiting
void ghiDoReceivingStatus(GHIConnection* connection);                                                         // ghiDoReceivingStatus
void fn_80381BB8(GHIConnection* connection);                                                         // ghiDoReceivingHeaders
void fn_8038215C(GHIConnection* connection);                                                         // ghiDoReceivingFile

#ifdef __cplusplus
}
#endif

#endif
