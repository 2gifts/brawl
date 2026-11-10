#include <algorithm>
#include <gf/gf_collection_io.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_heap_manager.h>
#include <mu/mu_collection.h>
#include <mu/mu_recorder.h>
#include <string.h>

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

// TEMP: part A (mu_recorder.cpp) owns the definition of Data; here only so that this part compiles on its own.
muRecorder::Data::~Data() {
    if (unk0) {
        gfHeapManager::free(unk0);
    }
    unk0 = 0;
}

void muRecorder::Data::reset(void* data) {
    if (unk0) {
        gfHeapManager::free(unk0);
    }
    unk0 = data;
}

namespace {
struct StringLess {
    bool operator()(const char* a, const char* b) const {
        return strcmp(a, b) < 0;
    }
};
} // namespace

char* muCollection::FileInfo::copyFileName(Heaps::HeapType heap) const {
    if (m_count == 0) {
        return 0;
    }
    u32 total = 0;
    const char** it;
    const char** names = (const char**)gfHeapManager::alloc(heap, m_count * sizeof(char*));
    it = names;
    for (u32 i = 0; i < m_count; i++) {
        *it = (char*)m_names.unk0 + total;
        total += strlen(*it) + 1;
        it++;
    }
    if (total == 0) {
        if (names) {
            gfHeapManager::free(names);
        }
        return 0;
    }
    std::sort(names, names + m_count, StringLess());
    char* out = (char*)gfHeapManager::alloc(heap, total);
    char* dst = out;
    it = names;
    for (u32 i = 0; i < m_count; i++) {
        const char* name = *it;
        u32 n = strlen(name);
        u32 len = n < 0x40 ? n : 0x40;
        memcpy(dst, name, len + 1);
        dst[len] = 0;
        dst += len + 1;
        it++;
    }
    if (names) {
        gfHeapManager::free(names);
    }
    return out;
}

muCollection::FileHandle::FileHandle(const char* name) {
    m_name[0] = 0;
    strcat(m_name, lbl_8059E148);
    m_name[0] = 0;
    strcat(m_name, name);
}

muCollection::FileHandle::FileHandle(const char* names, int index) {
    m_name[0] = 0;
    strcat(m_name, lbl_8059E148);
    for (int i = 0; i < index; i++) {
        names += strlen(names) + 1;
    }
    m_name[0] = 0;
    strcat(m_name, names);
}

static inline int getBaseKind(int kind) {
    switch (kind) {
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    default:
        return 0;
    }
}

muCollection::muCollection(int kind, nw4r::g3d::ResFile modelRes, nw4r::g3d::ResFile msgRes, void* msgData,
                           void* scnRoot, int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2, u8 unk0,
                           u8 unk2)
    : muRecorder(getBaseKind(kind), modelRes, msgRes, msgData, scnRoot, drawPriority, heap, heap2, unk0, unk2,
                 "muCollection"),
      m_io(new (m_heap2) gfCollectionIO(m_heap2)), m_ioPending(false), m_fileType(kind), m_loadSize(0), m_busy(0),
      m_result(2) {
    unk2C_b1 = false;
}

muCollection::FileInfo::FileInfo() : unk0(0), unk1(0), m_count(0), m_free(0) {
}

muCollection::FileInfo::~FileInfo() {
}

muCollection::~muCollection() {
    m_io->isDropReady(); // HYPOTHESIS: the result is not used
}

bool muCollection::isDropReady() {
    bool ready = false;
    if (m_busy == 0 && m_io->isDropReady()) {
        ready = true;
    }
    return ready;
}
