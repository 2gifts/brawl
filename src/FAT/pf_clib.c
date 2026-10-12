#include <types.h>

// HYPOTHESIS: this unit is pf_clib + pf_w_clib + pf_service glued together
// (names from the ogws VF headers / open_rvl); the asm order lines up with that.

s32 VFipf_toupper(s32 c) {
    if (c >= 'a' && c <= 'z') {
        return c - 0x20;
    }
    return c;
}

void* VFipf_memcpy(void* dst, void* src, u32 length) {
    u8* d = dst;
    u8* s = src;

    if (((u32)d % 4) == 0 && ((u32)s % 4) == 0) {
        for (; length > 3; length -= 4) {
            *(u32*)d = *(u32*)s;
            d += 4;
            s += 4;
        }
    }
    for (; length != 0; length--) {
        *d++ = *s++;
    }
    return dst;
}

void* VFipf_memset(void* dst, s32 c, u32 length) {
    u8* d = dst;

    for (; ((u32)d % 4) != 0; length--) {
        *d++ = c;
    }
    for (; length > 3; length -= 4) {
        *(u32*)d = (c << 24) | (c << 16) | (c << 8) | c;
        d += 4;
    }
    for (; length != 0; length--) {
        *d++ = c;
    }
    return dst;
}

u32 VFipf_strlen(const s8* s) {
    const s8* p = s;
    while (*p != 0) {
        p++;
    }
    return p - s;
}

s8* VFipf_strcpy(s8* dst, const s8* src) {
    s8* d = dst;
    while ((*d = *src) != 0) {
        src++;
        d++;
    }
    return dst;
}

s32 VFipf_strcmp(const s8* s1, const s8* s2) {
    const u8* a = (const u8*)s1;
    const u8* b = (const u8*)s2;
    while (*a != 0 && *b != 0 && *a == *b) {
        a++;
        b++;
    }
    return *a - *b;
}

s32 VFipf_strncmp(const s8* s1, const s8* s2, u32 length) {
    const u8* a = (const u8*)s1;
    const u8* b = (const u8*)s2;
    for (; length != 0; length--, a++, b++) {
        if (*a == 0 || *b == 0 || *a != *b) {
            return *a - *b;
        }
    }
    return 0;
}

u32 VFipf_w_strlen(const u16* s) {
    const u16* p = s;
    while (*p != 0) {
        p++;
    }
    return ((s32)p - (s32)s) >> 1;
}

u16* VFipf_w_strcpy(u16* dst, const u16* src) {
    u16* d = dst;
    while ((*d = *src) != 0) {
        src++;
        d++;
    }
    return dst;
}

s32 VFipf_w_strncmp(const u16* s1, const u16* s2, u32 length) {
    for (; length != 0; length--, s1++, s2++) {
        if (*s1 == 0 || *s2 == 0 || *s1 != *s2) {
            return *s1 - *s2;
        }
    }
    return 0;
}

// HYPOTHESIS: no name evidence for these two, they pack/unpack a u32 as two u16 halves
u32 fn_803D6598(u16 hi, u16 lo) {
    return (hi << 16) + lo;
}

void fn_803D65A4(u32 v, u16* hi, u16* lo) {
    *hi = v >> 16;
    *lo = v;
}

u16 VFiPF_GET_LE_U16(const u8* buf) {
    return buf[0] | (buf[1] << 8);
}

void VFiPF_LE16_TO_U16_STR(u8* str, u32 num) {
    u32 i;
    for (i = 0; i < num; i += 2) {
        u8 a = str[i];
        u8 b = str[i + 1];
        str[i] = b;
        str[i + 1] = a;
    }
}
