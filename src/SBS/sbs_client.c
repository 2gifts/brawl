#include <types.h>
#include <stdio.h>
#include <stdarg.h>
#include <GameSpy/platform/rvl/dwc/dwci_error.h>
#include <GameSpy/platform/rvl/dwc/dwc_report_fwd.h>
#include <GameSpy/platform/rvl/dwc/dwc_encsession.h>
#include <GameSpy/common/gsCore.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80534618[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80534628[0x10] __attribute__((aligned(8))) = { 0 };

// HYPOTHESIS: layout of the SBS client state in .data, only the first two words are used here.
typedef struct SBSClientState {
    int state;
    int result;
    unsigned char _08[0x28];
    void* buffer;
    union {
        unsigned char _34[0x24];
        struct {
            unsigned char _34a[0x14];
            void* encKey;
        };
    };
} SBSClientState;

extern SBSClientState lbl_8048E680;
extern const char lbl_80415790[0x578];

// HYPOTHESIS: dwc_auth_interface.h is not reachable from this unit's include path, so the auth server query is declared locally.
int DWCi_Auth_GetServer(void);

void HandleEncResponse(void);
int DWC_ProcessGHTTP(void);
int DWC_ShutdownGHTTP(void);
void EncSessionProcess(void);
void PostData(void);
void RegisterFileIdToSake(void);
void RegisterFileIdToIndexedTable(void);
void GetData(void);
int DWC_GetLastError(int* errorCode);
void* memcpy(void* dest, const void* src, unsigned long count);

int SBS_Process(void) {
    switch (lbl_8048E680.state) {
    case 6:
        PostData();
        break;
    case 2:
        DWC_ProcessGHTTP();
        break;
    case 3:
        EncSessionProcess();
        break;
    case 4:
        gsCoreThink(0);
        break;
    case 7:
        DWC_ShutdownGHTTP();
        RegisterFileIdToSake();
        break;
    case 8:
        RegisterFileIdToIndexedTable();
        break;
    case 9:
        GetData();
        break;
    case 10:
        gsCoreThink(0);
        if (fn_8035E848() != 0) {
            lbl_8048E680.state = 1;
            lbl_8048E680.result = 4;
        }
        break;
    }
    return lbl_8048E680.result;
}

void EncSessionProcess(void) {
    switch (DWCi_EncSessionProcess()) {
    case 8:
        HandleEncResponse();
        DWCi_EncSessionShutdown();
        break;
    case 0:
        lbl_8048E680.state = 0;
        lbl_8048E680.result = 1;
        DWCi_EncSessionShutdown();
        break;
    case 1:
        DWC_Printf(8, lbl_80415790 + 0x164);
        if (DWC_GetLastError(0) == 0) {
            lbl_8048E680.state = 0;
            lbl_8048E680.result = 0xB;
        } else {
            lbl_8048E680.state = 5;
            lbl_8048E680.result = 3;
        }
        DWCi_EncSessionShutdown();
        break;
    }
}

