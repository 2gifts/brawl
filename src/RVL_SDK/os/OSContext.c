// OSContext: saving/restoring the cpu register file (gprs, gqrs, fprs and the paired singles) plus the lazy
// fpu switch that kicks in when something touches the fpu without its context loaded. most of this is
// hand written asm in the original, so it is hand written asm here too.
#include "revolution/os.h"
#include "revolution/db.h"

#define OS_CONTEXT_STATE_FP_SAVED 0x1
#define OS_CONTEXT_STATE_EXC 0x2
#define MSR_FP_BIT 0x2000

// lowmem bookkeeping
#define OS_CURRENT_CONTEXT_PHYS (*(volatile u32 *)0x800000C0)
#define OS_CURRENT_CONTEXT (*(OSContext * volatile *)0x800000D4)
#define OS_CURRENT_FPU_CONTEXT (*(OSContext * volatile *)0x800000D8)

static asm void __OSLoadFPUContext(register u32 unused, register OSContext *ctx) {
    nofralloc

    lhz r5, ctx->state
    clrlwi. r5, r5, 31
    beq exit

    lfd f0, ctx->fpscr_pad
    mtfsf 255, f0
    mfspr r5, 0x398
    rlwinm. r5, r5, 3, 31, 31
    beq load_fprs

    psq_l f0, OSContext.psf[0](ctx), 0, 0
    psq_l f1, OSContext.psf[1](ctx), 0, 0
    psq_l f2, OSContext.psf[2](ctx), 0, 0
    psq_l f3, OSContext.psf[3](ctx), 0, 0
    psq_l f4, OSContext.psf[4](ctx), 0, 0
    psq_l f5, OSContext.psf[5](ctx), 0, 0
    psq_l f6, OSContext.psf[6](ctx), 0, 0
    psq_l f7, OSContext.psf[7](ctx), 0, 0
    psq_l f8, OSContext.psf[8](ctx), 0, 0
    psq_l f9, OSContext.psf[9](ctx), 0, 0
    psq_l f10, OSContext.psf[10](ctx), 0, 0
    psq_l f11, OSContext.psf[11](ctx), 0, 0
    psq_l f12, OSContext.psf[12](ctx), 0, 0
    psq_l f13, OSContext.psf[13](ctx), 0, 0
    psq_l f14, OSContext.psf[14](ctx), 0, 0
    psq_l f15, OSContext.psf[15](ctx), 0, 0
    psq_l f16, OSContext.psf[16](ctx), 0, 0
    psq_l f17, OSContext.psf[17](ctx), 0, 0
    psq_l f18, OSContext.psf[18](ctx), 0, 0
    psq_l f19, OSContext.psf[19](ctx), 0, 0
    psq_l f20, OSContext.psf[20](ctx), 0, 0
    psq_l f21, OSContext.psf[21](ctx), 0, 0
    psq_l f22, OSContext.psf[22](ctx), 0, 0
    psq_l f23, OSContext.psf[23](ctx), 0, 0
    psq_l f24, OSContext.psf[24](ctx), 0, 0
    psq_l f25, OSContext.psf[25](ctx), 0, 0
    psq_l f26, OSContext.psf[26](ctx), 0, 0
    psq_l f27, OSContext.psf[27](ctx), 0, 0
    psq_l f28, OSContext.psf[28](ctx), 0, 0
    psq_l f29, OSContext.psf[29](ctx), 0, 0
    psq_l f30, OSContext.psf[30](ctx), 0, 0
    psq_l f31, OSContext.psf[31](ctx), 0, 0

load_fprs:
    lfd f0, ctx->fpr[0]
    lfd f1, ctx->fpr[1]
    lfd f2, ctx->fpr[2]
    lfd f3, ctx->fpr[3]
    lfd f4, ctx->fpr[4]
    lfd f5, ctx->fpr[5]
    lfd f6, ctx->fpr[6]
    lfd f7, ctx->fpr[7]
    lfd f8, ctx->fpr[8]
    lfd f9, ctx->fpr[9]
    lfd f10, ctx->fpr[10]
    lfd f11, ctx->fpr[11]
    lfd f12, ctx->fpr[12]
    lfd f13, ctx->fpr[13]
    lfd f14, ctx->fpr[14]
    lfd f15, ctx->fpr[15]
    lfd f16, ctx->fpr[16]
    lfd f17, ctx->fpr[17]
    lfd f18, ctx->fpr[18]
    lfd f19, ctx->fpr[19]
    lfd f20, ctx->fpr[20]
    lfd f21, ctx->fpr[21]
    lfd f22, ctx->fpr[22]
    lfd f23, ctx->fpr[23]
    lfd f24, ctx->fpr[24]
    lfd f25, ctx->fpr[25]
    lfd f26, ctx->fpr[26]
    lfd f27, ctx->fpr[27]
    lfd f28, ctx->fpr[28]
    lfd f29, ctx->fpr[29]
    lfd f30, ctx->fpr[30]
    lfd f31, ctx->fpr[31]

exit:
    // MATCH-ONLY: a plain blr here makes the assembler turn the branch above into beqlr, retail has the branch
    opword 0x4E800020
}

