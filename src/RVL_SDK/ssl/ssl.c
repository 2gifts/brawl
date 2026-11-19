#include <revolution/OS/OS.h>
#include <string.h>
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

// TODO: clean up, give variables real names
void fn_803A9624(OSMutex* mutex) {
    OSLockMutex(mutex);
}

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
/* jj_803A9130 */
#define IOVec IOVec_jj803A9130
extern int IOS_Open(void *p, int flag);
extern int IOS_Close(int fd);
extern int IOS_Ioctlv(int fd, int cmd, int flag1, int flag2, void *data);
extern char lbl_80492710[];
struct IOVec { void *base; int len; };
// TODO: clean up, give variables real names
int SSLShutdown(int arg0) {
    int local __attribute__((aligned(32)));
    int result __attribute__((aligned(32)));
    struct IOVec iov[2];
    int fd;
    fd = IOS_Open(lbl_80492710, 0);
    if (fd < 0) return -1;
    local = arg0;
    result = -1;
    iov[0].base = &result;
    iov[0].len = 0x20;
    iov[1].base = &local;
    iov[1].len = 0x20;
    IOS_Ioctlv(fd, 6, 1, 1, &iov[0]);
    IOS_Close(fd);
    return result;
}
#undef IOVec

/* jj_803A8A2C */
#define IOVec IOVec_jj803A8A2C
extern int IOS_Open(void *p, int flag);
extern int IOS_Close(int fd);
extern int IOS_Ioctlv(int fd, int cmd, int flag1, int flag2, void *data);
extern char lbl_80492710[];

struct IOVec {
    void *base;
    int len;
};

// TODO: clean up, give variables real names
int fn_803A8A2C(int arg0, int arg1) {
    int local0 __attribute__((aligned(32)));
    int local1 __attribute__((aligned(32)));
    int val __attribute__((aligned(32)));
    struct IOVec iov[3];
    int fd;

    fd = IOS_Open(lbl_80492710, 0);
    if (fd < 0) {
        return -1;
    }

    local0 = arg0;
    local1 = arg1;
    val = -1;
    iov[0].base = &val;
    iov[0].len = 0x20;
    iov[1].base = &local0;
    iov[1].len = 0x20;
    iov[2].base = &local1;
    iov[2].len = 0x20;

    IOS_Ioctlv(fd, 2, 1, 2, &iov[0]);
    IOS_Close(fd);
    return val;
}
#undef IOVec

/* jj_803A9490 */
#define IOVec IOVec_jj803A9490
extern int IOS_Open(void *p, int flag);
extern int IOS_Close(int fd);
extern int IOS_Ioctlv(int fd, int cmd, int flag1, int flag2, void *data);
extern char lbl_80492710[];

struct IOVec {
    void *base;
    int len;
};

// TODO: clean up, give variables real names
int fn_803A9490(int arg0, int arg1) {
    int val __attribute__((aligned(32)));
    int local0 __attribute__((aligned(32)));
    int local1 __attribute__((aligned(32)));
    struct IOVec iov[3];
    int fd;

    fd = IOS_Open(lbl_80492710, 0);
    if (fd < 0) {
        return -1;
    }

    local0 = arg0;
    val = -1;
    local1 = arg1;
    iov[0].base = &val;
    iov[0].len = 0x20;
    iov[1].base = &local0;
    iov[1].len = 0x20;
    iov[2].base = &local1;
    iov[2].len = 0x20;

    IOS_Ioctlv(fd, 0xd, 1, 2, &iov[0]);
    IOS_Close(fd);
    return val;
}
#undef IOVec

/* jj_803A9558 */
#define IOVec IOVec_jj803A9558
extern int IOS_Open(void *p, int flag);
extern int IOS_Close(int fd);
extern int IOS_Ioctlv(int fd, int cmd, int flag1, int flag2, void *data);
extern char lbl_80492710[];

struct IOVec {
    void *base;
    int len;
};

// TODO: clean up, give variables real names
int fn_803A9558(int arg0, int arg1) {
    int val __attribute__((aligned(32)));
    int local0 __attribute__((aligned(32)));
    int local1 __attribute__((aligned(32)));
    struct IOVec iov[3];
    int fd;

    fd = IOS_Open(lbl_80492710, 0);
    if (fd < 0) {
        return -1;
    }

    local0 = arg0;
    val = -1;
    local1 = arg1;
    iov[0].base = &val;
    iov[0].len = 0x20;
    iov[1].base = &local0;
    iov[1].len = 0x20;
    iov[2].base = &local1;
    iov[2].len = 0x20;

    IOS_Ioctlv(fd, 14, 1, 2, &iov[0]);
    IOS_Close(fd);
    return val;
}
#undef IOVec

