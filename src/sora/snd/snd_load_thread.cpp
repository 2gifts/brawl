#include <snd/snd_load_thread.h>
#include <snd/snd_system.h>
#include <types.h>

void sndLoadThread::run() {
    sndSystem* system = g_sndSystem;
    for (;;) {
        OSLockMutex(&unk340);
        while (unkCC8 == 0 && unkCCC == 0 && unkCD0) {
            OSWaitCond(&unk358, &unk340);
        }
        while (unkCC8 != 0 && unkCCC < 1) {
            unkCB8[unkCCC] = unk368[0];
            ++unkCCC;
            for (u32 i = 1; i < unkCC8; ++i) {
                unk368[i - 1] = unk368[i];
            }
            --unkCC8;
        }
        OSUnlockMutex(&unk340);
        while (unkCCC != 0) {
            switch (unkCB8[0].unk0) {
            case 1:
                system->loadGroupThread(unkCB8[0].unk8, unkCB8[0].unk4);
                break;
            case 2:
                system->freeGroupThread(unkCB8[0].unkC, unkCB8[0].unk4);
                break;
            }
            for (u32 i = 1; i < unkCCC; ++i) {
                unkCB8[i - 1] = unkCB8[i];
            }
            --unkCCC;
        }
        if (!unkCD0) {
            OSExitThread(NULL);
            unkCD0 = true;
        }
        OSSignalCond(&unk360);
    }
}

void sndLoadThread::addFreeGroup(int value, int slot) {
    u8 entryLow;
    int high = (value >> 8) & 0xFF;
    int low = value & 0xFF;
    bool removed = false;
    u8 entryHigh;
    OSLockMutex(&unk340);
    for (int i = unkCC8 - 1; i >= 0; --i) {
        entryLow = unk368[i].unkC & 0xFF;
        entryHigh = (unk368[i].unkC >> 8) & 0xFF;
        if (high == entryHigh && entryLow >= low && unk368[i].unk0 == 1) {
            for (u32 j = i + 1; j < unkCC8; ++j) {
                unk368[j - 1] = unk368[j];
            }
            --unkCC8;
            removed = true;
            if (entryLow == low) {
                break;
            }
        }
    }
    if (removed) {
        OSUnlockMutex(&unk340);
        return;
    }
    if (unkCC8 < 149) {
        unk368[unkCC8].unk0 = 2;
        unk368[unkCC8].unk8 = static_cast<SndGroupID>(-1);
        unk368[unkCC8].unk4 = slot;
        unk368[unkCC8].unkC = value;
        ++unkCC8;
    }
    OSUnlockMutex(&unk340);
    OSSignalCond(&unk358);
}

bool sndLoadThread::isFinish() {
    OSLockMutex(&unk340);
    bool finished = unkCC8 + unkCCC == 0;
    OSUnlockMutex(&unk340);
    return finished;
}

bool sndLoadThread::isFinish(SndGroupID id) {
    OSLockMutex(&unk340);
    if (unkCCC + unkCC8 == 0) {
        OSUnlockMutex(&unk340);
        return true;
    }
    for (u32 i = 0; i < unkCC8; ++i) {
        if (static_cast<u32>(id) == static_cast<u32>(unk368[i].unk8)) {
            OSUnlockMutex(&unk340);
            return false;
        }
    }
    for (u32 i = 0; i < unkCCC; ++i) {
        if (static_cast<u32>(id) == static_cast<u32>(unkCB8[i].unk8)) {
            OSUnlockMutex(&unk340);
            return false;
        }
    }
    OSUnlockMutex(&unk340);
    return true;
}

bool sndLoadThread::isFinishFree() {
    OSLockMutex(&unk340);
    if (unkCCC + unkCC8 == 0) {
        OSUnlockMutex(&unk340);
        return true;
    }
    for (u32 i = 0; i < unkCC8; ++i) {
        if (unk368[i].unk0 == 2) {
            OSUnlockMutex(&unk340);
            return false;
        }
    }
    for (u32 i = 0; i < unkCCC; ++i) {
        if (unkCB8[i].unk0 == 2) {
            OSUnlockMutex(&unk340);
            return false;
        }
    }
    OSUnlockMutex(&unk340);
    return true;
}
