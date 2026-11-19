#include <string.h>
#include <cstring>
#include <gf/gf_archive.h>
#include <gf/gf_archive_db.h>
#include <memory.h>
#include <revolution/OS/OSError.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_archive_manager.h>
#include <ut/ut_list.h>
#include <ut/ut_relocate.h>

// HYPOTHESIS: view of the MSL std::string that gfArchive::getName/getPath return by value
// (the top bit of the first word selects the heap text at +8, short text starts at +1).
struct gfArchiveString {
    u32 m_word;
    u32 m_size;
    char* m_heapText;

    const char* c_str() const {
        if (m_word >> 31) {
            return m_heapText;
        }
        return reinterpret_cast<const char*>(this) + 1;
    }
    ~gfArchiveString() {
        if (m_word >> 31) {
            delete m_heapText;
        }
    }
};

extern "C" {
// HYPOTHESIS: unnamed gfArchive entry points (map: gfArchive __ct/getName/getPath/readFile/
// readFileRequest/cancelRequest/isCanceled/getReturnStatus/isExistEffect); types from register use.
gfArchive* fn_800137C0(gfArchive* archive, Heaps::HeapType heap, gfArchive* source);
gfArchiveString fn_800146F0(const char* path);
gfArchiveString fn_80014480(const char* path);
u32 fn_80014ED4(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset);
u32 fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);
u32 fn_8001583C(gfArchive* archive);
u32 fn_8001588C(gfArchive* archive);
s32 fn_80015DAC(gfArchive* archive);
}

utArchiveManager::utArchiveManager(u32 taskCount) : m_database(nullptr), m_taskCount(taskCount) {
    m_freeList.m_head = m_freeList.m_tail = nullptr;
    m_freeList.m_length = 0;
    m_loadingList.m_head = m_loadingList.m_tail = nullptr;
    m_loadingList.m_length = 0;
    m_loadedList.m_head = m_loadedList.m_tail = nullptr;
    m_loadedList.m_length = 0;
    m_removeList.m_head = m_removeList.m_tail = nullptr;
    m_removeList.m_length = 0;
    m_cancelList.m_head = m_cancelList.m_tail = nullptr;
    m_cancelList.m_length = 0;
    m_database = gfArchiveDatabase::create(taskCount, Heaps::System);
    m_tasks = new (Heaps::System) LoadTask[taskCount];
    for (u32 i = 0; i < taskCount; i++) {
        m_tasks[i].clear();
        m_freeList.addTail(&m_tasks[i]);
    }
}

// HYPOTHESIS: inline task release (no out-of-line copy exists); it unlinks the task, drops the
// database reference and returns the task to the free list.
inline void utArchiveManager::releaseTask(LoadTask* task) {
    if (m_removeList.isExistNode(task) != -1) {
        m_removeList.remove(task);
    }
    if (m_loadedList.isExistNode(task) != -1) {
        m_loadedList.remove(task);
    }
    if (m_loadingList.isExistNode(task) != -1) {
        m_loadingList.remove(task);
    }
    if (m_cancelList.isExistNode(task) != -1) {
        m_cancelList.remove(task);
    }
    if (task->m_archive->m_refCount <= 0) {
        OSReport("already release %d \n", task->m_archive->m_archiveFilePath);
    }
    m_database->release(task->m_resourceId);
    task->clear();
    m_freeList.addTail(task);
}

utArchiveManager::~utArchiveManager() {
    while (m_removeList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_removeList.getNode(0)));
    }
    while (m_loadingList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_loadedList.getNode(0)));
    }
    while (m_loadedList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_loadedList.getNode(0)));
    }
    while (m_cancelList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_cancelList.getNode(0)));
    }
    delete[] m_tasks;
    m_tasks = nullptr;
    delete m_database;
    m_database = nullptr;
}

utArchiveManager::LoadTask* utArchiveManager::isExist(utList* list, const char* path) {
    for (LoadTask* task = static_cast<LoadTask*>(list->getNode(0)); task != nullptr;
         task = static_cast<LoadTask*>(task->m_next)) {
        if (strcmp(fn_800146F0(task->m_archive->m_archiveFilePath).c_str(), fn_800146F0(path).c_str()) == 0) {
            if (strcmp(fn_80014480(task->m_archive->m_archiveFilePath).c_str(), fn_80014480(path).c_str()) == 0) {
                return task;
            }
        }
    }
    return nullptr;
}