static asm void __OSSaveFPUContext(register u32 unused0, register u32 unused1, register OSContext *ctx) {
    nofralloc

    lhz r3, ctx->state
    ori r3, r3, OS_CONTEXT_STATE_FP_SAVED
    sth r3, ctx->state

    stfd f0, ctx->fpr[0]
    stfd f1, ctx->fpr[1]
    stfd f2, ctx->fpr[2]
    stfd f3, ctx->fpr[3]
    stfd f4, ctx->fpr[4]
    stfd f5, ctx->fpr[5]
    stfd f6, ctx->fpr[6]
    stfd f7, ctx->fpr[7]
    stfd f8, ctx->fpr[8]
    stfd f9, ctx->fpr[9]
    stfd f10, ctx->fpr[10]
    stfd f11, ctx->fpr[11]
    stfd f12, ctx->fpr[12]
    stfd f13, ctx->fpr[13]
    stfd f14, ctx->fpr[14]
    stfd f15, ctx->fpr[15]
    stfd f16, ctx->fpr[16]
    stfd f17, ctx->fpr[17]
    stfd f18, ctx->fpr[18]
    stfd f19, ctx->fpr[19]
    stfd f20, ctx->fpr[20]
    stfd f21, ctx->fpr[21]
    stfd f22, ctx->fpr[22]
    stfd f23, ctx->fpr[23]
    stfd f24, ctx->fpr[24]
    stfd f25, ctx->fpr[25]
    stfd f26, ctx->fpr[26]
    stfd f27, ctx->fpr[27]
    stfd f28, ctx->fpr[28]
    stfd f29, ctx->fpr[29]
    stfd f30, ctx->fpr[30]
    stfd f31, ctx->fpr[31]

    mffs f0
    stfd f0, ctx->fpscr_pad
    lfd f0, ctx->fpr[0]
    mfspr r3, 0x398
    rlwinm. r3, r3, 3, 31, 31
    beq exit

    psq_st f0, OSContext.psf[0](ctx), 0, 0
    psq_st f1, OSContext.psf[1](ctx), 0, 0
    psq_st f2, OSContext.psf[2](ctx), 0, 0
    psq_st f3, OSContext.psf[3](ctx), 0, 0
    psq_st f4, OSContext.psf[4](ctx), 0, 0
    psq_st f5, OSContext.psf[5](ctx), 0, 0
    psq_st f6, OSContext.psf[6](ctx), 0, 0
    psq_st f7, OSContext.psf[7](ctx), 0, 0
    psq_st f8, OSContext.psf[8](ctx), 0, 0
    psq_st f9, OSContext.psf[9](ctx), 0, 0
    psq_st f10, OSContext.psf[10](ctx), 0, 0
    psq_st f11, OSContext.psf[11](ctx), 0, 0
    psq_st f12, OSContext.psf[12](ctx), 0, 0
    psq_st f13, OSContext.psf[13](ctx), 0, 0
    psq_st f14, OSContext.psf[14](ctx), 0, 0
    psq_st f15, OSContext.psf[15](ctx), 0, 0
    psq_st f16, OSContext.psf[16](ctx), 0, 0
    psq_st f17, OSContext.psf[17](ctx), 0, 0
    psq_st f18, OSContext.psf[18](ctx), 0, 0
    psq_st f19, OSContext.psf[19](ctx), 0, 0
    psq_st f20, OSContext.psf[20](ctx), 0, 0
    psq_st f21, OSContext.psf[21](ctx), 0, 0
    psq_st f22, OSContext.psf[22](ctx), 0, 0
    psq_st f23, OSContext.psf[23](ctx), 0, 0
    psq_st f24, OSContext.psf[24](ctx), 0, 0
    psq_st f25, OSContext.psf[25](ctx), 0, 0
    psq_st f26, OSContext.psf[26](ctx), 0, 0
    psq_st f27, OSContext.psf[27](ctx), 0, 0
    psq_st f28, OSContext.psf[28](ctx), 0, 0
    psq_st f29, OSContext.psf[29](ctx), 0, 0
    psq_st f30, OSContext.psf[30](ctx), 0, 0
    psq_st f31, OSContext.psf[31](ctx), 0, 0

exit:
    // MATCH-ONLY: a plain blr here makes the assembler turn the branch above into beqlr, retail has the branch
    opword 0x4E800020
}

