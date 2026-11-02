#include "file_struct.h"

#pragma exceptions on

void __prep_buffer(FILE *pFile) {
    pFile->buffer_ptr = pFile->buffer;
    pFile->buffer_len = pFile->buffer_size;
    pFile->buffer_len -= pFile->pos & pFile->buffer_alignment;
    pFile->buffer_pos = pFile->pos;
}

int __load_buffer(FILE *pFile, size_t *pLoaded, int alignment) {
    int res;
    long n;
    unsigned char* p;

    __prep_buffer(pFile);

    if (alignment == 1) {
        pFile->buffer_len = pFile->buffer_size;
    }

    res = (*pFile->read_proc)(pFile->handle, pFile->buffer, (size_t*)&pFile->buffer_len, pFile->ref);

    if (res == 2) {
        pFile->buffer_len = 0;
    }

    if (pLoaded != 0) {
        *pLoaded = pFile->buffer_len;
    }

    if (res != 0) {
        return res;
    }

    pFile->pos += pFile->buffer_len;

    if (!pFile->mode.binary) {
        n = pFile->buffer_len;
        p = pFile->buffer;

        while (n--) {
            if (*p++ == '\n') {
                pFile->pos++;
            }
        }
    }

    return 0;
}

int __flush_buffer(FILE *pFile, size_t *pFlushed) {
    size_t len;
    int res;

    len = pFile->buffer_ptr - pFile->buffer;

    if (len != 0) {
        pFile->buffer_len = len;

        res = (*pFile->write_proc)(pFile->handle, pFile->buffer, (size_t*)&pFile->buffer_len, pFile->ref);
    
        if (pFlushed != 0) {
            *pFlushed = pFile->buffer_len;
        }

        if (res != 0) {
            return res;
        }

        pFile->pos += pFile->buffer_len;
    }

    __prep_buffer(pFile);
    return 0;
}
