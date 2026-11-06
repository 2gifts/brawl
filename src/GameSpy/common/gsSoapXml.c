#include <types.h>
#include <string.h>
#include <GameSpy/common/gsXML.h>
#include <GameSpy/ghttp/ghttpConnection.h>
#include "gsStringUtil.h"

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 1 function in this unit stop matching.
#pragma auto_inline off

void gsXmlFreeWriter(GSXmlStreamWriter stream) {
    GSIXmlStreamWriter* writer = stream;

    gsifree(writer->mBuffer);
    gsifree(writer);
}

GSSoapTask* fn_803621B8(const char* URL, const char* service, GSXmlStreamWriter request, GSSoapCallbackFunc callback, void* userData) {
    GSSoapTask* aSoapTask;
    GSTask* aCoreTask;

    aSoapTask = gsimalloc(sizeof(GSSoapTask));
    if (aSoapTask == NULL) {
        return NULL;
    }
    aSoapTask->mCallbackFunc = callback;
    aSoapTask->mCustomFunc = NULL;
    aSoapTask->mURL = URL;
    aSoapTask->mService = service;
    aSoapTask->mRequestSoap = request;
    aSoapTask->mPostData = NULL;
    aSoapTask->mResponseSoap = NULL;
    aSoapTask->mResponseBuffer = NULL;
    aSoapTask->mUserData = userData;
    aSoapTask->mRequestResult = 0;
    aSoapTask->mCompleted = 0;

    aCoreTask = fn_8035EA18();
    if (aCoreTask == NULL) {
        gsifree(aSoapTask);
        return NULL;
    }
    aCoreTask->mCallbackFunc = gsiSoapTaskCallback;
    aCoreTask->mExecuteFunc = fn_80362384;
    aCoreTask->mThinkFunc = gsiSoapTaskThink;
    aCoreTask->mCleanupFunc = gsiSoapTaskCleanup;
    aCoreTask->mCancelFunc = gsiSoapTaskCancel;
    aCoreTask->mTaskData = aSoapTask;
    aSoapTask->mCoreTask = aCoreTask;
    fn_8035E87C(aCoreTask, 0);
    return aSoapTask;
}

int fn_803622B0(int request, int result, char* buffer, int bufferLen, void* param) {
    GSSoapTask* aSoapTask = param;
    GSXmlStreamReader reader;

    aSoapTask->mRequestResult = result;
    aSoapTask->mCompleted = 1;
    aSoapTask->mResponseBuffer = buffer;
    if (result == 0) {
        reader = fn_80362C98();
        aSoapTask->mResponseSoap = reader;
        if (reader == NULL) {
            aSoapTask->mRequestResult = 1;
        } else if (!gsXmlParseBuffer(reader, buffer, bufferLen)) {
            aSoapTask->mRequestResult = 7;
        }
    }
    return 0;
}

int gsiSoapTaskThink(void* taskData) {
    GSSoapTask* aSoapTask = taskData;

    if (aSoapTask->mCompleted) {
        return 4;
    }
    ghttpRequestThink(aSoapTask->mRequestId);
    return 1;
}

void fn_80362384(void* taskData) {
    GSSoapTask* aSoapTask = taskData;
    void* post;

    post = ghttpNewPost();
    aSoapTask->mPostData = post;
    if (post == NULL) {
        aSoapTask->mCompleted = 1;
        aSoapTask->mRequestResult = 1;
        return;
    }
    ghttpPostSetAutoFree(post, 0);
    ghttpPostAddXml(aSoapTask->mPostData, aSoapTask->mRequestSoap);
    if (aSoapTask->mCustomFunc) {
        aSoapTask->mCustomFunc(aSoapTask->mPostData, aSoapTask->mUserData);
    }
    aSoapTask->mRequestId = ghttpGetExA(aSoapTask->mURL, aSoapTask->mService, NULL, 0, aSoapTask->mPostData, 0, 0, NULL,
                                        fn_803622B0, aSoapTask);
}

void gsiSoapTaskCancel(void* taskData) {
    GSSoapTask* aSoapTask = taskData;

    if (!aSoapTask->mCompleted) {
        if (aSoapTask->mRequestId >= 0) {
            ghttpCancelRequest(aSoapTask->mRequestId);
        }
        aSoapTask->mRequestResult = 0x12;
        aSoapTask->mCompleted = 1;
    }
}

void gsiSoapTaskCallback(void* taskData, int result) {
    GSSoapTask* aSoapTask = taskData;

    aSoapTask->mCallbackFunc(aSoapTask->mRequestResult, aSoapTask->mRequestSoap, aSoapTask->mResponseSoap, aSoapTask->mUserData);
}

