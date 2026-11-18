#include <types.h>
// Havok translation unit hkSolverCheck.o (main.dol 0x8029F098-0x8029F334).
// Functions in address order:
//   0x8029F098    48  findChar
//   0x8029F0C8   620  processFlyingColors

// Returns the first occurrence of c in s (including the terminator position is not searched), or 0.
const char* findChar(const char* s, char c) {
    while (*s != c) {
        if (*s == 0) {
            return 0;
        }
        s++;
    }
    return s;
}

// Not yet decompiled in this unit:
//   0x8029F0C8   620  processFlyingColors

// TODO: clean up, give variables real names
namespace jj_8029F0C8 {
#pragma use_lmw_stmw on
typedef unsigned long size_t;
typedef unsigned long long u64;
typedef long long s64;

extern "C" {
size_t strlen(const char*);
char* strncat(char*, const char*, size_t);
char* strstr(const char*, const char*);
char* fn_8029F098(char* s, char c);
void fn_8027CD04(const char*);
u64 fn_802A0B88(void);

extern unsigned long lbl_8040F9A0[3];
extern char lbl_8040FA10[];
extern const char* lbl_80486668[];
extern char lbl_8059C490[4];
extern const unsigned long lbl_8059C4A8;
}

#define HK_MIN(a, b) ((a) < (b) ? (a) : (b))

extern "C" int fn_8029F0C8(int product) {
    unsigned long keycode;
    unsigned long expiring;
    unsigned long key;
    int i;
    int expiry;

    keycode = lbl_8059C4A8;
    expiring = keycode >> 31;
    for (i = 0; i < 3; i++) {
        key = lbl_8040F9A0[i];
        char* p = fn_8029F098(lbl_8059C490, '.');
        if (p == 0) {
            fn_8027CD04(lbl_8040FA10);
            return 0;
        }
        if (strstr(p, lbl_8040FA10 + 0x7B) != 0 && product != 0) {
            char buf[256];
            buf[0] = 0;
            const char* msg = lbl_8040FA10 + 0x81;
            strncat(buf, msg, HK_MIN(strlen(msg), 0xFE - strlen(buf)));
            strncat(buf, lbl_80486668[product], HK_MIN(strlen(msg), 0xFE - strlen(buf)));
            fn_8027CD04(buf);
        }
        p = fn_8029F098(p + 1, '.');
        if (p == 0) {
            fn_8027CD04(lbl_8040FA10);
            return 0;
        }
        const char* q = p + 1;
        if (expiring) {
            expiry = (keycode & 0x7FFFFFFF) ^ key;
            u64 now = fn_802A0B88();
            if (now < (u64)(s64)expiry && (u64)(s64)expiry - now < 0x209A5AD) {
                if (product != 0 && i == 2) {
                    fn_8027CD04(lbl_8040FA10 + 0xBC);
                    return 0;
                }
                return 1;
            }
        } else {
            unsigned long h = 0;
            while (*q++) {
                h = h * 23 + *q;
            }
            h ^= key;
            if (keycode == (h & 0x7FFFFFFF)) {
                if (product != 0 && i == 2) {
                    fn_8027CD04(lbl_8040FA10 + 0xBC);
                    return 0;
                }
                return 1;
            }
        }
    }
    fn_8027CD04(lbl_8040FA10);
    return 0;
}
}

