#include "cctype"
#include "file_struct.h"

#pragma exceptions on

int fflush(FILE*);
void free(void*);
int __flush_all(void);
long ftell(FILE*);
int __flush_buffer(FILE*, size_t*);
size_t strlen(const char*);

int fclose(FILE* pFile) {
    int flush_res, close_res;

    if (pFile == 0) {
        return -1;
    }

    if (pFile->mode.file == 0) {
        return 0;
    }

    flush_res = fflush(pFile);
    close_res = (*pFile->close_proc)(pFile->handle);
    pFile->mode.file = 0;
    pFile->handle = 0;

    if (pFile->state.free_buffer) {
        free(pFile->buffer);
    }

    return ((flush_res || close_res) ? -1 : 0);
}

int fflush(FILE* pFile) {
    long pos;

    if (pFile == 0) {
        return __flush_all();
    }

    if (pFile->state.error || pFile->mode.file == 0) {
        return -1;
    }

    if (pFile->mode.io == 1) {
        return 0;
    }

    if (pFile->state.io_state >= 3) {
        pFile->state.io_state = 2;
    }

    if (pFile->state.io_state == 2) {
        pFile->buffer_len = 0;
    }

    if (pFile->state.io_state != 1) {
        pFile->state.io_state = 0;
        return 0;
    }

    if (pFile->mode.file != 1) {
        pos = 0;
    } else {
        pos = ftell(pFile);
    }

    if (__flush_buffer(pFile, 0)) {
        pFile->state.error = 1;
        pFile->buffer_len = 0;
        return -1;
    }

    pFile->state.io_state = 0;
    pFile->pos = pos;
    pFile->buffer_len = 0;
    return 0;
}

int __msl_strnicmp(const char* pStr1, const char* pStr2, int n) {
    int i;
    char c1, c2;

    for (i = 0; i < n; i++) {
        c1 = tolower(*pStr1++);
        c2 = tolower(*pStr2++);

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }

        if (!c1) {
            return 0;
        }
    }

    return 0;
}

char* __msl_itoa(int val, char* str, unsigned int radix) {
    int neg;
    int i;
    char c;
    int start;
    int end;

    neg = 0;
    i = 0;

    if (val < 0) {
        val = -val;
        neg = 1;
    }

    do {
        int digit = val % radix;

        if (digit > 9) {
            str[i++] = digit + 0x37;
        }
        else {
            str[i++] = digit + 0x30;
        }

        val /= radix;
    } while (val != 0);

    if (neg != 0) {
        str[i++] = '-';
    }

    str[i++] = '\0';

    start = 0;
    end = strlen(str) - 1;

    while (start < end) {
        c = str[start];
        str[start++] = str[end];
        str[end--] = c;
    }

    return str;
}