/* jj_803A88D4 */
#define IOVec IOVec_jj803A88D4
extern int IOS_Open(void *, int);
extern int IOS_Close(int);
extern int IOS_Ioctlv(int,int,int,int,void *);
extern char lbl_80492710[];
extern const char *lbl_8059FE20;
#define lbl_805A11F8 (*(int*)&lbl_805A11F8)
struct IOVec { void *base; int len; };
// TODO: clean up, give variables real names
int fn_803A88D4(int arg0, char *arg1) {
 char buf[256] __attribute__((aligned(32)));
 struct IOVec iov[3] __attribute__((aligned(32)));
 int val __attribute__((aligned(32)));
 int local0 __attribute__((aligned(32)));
 int fd;
 char *p;
 unsigned long len, i;
 fd=IOS_Open(lbl_80492710,0);
 if (!lbl_805A11F8) { OSRegisterVersion(lbl_8059FE20); lbl_805A11F8=1; }
 if(fd<0) return -1;
 p=arg1; i=0;
 while(i<256 && *p) { ++i; ++p; }
 len=p-arg1;
 if(!len) return -1;
 memset(buf,0,256);
 if(len>256) len=256;
 memcpy(buf,arg1,len);
 val=-1;
 local0=arg0;
 iov[0].base=&val; iov[0].len=32;
 iov[1].base=&local0; iov[1].len=32;
 iov[2].base=buf; iov[2].len=256;
 IOS_Ioctlv(fd,1,1,2,iov);
 IOS_Close(fd);
 return *(int *)iov[0].base;
}
#undef lbl_805A11F8
#undef IOVec

/* jj_803A8E70 */
#define IOVec IOVec_jj803A8E70
extern int IOS_Open(void *p, int flag);
extern int IOS_Close(int fd);
extern int IOS_Ioctlv(int fd, int cmd, int flag1, int flag2, void *data);
extern void *memcpy(void *dst, const void *src, unsigned long n);
extern void *memset(void *dst, int c, unsigned long n);
extern char lbl_80492710[];

struct IOVec {
    void *base;
    int len;
};

// TODO: clean up, give variables real names
int SSLWrite(int arg0, char *buf, unsigned int len) {
    char tmp[32] __attribute__((aligned(32)));
    int fd;
    int r27;
    unsigned int head;
    int total;
    unsigned int n;
    int status_h[8] __attribute__((aligned(32)));
    int hdr_h[8] __attribute__((aligned(32)));
    struct IOVec iov_h[4] __attribute__((aligned(32)));
    int status_m[8] __attribute__((aligned(32)));
    int hdr_m[8] __attribute__((aligned(32)));
    struct IOVec iov_m[4] __attribute__((aligned(32)));
    int status_t[8] __attribute__((aligned(32)));
    int hdr_t[8] __attribute__((aligned(32)));
    struct IOVec iov_t[4] __attribute__((aligned(32)));

    fd = IOS_Open(lbl_80492710, 0);
    r27 = -1;
    if (fd < 0) {
        return -1;
    }
    head = ((unsigned int)buf & 31) ? 32 - ((unsigned int)buf & 31) : 0;
    total = 0;
    memset(tmp, 0, 32);
    if (head != 0) {
        if (head > len) {
            head = len;
        }
        memcpy(tmp, buf, head);
        status_h[0] = -1;
        hdr_h[0] = arg0;
        iov_h[0].base = &status_h[0];
        iov_h[0].len = 0x20;
        iov_h[1].base = &hdr_h[0];
        iov_h[1].len = 0x20;
        iov_h[2].base = tmp;
        iov_h[2].len = head;
        IOS_Ioctlv(fd, 5, 1, 2, iov_h);
        r27 = status_h[0];
        if (r27 > 0) {
            total = r27;
            if ((unsigned int)r27 < head) {
                IOS_Close(fd);
                return r27;
            }
            buf += r27;
            len -= r27;
        } else {
            IOS_Close(fd);
            return r27;
        }
    }

    if (len != 0) {
        n = len & ~31u;
        if (n != 0) {
            hdr_m[0] = arg0;
            status_m[0] = -1;
            iov_m[0].base = &status_m[0];
            iov_m[0].len = 0x20;
            iov_m[1].base = &hdr_m[0];
            iov_m[1].len = 0x20;
            iov_m[2].base = buf;
            iov_m[2].len = n;
            IOS_Ioctlv(fd, 5, 1, 2, iov_m);
            r27 = status_m[0];
            if (r27 > 0) {
                total += r27;
                if ((unsigned int)r27 < n) {
                    IOS_Close(fd);
                    return total;
                }
                buf += r27;
                len -= r27;
            } else {
                IOS_Close(fd);
                if (total > 0) {
                    return total;
                }
                return r27;
            }
        }
    }

    if (len != 0) {
        n = len & 31;
        if (n != 0) {
            memset(tmp, 0, 32);
            memcpy(tmp, buf, n);
            status_t[0] = -1;
            hdr_t[0] = arg0;
            iov_t[0].base = &status_t[0];
            iov_t[0].len = 0x20;
            iov_t[1].base = &hdr_t[0];
            iov_t[1].len = 0x20;
            iov_t[2].base = tmp;
            iov_t[2].len = n;
            IOS_Ioctlv(fd, 5, 1, 2, iov_t);
            r27 = status_t[0];
            if (r27 > 0) {
                total += r27;
            } else {
                IOS_Close(fd);
                if (total > 0) {
                    return total;
                }
                return r27;
            }
        }
    }

    if (total > 0) {
        r27 = total;
    }
    IOS_Close(fd);
    return r27;
}
#undef IOVec

