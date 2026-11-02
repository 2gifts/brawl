#include "NMWException.h"

// The std handlers and __throw_catch_compare are adapted from zeldaret/ss, the array helpers from Petari.

typedef void (*unexpected_handler)();
unexpected_handler set_unexpected(unexpected_handler handler);
void unexpected();

typedef void (*terminate_handler)();
terminate_handler set_terminate(terminate_handler handler);
void terminate();

extern "C" {
extern void abort();
}

namespace std {
static void dthandler() { abort(); }

static terminate_handler thandler = dthandler;

extern terminate_handler set_terminate(terminate_handler handler) {
    terminate_handler old = thandler;
    thandler              = handler;
    return old;
}

extern void terminate() { thandler(); }

static void duhandler() { terminate(); }

static unexpected_handler uhandler = duhandler;

extern unexpected_handler set_unexpected(unexpected_handler handler) {
    unexpected_handler old = uhandler;
    uhandler               = handler;
    return old;
}

extern void unexpected() { uhandler(); }
} // namespace std

extern "C" char __throw_catch_compare(const char* throwtype, const char* catchtype, long* offset_result) {
    const char *cptr1, *cptr2;

    *offset_result = 0;

    if ((cptr2 = catchtype) == 0) {
        return true;
    }

    cptr1 = throwtype;

    if (*cptr2 == 'P') {
        cptr2++;
        if (*cptr2 == 'C')
            cptr2++;
        if (*cptr2 == 'V')
            cptr2++;
        if (*cptr2 == 'v') {
            if (*cptr1 == 'P' || *cptr1 == '*') {
                return true;
            }
        }
        cptr2 = catchtype;
    }

    switch (*cptr1) {
    case '*':
    case '!':
        if (*cptr1++ != *cptr2++)
            return false;
        for (;;) {
            if (*cptr1 == *cptr2++) {
                if (*cptr1++ == '!') {
                    long offset;

                    for (offset = 0; *cptr1 != '!';) {
                        offset = offset * 10 + *cptr1++ - '0';
                    }
                    *offset_result = offset;
                    return true;
                }
            } else {
                while (*cptr1++ != '!') { }
                while (*cptr1++ != '!') { }
                if (*cptr1 == 0)
                    return false;

                cptr2 = catchtype + 1;
            }
        }
        return false;
    }

    while ((*cptr1 == 'P' || *cptr1 == 'R') && *cptr1 == *cptr2) {
        cptr1++;
        cptr2++;

        if (*cptr2 == 'C') {
            if (*cptr1 == 'C')
                cptr1++;
            cptr2++;
        }
        if (*cptr1 == 'C')
            return false;

        if (*cptr2 == 'V') {
            if (*cptr1 == 'V')
                cptr1++;
            cptr2++;
        }
        if (*cptr1 == 'V')
            return false;
    }

    for (; *cptr1 == *cptr2; cptr1++, cptr2++) {
        if (*cptr1 == 0)
            return true;
    }

    return false;
}

class __partial_array_destructor {
public:
    /* 0x00 */ void* mArrayStart;
    /* 0x04 */ size_t mElemSize;
    /* 0x08 */ size_t mArraySize;
    /* 0x0C */ ctor_dtor_ptr mDtorPtr;
    /* 0x10 */ size_t mCurElement;

    __partial_array_destructor(void* arr, unsigned int size, unsigned int count, ctor_dtor_ptr ptr) {
        mArrayStart = arr;
        mElemSize = size;
        mArraySize = count;
        mDtorPtr = ptr;
        mCurElement = mArraySize;
    }

    ~__partial_array_destructor() {
        if (mCurElement < mArraySize && mDtorPtr) {
            for (char* cur = (char*)mArrayStart + (mElemSize * mCurElement); mCurElement > 0; mCurElement--) {
                cur -= mElemSize;
                ((void (*)(void*, short))mDtorPtr)(cur, -1);
            }
        }
    }
};

extern "C" {

void* __construct_new_array(void* block, ctor_dtor_ptr ctor, ctor_dtor_ptr dtor, size_t size, size_t n) {
    char* ptr = (char*)block;

    if (ptr != 0) {
        *(size_t*)ptr = size;
        *(size_t*)(ptr + 4) = n;
        ptr += 0x10;

        if (ctor) {
            __partial_array_destructor pad(ptr, size, n, dtor);

            char* p;
            for (pad.mCurElement = 0, p = ptr; pad.mCurElement < n; pad.mCurElement++, p += size) {
                ((void (*)(void*, short))ctor)(p, 1);
            }
        }
    }
    return ptr;
}

void __construct_array(void* pBlock, ctor_dtor_ptr ctor, ctor_dtor_ptr dtor, size_t size, size_t n) {
    __partial_array_destructor pad(pBlock, size, n, dtor);

    char* p = (char*)pBlock;
    for (pad.mCurElement = 0; pad.mCurElement < n; p += size, pad.mCurElement++) {
        ((void (*)(void*, short))ctor)(p, 1);
    }
}

void __destroy_arr(void* pArraySource, ctor_dtor_ptr dtor, size_t size, size_t num) {
    char* cur;

    for (cur = (char*)pArraySource + (size * num); num > 0; num--) {
        cur -= size;
        ((void (*)(void*, short))dtor)(cur, -1);
    }
}

void __destroy_new_array(void* pArraySource, ctor_dtor_ptr dtor) {
    if (pArraySource != 0) {
        if (dtor != 0) {
            size_t i, objs, obj_size;
            char* cur;

            // why are the 8 and 12 important to match?
            obj_size = *(size_t*)((char*)pArraySource - 2 * 8);
            objs = *(size_t*)((char*)pArraySource - 12);
            cur = (char*)pArraySource + obj_size * objs;

            for (i = 0; i < objs; i++) {
                cur -= obj_size;
                ((void (*)(void*, short))dtor)(cur, -1);
            }
        }

        ::operator delete[]((char*)pArraySource - 2 * 8);
    }
}
}
