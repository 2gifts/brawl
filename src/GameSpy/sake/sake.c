#include <types.h>
#include <stdio.h>
#include <string.h>
#include <GameSpy/sake/sakeMain.h>

char lbl_80536500[SAKE_MAX_URL_LENGTH] = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own; they follow the GameSpy SDK's
// sakeMain.c and sakeRequest.c.
// Checked: without this pragma 2 functions in this unit stop matching.
#pragma auto_inline off

int fn_8038D394(SAKE* sakePtr) {
    SAKE sake;

    if (lbl_805A1048 != 1) {
        return 1;
    }
    if (fn_8035E848() != 0) {
        return 2;
    }
    sake = gsimalloc(sizeof(SAKEInternal));
    if (sake == NULL) {
        return 3;
    }
    memset(sake, 0, sizeof(SAKEInternal));
    *sakePtr = sake;
    return 0;
}

void fn_8038D480(SAKE sake, int profileId, const char* loginTicket) {
    sake->mProfileId = profileId;
    memcpy(sake->mLoginTicket, loginTicket, SAKE_LOGIN_TICKET_LENGTH);
    sake->mIsProfileAuthenticated = 1;
}

SAKERequest fn_8038D4D0(SAKE sake, void* input, void* callback, void* userData) {
    SAKERequest request;
    SAKERequest result;
    int startResult;

    request = fn_8038D77C(sake, 0, input, callback, userData);
    result = request;
    if (request == NULL) {
        result = NULL;
    } else {
        startResult = fn_8038E2AC(request);
        sake->mStartRequestResult = startResult;
        if (startResult != 0) {
            fn_8038D84C(result);
            result = NULL;
        }
    }
    return result;
}

int fn_8038D54C(SAKE sake, int fileId, char* url) {
    if (sake == NULL || url == NULL || !sake->mIsGameAuthenticated || !sake->mIsProfileAuthenticated) {
        return 0;
    }
    return snprintf(url, SAKE_MAX_URL_LENGTH, lbl_80490DA0, sake->mGameName, fileId, sake->mGameId, sake->mProfileId) >= 0;
}

int fn_8038D5D0(SAKE sake, char* url) {
    if (sake == NULL || url == NULL || !sake->mIsGameAuthenticated || !sake->mIsProfileAuthenticated) {
        return 0;
    }
    return snprintf(url, SAKE_MAX_URL_LENGTH, lbl_80490DF4, sake->mGameName, sake->mGameId, sake->mProfileId) >= 0;
}

int fn_8038D64C(const char* headers, int* result) {
    const char* headerName = lbl_80490E3C;
    const char* header = strstr(headers, headerName);
    int value;
    int found;
    int clamped;

    if (header != NULL && sscanf(header + strlen(headerName), lbl_8059FBD8, &value) == 1) {
        found = 1;
    } else {
        found = 0;
    }
    if (!found) {
        return 0;
    }
    clamped = value;
    if (clamped >= 6) {
        clamped = 6;
    }
    *result = clamped;
    return 1;
}

int fn_8038D6F8(const char* headers, int* fileId) {
    const char* headerName = lbl_80490E50;
    const char* header = strstr(headers, headerName);

    if (header != NULL) {
        if (sscanf(header + strlen(headerName), lbl_8059FBD8, fileId) == 1) {
            return 1;
        }
    }
    return 0;
}

int fn_8038DC08(SAKERequest request) {
    SAKEIRequestInfo* info;
    int result;

    info = request->mInfo;
    result = info->mValidateInputFunc(request);
    if (result != 0) {
        return result;
    }
    request->mSoapRequest = fn_80362578(lbl_8059FBE0, 1);
    if (request->mSoapRequest == NULL) {
        return 2;
    }
    fn_80364034(request->mSoapRequest, lbl_8059FBF0, request->mInfo->mFuncName);
    fn_80365548(request->mSoapRequest, lbl_8059FBF0, lbl_8059FBF4, request->mSake->mGameId);
    fn_803646CC(request->mSoapRequest, lbl_8059FBF0, lbl_80490F90, request->mSake->mSecretKey);
    fn_803646CC(request->mSoapRequest, lbl_8059FBF0, lbl_80490F9C, request->mSake->mLoginTicket);
    result = info->mFillSoapRequestFunc(request);
    if (result != 0) {
        gsXmlFreeWriter(request->mSoapRequest);
        request->mSoapRequest = NULL;
        if (info->mFreeDataFunc) {
            info->mFreeDataFunc(request);
        }
        return result;
    }
    fn_80364340(request->mSoapRequest, lbl_8059FBF0, request->mInfo->mFuncName);
    fn_80362E4C(request->mSoapRequest);
    return 0;
}

