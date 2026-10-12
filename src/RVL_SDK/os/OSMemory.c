// OSMemory: physical memory size getters, the bat setup for mem1/mem2 (done in real mode, one tiny asm
// routine per supported memory layout) and the memory protection interrupt.
#include "revolution/os.h"
#include "revolution/os/OSReset.h"

#define MI_REGS ((volatile u16 *)0xCC004000)

extern OSErrorHandler __OSErrorTable[17];


static BOOL OnShutdown(BOOL final, u32 event);
static OSShutdownFunctionInfo ShutdownFunctionInfo = {OnShutdown, 127, NULL, NULL};

u32 OSGetPhysicalMem1Size(void) {
    return *(u32 *)0x80003100;
}

u32 OSGetPhysicalMem2Size(void) {
    return *(u32 *)0x80003118;
}

u32 OSGetConsoleSimulatedMem1Size(void) {
    return *(u32 *)0x80003104;
}

u32 OSGetConsoleSimulatedMem2Size(void) {
    return *(u32 *)0x8000311C;
}

static BOOL OnShutdown(BOOL final, u32 event) {
#pragma unused(event)

    if (final) {
        MI_REGS[0x10 / 2] = 0xFF;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                           OS_INTERRUPTMASK_MEM_3);
    }

    return TRUE;
}

// TYPO
static void MEMIntrruptHandler(__OSInterrupt intr, OSContext* ctx) {
#pragma unused(intr)

    u32 dsisr = MI_REGS[0x1E / 2];
    u32 dar = (MI_REGS[0x24 / 2] & 0x3ff) << 0x10 | MI_REGS[0x22 / 2];
    MI_REGS[0x20 / 2] = 0;

    if (__OSErrorTable[OS_ERROR_PROTECTION] != NULL) {
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, ctx, dsisr, dar);
    } else {
        __OSUnhandledException(OS_ERROR_PROTECTION, ctx, dsisr, dar);
    }
}

