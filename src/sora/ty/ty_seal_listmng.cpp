#include <ty/ty_seal_listmng.h>
#include <types.h>

bool tySealListDataManager::isLoadFinish() {
    if (!unk3EA8) {
        if (!unk3EAC.isReady()) {
            return false;
        }
        u32 size = unk3EAC.getSize();
        setData(static_cast<u8*>(unk3EAC.getBuffer()), size);
        unk3EA8 = true;
        return true;
    }
    return true;
}

void tySealListDataManager::sortNew() {
    for (u32 i = 0; i < unk2F04; ++i) {
        unk7E4[i] = unk2F08[i];
    }
}

void tySealListDataManager::clearNarrow() {
    for (u32 i = 0; i < unkC; ++i) {
        unk1784[i] = 0;
    }
    unkC = 0;
}

u32 tySealListDataManager::getSealList(u32 count, u32 start, void** output) {
    u32 i = 0;
    for (; i < count && i + start < unk2EF4; ++i) {
        *output++ = &unk0[unk7E4[i + start]];
    }
    return i;
}

void* tySealListDataManager::getSealDataId(u32 id) {
    if (!unk3EA8 && !unk3EB0) {
        return NULL;
    }
    for (u32 i = 0; i < unk2EF4; ++i) {
        if (id == unk0[i].unk0) {
            return &unk0[i];
        }
    }
    return NULL;
}

bool isPuttingOn(Vec2f* first, u32 firstCount, Vec2f* second, u32 secondCount, bool both) {
    if (isPut(first, firstCount, second, secondCount)) {
        return true;
    }
    if (both) {
        if (isPut(second, secondCount, first, firstCount)) {
            return true;
        }
    }
    if (isCross(first, firstCount, second, secondCount)) {
        return true;
    }
    return false;
}

bool tySealListDataManager::isPutChara(int chara, u32 id) {
    if (chara == 4) {
        chara = 3;
    }
    u32 mask;
    u32 flags;
    if (chara < 32) {
        mask = 1 << chara;
        flags = static_cast<__typeof__(unk0)>(getSealDataId(id))->unk1C;
    } else {
        mask = 1 << (chara - 32);
        flags = static_cast<__typeof__(unk0)>(getSealDataId(id))->unk20;
    }
    return (flags & mask) != 0;
}
