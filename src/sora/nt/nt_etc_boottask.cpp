#include <mu/mu_boot_network.h>
#include <stdio.h>
#include <types.h>
#include <nt/nt_etc_xchg.h>

void nteBootTask::notify_Error(int kind, int unk, int code) {
    char message[32];
    message[0] = '\0';
    if (code != 0) {
        sprintf(message, "ErrorCode:%d", code);
    }
}

void nteBootTask::startTask() {
    if (unk48 == NULL) {
        unk48 = new (unk44) nteExchange(unk44);
        if (unk48 == NULL) {
            m_finished = true;
        } else {
            unk5C = 1;
            m_finished = false;
        }
    }
}