asm void OSSaveFPUContext(register OSContext *ctx) {
    nofralloc

    addi r5, ctx, 0
    b __OSSaveFPUContext
}

asm void OSSetCurrentContext(register OSContext *ctx) {
    nofralloc

    lis r4, 0x80000000@ha
    stw ctx, 0xD4(r4)

    clrlwi r5, ctx, 2
    stw r5, 0xC0(r4)

    lwz r5, 0xD8(r4)
    cmpw r5, ctx
    bne other_fpu_owner

    lwz r6, ctx->srr1
    ori r6, r6, MSR_FP_BIT
    stw r6, ctx->srr1
    mfmsr r6
    ori r6, r6, 2
    mtmsr r6
    blr

other_fpu_owner:
    lwz r6, ctx->srr1
    rlwinm r6, r6, 0, 19, 17
    stw r6, ctx->srr1
    mfmsr r6
    rlwinm r6, r6, 0, 19, 17
    ori r6, r6, 2
    mtmsr r6

    isync
    blr
}

OSContext *OSGetCurrentContext(void) {
    return OS_CURRENT_CONTEXT;
}

asm u32 OSSaveContext(register OSContext *ctx) {
    nofralloc

    stmw r13, ctx->gpr[13]

    mfspr r0, GQR1
    stw r0, ctx->gqr[1]
    mfspr r0, GQR2
    stw r0, ctx->gqr[2]
    mfspr r0, GQR3
    stw r0, ctx->gqr[3]
    mfspr r0, GQR4
    stw r0, ctx->gqr[4]
    mfspr r0, GQR5
    stw r0, ctx->gqr[5]
    mfspr r0, GQR6
    stw r0, ctx->gqr[6]
    mfspr r0, GQR7
    stw r0, ctx->gqr[7]

    mfcr r0
    stw r0, ctx->cr

    mflr r0
    stw r0, ctx->lr
    stw r0, ctx->srr0

    mfmsr r0
    stw r0, ctx->srr1

    mfctr r0
    stw r0, ctx->ctr

    mfxer r0
    stw r0, ctx->xer

    stw r1, ctx->gpr[1]
    stw r2, ctx->gpr[2]

    li r0, 1
    stw r0, ctx->gpr[3]

    li r3, 0
    blr
}

