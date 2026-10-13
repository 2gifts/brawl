#include <types.h>

// HYPOTHESIS: same layout as PF_STR in the ogws vf_struct.h (0x10 bytes), declared
// locally for now so this unit doesn't depend on that header yet.
typedef struct PF_STR {
    const s8* p_head;
    const s8* p_tail;
    const s8* p_local;
    u32 code_mode;
} PF_STR;

// only the small leaf functions are drafted so far; the rest of the unit
// (move/init/len/cmp/toupper) is still asm-only.

void VFiPFSTR_SetCodeMode(PF_STR* p_str, u32 code_mode) {
    p_str->code_mode = code_mode;
}

u32 VFiPFSTR_GetCodeMode(PF_STR* p_str) {
    return p_str->code_mode;
}

void VFiPFSTR_SetLocalStr(PF_STR* p_str, s8* p_local) {
    // HYPOTHESIS: mode 1 (probably the plain/ascii-ish one) ignores the caller's
    // buffer and falls back to the head pointer
    if (p_str->code_mode == 1 || p_local == 0) {
        p_str->p_local = p_str->p_head;
    } else {
        p_str->p_local = p_local;
    }
}

const s8* VFiPFSTR_GetStrPos(PF_STR* p_str, u32 target) {
    if (target == 1) {
        return p_str->p_head;
    }
    if (target == 2) {
        return p_str->p_tail;
    }
    return p_str->p_local;
}