u32 utArchiveManager::reqLoad(const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id,
                              utArchiveEventObserver* observer, bool async) {
    LoadTask* source = nullptr;
    LoadTask* found = nullptr;
    if (path != nullptr) {
        found = isExist(&m_loadedList, path);
        if (found == nullptr) {
            found = isExist(&m_loadingList, path);
        }
    }
    if (found != nullptr) {
        if (mode == 1) {
            source = found;
        } else {
            found->m_refCount++;
            return found->m_resourceId;
        }
    }
    LoadTask* task = static_cast<LoadTask*>(m_freeList.getNode(0));
    if (task != nullptr) {
        m_freeList.remove(task);
        u32 sourceId = 0xFFFF;
        task->init();
        u32 result = 0;
        gfArchive* archive;
        if (source == nullptr) {
            archive = new (heap) gfArchive();
            if (strstr(path, ".lst") == nullptr) {
                if (async == true) {
                    result = fn_800151FC(archive, path, heap, offset, size, id);
                } else {
                    result = fn_80014ED4(archive, path, heap, offset);
                }
            }
        } else {
            archive = static_cast<gfArchive*>(operator new(sizeof(gfArchive), heap));
            if (archive != nullptr) {
                archive = fn_800137C0(archive, heap, source->m_archive);
            }
            sourceId = source->m_resourceId;
            result = 1;
        }
        if (result == 1) {
            task->m_resourceId = m_database->add(archive, true);
            task->m_archive = archive;
            task->m_sourceId = sourceId;
            if (async == true) {
                m_loadingList.addTail(task);
            } else {
                m_loadedList.addTail(task);
            }
            return task->m_resourceId;
        }
        OSReport("#failed utArchiveManager::reqLoad [%s] \n", path);
        delete archive;
        return 0xFFFF;
    }
    return 0xFFFF;
}

