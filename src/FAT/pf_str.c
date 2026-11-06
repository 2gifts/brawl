#include <types.h>
#include <FAT/pf2.h>


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

// Function bodies below are derived from https://github.com/jj-link/brawl.

u16 fn_803D68B8(PF_STR* p_str) {
    return p_str->p_tail - p_str->p_head;
}

s32 PFSTR_InitStr(PF_STR* p_str, const char* s, u32 code_mode) {
    if (code_mode == 1) {
        p_str->p_head = (const s8*)s;
        p_str->p_tail = (const s8*)s + VFipf_strlen(s);
    } else if (code_mode == 2) {
        p_str->p_head = (const s8*)s;
        p_str->p_tail = (const s8*)s + VFipf_w_strlen((const u16*)s) * 2;
    }
    p_str->code_mode = code_mode;
    return 0;
}

// HYPOTHESIS: code-page function table; only the slots this unit calls are typed
typedef struct PFCodeFuncs {
    u8 _pad0[0x48];
    s32 (*oem2unicode)(const s8* src, u16* dst);
    u8 _pad4C[0x8];
    int (*is_lead)(s8 c, int mode);
    s32 (*char_size)(const s8* s);
} PFCodeFuncs;
extern PFCodeFuncs lbl_805644C0;

s32 PFSTR_StrCmp(const PF_STR* p_str, const s8* s) {
    if (p_str->code_mode == 1) {
        return VFipf_strcmp((const char*)p_str->p_head, (const char*)s);
    }
    {
    const u16* wide = (const u16*)p_str->p_head;
    u16 unicode;
    u16 ch;
    do {
        lbl_805644C0.oem2unicode(s, &unicode);
        ch = *wide;
        s++;
        wide++;
        if (ch != unicode) break;
        if (wide[-1] == 0) break;
    } while (unicode != 0);
    return wide[-1] - unicode;
    }
}

u32 PFSTR_StrNumChar(PF_STR* p_str, u32 target) {
    const s8* p;
    u32 count;
    if (target == 1) {
        p = p_str->p_head;
    } else {
        p = p_str->p_tail;
    }
    if (p_str->code_mode == 1) {
        count = 0;
        while (*p != 0) {
            if (lbl_805644C0.is_lead(*p, 1)) {
                p++;
            }
            p++;
            count++;
        }
    } else {
        count = 0;
        while (p[0] != 0 || p[1] != 0) {
            p += lbl_805644C0.char_size(p);
            count++;
        }
    }
    return count;
}

void PFSTR_MoveStrPos(PF_STR* p_str, s16 num_s8) {
    s16 i;
    s16 offset;
    const s8* p;
    const u16* w;
    s32 size;
    offset = 0;
    if (p_str->code_mode == 1) {
        p = p_str->p_head;
        while (num_s8 != 0) {
            if (lbl_805644C0.is_lead(*p, 1)) {
                offset++;
            }
            offset++;
            num_s8--;
        }
    } else {
        w = (const u16*)p_str->p_head;
        for (i = 0; i < num_s8; i++) {
            size = lbl_805644C0.char_size((const s8*)w);
            w += size;
            offset += (s16)size;
        }
    }
    p_str->p_head += offset;
}

void PFSTR_ToUpperNStr(PF_STR* p_str, u16 num, s8* dest) {
    u32 c;
    int is_lower;
    if (p_str->code_mode == 1) {
        const s8* p = p_str->p_head;
        while (num != 0 && *p != 0) {
            *dest = VFipf_toupper(*p);
            dest++;
            p++;
            num--;
        }
    } else {
        const u16* w = (const u16*)p_str->p_head;
        while (num != 0 && *w != 0) {
            c = *w;
            is_lower = c >= 0x61 && c <= 0x7a;
            if (is_lower) {
                c -= 0x20;
            }
            dest[0] = c;
            dest[1] = (u8)(c >> 8);
            w++;
            num--;
            dest += 2;
        }
        *dest = 0;
    }
    *dest = 0;
}
