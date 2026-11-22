#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 10 functions in this unit stop matching.
#pragma auto_inline off

// TODO: clean up, give variables real names
void* fn_803EB4EC(void* arg0, void* arg1) {
    if (arg0 != 0) {
        return 0;
    }
    return arg1;
}

// TODO: clean up, give variables real names
s32 fn_803EB504(s32 arg0) {
    if (arg0 == 0) {
        return 0;
    }
    if (arg0 == 1) {
        return 1;
    }
    return -1;
}

s32 fn_803EAAE4(s32 arg0);

// TODO: clean up, give variables real names
s32 fn_803EBA18(s8 arg0) {
    s32 result = fn_803EAAE4(arg0);
    return fn_803EB504(result);
}

// PrFile2 -> FA error table
extern s32 lbl_80492FE8[0xA0 / 4];

// TODO: clean up, give variables real names
s32 fn_803EB48C(s32 err) {
    if (err == 0) return 0;
    if (err == -1) return -1;
    if (err > 0 && err < 0xA0) return lbl_80492FE8[err];
    if (err == 0x1000) return 5;
    return err;
}

int PFAPI_convertReturnValue(int err) {
    if (err != 0) {
        return -1;
    }
    return 0;
}

// TODO: clean up, give variables real names
int fn_803EB5D8(PF2_DIR* dir) {
    return PFAPI_convertReturnValue(PFDIR_closedir(dir));
}

// TODO: clean up, give variables real names
int fn_803EB5FC(void) {
    return fn_803EB48C(fn_803EA188());
}

// TODO: clean up, give variables real names
int fn_803EB620(PF2_FILE* file) {
    return PFAPI_convertReturnValue(fn_803E5774(file));
}

// TODO: clean up, give variables real names
u32 fn_803EB6E0(void* buf, u32 size, u32 count, PF2_FILE* file) {
    u32 count_read;

    fn_803E5A50(buf, size, count, file, &count_read);
    return count_read;
}

// TODO: clean up, give variables real names
int fn_803EB708(PF2_FILE* file, s32 offset, s32 origin) {
    return PFAPI_convertReturnValue(fn_803E5D00(file, offset, origin));
}

// TODO: clean up, give variables real names
int fn_803EB7A0(FAEntryInfo* dta) {
    return PFAPI_convertReturnValue(PFDIR_fsnext((PF2_DTA*)dta));
}

// TODO: clean up, give variables real names
u32 fn_803EB824(void* buf, u32 size, u32 count, PF2_FILE* file) {
    u32 count_written;

    fn_803E5B8C(buf, size, count, file, &count_written);
    return count_written;
}

// TODO: clean up, give variables real names
int fn_803EB84C(s8 drv_char) {
    return PFAPI_convertReturnValue(fn_803EA198(drv_char));
}

// TODO: clean up, give variables real names
int fn_803EB874(void* config, void* param) {
    int err;

    err = fn_803E3610();
    if (err == 0) {
        fn_803EBBD8();
    }
    return PFAPI_convertReturnValue(err);
}

// TODO: clean up, give variables real names
int fn_803EB8B4(s8 drv_char) {
    int result = 0;
    int err = fn_803EAE34(drv_char, &result);

    if (err == 0) {
        return result;
    }
    return PFAPI_convertReturnValue(err);
}

// TODO: clean up, give variables real names
int fn_803EB948(s8 drv_char) {
    return PFAPI_convertReturnValue(fn_803EA788(drv_char));
}

// TODO: clean up, give variables real names
// HYPOTHESIS: the PrFile2 volume set, as in pf_dir.c; only last_error is known.
extern struct PF2_VOLUME_SET {
    u8 _0[0x40];
    s32 last_error;
} lbl_805644C0;


// TODO: clean up, give variables real names
s32 fn_803EB3BC(const s8* mode) {
    s32 flags;
    s32 i;

    if (mode == 0) return 10;
    i = 1;
    switch (mode[0]) {
    case 'r': flags = 2; break;
    case 'w': flags = 1; break;
    case 'a': flags = 4; break;
    default: return 0;
    }
    if (mode[1] == 'b') i = 2;
    switch (mode[i++]) {
    case 0: return flags;
    case 't':
    default: return 0;
    case '+':
        switch (mode[i]) {
        case 0: return flags | 8;
        default: return 0;
        }
    }
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803EB528(s32* drv_list, void* dev) {
    int result;

    if (drv_list == 0 || *drv_list == 0) {
        result = 10;
        lbl_805644C0.last_error = result;
        goto done;
    }

    while (*drv_list != 0) {
        if (dev != 0) {
            result = fn_803EA488(*drv_list, dev, *(s32*)((char*)dev + 0x50));
        } else {
            result = fn_803EA488(*drv_list, 0, 0);
        }
        if (result != 0) {
            return PFAPI_convertReturnValue(result);
        }
        drv_list++;
    }

done:
    return PFAPI_convertReturnValue(result);
}

// TODO: clean up, give variables real names
PF2_FILE* fn_803EB644(const char* path, const char* mode) {
    PF_STR str;
    PF2_FILE* file;
    s32 flags;
    s32 err;

    flags = fn_803EB3BC((const s8*)mode);
    if (flags == 0) {
        lbl_805644C0.last_error = 10;
        return 0;
    }
    err = PFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = PFFILE_fopen(&str, flags, &file);
    } else {
        lbl_805644C0.last_error = err;
    }
    return fn_803EB4EC((void*)err, file);
}

// TODO: clean up, give variables real names
int fn_803EB72C(const char* pattern, int attr, FAEntryInfo* dta) {
    PF_STR str;
    int result = PFSTR_InitStr(&str, pattern, 1);
    if (result == 0) {
        // MATCH-ONLY: the original passes the attribute as an int; the prototype takes a u8, which adds a clrlwi.
        result = ((int (*)(PF_STR*, int, PF2_DTA*))PFDIR_fsfirst)(&str, attr, (PF2_DTA*)dta);
    } else {
        lbl_805644C0.last_error = result;
    }
    return PFAPI_convertReturnValue(result);
}

// TODO: clean up, give variables real names
int fn_803EB7C4(const char* path, FAStat* st) {
    PF_STR str;
    int result = PFSTR_InitStr(&str, path, 1);
    if (result == 0) {
        result = PFDIR_fstat(&str, st);
    } else {
        lbl_805644C0.last_error = result;
    }
    return PFAPI_convertReturnValue(result);
}

// TODO: clean up, give variables real names
int fn_803EB8F8(const char* path) {
    PF_STR str;
    s32 result = PFSTR_InitStr(&str, path, 1);
    if (result == 0) {
        result = PFDIR_mkdir(&str);
    } else {
        lbl_805644C0.last_error = result;
    }
    return PFAPI_convertReturnValue(result);
}

// TODO: clean up, give variables real names
PF2_DIR* fn_803EB970(const char* path) {
    PF_STR str;
    PF2_DIR* dir;
    s32 result = PFSTR_InitStr(&str, path, 1);
    if (result == 0) {
        result = PFDIR_opendir(&str, &dir);
    } else {
        lbl_805644C0.last_error = result;
    }
    return fn_803EB4EC((void*)result, dir);
}

// TODO: clean up, give variables real names
int fn_803EB9C8(const char* path) {
    PF_STR str;
    s32 result = PFSTR_InitStr(&str, path, 1);
    if (result == 0) {
        result = PFFILE_remove(&str);
    } else {
        lbl_805644C0.last_error = result;
    }
    return PFAPI_convertReturnValue(result);
}
