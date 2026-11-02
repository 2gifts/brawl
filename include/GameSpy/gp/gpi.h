#ifndef GAMESPY_GP_GPI_H
#define GAMESPY_GP_GPI_H

// GameSpy SDK Presence (gp.c, gpiBuffer.c, gpiUtility.c). The connection
// handle stays opaque here; only the buffer layout is declared. The functions
// are unnamed in our symbols; the SDK names are noted next to them.
#include <types.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* GPConnection;
typedef int GPResult;

typedef struct GPIBuffer {
    char* buffer;  // 0x0
    int size;      // 0x4
    int len;       // 0x8
    int pos;       // 0xC
} GPIBuffer;

// the Presence availability result (__GSIACResult)
extern int lbl_805A1048;
// "Out of memory."-style error strings of gpiBuffer.c
extern char lbl_8048ED80[];
extern char lbl_8048ED90[];

int fn_8035DA30(int low, int high);                                               // Util_RandInt
void fn_8035DAA4(const char* input, char* output, int inlen, int encodingType);   // B64Encode

GPResult gpInitialize(GPConnection* connection);                                    // gpInitialize
GPResult gpiInitialize(GPConnection* connection);                                    // gpiInitialize
GPResult gpiAppendCharToBuffer(GPConnection* connection, GPIBuffer* outputBuffer, int c);   // gpiAppendCharToBuffer
GPResult gpiSendData(GPConnection* connection, int sock, const char* buffer, int bufferLen, int* closed, int* sent, const char* id);  // gpiSendData
GPResult gpiSendOrBufferStringLenToPeer(GPConnection* connection, void* peer, const char* string, int len);  // gpiSendOrBufferStringLen
GPResult gpiSendOrBufferString(GPConnection* connection, void* peer, const char* string);    // gpiSendOrBufferString
GPResult gpiSendFromBuffer(GPConnection* connection, int sock, GPIBuffer* outputBuffer, int* closed, int clipSentData, const char* id);  // gpiSendFromBuffer
GPResult gpiClipBufferToPosition(GPConnection* connection, GPIBuffer* buffer);                 // gpiClipBufferToPosition
void gpiCallErrorCallback(GPConnection* connection, int reason, int fatal);                 // gpiDisconnect callback
void strzcpy(char* dest, const char* src, u32 len);                            // strzcpy
int gpiValueForKey(const char* command, const char* key, char* value, int len);       // gpiValueForKey
void gpiSetError(GPConnection* connection, int errorCode, const char* errorString);  // gpiSetError
void gpiSetErrorString(GPConnection* connection, const char* errorString);              // gpiSetErrorString
void gpiEncodeString(const char* passwordIn, char* passwordOut);                       // gpiEncodeString

#ifdef __cplusplus
}
#endif

#endif
