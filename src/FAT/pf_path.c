#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

// HYPOTHESIS: code conversion function table; only the two entries used here are typed
typedef struct PF_CODE_FUNCS {
    u8 _pad[0x48];
    s32 (*oem2unicode)(const s8* src, u16* dst);
    s32 (*unicode2oem)(const u16* src, s8* dst);
} PF_CODE_FUNCS;
extern PF_CODE_FUNCS lbl_805644C0;
extern s8 lbl_8059FF08;
extern s8 lbl_8059FF09;

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