asm void OSLoadContext(register OSContext *ctx) {
    nofralloc

    // if the saved pc is inside the restartable part of OSDisableInterrupts, back it up to the start
    lis r4, __RAS_OSDisableInterrupts_begin@ha
    lwz r6, ctx->srr0
    addi r5, r4, __RAS_OSDisableInterrupts_begin@l
    cmplw r6, r5
    ble not_in_disable
    lis r4, __RAS_OSDisableInterrupts_end@ha
    addi r0, r4, __RAS_OSDisableInterrupts_end@l
    cmplw r6, r0
    bge not_in_disable
    stw r5, ctx->srr0

not_in_disable:
    lwz r0, ctx->gpr[0]
    lwz r1, ctx->gpr[1]
    lwz r2, ctx->gpr[2]
    lhz r4, ctx->state
    rlwinm. r5, r4, 0, 30, 30
    beq load_saved_gprs

    rlwinm r4, r4, 0, 31, 29
    sth r4, ctx->state
    lmw r5, ctx->gpr[5]
    b load_special

load_saved_gprs:
    lmw r13, ctx->gpr[13]

load_special:
    lwz r4, ctx->gqr[1]
    mtspr GQR1, r4
    lwz r4, ctx->gqr[2]
    mtspr GQR2, r4
    lwz r4, ctx->gqr[3]
    mtspr GQR3, r4
    lwz r4, ctx->gqr[4]
    mtspr GQR4, r4
    lwz r4, ctx->gqr[5]
    mtspr GQR5, r4
    lwz r4, ctx->gqr[6]
    mtspr GQR6, r4
    lwz r4, ctx->gqr[7]
    mtspr GQR7, r4

    lwz r4, ctx->cr
    mtcrf 255, r4
    lwz r4, ctx->lr
    mtlr r4
    lwz r4, ctx->ctr
    mtctr r4
    lwz r4, ctx->xer
    mtxer r4

    mfmsr r4
    rlwinm r4, r4, 0, 17, 15
    rlwinm r4, r4, 0, 31, 29
    mtmsr r4

    lwz r4, ctx->srr0
    mtsrr0 r4

    lwz r4, ctx->srr1
    mtsrr1 r4

    lwz r4, ctx->gpr[4]
    lwz r3, ctx->gpr[3]

    rfi
}

asm u32 OSGetStackPointer(void) {
    nofralloc

    mr r3, r1
    blr
}

asm int OSSwitchFiber(register u32 func, register u32 stack) {
    nofralloc

    mflr r0
    mr r5, r1
    stwu r5, -8(stack)
    mr r1, stack
    stw r0, 4(r5)
    mtlr func
    blrl

    lwz r5, 0(r1)
    lwz r0, 4(r5)
    mtlr r0
    mr r1, r5
    blr
}

asm int OSSwitchFiberEx(register u32 a0, register u32 a1, register u32 a2, register u32 a3, register u32 func, register u32 stack) {
    nofralloc

    mflr r0
    mr r9, r1
    stwu r9, -8(stack)
    mr r1, stack
    stw r0, 4(r9)
    mtlr func
    blrl

    lwz r5, 0(r1)
    lwz r0, 4(r5)
    mtlr r0
    mr r1, r5
    blr
}

void OSClearContext(OSContext *ctx) {
    ctx->mode = 0;
    ctx->state = 0;

    if (ctx == OS_CURRENT_FPU_CONTEXT) {
        OS_CURRENT_FPU_CONTEXT = NULL;
    }
}

asm void OSInitContext(register OSContext *ctx, register u32 srr0, register u32 stack) {
    nofralloc

    stw srr0, ctx->srr0
    stw stack, ctx->gpr[1]

    li r11, 0
    ori r11, r11, 0x9032
    stw r11, ctx->srr1

    li r0, 0
    stw r0, ctx->cr
    stw r0, ctx->xer

    stw r2, ctx->gpr[2]
    stw r13, ctx->gpr[13]

    stw r0, ctx->gpr[3]
    stw r0, ctx->gpr[4]
    stw r0, ctx->gpr[5]
    stw r0, ctx->gpr[6]
    stw r0, ctx->gpr[7]
    stw r0, ctx->gpr[8]
    stw r0, ctx->gpr[9]
    stw r0, ctx->gpr[10]
    stw r0, ctx->gpr[11]
    stw r0, ctx->gpr[12]
    stw r0, ctx->gpr[14]
    stw r0, ctx->gpr[15]
    stw r0, ctx->gpr[16]
    stw r0, ctx->gpr[17]
    stw r0, ctx->gpr[18]
    stw r0, ctx->gpr[19]
    stw r0, ctx->gpr[20]
    stw r0, ctx->gpr[21]
    stw r0, ctx->gpr[22]
    stw r0, ctx->gpr[23]
    stw r0, ctx->gpr[24]
    stw r0, ctx->gpr[25]
    stw r0, ctx->gpr[26]
    stw r0, ctx->gpr[27]
    stw r0, ctx->gpr[28]
    stw r0, ctx->gpr[29]
    stw r0, ctx->gpr[30]
    stw r0, ctx->gpr[31]

    stw r0, ctx->gqr[0]
    stw r0, ctx->gqr[1]
    stw r0, ctx->gqr[2]
    stw r0, ctx->gqr[3]
    stw r0, ctx->gqr[4]
    stw r0, ctx->gqr[5]
    stw r0, ctx->gqr[6]
    stw r0, ctx->gqr[7]

    b OSClearContext
}

