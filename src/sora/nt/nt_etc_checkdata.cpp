#include <gf/gf_crypto_aes.h>
#include <gf/gf_heap_manager.h>
#include <nt/nt_etc_checkdata.h>
#include <revolution/OS/OSHardware.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0458[0x8] __attribute__((aligned(4))) = { 0 };

void nteCheckData::showError(s32 error, s32 kind) {
    if (kind == 6) {
        unk37C = true;
    } else if (kind == 7) {
        unk37C = true;
    }
}

void nteCheckData::notifyDoneNDProc(s32 proc, s32 result, u32 size) {
    switch (proc) {
    case 1:
        if (result != 0) {
            unk37C = true;
        }
        break;
    case 2:
        break;
    case 3:
        if (result == 0) {
            gfCryptoAES crypto;
            crypto.m_freeDst = true;
            if (crypto.decrypt(unk358, size, unk350)) {
                while (!crypto.m_isDone) {
                    OSSleepTicks(OS_MSEC_TO_TICKS(1));
                }
                if (crypto.m_result == 0) {
                    if (unk354 == 1) {
                        _reflectSetting(crypto.m_dst, crypto.m_dstSize);
                    } else if (unk354 == 2) {
                        _storeDelivData(crypto.m_dst, crypto.m_dstSize);
                    }
                }
            }
        }
        if (unk358) {
            gfHeapManager::free(unk358);
            unk358 = NULL;
        }
        unk35C = true;
        break;
    }
}
