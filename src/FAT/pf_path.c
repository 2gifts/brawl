#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

// HYPOTHESIS: code conversion function table; only the two entries used here are typed
typedef struct PF_CODE_FUNCS {
    u8 _pad[0x48];
    s32 (*oem2unicode)(const s8* src, u16* dst);
    s32 (*unicode2oem)(const u16* src, s8* dst);
    u8 _pad50[0x10];
    u32 flags;
    PF_CRITICAL critical;
} PF_CODE_FUNCS;
extern PF_CODE_FUNCS lbl_805644C0;
extern s8 lbl_8059FF08;
extern s8 lbl_8059FF09;

u32 VFiPFSTR_GetCodeMode(PF_STR* p_str);
const s8* VFiPFSTR_GetStrPos(PF_STR* p_str, u32 target);
s8* VFipf_strcpy(s8* dst, const s8* src);
u16* VFipf_w_strcpy(u16* dst, const u16* src);
s32 VFipf_w_strncmp(const u16* s1, const u16* s2, u32 length);
void PF_enterCritical(PF_CRITICAL* critical);
void PF_exitCritical(PF_CRITICAL* critical);
int PFSTR_StrNCmp(PF_STR* p_str, const s8* s, u32 a, u32 b, u32 c);
int fn_803E72B4(int a, int b, int c);

int PFPATH_cmpNameUni(int a, int b) {
    return fn_803E72B4(a, b, 1) == 0;
}

int PFPATH_SplitPath(PF2_STR* path, PF2_STR* dir_path, PF2_STR* filename) {
    return fn_803E5F58(path, dir_path, filename, 0);
}

int PFPATH_cmpTailSFN(const char* sfn_name, const char* pattern) {
    return VFipf_strcmp(sfn_name, pattern) != 0;
}

int PFPATH_SplitPathPattern(PF2_STR* path, PF2_STR* dir_path, PF2_STR* filename) {
    return fn_803E5F58(path, dir_path, filename, 1);
}

void PFPATH_InitTokenOfPath(PF_STR* p_str, s8* path, u32 code_mode) {
    p_str->p_head = path;
    p_str->p_tail = path;
    p_str->code_mode = code_mode;
}

int PFPATH_CheckExtShortNameSignature(PF_STR* p_str) {
    int result = 0;
    s8 signature[2];
    signature[0] = lbl_8059FF08;
    signature[1] = lbl_8059FF09;
    if (PFSTR_StrNCmp(p_str, signature, 1, 0, 2) == 0) {
        result = 1;
    }
    return result;
}

s32 PFPATH_GetLengthFromUnicode(const u16* str) {
    s32 index = 0;
    s32 length = 0;
    s16 oem_width;
    s16 uni_width;
    s8 buf[2];
    while (str[index] != 0) {
        s32 width = lbl_805644C0.unicode2oem(str + index, buf);
        fn_803D65A4(width, &oem_width, &uni_width);
        length += oem_width;
        index += uni_width >> 1;
    }
    return length;
}

s32 PFPATH_transformInUnicode(u16* dst, const s8* src) {
    s32 index;
    s32 count = 0;
    s16 oem_width;
    s16 uni_width;
    index = 0;
    while (src[index] != 0) {
        s32 width = lbl_805644C0.oem2unicode(src + index, dst);
        fn_803D65A4(width, &oem_width, &uni_width);
        count++;
        dst = (u16*)((u8*)dst + (uni_width & ~1));
        index += oem_width;
    }
    *dst = 0;
    return count;
}

// HYPOTHESIS: argument layouts from register use
s32 PFPATH_CheckExtShortName(PF_STR* p_str, u32 target, u32 is_long);
s32 PFPATH_cmpNameImpl(const s8* name, const s8* pattern, u32* p_is_end);
int PFSTR_StrCmp(PF_STR* p_str, const char* s);

