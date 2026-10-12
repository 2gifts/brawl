#include <gf/gf_file_io_manager.h>
#include <gf/gf_file_io_request.h>
#include <revolution/OS/OSMutex.h>

#include <gf/gf_file_io_manager.h>
#include <gf/gf_file_io_request.h>
#include <types.h>

struct gfFileIOSlots {
    u16 m_size;
    u16 unk2;
    gfFileIORequest* m_elements[128];
};

// MATCH-ONLY: gfFileIORequestQueue::m_queue and utQueue::unk2 are private in the headers, so this view exposes them.
struct gfFileIORequestQueueView {
    OSMutex mutex;
    gfFileIOSlots* list;
};

void gfFileIOManager::push(gfFileIORequest* req) {
    gfFileIORequestQueueView* queue = (gfFileIORequestQueueView*) m_queue1;
    OSLockMutex(&queue->mutex);
    u16 idx;
    gfFileIOSlots* list = queue->list;
    idx = list->unk2;
    list->unk2 = idx + 1;
    list->m_elements[idx] = req;
    OSUnlockMutex(&queue->mutex);
}


struct gfFileIORequestPoolView {
    s32 unk0;
    u32* usedBits;
    gfFileIORequest* requests;
};

// MATCH-ONLY: gfFileIOManager leaves offset 0x10 as an unnamed char array in the header, so this view exposes the pool pointer.
struct gfFileIOManagerView {
    char pad0[0x10];
    gfFileIORequestPoolView* pool;
};

extern "C" s32 fn_80044248(u32* usedBits);
extern "C" s32 fn_80044360(u32* usedBits);

gfFileIORequest* gfFileIOManager::allocRequest() {
    gfFileIOManagerView* self = (gfFileIOManagerView*) this;
    gfFileIORequestPoolView* pool = self->pool;
    gfFileIORequest* req;
    if (fn_80044248(pool->usedBits) != 0) {
        s32 idx = fn_80044360(pool->usedBits);
        // HYPOTHESIS: usedBits points at a header word, so the bit words start 4 bytes in.
        pool->usedBits[(idx >> 5) + 1] |= 1 << (idx & 0x1F);
        req = (gfFileIORequest*) ((u8*) pool->requests + idx * 0xA4);
    } else {
        req = NULL;
    }
    gfFileIORequest* result = NULL;
    if (req != NULL) {
        // MATCH-ONLY: the second null check is kept so the compiler emits the same branch pair.
        if (req != NULL) {
            req->m_isCached = false;
            req->unk16_b7 = 0;
            req->unk17_b6 = 0;
            req->unk17_b5 = 0;
            req->m_isCancelRequested = false;
            req->m_isCanceled = false;
            req->unk17_b2 = 0;
            req->unk17_b1 = 0;
            req->unk17_b0 = 0;
            req->unk18_b7 = 0;
            req->unk18_b6 = 0;
            req->m_isReady = 1;
            req->m_notifyCallback = NULL;
        }
        result = req;
    }
    return result;
}