int fn_8038DD48(SAKERequest request, SAKEIRequestInfo* info) {
    int result;

    request->mInfo = info;
    result = fn_8038DC08(request);
    if (result != 0) {
        return result;
    }
    if (lbl_80536500[0] == '\0') {
        snprintf(lbl_80536500, SAKE_MAX_URL_LENGTH, lbl_80490FA8, request->mSake->mGameName);
    }
    fn_803621B8(lbl_80536500, request->mInfo->mSoapAction, request->mSoapRequest, fn_8038DA60, request);
    return 0;
}

int fn_8038E268(SAKERequest request) {
    int found = fn_80367A60(request->mSoapResponse, lbl_80491088, request->mOutput);
    int result = 0;

    if (found == 0) {
        result = 5;
    }
    return result;
}

int fn_8038E2AC(SAKERequest request) {
    return fn_8038DD48(request, &lbl_80491104);
}

int fn_8038E370(SAKERequest request) {
    int result = 4;

    if (*(void**)request->mInput != NULL) {
        result = 0;
    }
    return result;
}

int sakeGetStartRequestResult(SAKE sake) {
    return sake->mStartRequestResult;
}

void sakeSetGame(SAKE sake, const char* gameName, int gameId, const char* secretKey) {
    strcpy(sake->mGameName, gameName);
    strcpy(sake->mSecretKey, secretKey);
    sake->mGameId = gameId;
    sake->mIsGameAuthenticated = 1;
}

// HYPOTHESIS: input layouts follow the GameSpy SDK's create, update and delete record inputs.
typedef struct CreateRecordInput {
    const char* mTableId;
    void* mFields;
    int mNumFields;
} CreateRecordInput;

typedef struct UpdateRecordInput {
    const char* mTableId;
    int mRecordId;
    void* mFields;
    int mNumFields;
} UpdateRecordInput;

typedef struct DeleteRecordInput {
    const char* mTableId;
    int mRecordId;
} DeleteRecordInput;

int sakeiCreateRecordFillSoapRequest(SAKERequest request) {
    CreateRecordInput* input = (CreateRecordInput*)request->mInput;

    fn_803646CC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC20, input->mTableId);
    sakeiFillSoapRequestFieldValues(request, input->mFields, input->mNumFields);
    return 0;
}

int sakeiUpdateRecordValidateInput(SAKERequest request) {
    UpdateRecordInput* input = (UpdateRecordInput*)request->mInput;

    if (input->mTableId == NULL) {
        return 4;
    }
    if (input->mNumFields <= 0) {
        return 6;
    }
    if (input->mFields != NULL) {
        return sakeiValidateRequestFields(input->mFields, input->mNumFields);
    }
    return 5;
}

int sakeiUpdateRecordFillSoapRequest(SAKERequest request) {
    UpdateRecordInput* input = (UpdateRecordInput*)request->mInput;

    fn_803646CC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC20, input->mTableId);
    fn_80365548(request->mSoapRequest, lbl_8059FC00, lbl_80491088, input->mRecordId);
    sakeiFillSoapRequestFieldValues(request, input->mFields, input->mNumFields);
    return 0;
}

int sakeiDeleteRecordFillSoapRequest(SAKERequest request) {
    DeleteRecordInput* input = (DeleteRecordInput*)request->mInput;

    fn_803646CC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC20, input->mTableId);
    fn_80365548(request->mSoapRequest, lbl_8059FC00, lbl_80491088, input->mRecordId);
    return 0;
}