int EncSessionRequest(const char* request, void* data, int size) {
    int status;
    switch (DWCi_Auth_GetServer()) {
    case 0:
        DWCi_EncSessionInitialize(1, lbl_80415790 + 0x102);
        break;
    case 1:
        DWCi_EncSessionInitialize(0, lbl_80415790 + 0x102);
        break;
    }
    status = DWCi_EncSessionGetAsync(request, lbl_8048E680.encKey, data, size, 0);
    switch (status) {
    case 0:
        lbl_8048E680.state = 3;
        status = 1;
        break;
    case 1:
        DWCi_SetError(9, -99009);
        status = 0;
        break;
    case 2:
        DWC_Printf(8, lbl_80415790 + 0x144);
        DWCi_SetError(9, -99001);
        lbl_8048E680.state = 5;
        lbl_8048E680.result = 3;
        status = 0;
        break;
    case 3:
        DWCi_SetError(9, -99009);
        status = 0;
        break;
    }
    return status;
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

int NumCmp(int *arg0, int *arg1) {
    return *arg0 - *arg1;
}

void cbGetCompleted(const void* data, int size, int result) {
    DWC_Printf(4, lbl_80415790 + 0x535);
    if (result != 0) {
        DWC_Printf(8, lbl_80415790 + 0x54A, result);
        lbl_8048E680.state = 5;
        lbl_8048E680.result = 3;
    } else {
        memcpy(lbl_8048E680.buffer, data, size);
        lbl_8048E680.state = 0;
        lbl_8048E680.result = 0;
    }
}

// HYPOTHESIS: label for the unit rodata strings used by the table error reporting.
extern const char lbl_80415D08[0x38];

int DWCi_TblStartRequestResultToError(int result, int* error);
int DWCi_TblRequestResultToAsyncResult(int result, int* error);
void DWCi_TblPrintf(int line, const char* format, ...);

int DWCi_TblHandleStartupResult(int result) {
    int code;
    int error;
    switch (result) {
    case 0:
        error = 0;
        code = 0;
        break;
    case 1:
        error = 8;
        code = 2;
        break;
    case 2:
        error = 8;
        code = 3;
        break;
    case 3:
        error = 9;
        code = 1;
        break;
    default:
        code = 17;
        break;
    }
    if (code != 0) {
        DWCi_TblPrintf(0x35, "DWCTblError[%d]\n", code);
        DWCi_SetError(error, -40000 - (code + 100));
    }
    return code;
}

int DWCi_TblHandleStartRequestResult(int id, int result) {
    int error;
    int code = DWCi_TblStartRequestResultToError(result, &error);
    if (code != 0) {
        DWCi_TblPrintf(0x45, "DWCTblError[%d]\n", code);
        DWCi_SetError(error, -40000 - (code + id * 100));
    }
    return code;
}

int DWCi_TblHandleRequestResult(int id, int result) {
    int error;
    int code = DWCi_TblRequestResultToAsyncResult(result, &error);
    if (code != 1) {
        DWCi_TblPrintf(0x55, lbl_80415D08 + 0x11, code);
        DWCi_SetError(error, -40000 - (code + id * 100 + 50));
    }
    return code;
}

int DWCi_TblHandleFileResult(int id, int result) {
    int code;
    int error = 20;
    switch (result) {
    case 0:
        error = 0;
        code = 1;
        break;
    case 1:
        code = 23;
        break;
    case 2:
        code = 24;
        break;
    case 3:
        code = 25;
        break;
    case 4:
        code = 26;
        break;
    case 5:
        code = 27;
        break;
    case 6:
        code = 28;
        break;
    default:
        code = 28;
        break;
    }
    if (code != 1) {
        DWCi_TblPrintf(0x65, lbl_80415D08 + 0x11, code);
        DWCi_SetError(error, -40000 - (code + id * 100 + 50));
    }
    return code;
}

void DWCi_TblPrintf(int line, const char* format, ...) {
    char buffer[256];
    va_list arguments;
    int length = snprintf(buffer, sizeof(buffer), lbl_80415D08 + 0x28, line);
    va_start(arguments, format);
    vsnprintf(buffer + length, sizeof(buffer) - length, format, arguments);
    va_end(arguments);
    DWC_Printf(0x8000000, buffer);
}

int DWCi_TblStartRequestResultToError(int result, int* error) {
    *error = 20;
    switch (result) {
    case 0:
        *error = 0;
        return 0;
    case 1:
        return 5;
    case 2:
        *error = 9;
        return 1;
    case 3:
        return 6;
    case 4:
        return 7;
    case 5:
        return 8;
    case 6:
        return 9;
    case 7:
        return 10;
    case 8:
        return 11;
    case 9:
        return 12;
    case 10:
        return 13;
    case 11:
        return 14;
    case 12:
        return 15;
    case 13:
        return 16;
    case 14:
        return 17;
    default:
        return 17;
    }
}

int DWCi_TblRequestResultToAsyncResult(int result, int* error) {
    *error = 20;
    switch (result) {
    case 0:
        *error = 0;
        return 1;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    case 4:
        return 5;
    case 5:
        return 6;
    case 6:
        *error = 9;
        return 7;
    case 7:
        return 8;
    case 8:
        return 9;
    case 9:
        return 10;
    case 10:
        return 11;
    case 11:
        return 12;
    case 12:
        return 13;
    case 13:
        return 14;
    case 14:
        return 15;
    case 15:
        return 16;
    case 16:
        return 17;
    case 17:
        return 18;
    case 18:
        return 19;
    case 19:
        return 20;
    case 20:
        return 21;
    case 21:
        return 22;
    default:
        return 22;
    }
}