s32 PFPATH_transformFromUnicodeToNormal(s8* dst, const u16* src) {
    const u16* cur;
    s32 index;
    s16 oem_width;
    s16 uni_width;
    u16 dot[2];
    u16 space[2];
    if ((lbl_805644C0.flags & 2) == 2) {
        dot[0] = '.';
        dot[1] = 0;
        space[0] = ' ';
        space[1] = 0;
        cur = src;
        while (*cur != 0) {
            if (VFipf_w_strncmp(src, dot, 1) == 0) {
                *dst = '.';
            } else if (VFipf_w_strncmp(src, space, 1) == 0) {
                *dst = ' ';
            } else {
                *dst = '_';
            }
            dst++;
            src++;
            cur++;
        }
    } else {
        index = 0;
        while (src[index] != 0) {
            s32 width = lbl_805644C0.unicode2oem(src + index, dst);
            fn_803D65A4(width, &oem_width, &uni_width);
            dst += oem_width;
            index += uni_width >> 1;
        }
    }
    *dst = 0;
    return 0;
}

void PFPATH_SetSearchPattern(s8* short_pattern, u16* long_pattern, PF_STR* p_pattern) {
    u16 star[2];
    if (VFiPFSTR_GetCodeMode(p_pattern) == 1) {
        if (VFipf_strcmp((const char*)p_pattern->p_head, "*.*") == 0) {
            VFipf_strcpy(short_pattern, (const s8*)"*");
        } else {
            VFipf_strcpy(short_pattern, (const s8*)p_pattern->p_head);
        }
    } else {
        if (PFSTR_StrCmp(p_pattern, "*.*") == 0) {
            star[0] = '*';
            star[1] = 0;
            VFipf_w_strcpy(long_pattern, star);
        } else {
            VFipf_w_strcpy(long_pattern, (const u16*)p_pattern->p_head);
        }
        if ((lbl_805644C0.flags & 2) == 2) {
            PF_enterCritical(&lbl_805644C0.critical);
            lbl_805644C0.flags = (lbl_805644C0.flags & ~3) | 1;
            PFPATH_transformFromUnicodeToNormal(short_pattern, long_pattern);
            lbl_805644C0.flags = (lbl_805644C0.flags & ~3) | 2;
            PF_exitCritical(&lbl_805644C0.critical);
        } else {
            PFPATH_transformFromUnicodeToNormal(short_pattern, long_pattern);
        }
    }
}

s32 PFPATH_cmpName(const s8* sShort, PF_STR* p_pattern, u32 is_short_search) {
    u32 is_end = 0;
    char buf[13];
    char* cur = buf;
    const s8* pattern = VFiPFSTR_GetStrPos(p_pattern, 3);
    VFipf_strcpy((s8*)buf, sShort);
    if ((lbl_805644C0.flags & 2) == 2 && VFiPFSTR_GetCodeMode(p_pattern) == 2) {
        if (VFipf_strcmp(buf, ".") != 0 && VFipf_strcmp(buf, "..") != 0
            && PFPATH_CheckExtShortName(p_pattern, 3, 1) == 0 && is_short_search == 0) {
            return 1;
        }
    }
    if (VFipf_strcmp((const char*)pattern, "*.") == 0) {
        while (*cur != 0 && *cur != '.') {
            cur++;
        }
        if (*cur == 0) {
            cur[0] = '.';
            cur[1] = 0;
        }
    } else if (*pattern == 0) {
        return 1;
    } else if (buf[0] == 0) {
        return 1;
    }
    return PFPATH_cmpNameImpl((const s8*)buf, pattern, &is_end);
}

s32 PFPATH_putShortName(u8* pDirEntry, const s8* short_name, u8 attr) {
    s32 i = 0;
    while (i < 8 && *short_name != 0 && *short_name != '.') {
        *pDirEntry++ = *short_name++;
        i++;
    }
    if (i == 0) {
        i = 0;
        while (i < 2 && *short_name != 0) {
            *pDirEntry++ = *short_name++;
            i++;
        }
    }
    if ((attr & 8) == 0) {
        while (i < 8) {
            *pDirEntry++ = ' ';
            i++;
        }
    }
    if (*short_name != 0) {
        if ((attr & 8) == 0) {
            short_name++;
        }
        while (*short_name != 0) {
            *pDirEntry++ = *short_name++;
            i++;
        }
    }
    while (i < 11) {
        *pDirEntry++ = ' ';
        i++;
    }
    return 0;
}
