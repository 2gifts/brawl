#include <so/so_archive_db.h>
#include <types.h>

// utArchiveManager members of main.dol that symbols.txt has not named yet (sora.map names in the comments).
extern "C" {
// utArchiveManager::reqLoad
u32 fn_800453AC(utArchiveManager* manager, const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id,
                utArchiveEventObserver* observer, bool async);
// utArchiveManager::addNoManageArchive
u32 fn_8004570C(utArchiveManager* manager, void* data, int size, Heaps::HeapType heap);
// utArchiveManager::process
void fn_800457F4(utArchiveManager* manager);
// utArchiveManager::unload
void fn_80045E64(utArchiveManager* manager, u32 resourceId);
// utArchiveManager::cancel
void fn_800461D4(utArchiveManager* manager, u32 resourceId);
}

// HYPOTHESIS: the archive manager table; dtk's 0x18-byte object also covers the manager count and the Processer.
utArchiveManager* lbl_27_bss_120[3];

utArchiveManager* soArchiveDb::getManager(u32 index) {
    return lbl_27_bss_120[index];
}

soArchiveItem::soArchiveItem(u32 index) {
    m_index = index;
    m_resourceId = 0xFFFF;
}

void soArchiveItem::init() {
    m_resourceId = 0xFFFF;
}

void soArchiveItem::reqLoad(const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id, bool async) {
    m_resourceId = fn_800453AC(lbl_27_bss_120[m_index], path, heap, mode, offset, size, id, &lbl_805A00C0, async);
}

void soArchiveItem::add(void* data, int size, Heaps::HeapType heap) {
    m_resourceId = fn_8004570C(lbl_27_bss_120[m_index], data, size, heap);
}

void soArchiveItem::resolve(u32 emCommonArcId) {
    lbl_27_bss_120[m_index]->resolveReference(m_resourceId, emCommonArcId, 0, 0, 0, 0);
}

void soArchiveItem::unload(int cancel) {
    if (m_resourceId != 0xFFFF) {
        if (cancel == 0) {
            fn_80045E64(lbl_27_bss_120[m_index], m_resourceId);
        } else {
            fn_800461D4(lbl_27_bss_120[m_index], m_resourceId);
        }
        m_resourceId = 0xFFFF;
    }
}

bool soArchiveItem::isExistEffect() {
    return lbl_27_bss_120[m_index]->isExistEffect(m_resourceId);
}

nw4r::g3d::ResFileData* soArchiveItem::getResFile(ARCNodeType type, u32 p2, u32 p3, u32 p4) {
    return lbl_27_bss_120[m_index]->getResFileFromId(m_resourceId, type, p2, p3, p4);
}

void* soArchiveItem::getData(ARCNodeType type, int fileIndex, int* size, int groupID, s32 archiveId) {
    return lbl_27_bss_120[m_index]->getData(m_resourceId, type, fileIndex, size, groupID, archiveId);
}

void* soArchiveItem::getData(const char* nodeName) {
    return lbl_27_bss_120[m_index]->getPublicAddress(m_resourceId, nodeName, 0, 0, -1);
}

void soArchiveDb::Processer::processBegin() {
    int index = 0;
    utArchiveManager** manager = lbl_27_bss_120;
    do {
        if (*manager != nullptr) {
            fn_800457F4(*manager);
        }
        ++index;
        ++manager;
    } while (index < 3);
}

// TODO: clean up, give variables real names
namespace jj_70004 {
extern "C" void* lbl_27_bss_120[];
extern "C" int getLoadResult__16utArchiveManagerFUlPl(void*, unsigned int, int);

struct Data {
    unsigned int index;
    unsigned int id;
};

extern "C" bool fn_27_70004(Data self) {
    unsigned int index = self.index;
    return getLoadResult__16utArchiveManagerFUlPl(lbl_27_bss_120[index], self.id, 0) == 1;
}
}

// TODO: clean up, give variables real names
namespace jj_70070 {
enum ARCNodeType { ARCNodeType_0 };
class utArchiveManager {
public:
    void* getResFileFromId(long id, ARCNodeType t, unsigned long a, unsigned long b, unsigned long c);
};
extern void* lbl_27_bss_120[];

struct Ref { unsigned int idx; long id; };

extern "C" void* fn_27_70070(Ref r, ARCNodeType t, unsigned long a, unsigned long b, unsigned long c) {
    return ((utArchiveManager*)lbl_27_bss_120[r.idx])->getResFileFromId(r.id, t, a, b, c);
}
}

