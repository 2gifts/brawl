#include "revolution.h"

// TODO: clean up, give variables real names
/* jj_801FC318 */
extern void* memset(void* dest, int c, unsigned int n);
extern char* strncpy(char* dest, const char* src, unsigned int n);

void* fn_801FC318(void* dest, const char* src1, const char* src2, unsigned char b1, unsigned char b2) {
    memset(dest, 0, 0x20);
    if (src1 != 0) {
        strncpy((char*)dest, src1, 4);
    }
    if (src2 != 0) {
        strncpy((char*)dest + 4, src2, 2);
    }
    ((unsigned char*)dest)[6] = b1;
    ((unsigned char*)dest)[7] = b2;
    return dest;
}

