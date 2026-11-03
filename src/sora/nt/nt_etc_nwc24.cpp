#include <gf/gf_heap_manager.h>
#include <memory.h>
#include <nt/nt_etc_nwc24.h>
#include <nt/nt_offline.h>
#include <revolution/NWC24/NWC24Config.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049ECB8[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049ECC8[0x18] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0450[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0454[0x4] __attribute__((aligned(4))) = { 0 };

// WiiConnect24 SDK calls, under the names RVL_SDK/nwc24/nwc24.c defines them with.
extern "C" {
// HYPOTHESIS: SDK signatures follow the argument registers and the records accessed by their callees.
int fn_803A0158();
int fn_803A01D0(int box);
int fn_803A0240(u32 appId);
int fn_803A02A4(NWC24MsgObj* message, u32 count, u32* found, u32* remaining);
int fn_803A1AE8(u32* count);
int fn_803A1A7C(u32* count);
int fn_803A1C10(u32 index);
int fn_803A19D8(void* info, u32 index);
int fn_803A2A18(NWC24DlTask* task, int type);
// HYPOTHESIS: the map labels the URL setter NWC24SetDlPriority; its argument follows the callee's string accesses.
int fn_803A2EA4(NWC24DlTask* task, const char* url);
int fn_803A3064(NWC24DlTask* task, u32 flags);
int fn_803A43D0(NWC24DlTask* task, const u8* key);
int fn_803A4684(NWC24DlTask* task, const u8* key);
int fn_803A4FCC(NWC24DlTask* task, u32 flags);
int fn_803A2C58(NWC24DlTask* task, u16 interval);
int fn_803A31D0(NWC24DlTask* task, s16 count);
}

extern const char lbl_80460F48[];
extern const char* lbl_80460F04[];
extern const u8 lbl_80409660[];
extern const u8 lbl_80409760[];
extern u16 lbl_8059E7D4;
extern s16 lbl_8059E7D2;

u64 nteWiiConnect24::getMyWiiID() {
    if (!_openLibrary(1)) return 0;
    u64 id;
    if (NWC24GetMyUserId(&id) != 0) id = 0;
    return id;
}

u16* nteWiiConnect24::friend_getName(u32 index) {
    if (index >= (u32)m_bufferSize) return 0;
    // HYPOTHESIS: each cached friend contains a UTF-16 name and two identifiers.
    struct { u16 unk0[12]; u64 unk18; u64 unk20; } *friends;
    friends = static_cast<__typeof__(friends)>(m_buffer);
    return friends[index].unk0;
}

u64 nteWiiConnect24::friend_getWiiID(u32 index) {
    if (index >= (u32)m_bufferSize) return 0;
    // HYPOTHESIS: each cached friend contains a UTF-16 name and two identifiers.
    struct { u16 unk0[12]; u64 unk18; u64 unk20; } *friends;
    friends = static_cast<__typeof__(friends)>(m_buffer);
    return friends[index].unk20;
}

void nteWiiConnect24::startSearchMail(u32 appId) {
    if (_openLibrary(1)) {
        fn_803A0158();
        fn_803A01D0(1);
        fn_803A0240(appId);
    }
}

bool nteWiiConnect24::getNextMailForMe(NWC24MsgObj* message, u32* remaining) {
    u32 found;
    u32 next;
    *remaining = 0;
    int error = fn_803A02A4(message, 1, &found, &next);
    if (error != 0) {
        _procGeneralError(error);
        return false;
    }
    if (found == 0) return false;
    *remaining = next;
    return true;
}

bool nteWiiConnect24::friend_makeList() {
    if (!_openLibrary(1)) return false;
    if (m_buffer) {
        gfHeapManager::free(m_buffer);
        m_bufferSize = 0;
        m_buffer = 0;
    }
    u32 count;
    int error = fn_803A1AE8(&count);
    if (error != 0 && !_procGeneralError(error)) return false;
    if (count == 0) return true;
    m_buffer = gfHeapManager::alloc(m_heap, count * 0x28);
    if (!m_buffer) return false;
    u32 capacity;
    error = fn_803A1A7C(&capacity);
    if (error != 0) {
        _procGeneralError(error);
        return false;
    }
    NtOffline offline;
    // HYPOTHESIS: the SDK friend record has two status words, a UTF-16 name, and two identifiers.
    struct {
        u32 unk0;
        u32 unk4;
        u16 unk8[12];
        u64 unk20;
        u8 unk28[0x18];
        u64 unk40;
        u8 unk48[0xf8];
    } info __attribute__((aligned(32)));
    for (u32 i = 0; i < capacity; ++i) {
        if (!fn_803A1C10(i)) continue;
        error = fn_803A19D8(&info, i);
        if (error == 0) {
            if (info.unk0 != 1 || info.unk4 != 2) continue;
            if (offline.getFriendIndexHaveWiiID(info.unk40) == 0xff) continue;
            // HYPOTHESIS: each cached friend contains a UTF-16 name and two identifiers.
            struct { u16 unk0[12]; u64 unk18; u64 unk20; } *friends;
            friends = m_bufferSize + static_cast<__typeof__(friends)>(m_buffer);
            friends->unk18 = info.unk20;
            friends->unk20 = info.unk40;
            memcpy(friends->unk0, info.unk8, 0x18);
            ++m_bufferSize;
        } else if (!_procGeneralError(error)) {
            return false;
        }
    }
    return true;
}

bool nteWiiConnect24::_setupDlTask(NWC24DlTask* task, u16 index, int* error) {
    int result = fn_803A2A18(task, 2);
    if (result != 0) {
        *error = result;
        return false;
    }
    char url[128];
    strcpy(url, lbl_80460F48);
    strcat(url, lbl_80460F04[index]);
    result = fn_803A2EA4(task, url);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A3064(task, 10);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A43D0(task, lbl_80409660);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A4684(task, lbl_80409760);
    if (result != 0) {
        *error = result;
        return false;
    }
    if (fn_803A4FCC(task, 1) != 0) return false;
    result = fn_803A2C58(task, lbl_8059E7D4);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A31D0(task, lbl_8059E7D2);
    if (result != 0) {
        *error = result;
        return false;
    }
    return true;
}