SAKERequest fn_8038D77C(SAKE sake, int type, void* input, void* callback, void* userData) {
    SAKERequest request;

    sake->mStartRequestResult = 0;
    if (input == NULL) {
        sake->mStartRequestResult = 3;
        return NULL;
    }
    if (sake->mIsGameAuthenticated == 0 || sake->mIsProfileAuthenticated == 0) {
        sake->mStartRequestResult = 1;
        return NULL;
    }
    request = gsimalloc(sizeof(SAKERequestInternal));
    if (request == NULL) {
        sake->mStartRequestResult = 2;
        return NULL;
    }
    memset(request, 0, sizeof(SAKERequestInternal));
    request->mSake = sake;
    request->mType = type;
    request->mInput = input;
    request->mCallback = callback;
    request->mCallbackData = userData;
    return request;
}

extern char lbl_8059FC04[7];  // "values"
extern char lbl_8059FC0C[5];  // "name"
extern char lbl_8059FC14[6];  // "value"

int sakeiFillSoapRequestFieldValues(SAKERequest request, SAKEField* fields, int numFields) {
    int i;

    fn_80364034(request->mSoapRequest, lbl_8059FC00, lbl_8059FC04);
    for (i = 0; i < numFields; fields++, i++) {
        fn_80364034(request->mSoapRequest, lbl_8059FC00, "RecordField");
        fn_803646CC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC0C, fields->mName);
        fn_80364034(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14);
        if (fields->mType == 0) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "byteValue");
            fn_80365548(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mByte);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "byteValue");
        } else if (fields->mType == 1) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "shortValue");
            fn_80365548(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mShort);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "shortValue");
        } else if (fields->mType == 2) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "intValue");
            fn_80365548(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mInt);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "intValue");
        } else if (fields->mType == 3) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "floatValue");
            fn_80365C7C(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mFloat);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "floatValue");
        } else if (fields->mType == 4) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "asciiStringValue");
            fn_803646CC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mAsciiString);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "asciiStringValue");
        } else if (fields->mType == 5) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "unicodeStringValue");
            fn_80364DAC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mUnicodeString);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "unicodeStringValue");
        } else if (fields->mType == 6) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "booleanValue");
            fn_80365548(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mBoolean != 0);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "booleanValue");
        } else if (fields->mType == 7) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "dateAndTimeValue");
            fn_80366B18(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mDateAndTime);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "dateAndTimeValue");
        } else if (fields->mType == 8) {
            fn_80364034(request->mSoapRequest, lbl_8059FC00, "binaryDataValue");
            fn_803663AC(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14, fields->mValue.mBinaryData.mValue, fields->mValue.mBinaryData.mLength);
            fn_80364340(request->mSoapRequest, lbl_8059FC00, "binaryDataValue");
        }
        fn_80364340(request->mSoapRequest, lbl_8059FC00, lbl_8059FC14);
        fn_80364340(request->mSoapRequest, lbl_8059FC00, "RecordField");
    }
    return fn_80364340(request->mSoapRequest, lbl_8059FC00, lbl_8059FC04);
}

// TODO: clean up, give variables real names
extern void fn_8035E00C(void* arg);

void fn_8038D41C(void* arg) {
    fn_8035E00C(arg);
}

void fn_8038D84C(SAKERequest request) {
    fn_8035E00C(request);
}

// TODO: clean up, give variables real names
typedef struct Entry { signed char *name; int type; void *data; int count; } Entry;
int sakeiValidateRequestFields(void *fields, int n) {
    Entry *p = (Entry *)fields;
    int i;
    for (i = 0; i < n; i++, p++) {
        if (!p->name || !*p->name) return 7;
        if (p->type < 0 || p->type >= 9) return 8;
        if (p->type == 4 && !p->data) return 9;
        if (p->type == 5 && !p->data) return 9;
        if (p->type == 8) {
            if (p->count < 0) return 9;
            if (!p->data && p->count > 0) return 9;
        }
    }
    return 0;
}
struct Inner {
    int first;
    Entry *third;
    int second;
};
struct S {
    char pad[8];
    struct Inner *p;
};
int fn_8038E1C0(struct S *arg) {
    struct Inner *p = arg->p;
    int second;
    Entry *third;
    if (p->first == 0) return 4;
    second = p->second;
    if (second < 0) return 6;
    third = p->third;
    if (third == 0 && second > 0) return 5;
    return sakeiValidateRequestFields(third, second);
}