/* jj_803A8BA0 */
#define IOVec IOVec_jj803A8BA0
extern int IOS_Open(void *p, int flag);
extern int IOS_Close(int fd);
extern int IOS_Ioctlv(int fd, int cmd, int flag1, int flag2, void *data);
extern void *memcpy(void *dst, const void *src, unsigned long n);
extern void *memset(void *dst, int c, unsigned long n);
extern char lbl_80492710[];

struct IOVec {
    void *base;
    unsigned int len;
};

// TODO: clean up, give variables real names
int fn_803A8BA0(int arg0, char *buf, unsigned int len) {
    char tmp[32] __attribute__((aligned(32)));
    int fd;
    int status;
    unsigned int head;
    int total;
    unsigned int n;
    int status_h[8] __attribute__((aligned(32)));
    int hdr_h[8] __attribute__((aligned(32)));
    struct IOVec iov_h[4] __attribute__((aligned(32)));
    int status_m[8] __attribute__((aligned(32)));
    int hdr_m[8] __attribute__((aligned(32)));
    struct IOVec iov_m[4] __attribute__((aligned(32)));
    int status_t[8] __attribute__((aligned(32)));
    int hdr_t[8] __attribute__((aligned(32)));
    struct IOVec iov_t[4] __attribute__((aligned(32)));

    fd = IOS_Open(lbl_80492710, 0);
    status = -1;
    if (fd < 0) {
        return -1;
    }
    if (len > 0x8000) {
        len = 0x8000;
    }
    head = ((unsigned int)buf & 31) ? 32 - ((unsigned int)buf & 31) : 0;
    total = 0;
    memset(tmp, 0, 32);
    if (head != 0) {
        if (head > len) {
            head = len;
        }
        hdr_h[0] = arg0;
        status_h[0] = -1;
        iov_h[0].base = &status_h[0];
        iov_h[0].len = 0x20;
        iov_h[1].base = tmp;
        iov_h[1].len = head;
        iov_h[2].base = &hdr_h[0];
        iov_h[2].len = 0x20;
        IOS_Ioctlv(fd, 4, 2, 1, iov_h);
        status = status_h[0];
        if (status > 0) {
            total = status;
            memcpy(buf, tmp, status);
            if ((unsigned int)status < head) {
                IOS_Close(fd);
                return total;
            }
            buf += status;
            len -= status;
        } else {
            IOS_Close(fd);
            return status;
        }
    }

    if (len != 0) {
        n = len & ~31u;
        if (n != 0) {
            hdr_m[0] = arg0;
            status_m[0] = -1;
            iov_m[0].base = &status_m[0];
            iov_m[0].len = 0x20;
            iov_m[1].base = buf;
            iov_m[1].len = n;
            iov_m[2].base = &hdr_m[0];
            iov_m[2].len = 0x20;
            IOS_Ioctlv(fd, 4, 2, 1, iov_m);
            status = status_m[0];
            if (status > 0) {
                total += status;
                if ((unsigned int)status < n) {
                    IOS_Close(fd);
                    return total;
                }
                buf += status;
                len -= status;
            } else {
                IOS_Close(fd);
                if (total > 0) {
                    return total;
                }
                return status;
            }
        }
    }

    if (len != 0) {
        n = len & 31;
        if (n != 0) {
            memset(tmp, 0, 32);
            hdr_t[0] = arg0;
            status_t[0] = -1;
            iov_t[0].base = &status_t[0];
            iov_t[0].len = 0x20;
            iov_t[1].base = tmp;
            iov_t[1].len = n;
            iov_t[2].base = &hdr_t[0];
            iov_t[2].len = 0x20;
            IOS_Ioctlv(fd, 4, 2, 1, iov_t);
            status = status_t[0];
            if (status > 0) {
                total += status;
                memcpy(buf, tmp, status);
            } else {
                IOS_Close(fd);
                if (total > 0) {
                    return total;
                }
                return status;
            }
        }
    }

    if (total > 0) {
        status = total;
    }
    IOS_Close(fd);
    return status;
}
#undef IOVec