GSXmlStreamReader fn_80362C98(void) {
    GSIXmlStreamReader* newStream;

    newStream = gsimalloc(sizeof(GSIXmlStreamReader));
    if (newStream == NULL) {
        return NULL;
    }
    newStream->mElementArray = ArrayNew(0x18, 0x20, fn_80362F4C);
    if (newStream->mElementArray == NULL) {
        gsifree(newStream);
        return NULL;
    }
    newStream->mAttributeArray = ArrayNew(0x18, 0x10, fn_80362F50);
    if (newStream->mAttributeArray == NULL) {
        ArrayFree(newStream->mElementArray);
        gsifree(newStream);
        return NULL;
    }
    newStream->mElemReadIndex = -1;
    newStream->mValueReadIndex = -1;
    return newStream;
}

int gsXmlParseBuffer(GSXmlStreamReader stream, char* data, int len) {
    int readPos = 0;
    GSIXmlStreamReader* reader = stream;
    int length = len;
    char* buffer = data;

    while (readPos < length) {
        if (!fn_80362F64(reader, buffer, length, &readPos, -1)) {
            return 0;
        }
    }
    return 1;
}

void gsXmlFreeReader(GSXmlStreamReader stream) {
    GSIXmlStreamReader* reader = stream;

    ArrayFree(reader->mAttributeArray);
    ArrayFree(reader->mElementArray);
    gsifree(reader);
}

int gsXmlMoveToStart(GSXmlStreamReader stream) {
    GSIXmlStreamReader* reader = stream;

    reader->mElemReadIndex = -1;
    reader->mValueReadIndex = -1;
    return 1;
}

int gsXmlMoveToNext(GSXmlStreamReader stream, const char* matchtag) {
    GSIXmlStreamReader* reader = stream;
    int i;

    for (i = reader->mElemReadIndex + 1; i < ArrayLength(reader->mElementArray); i++) {
        if (fn_80367B44(matchtag, ArrayNth(reader->mElementArray, i))) {
            reader->mElemReadIndex = i;
            reader->mValueReadIndex = -1;
            return 1;
        }
    }
    return 0;
}

int fn_803679BC(GSXmlStreamReader stream, const char* matchtag, char* valueOut, int maxLen) {
    const char* strValue = NULL;
    int strLen = 0;

    if (!fn_803678E0(stream, matchtag, &strValue, &strLen)) {
        valueOut[0] = '\0';
        return 0;
    }
    strncpy(valueOut, strValue, maxLen < strLen ? maxLen : strLen);
    valueOut[maxLen - 1 < strLen ? maxLen - 1 : strLen] = '\0';
    return 1;
}

const char* gsXmlWriterGetData(GSXmlStreamWriter stream) {
    GSIXmlStreamWriter* writer = (GSIXmlStreamWriter*)stream;

    return writer->mBuffer;
}

int gsXmlWriterGetDataLength(GSXmlStreamWriter stream) {
    GSIXmlStreamWriter* writer = (GSIXmlStreamWriter*)stream;

    return writer->mLen;
}

int gsiSoapTaskCleanup(void* taskData) {
    GSSoapTask* aSoapTask = (GSSoapTask*)taskData;

    if (aSoapTask->mResponseSoap != NULL) {
        gsXmlFreeReader(aSoapTask->mResponseSoap);
    }
    if (aSoapTask->mResponseBuffer != NULL) {
        gsifree(aSoapTask->mResponseBuffer);
    }
    if (aSoapTask->mPostData != NULL) {
        ghttpFreePost(aSoapTask->mPostData);
    }
    gsifree(aSoapTask);
    return 1;
}

int _UCS2CharToUTF8String(UCS2Char theUCS2Char, UTF8String theUTF8String) {
    if (theUCS2Char <= 0x7F) {
        theUTF8String[0] = (char)theUCS2Char;
        return 1;
    }
    if (theUCS2Char <= 0x7FF) {
        theUTF8String[0] = (char)(UTF8_TWO_BYTE_TAG | (theUCS2Char >> 6));
        theUTF8String[1] = (char)(UTF8_FOLLOW_BYTE_TAG | (theUCS2Char & UTF8_FOLLOW_BYTE_MASK));
        return 2;
    }
    theUTF8String[0] = (char)(UTF8_THREE_BYTE_TAG | (theUCS2Char >> 12));
    theUTF8String[1] = (char)(UTF8_FOLLOW_BYTE_TAG | ((theUCS2Char >> 6) & UTF8_FOLLOW_BYTE_MASK));
    theUTF8String[2] = (char)(UTF8_FOLLOW_BYTE_TAG | (theUCS2Char & UTF8_FOLLOW_BYTE_MASK));
    return 3;
}
