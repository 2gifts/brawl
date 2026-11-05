#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <nt/nt_etc_nwc24.h>
#include <sr/sr_common.h>
#include <types.h>

// The exchange of data through WiiConnect24 (nt_etc_xchg.o in the map of the debug build): it runs its jobs (sending and
// receiving the mails) on a thread of its own.
// HYPOTHESIS: the names of the fields follow the constructor and _requestJob. The map also has the thunk
// @24@processMailData: a second interface (not known) sits at 0x18.
class nteExchange : public nteWiiConnect24, public gfRunnable {
public:
    void* m_unk18;                      // 0x18 the vtable of the interface of processMailData
    gfThread* m_thread;                 // 0x1C the thread of the current job
    int m_unk20;
    int m_job;                          // 0x24 what _requestJob was asked to do
    int m_jobArg;                       // 0x28
    u8 m_unk2C[4];
    u64 m_unk30[8];                 // 0x30 cleared by the constructor
    u16 m_unk70;
    int m_unk74;
    int m_unk78;
    int m_unk7C;
    u16 m_unk80[4];                      // 0x80 cleared by the constructor
    void* m_unk88;
    bool m_jobDone;                     // 0x8C the last job is over (true before any job)
    int m_result;                       // 0x90 how the last job ended (0: well)

    nteExchange(Heaps::HeapType heap);
    ~nteExchange();
    virtual void run();
    // Starts a job on the thread; false when the thread of the last job is still running.
    // HYPOTHESIS: the types of the parameters.
    bool _requestJob(int job, int arg);
    void mail_AddSendTo(u64 recipient);
    void mail_ClearSendTo();
    u16 getRemainSendPoint();
    int _mail_SendMail();
    int _mail_ReceiveMail(int arg, u16* result);
};
static_assert(sizeof(nteExchange) == 0x98, "Class is wrong size!");
