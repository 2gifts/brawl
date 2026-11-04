#include <mu/mu_boot_network.h>
#include <stdio.h>
#include <types.h>

void nteBootTask::notify_Error(int kind, int unk, int code) {
    char message[32];
    message[0] = '\0';
    if (code != 0) {
        sprintf(message, "ErrorCode:%d", code);
    }
}