inline utArchiveManager::LoadTask* utArchiveManager::getTask(gfArchive* archive) {
    if (archive != nullptr) {
        for (u32 i = 0; i < m_taskCount; i++) {
            LoadTask* task = &m_tasks[i];
            if (task->m_resourceId != 0xFFFF) {
                gfArchive* other = m_database->get(task->m_resourceId);
                if (other != nullptr) {
                    if (other == archive) {
                        return task;
                    }
                } else {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}

// HYPOTHESIS: utArchiveManager::addNoManageArchive(gfArchive*); em_manager.cpp still calls it by this name.
extern "C" u32 fn_800455E0(utArchiveManager* manager, gfArchive* archive) {
    utArchiveManager::LoadTask* found = manager->getTask(archive);
    if (found != nullptr) {
        found->m_refCount++;
        return found->m_resourceId;
    }
    utArchiveManager::LoadTask* task = static_cast<utArchiveManager::LoadTask*>(manager->m_freeList.getNode(0));
    if (task != nullptr) {
        manager->m_freeList.remove(task);
        task->init();
        task->m_resourceId = manager->m_database->add(archive, false);
        task->m_archive = archive;
        manager->m_loadedList.addTail(task);
        return task->m_resourceId;
    }
    return 0xFFFF;
}

u32 utArchiveManager::addNoManageArchive(void* data, int size, Heaps::HeapType heap) {
    LoadTask* task = static_cast<LoadTask*>(m_freeList.getNode(0));
    if (task != nullptr) {
        m_freeList.remove(task);
        gfArchive* archive = new (heap) gfArchive();
        archive->setFileImage(data, size, heap);
        task->init();
        task->m_resourceId = m_database->add(archive, false);
        task->m_archive = archive;
        m_loadedList.addTail(task);
        return task->m_resourceId;
    }
    return 0xFFFF;
}

// HYPOTHESIS: inline lookup of a copy task that still waits for its source archive.
inline utArchiveManager::LoadTask* utArchiveManager::getWaitingCopy(utList* list, u32 sourceId) {
    for (utArchiveManager::LoadTask* task = static_cast<utArchiveManager::LoadTask*>(list->getNode(0));
         task != nullptr; task = static_cast<utArchiveManager::LoadTask*>(task->m_next)) {
        if (task->m_sourceId == sourceId && !task->m_archive->unk9) {
            return task;
        }
    }
    return nullptr;
}

// HYPOTHESIS: inline reference drop shared by unload and process.
inline void utArchiveManager::unloadTask(LoadTask* task) {
    if (task->m_refCount != 0) {
        if (--task->m_refCount == 0) {
            if (m_loadingList.isExistNode(task) != -1 && !task->m_archive->unk9) {
                m_loadingList.remove(task);
                m_removeList.addTail(task);
            } else {
                releaseTask(task);
            }
        }
    } else {
        releaseTask(task);
    }
}

void utArchiveManager::process() {
    LoadTask* task = static_cast<LoadTask*>(m_loadingList.m_head);
    while (task != nullptr) {
        LoadTask* next = static_cast<LoadTask*>(task->m_next);
        if (task->m_archive->unk9 == true) {
            s32 result = 2;
            if (fn_80015DAC(task->m_archive) == 0) {
                result = 1;
            }
            task->m_observer->notifyComp(task->m_resourceId, result);
            task->m_sourceId = 0xFFFF;
            m_loadingList.remove(task);
            m_loadedList.addTail(task);
        }
        task = next;
    }

    task = static_cast<LoadTask*>(m_removeList.m_head);
    while (task != nullptr) {
        LoadTask* next = static_cast<LoadTask*>(task->m_next);
        bool waiting = false;
        if (getWaitingCopy(&m_loadingList, task->m_resourceId) != nullptr ||
            getWaitingCopy(&m_removeList, task->m_resourceId) != nullptr) {
            waiting = true;
        }
        if (!waiting && task->m_archive->unk9 == true) {
            unloadTask(task);
        }
        task = next;
    }

    task = static_cast<LoadTask*>(m_cancelList.m_head);
    while (task != nullptr) {
        LoadTask* next = static_cast<LoadTask*>(task->m_next);
        if (task->m_archive->unk9 == true || fn_8001588C(task->m_archive) == 1) {
            releaseTask(task);
        }
        task = next;
    }
}

// HYPOTHESIS: inline lookup of a list's task by resource id.
inline utArchiveManager::LoadTask* utArchiveManager::getListTask(utList* list, u32 resourceId) {
    if (resourceId != 0xFFFF) {
        for (utArchiveManager::LoadTask* task = static_cast<utArchiveManager::LoadTask*>(list->getNode(0));
             task != nullptr; task = static_cast<utArchiveManager::LoadTask*>(task->m_next)) {
            if (task->m_resourceId == resourceId) {
                return task;
            }
        }
    }
    return nullptr;
}

s32 utArchiveManager::getLoadResult(u32 resourceId, s32* status) {
    if (resourceId != 0xFFFF) {
        utArchiveManager::LoadTask* task = getListTask(&m_loadedList, resourceId);
        if (task != nullptr) {
            s32 result = fn_80015DAC(task->m_archive);
            if (status != nullptr) {
                *status = result;
            }
            s32 ret = 2;
            if (result == 0) {
                ret = 1;
            }
            return ret;
        }
    }
    return 0;
}

u32 utArchiveManager::isExistRemoveTask() {
    return m_removeList.getLength() != 0;
}

u32 utArchiveManager::isExistCancelTask() {
    return m_cancelList.getLength() != 0;
}

// HYPOTHESIS: inline lookup of a task slot by resource id.
inline utArchiveManager::LoadTask* utArchiveManager::getTask(u32 resourceId) {
    if (resourceId != 0xFFFF) {
        for (u32 i = 0; i < m_taskCount; i++) {
            if (resourceId == m_tasks[i].m_resourceId) {
                return &m_tasks[i];
            }
        }
    }
    return nullptr;
}

// HYPOTHESIS: utArchiveManager::unload(u32); em_manager.cpp still calls it by this name.
extern "C" void fn_80045E64(utArchiveManager* manager, u32 resourceId) {
    utArchiveManager::LoadTask* task = manager->getTask(resourceId);
    if (task != nullptr) {
        manager->unloadTask(task);
    }
}

void utArchiveManager::cancel(u32 resourceId) {
    LoadTask* task = getTask(resourceId);
    if (task != nullptr) {
        if (task->m_refCount != 0) {
            if (--task->m_refCount == 0) {
                if (m_loadingList.isExistNode(task) != -1 && !task->m_archive->unk9) {
                    m_loadingList.remove(task);
                    if (fn_8001583C(task->m_archive) == 1) {
                        m_cancelList.addTail(task);
                        return;
                    }
                }
                releaseTask(task);
            }
        } else {
            releaseTask(task);
        }
    }
}

void* utArchiveManager::getData(u32 resourceId, ARCNodeType type, int fileIndex, int* size, int groupID, s32 archiveId) {
    gfArchive* archive = m_database->get(resourceId);
    if (archive != nullptr) {
        if (archiveId == -1) {
            return archive->getData(groupID, type, fileIndex, size, 0xFFFE);
        }
        return archive->getData(groupID, type, fileIndex, size, archiveId);
    }
    return nullptr;
}

nw4r::g3d::ResFileData* utArchiveManager::getResFileFromId(s32 p1, ARCNodeType type, u32 p3, u32 p4, u32 p5) {
    int size = 0;
    return static_cast<nw4r::g3d::ResFileData*>(getData(p1, type, p3, &size, p4, p5));
}

void* utArchiveManager::getPublicAddress(u32 paramArcId, const char* nodeName, u16 fileIndex, u32 groupID, u32 endian) {
    int size = 0;
    u8* data = static_cast<u8*>(getData(paramArcId, Data_Type_Misc, fileIndex, &size, groupID, endian));
    if (data != nullptr) {
        utRelocate relocate(data, size);
        return relocate.getPublicAddress(nodeName);
    }
    return nullptr;
}

void utArchiveManager::resolveReference(u32 paramArcId, u32 emCommonArcId, u32 p3, u32 p4, u32 p5, u32 p6) {
    int size = 0;
    utRelocate relocate(static_cast<u8*>(getData(paramArcId, Data_Type_Misc, p3, &size, p5, -1)), size);
    int commonSize = 0;
    utRelocate common(static_cast<u8*>(getData(emCommonArcId, Data_Type_Misc, p4, &commonSize, p6, -1)), commonSize);
    relocate.resolveReference(&common);
}

void utArchiveManager::resolveReferenceExternalData(u8* data, u32 size, u32 resourceId, u32 fileIndex, u32 groupID) {
    utRelocate relocate(data, size);
    int externalSize = 0;
    utRelocate external(static_cast<u8*>(getData(resourceId, Data_Type_Misc, fileIndex, &externalSize, groupID, -1)),
                        externalSize);
    relocate.resolveReference(&external);
}

bool utArchiveManager::isExistEffect(u32 resourceId) {
    if (resourceId != 0xFFFF) {
        gfArchive* archive = m_database->get(resourceId);
        if (archive != nullptr) {
            return archive->isExistEffect();
        }
    }
    return false;
}

utArchiveEventObserverNull lbl_805A00C0;

// TODO: clean up, give variables real names
namespace jj_8004570C {

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
extern "C" gfArchive* __ct__9gfArchiveFv(gfArchive* self);

struct ArchiveLoadNode {
    char pad0[8];
    s16 unk8;
    char padA[2];
    s32 unkC;
    s32 unk10;
    gfArchive* unk14;
};

struct ArchiveLoader {
    char pad0[4];
    gfArchiveDatabase* db;
    char pad8[8];
    utList freeList;
    char pad1c[0xC];
    utList usedList;
};

extern "C" s32 fn_8004570C(ArchiveLoader* self, void* data, s32 size, Heaps::HeapType heap) {
    ArchiveLoadNode* node = (ArchiveLoadNode*)self->freeList.getNode(0);
    if (node != nullptr) {
        self->freeList.remove((utListNode*)node);
        gfArchive* p = (gfArchive*)__nw__FUlQ25Heaps8HeapType(sizeof(gfArchive), heap);
        gfArchive* arc = p;
        if (p != nullptr) {
            arc = __ct__9gfArchiveFv(p);
        }
        arc->setFileImage(data, size, heap);
        node->unkC = 0xFFFF;
        node->unk10 = 0xFFFF;
        node->unk14 = nullptr;
        node->unk8 = 1;
        node->unkC = self->db->add(arc, false);
        node->unk14 = arc;
        self->usedList.addTail((utListNode*)node);
        return node->unkC;
    }
    return 0xFFFF;
}
}

namespace jj_80045240 {
#pragma opt_loop_invariants off
struct Str { unsigned int flags; unsigned int unused; char* ptr; };
extern "C" void fn_800146F0(Str*, void*);
extern "C" void fn_80014480(Str*, void*);
extern "C" void __dl__FPv(void*);
extern "C" void* fn_80045240(void* self, utList* list, void* key) {
    utListNode* node = list->getNode(0);
    while (node) {
        Str a, b;
        fn_800146F0(&a, *(char**)((char*)node + 0x14) + 0x28);
        fn_800146F0(&b, key);
        unsigned int flag = b.flags >> 31;
        const char* bp;
        const char* ap;
        if (flag == 0) bp = (char*)&b + 1; else bp = b.ptr;
        if ((a.flags >> 31) == 0) ap = (char*)&a + 1; else ap = a.ptr;
        bool equal = strcmp(ap, bp) == 0;
        if (flag) __dl__FPv(b.ptr);
        if (a.flags >> 31) __dl__FPv(a.ptr);
        if (equal) {
            Str c, d;
            fn_80014480(&c, *(char**)((char*)node + 0x14) + 0x28);
            fn_80014480(&d, key);
            flag = d.flags >> 31;
            if (flag == 0) bp = (char*)&d + 1; else bp = d.ptr;
            if ((c.flags >> 31) == 0) ap = (char*)&c + 1; else ap = c.ptr;
            equal = strcmp(ap, bp) == 0;
            if (flag) __dl__FPv(d.ptr);
            if (c.flags >> 31) __dl__FPv(c.ptr);
            if (equal) return node;
        }
        node = node->m_next;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_80044C3C {
extern "C" {
extern char lbl_8042B644[];
extern char lbl_8042B600[];
void* fn_80044BFC(void*, int);
void fn_803F0D4C(void*, void* (*)(void*, int));
void __dt__9gfArchiveFv(gfArchive*, int);
void __dt__17gfArchiveDatabaseFv(gfArchiveDatabase*, int);
void __dl__FPv(void*);
}
struct Node : utListNode { u16 value; u16 pad; u32 index; u32 other; gfArchive* archive; };
struct DBView { u16 capacity; u16 size; gfArchiveDatabaseEntry* entries; };
struct Manager { void* vtable; DBView* db; u32 count; void* nodes; utList freeList; utList list1; utList list2; utList list3; utList list4; };
inline void processNode(Manager* self, Node* node) {
    if (self->list3.isExistNode(node) != -1) self->list3.remove(node);
    if (self->list2.isExistNode(node) != -1) self->list2.remove(node);
    if (self->list1.isExistNode(node) != -1) self->list1.remove(node);
    if (self->list4.isExistNode(node) != -1) self->list4.remove(node);
    if (node->archive->m_refCount <= 0) OSReport(lbl_8042B600, (char*)node->archive + 0x28);
    DBView* db = self->db;
    s32 index = node->index;
    gfArchiveDatabaseEntry* entry = &db->entries[index];
    if (--entry->m_archive->m_refCount == 0) {
        if (entry->m_isOwner) __dt__9gfArchiveFv(entry->m_archive, 1);
        entry->m_archive = 0;
        entry->m_isActive = false;
    }
    if (!db->entries[index].m_isActive) --db->size;
    node->index = 0xffff;
    node->other = 0xffff;
    node->value = 0;
    node->archive = 0;
    self->freeList.addTail(node);
}
inline void processFirstNode(Manager* self, utListNode* listNode) {
    Node* node = (Node*)listNode;
    if (self->list3.isExistNode(node) != -1) self->list3.remove(node);
    if (self->list2.isExistNode(node) != -1) self->list2.remove(node);
    if (self->list1.isExistNode(node) != -1) self->list1.remove(node);
    if (self->list4.isExistNode(node) != -1) self->list4.remove(node);
    if (node->archive->m_refCount <= 0) OSReport(lbl_8042B600, (char*)node->archive + 0x28);
    DBView* db = self->db;
    s32 index = node->index;
    gfArchiveDatabaseEntry* entry = &db->entries[index];
    if (--entry->m_archive->m_refCount == 0) {
        if (entry->m_isOwner) __dt__9gfArchiveFv(entry->m_archive, 1);
        entry->m_archive = 0;
        entry->m_isActive = false;
    }
    if (!db->entries[index].m_isActive) --db->size;
    node->index = 0xffff;
    node->other = 0xffff;
    node->value = 0;
    node->archive = 0;
    self->freeList.addTail(node);
}
#define PROCESS(LIST) processNode(self, (Node*)LIST.getNode(0));
extern "C" void* fn_80044C3C(void* p, int flag) {
    Manager* self = (Manager*)p;
    if (self) {
        self->vtable = lbl_8042B644;
        while (*(u32*)((char*)&self->list3 + 8)) { processFirstNode(self, self->list3.getNode(0)); }
        while (*(u32*)((char*)&self->list1 + 8)) { PROCESS(self->list2) }
        while (*(u32*)((char*)&self->list2 + 8)) { PROCESS(self->list2) }
        while (*(u32*)((char*)&self->list4 + 8)) { PROCESS(self->list4) }
        fn_803F0D4C(self->nodes, fn_80044BFC);
        self->nodes = 0;
        __dt__17gfArchiveDatabaseFv((gfArchiveDatabase*)self->db, 1);
        self->db = 0;
        if (flag > 0) __dl__FPv(self);
    }
    return p;
}
}

