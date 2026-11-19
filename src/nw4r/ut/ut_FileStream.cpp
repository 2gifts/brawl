#include <types.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace ut {

NW4R_UT_RTTI_DEF_DERIVED(FileStream, IOStream);

void FileStream::Cancel() {}

bool FileStream::CancelAsync(StreamCallback pCallback, void* pCallbackArg) {
#pragma unused(pCallback)
#pragma unused(pCallbackArg)
    return true;
}

u32 FileStream::FilePosition::Skip(s32 offset) {
    if (offset != 0) {
        s64 newOffset = mPosition + offset;
        mPosition = Clamp<s64>(newOffset, 0, mFileSize);
    }

    return mPosition;
}

u32 FileStream::FilePosition::Append(s32 offset) {
    s64 newOffset = mPosition + offset;

    if (newOffset < 0) {
        mPosition = 0;
    } else {
        mPosition = newOffset;
        mFileSize = Max(mPosition, mFileSize);
    }

    return mPosition;
}

void FileStream::FilePosition::Seek(s32 offset, u32 origin) {
    switch (origin) {
    case SEEK_ORIGIN_BEG: {
        mPosition = 0;
        break;
    }

    case SEEK_ORIGIN_END: {
        mPosition = mFileSize;
        break;
    }

    case SEEK_ORIGIN_CUR:
    default: {
        break;
    }
    }

    Skip(offset);
}

DECOMP_FORCEACTIVE(ut_FileStream_cpp,
                   FileStream::GetRuntimeTypeInfo);

} // namespace ut
} // namespace nw4r

// TODO: clean up, give variables real names
namespace jj_8015CD80 {
typedef unsigned char u8;

struct Sub {
    u8 pad[0x1C];
    void (*fn)(void*, Sub*, void*);
    void* ptr;
    u8 flag;
};

struct Outer {
    u8 pad[0x3C];
    Sub* sub;
};

// TODO: clean up, give variables real names
extern "C" void fn_8015CD80(void* a, Outer* b) {
    Sub* sub = b->sub;
    sub->flag = 0;
    if (sub->fn) {
        sub->fn(a, sub, sub->ptr);
    }
}
}

namespace jj_8015CD54 {
typedef void (*FnPtr)(void*, void*, void*);

struct Inner {
    unsigned char pad0[8];
    void* a8;
    FnPtr fn;
    void* c10;
    unsigned char pad1[0x6c - 0x14];
    unsigned char flag6c;
};

struct Outer {
    unsigned char pad0[0x3c];
    Inner* inner;
};

// TODO: clean up, give variables real names
extern "C" void fn_8015CD54(void* a, Outer* r4) {
    Inner* m = r4->inner;
    m->flag6c = 0;
    m->a8 = a;
    if (m->fn == 0) {
        return;
    }
    m->fn(a, m, m->c10);
}
}

