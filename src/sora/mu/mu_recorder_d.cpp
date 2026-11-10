#include <gf/gf_heap_manager.h>
#include <mu/mu_collection.h>
#include <mu/mu_recorder.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/OS/OSCache.h>
#include <string.h>

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

// HYPOTHESIS: the jpeg decoder (stand-ins; the context is the stack of the decoder).
struct muJpegSize {
    u32 m_width;
    u32 m_height;
    u32 m_size;
};
struct muJpegInfo {
    u32 unk0;
    u32 m_jpegSize;     // HYPOTHESIS
    u32 m_jpegData;     // HYPOTHESIS
};
extern "C" void fn_80260730(void* context, u32 jpegData, u32 jpegSize, int format, int unk, int unk2, muJpegSize* out);
extern "C" void fn_802608BC(void* context, u32 jpegData, u32 jpegSize, void* pixels, u32 size, int format, int unk,
                            int unk2);

// TEMP: part A (mu_recorder.cpp) owns the definition of Data::reset; here only so that this part inlines it.
void muRecorder::Data::reset(void* data) {
    if (unk0) {
        gfHeapManager::free(unk0);
    }
    unk0 = data;
}

void* muCollection::lock() {
    m_busy = 1;
    return m_io;
}

// HYPOTHESIS: processDefault takes the three words as (drive, data, size); see the note at muCollection::m_argFlag.
void muCollection::write(void* data, int size, int unk) {
    m_argFlag = 0;
    m_argData = data;
    m_writeName = 0;
    m_argSize = size;
    m_argUnk = unk;
    m_busy = 4;
}

void muCollection::overwriteIf1000(FileKey key, int size, int unk) {
    s32 id = key.m_drive;
    m_keyName.m_name[0] = 0;
    strcat(m_keyName.m_name, key.m_name);
    m_argFlag = 1;
    m_argId = id;
    m_writeName = m_keyName.m_name;
    m_argSize = size;
    m_argUnk = unk;
    m_busy = 4;
}

void muCollection::read(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 5;
    }
}

void muCollection::copy(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 7;
    }
}

void muCollection::erase(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        FileName fileName;
        fileName.m_name[0] = 0;
        strcat(fileName.m_name, name.m_name);
        m_readName = fileName;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 6;
    }
}

void muCollection::erase(const FileKey& key) {
    s32 id = key.m_drive;
    FileName fileName;
    fileName.m_name[0] = 0;
    strcat(fileName.m_name, key.m_name);
    m_readName = fileName;
    m_argDrive = id;
    m_argData = m_readName.m_name;
    m_busy = 6;
}

void muCollection::move(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 8;
    }
}

void muCollection::readSummary(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_summaryName = name;
        m_argDrive = drive;
        m_argData = m_summaryName.m_name;
        m_busy = 9;
    }
}

void muCollection::checkLimit(int drive) {
    m_argDrive = drive;
    m_busy = 3;
}

void muCollection::scanAll(int unk, int unk2) {
    m_scanDrive0 = 1;
    m_scanDrive1 = 1;
    m_scanFlagA = unk;
    m_scanFlagB = unk2;
    m_busy = 2;
}

void muCollection::scan(int drive, int unk, int unk2) {
    m_scanDrive0 = drive == 0;
    m_scanDrive1 = drive == 1;
    m_scanFlagA = unk;
    m_scanFlagB = unk2;
    m_busy = 2;
}

void muCollection::unlock() {
    m_busy = 0;
}

muCollection::Summary::Decoded muCollection::Summary::decodeJpeg(Heaps::HeapType heap, int unk) const {
    const muJpegInfo* info = static_cast<const muJpegInfo*>(m_jpegInfo);
    u32 jpegData = info->m_jpegData;
    u32 jpegSize = info->m_jpegSize;
    u8 context[0x1C00];
    muJpegSize jpeg;
    fn_80260730(context, jpegData, jpegSize, 6, unk, 0, &jpeg);
    Decoded result;
    u32 size = jpeg.m_size;
    result.m_pixels.reset(gfHeapManager::alloc(heap, size, 0x20));
    result.m_width = jpeg.m_width;
    result.m_height = jpeg.m_height;
    DCInvalidateRange(result.m_pixels.unk0, size);
    fn_802608BC(context, jpegData, jpegSize, result.m_pixels.unk0, size, 6, unk, 0);
    DCStoreRange(result.m_pixels.unk0, size);
    GXInvalidateTexAll();
    return result;
}
