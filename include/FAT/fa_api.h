#ifndef FAT_FA_API_H
#define FAT_FA_API_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

int FAErrnum(void);
int FAIsWriteProtected(s8 drv_char);
int FAMkdir(const char* path);
int FAGetdev(s8 drv_char);
int FAMount(s8 drv_char);
// HYPOTHESIS: the first argument must be 0; any other value is rejected with -2.
int FARegistCB(int mode, void (*insert_cb)(s8 drv_char), void (*eject_cb)(s8 drv_char));
void FANotifySDInsert(s8 drv_char);
void FANotifySDEject(s8 drv_char);

#ifdef __cplusplus
}
#endif

#endif