void OSDumpContext(OSContext *ctx) {
    u32 i;
    u32 *sp;

    OSReport("------------------------- Context 0x%08x -------------------------\n", ctx);

    for (i = 0; i < 16; i++) {
        OSReport("r%-2d  = 0x%08x (%14d)  r%-2d  = 0x%08x (%14d)\n", i, ctx->gpr[i], ctx->gpr[i], i + 16,
                 ctx->gpr[i + 16], ctx->gpr[i + 16]);
    }

    OSReport("LR   = 0x%08x                   CR   = 0x%08x\n", ctx->lr, ctx->cr);
    OSReport("SRR0 = 0x%08x                   SRR1 = 0x%08x\n", ctx->srr0, ctx->srr1);

    OSReport("\nGQRs----------\n");
    for (i = 0; i < 4; i++) {
        OSReport("gqr%d = 0x%08x \t gqr%d = 0x%08x\n", i, ctx->gqr[i], i + 4, ctx->gqr[i + 4]);
    }

    if (ctx->state & OS_CONTEXT_STATE_FP_SAVED) {
        OSContext tempCtx;
        BOOL enabled = OSDisableInterrupts();
        OSContext *currCtx = OSGetCurrentContext();

        OSClearContext(&tempCtx);
        OSSetCurrentContext(&tempCtx);

        OSReport("\n\nFPRs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("fr%d \t= %d \t fr%d \t= %d\n", i, (u32)ctx->fpr[i], i + 1, (u32)ctx->fpr[i + 1]);
        }

        OSReport("\n\nPSFs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("ps%d \t= 0x%x \t ps%d \t= 0x%x\n", i, (u32)ctx->psf[i], i + 1, (u32)ctx->psf[i + 1]);
        }

        OSClearContext(&tempCtx);
        OSSetCurrentContext(currCtx);
        OSRestoreInterrupts(enabled);
    }

    OSReport("\nAddress:      Back Chain    LR Save\n");
    i = 0;
    sp = (u32 *)ctx->gpr[1];
    for (; sp != NULL && sp != (u32 *)0xFFFFFFFF && i++ < 16; sp = (u32 *)*sp) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", sp, *sp, sp[1]);
    }
}

static asm void OSSwitchFPUContext(register u8 err, register OSContext *ctx) {
    nofralloc

    mfmsr r5
    ori r5, r5, MSR_FP_BIT
    mtmsr r5

    isync

    lwz r5, ctx->srr1
    ori r5, r5, MSR_FP_BIT
    mtsrr1 r5

    lis r3, 0x80000000@ha
    lwz r5, 0xD8(r3)
    stw ctx, 0xD8(r3)
    cmpw r5, ctx
    beq same_owner
    cmpwi r5, 0
    beq no_owner
    bl __OSSaveFPUContext

no_owner:
    bl __OSLoadFPUContext

same_owner:
    lwz r3, ctx->cr
    mtcrf 255, r3
    lwz r3, ctx->lr
    mtlr r3
    lwz r3, ctx->srr0
    mtsrr0 r3
    lwz r3, ctx->ctr
    mtctr r3
    lwz r3, ctx->xer
    mtxer r3

    lhz r3, ctx->state
    rlwinm r3, r3, 0, 31, 29
    sth r3, ctx->state

    lwz r5, ctx->gpr[5]
    lwz r3, ctx->gpr[3]
    lwz r4, ctx->gpr[4]

    rfi
}

void __OSContextInit(void) {
    __OSSetExceptionHandler(7, OSSwitchFPUContext);
    OS_CURRENT_FPU_CONTEXT = NULL;
    DBPrintf("FPU-unavailable handler installed\n");
}