// TODO: clean up, give variables real names
/* jj_8038D850 */
extern char lbl_80490E60[0x130];
extern char lbl_8059FBE8[8];
int strcmp(const char *s1, const char *s2);

int fn_8038D850(const char *arg0) {
    char *base = lbl_80490E60;
    int result;
    int comparison;

    if (strcmp(arg0, lbl_8059FBE8) == 0) {
        return 0;
    }
    if (strcmp(arg0, base + 0x20) == 0) {
        return 1;
    }
    if (strcmp(arg0, base + 0x38) == 0) {
        return 2;
    }
    if (strcmp(arg0, base + 0x48) == 0) {
        return 7;
    }
    if (strcmp(arg0, base + 0x5C) == 0) {
        return 8;
    }
    if (strcmp(arg0, base + 0x70) == 0) {
        return 9;
    }
    if (strcmp(arg0, base + 0x84) == 0) {
        return 0xA;
    }
    if (strcmp(arg0, base + 0x94) == 0) {
        return 0xB;
    }
    if (strcmp(arg0, base + 0xA4) == 0) {
        return 0xC;
    }
    if (strcmp(arg0, base + 0xB4) == 0) {
        return 0xD;
    }
    if (strcmp(arg0, base + 0xC8) == 0) {
        return 0xE;
    }
    if (strcmp(arg0, base + 0xD8) == 0) {
        return 0xF;
    }
    if (strcmp(arg0, base + 0xEC) == 0) {
        return 0x10;
    }
    if (strcmp(arg0, base + 0xFC) == 0) {
        return 0x11;
    }
    if (strcmp(arg0, base + 0x108) == 0) {
        return 0x12;
    }
    if (strcmp(arg0, base + 0x114) == 0) {
        return 0x13;
    }
    comparison = strcmp(arg0, base + 0x124);
    result = 0x15;
    if (comparison == 0) {
        result = 0x14;
    }
    return result;
}

// sakeiSoapCallback: translates the HTTP result and the result tag of the SOAP response, lets the request callback
// look at the output and frees the request.
void fn_8038DA60(int httpResult, GSXmlStreamWriter requestData, GSXmlStreamReader response, void* userData) {
    SAKERequest request = (SAKERequest)userData;
    void* output = NULL;
    int result;
    char resultStr[0x20];

    if (request == NULL || request->mSake == NULL || request->mInfo == NULL) {
        return;
    }
    switch (httpResult) {
    case 0:
        result = 0;
        break;
    case 1:
        result = 6;
        break;
    default:
        result = 4;
        break;
    }
    if (result == 0) {
        if (!gsXmlMoveToStart(response) || !gsXmlMoveToNext(response, request->mInfo->mResponseTag)
            || !fn_803679BC(response, request->mInfo->mResultTag, resultStr, 0x20)) {
            result = 5;
        } else {
            result = fn_8038D850(resultStr);
            if (result == 0 && request->mInfo->mSakeOutputSize != 0) {
                request->mOutput = gsimalloc(request->mInfo->mSakeOutputSize);
                if (request->mOutput != NULL) {
                    request->mSoapResponse = response;
                    result = request->mInfo->mProcessSoapResponseFunc(request);
                    if (result == 0) {
                        output = request->mOutput;
                    }
                } else {
                    result = 6;
                }
            }
        }
    }
    if (request->mCallback != NULL) {
        ((void (*)(SAKE, SAKERequest, int, void*, void*, void*))request->mCallback)(
            request->mSake, request, result, request->mInput, output, request->mCallbackData);
    }
    if (request->mInfo->mFreeDataFunc != NULL) {
        request->mInfo->mFreeDataFunc(request);
    }
    gsifree(request->mOutput);
    request->mOutput = NULL;
    gsifree(request);
}
