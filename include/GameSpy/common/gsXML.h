#ifndef GAMESPY_COMMON_GSXML_H
#define GAMESPY_COMMON_GSXML_H

// GameSpy SDK XML streams and SOAP tasks (gsXML.c, gsSoap.c). The public
// stream handles are opaque; the internal layouts follow the SDK. The
// functions are unnamed in our symbols; the SDK names are noted next to them.
#include <types.h>
#include <GameSpy/common/darray.h>
#include <GameSpy/common/gsCore.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* GSXmlStreamWriter;
typedef void* GSXmlStreamReader;

typedef struct GSIXmlStreamWriter {
    char* mBuffer;    // 0x0
    int mLen;         // 0x4
    int mCapacity;    // 0x8
    int mClosed;      // 0xC
} GSIXmlStreamWriter;

typedef struct GSIXmlStreamReader {
    DArray mElementArray;    // 0x0
    DArray mAttributeArray;  // 0x4
    int mElemReadIndex;      // 0x8
    int mValueReadIndex;     // 0xC
} GSIXmlStreamReader;

typedef void (*GSSoapCallbackFunc)(int httpResult, GSXmlStreamWriter requestData, GSXmlStreamReader responseData, void* userData);
typedef void (*GSSoapCustomFunc)(void* postData, void* userData);

typedef struct GSSoapTask {
    GSSoapCallbackFunc mCallbackFunc;  // 0x0
    GSSoapCustomFunc mCustomFunc;      // 0x4
    const char* mURL;                  // 0x8
    const char* mService;              // 0xC
    GSXmlStreamWriter mRequestSoap;    // 0x10
    GSXmlStreamReader mResponseSoap;   // 0x14
    char* mResponseBuffer;             // 0x18
    void* mPostData;                   // 0x1C
    void* mUserData;                   // 0x20
    GSTask* mCoreTask;                 // 0x24
    int mRequestId;                    // 0x28
    int mRequestResult;                // 0x2C
    int mCompleted;                    // 0x30
} GSSoapTask;

__static_assert(sizeof(GSSoapTask) == 0x34, "GSSoapTask");

// "</SOAP-ENV:Body></SOAP-ENV:Envelope>"
extern char lbl_8048E9C0[0x25];

GSSoapTask* fn_803621B8(const char* URL, const char* service, GSXmlStreamWriter request, GSSoapCallbackFunc callback, void* userData);  // gsiExecuteSoap
int fn_803622B0(int request, int result, char* buffer, int bufferLen, void* param);  // gsiSoapTaskHttpCompletedCallback
void fn_80362384(void* taskData);       // gsiSoapTaskExecute
GSXmlStreamWriter fn_80362578(const char** namespaces, int count);                              // gsXmlCreateStreamWriter
GSXmlStreamReader fn_80362C98(void);    // gsXmlCreateStreamReader
int fn_80362E4C(GSXmlStreamWriter stream);   // gsXmlCloseWriter
void fn_80362F4C(void* elem);           // gsiXmlUtilElementFree
void fn_80362F50(void* elem);           // gsiXmlUtilAttributeFree
int fn_80362F64(GSIXmlStreamReader* reader, char* buffer, int len, int* pos, int parentIndex);  // gsiXmlUtilParseElement
int fn_80364034(GSXmlStreamWriter stream, const char* ns, const char* tag);                    // gsXmlWriteOpenTag
int fn_80364340(GSXmlStreamWriter stream, const char* ns, const char* tag);                    // gsXmlWriteCloseTag
int fn_803646CC(GSXmlStreamWriter stream, const char* ns, const char* tag, const char* value); // gsXmlWriteStringElement
int fn_80365548(GSXmlStreamWriter stream, const char* ns, const char* tag, int value);         // gsXmlWriteIntElement
int fn_803678E0(GSXmlStreamReader stream, const char* matchtag, const char** valueOut, int* lenOut);  // gsXmlReadChildAsString
int fn_803679BC(GSXmlStreamReader stream, const char* matchtag, char* valueOut, int maxLen);  // gsXmlReadChildAsStringNT
int fn_80367A60(GSXmlStreamReader stream, const char* matchtag, int* valueOut);  // gsXmlReadChildAsInt
int fn_80367B44(const char* matchtag, void* elem);  // gsiXmlUtilTagMatches
int gsiSoapTaskThink(void* taskData);
void gsiSoapTaskCancel(void* taskData);
void gsiSoapTaskCallback(void* taskData, int result);
int gsiSoapTaskCleanup(void* taskData);
int gsXmlParseBuffer(GSXmlStreamReader stream, char* data, int len);
void gsXmlFreeWriter(GSXmlStreamWriter stream);
void gsXmlFreeReader(GSXmlStreamReader stream);
int gsXmlMoveToStart(GSXmlStreamReader stream);
int gsXmlMoveToNext(GSXmlStreamReader stream, const char* matchtag);
const char* gsXmlWriterGetData(GSXmlStreamWriter stream);
int gsXmlWriterGetDataLength(GSXmlStreamWriter stream);

#ifdef __cplusplus
}
#endif

#endif