static asm void ConfigMEM1_24MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x800001FF@ha
    addi r3, r3, 0x800001FF@l
    lis r6, 0x01000002@ha
    addi r6, r6, 0x01000002@l
    lis r5, 0x810000FF@ha
    addi r5, r5, 0x810000FF@l
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM1_48MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x800003FF@ha
    addi r3, r3, 0x800003FF@l
    lis r6, 0x02000002@ha
    addi r6, r6, 0x02000002@l
    lis r5, 0x820001FF@ha
    addi r5, r5, 0x820001FF@l
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_52MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900003FF@ha
    addi r3, r3, 0x900003FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    lis r4, 0x12000002@ha
    addi r4, r4, 0x12000002@l
    lis r3, 0x920001FF@ha
    addi r3, r3, 0x920001FF@l
    lis r6, 0x13000002@ha
    addi r6, r6, 0x13000002@l
    lis r5, 0x9300007F@ha
    addi r5, r5, 0x9300007F@l
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r4
    mtspr 0x23c, r3
    isync
    mtspr 0x234, r7
    mtspr 0x235, r4
    mtspr 0x234, r3
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r6
    mtspr 0x23e, r5
    isync
    mtspr 0x236, r7
    mtspr 0x237, r6
    mtspr 0x236, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_56MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900003FF@ha
    addi r3, r3, 0x900003FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    lis r4, 0x12000002@ha
    addi r4, r4, 0x12000002@l
    lis r3, 0x920001FF@ha
    addi r3, r3, 0x920001FF@l
    lis r6, 0x13000002@ha
    addi r6, r6, 0x13000002@l
    lis r5, 0x930000FF@ha
    addi r5, r5, 0x930000FF@l
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r4
    mtspr 0x23c, r3
    isync
    mtspr 0x234, r7
    mtspr 0x235, r4
    mtspr 0x234, r3
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r6
    mtspr 0x23e, r5
    isync
    mtspr 0x236, r7
    mtspr 0x237, r6
    mtspr 0x236, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_64MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900007FF@ha
    addi r3, r3, 0x900007FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    mtspr 0x234, r7
    mtspr 0x235, r7
    isync
    mtspr 0x236, r7
    mtspr 0x237, r7
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r7
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_112MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900007FF@ha
    addi r3, r3, 0x900007FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD0000FFF@ha
    addi r5, r5, 0xD0000FFF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    lis r4, 0x14000002@ha
    addi r4, r4, 0x14000002@l
    lis r3, 0x940003FF@ha
    addi r3, r3, 0x940003FF@l
    lis r6, 0x16000002@ha
    addi r6, r6, 0x16000002@l
    lis r5, 0x960001FF@ha
    addi r5, r5, 0x960001FF@l
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r4
    mtspr 0x23c, r3
    isync
    mtspr 0x234, r7
    mtspr 0x235, r4
    mtspr 0x234, r3
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r6
    mtspr 0x23e, r5
    isync
    mtspr 0x236, r7
    mtspr 0x237, r6
    mtspr 0x236, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_128MB(void) {
    nofralloc

    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x90000FFF@ha
    addi r3, r3, 0x90000FFF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD0000FFF@ha
    addi r5, r5, 0xD0000FFF@l
    isync
    mtspr 0x238, r7
    mtspr 0x239, r4
    mtspr 0x238, r3
    isync
    mtspr 0x230, r7
    mtspr 0x231, r4
    mtspr 0x230, r3
    isync
    mtspr 0x23a, r7
    mtspr 0x23b, r6
    mtspr 0x23a, r5
    isync
    mtspr 0x232, r7
    mtspr 0x233, r7
    isync
    mtspr 0x234, r7
    mtspr 0x235, r7
    isync
    mtspr 0x236, r7
    mtspr 0x237, r7
    isync
    mtspr 0x23c, r7
    mtspr 0x23d, r7
    isync
    mtspr 0x23e, r7
    mtspr 0x23f, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM_ES1_0(void) {
    nofralloc

    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x80000FFF@ha
    addi r3, r3, 0x80000FFF@l
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void RealMode(register void* config) {
    nofralloc

    clrlwi config, config, 2
    mtspr 0x1a, config
    mfmsr config
    rlwinm config, config, 0, 0x1c, 0x19
    mtspr 0x1b, config
    rfi
}

static void BATConfig(void) {
    u32 mem1sim;
    u32 mem1phys;
    void* mem2end;

    if (*(u32 *)0x80003138 == 0) {
        // @bug Checks function address rather than result
        if (OSGetPhysicalMem1Size == 0) {
            RealMode(ConfigMEM_ES1_0);
            return;
        }
    }

    mem1sim = OSGetConsoleSimulatedMem1Size();
    mem1phys = OSGetPhysicalMem1Size();
    if (OSGetConsoleSimulatedMem1Size() < mem1phys && mem1sim == 0x1800000) {
        DCInvalidateRange((void*)0x81800000, 0x1800000);
        MI_REGS[0x28 / 2] = 2;
    }

    if (mem1sim <= 0x1800000) {
        RealMode(ConfigMEM1_24MB);
    } else if (mem1sim <= 0x3000000) {
        RealMode(ConfigMEM1_48MB);
    }

    mem2end = *(void**)0x80003120;
    if (OSGetConsoleSimulatedMem2Size() <= 0x4000000) {
        if (mem2end <= (void*)0x93400000) {
            RealMode(ConfigMEM2_52MB);
        } else if (mem2end <= (char*)0x93400000 + 0x400000) {
            RealMode(ConfigMEM2_56MB);
        } else {
            RealMode(ConfigMEM2_64MB);
        }
    } else if (OSGetConsoleSimulatedMem2Size() <= 0x8000000) {
        if (mem2end <= (void*)0x97000000) {
            RealMode(ConfigMEM2_112MB);
        } else {
            RealMode(ConfigMEM2_128MB);
        }
    }
}

void __OSInitMemoryProtection(void) {
    BOOL enabled = OSDisableInterrupts();

    MI_REGS[0x20 / 2] = 0;
    MI_REGS[0x10 / 2] = 0xFF;

    __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                       OS_INTERRUPTMASK_MEM_3);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_0, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_1, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_2, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_3, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_ADDRESS, MEMIntrruptHandler);
    OSRegisterShutdownFunction(&ShutdownFunctionInfo);

    BATConfig();
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_ADDRESS);

    OSRestoreInterrupts(enabled);
}
