#include <revolution/OS/OSMutex.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805376C0[0x3020] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A11F8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A11FC[0x4] __attribute__((aligned(4))) = { 0 };

// TODO: clean up, give variables real names
void fn_803A9620(OSMutex* mutex) {
    OSInitMutex(mutex);
}

void fn_803A9624(OSMutex* mutex) {
    OSLockMutex(mutex);
}

void fn_803A9628(OSMutex* mutex) {
    OSUnlockMutex(mutex);
}

// TODO: clean up, give variables real names
/* jj_803A8AF4 */
extern int fn_802123A8(void *p, int flag);
extern int fn_80212588(int fd);
extern int fn_80213090(int fd, int cmd, int flag1, int flag2, void *data);
extern char lbl_80492710[];

struct IOVec {
    void *base;
    int len;
};

int fn_803A8AF4(int arg0) {
    int local __attribute__((aligned(32)));
    int val __attribute__((aligned(32)));
    struct IOVec iov[2];
    int fd;

    fd = fn_802123A8(lbl_80492710, 0);
    if (fd < 0) {
        return -1;
    }

    local = arg0;
    val = -1;
    iov[0].base = &val;
    iov[0].len = 0x20;
    iov[1].base = &local;
    iov[1].len = 0x20;

    fn_80213090(fd, 3, 1, 1, &iov[0]);
    fn_80212588(fd);
    return val;
}

