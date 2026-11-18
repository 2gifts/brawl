typedef unsigned char u8;

extern u8 Debug_BBA;
extern u8 _stack_addr[];
extern u8 _SDA_BASE_[];
extern u8 _SDA2_BASE_[];
extern u8 _rom_copy_info[];
extern u8 _bss_init_info[];

extern void __init_hardware(void);
extern void __OSPSInit(void);
extern void __OSFPRInit(void);
extern void __OSCacheInit(void);
extern void DBInit(void);
extern void OSInit(void);
extern void OSResetSystem(void);
extern void InitMetroTRK(void);
extern void InitMetroTRK_BBA(void);
extern void __init_user(void);
extern int main(int argc, char** argv);
extern void exit(int);
extern void* memcpy(void*, const void*, unsigned long);
extern void* memset(void*, int, unsigned long);
extern void __flush_cache(void* addr, unsigned long size);

static void __check_pad3(void);
static void __set_debug_bba(void);
static u8 __get_debug_bba(void);
static void __init_registers(void);
static void __init_data(void);

static __declspec(section ".init") asm void __check_pad3(void) {
    nofralloc
    lis r3, 0x8000
    lhz r0, 0x30e4(r3)
    andi. r0, r0, 0xeef
    cmpwi r0, 0xeef
    bnelr
    li r3, 0x0
    li r4, 0x0
    li r5, 0x0
    b OSResetSystem
    blr
}

static __declspec(section ".init") void __set_debug_bba(void) {
    Debug_BBA = 1;
}

static __declspec(section ".init") u8 __get_debug_bba(void) {
    return Debug_BBA;
}

__declspec(weak) __declspec(section ".init") asm void __start(void) {
    nofralloc
    bl __init_registers
    bl __init_hardware
    li r0, -0x1
    stwu r1, -0x8(r1)
    stw r0, 0x4(r1)
    stw r0, 0x0(r1)
    bl __init_data
    li r0, 0x0
    lis r6, 0x8000
    addi r6, r6, 0x44
    stw r0, 0x0(r6)
    lis r6, 0x8000
    addi r6, r6, 0xf4
    lwz r6, 0x0(r6)
    cmplwi r6, 0x0
    beq L80004084
    lwz r7, 0xc(r6)
    b L800040A4
L80004084:
    lis r5, 0x8000
    addi r5, r5, 0x34
    lwz r5, 0x0(r5)
    cmplwi r5, 0x0
    beq L800040E0
    lis r7, 0x8000
    addi r7, r7, 0x30e8
    lwz r7, 0x0(r7)
L800040A4:
    li r5, 0x0
    cmplwi r7, 0x2
    beq L800040D0
    cmplwi r7, 0x3
    li r5, 0x1
    beq L800040D0
    cmplwi r7, 0x4
    bne L800040E0
    li r5, 0x2
    bl __set_debug_bba
    b L800040E0
L800040D0:
    lis r6, InitMetroTRK@ha
    addi r6, r6, InitMetroTRK@l
    mtlr r6
    blrl
L800040E0:
    lis r6, 0x8000
    addi r6, r6, 0xf4
    lwz r5, 0x0(r6)
    cmplwi r5, 0x0
    beq+ L80004150
    lwz r6, 0x8(r5)
    cmplwi r6, 0x0
    beq+ L80004150
    add r6, r5, r6
    lwz r14, 0x0(r6)
    cmplwi r14, 0x0
    beq L80004150
    addi r15, r6, 0x4
    mtctr r14
L80004118:
    addi r6, r6, 0x4
    lwz r7, 0x0(r6)
    add r7, r7, r5
    stw r7, 0x0(r6)
    bdnz L80004118
    lis r5, 0x8000
    addi r5, r5, 0x34
    clrrwi r7, r15, 5
    stw r7, 0x0(r5)
    lis r5, 0x8000
    addi r5, r5, 0x3110
    clrrwi r7, r15, 5
    stw r7, 0x0(r5)
    b L80004158
L80004150:
    li r14, 0x0
    li r15, 0x0
L80004158:
    bl DBInit
    bl OSInit
    lis r4, 0x8000
    addi r4, r4, 0x30e6
    lhz r3, 0x0(r4)
    andi. r5, r3, 0x8000
    beq L80004180
    andi. r3, r3, 0x7fff
    cmplwi r3, 0x1
    bne L80004184
L80004180:
    bl __check_pad3
L80004184:
    bl __get_debug_bba
    cmplwi r3, 0x1
    bne L80004194
    bl InitMetroTRK_BBA
L80004194:
    bl __init_user
    mr r3, r14
    mr r4, r15
    bl main
    b exit
}

static __declspec(section ".init") asm void __init_registers(void) {
    nofralloc
    li r0, 0x0
    li r3, 0x0
    li r4, 0x0
    li r5, 0x0
    li r6, 0x0
    li r7, 0x0
    li r8, 0x0
    li r9, 0x0
    li r10, 0x0
    li r11, 0x0
    li r12, 0x0
    li r14, 0x0
    li r15, 0x0
    li r16, 0x0
    li r17, 0x0
    li r18, 0x0
    li r19, 0x0
    li r20, 0x0
    li r21, 0x0
    li r22, 0x0
    li r23, 0x0
    li r24, 0x0
    li r25, 0x0
    li r26, 0x0
    li r27, 0x0
    li r28, 0x0
    li r29, 0x0
    li r30, 0x0
    li r31, 0x0
    lis r1, _stack_addr@h
    ori r1, r1, _stack_addr@l
    lis r2, _SDA2_BASE_@h
    ori r2, r2, _SDA2_BASE_@l
    lis r13, _SDA_BASE_@h
    ori r13, r13, _SDA_BASE_@l
    blr
}

static __declspec(section ".init") asm void __init_data(void) {
    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    lis r29, _rom_copy_info@ha
    addi r29, r29, _rom_copy_info@l
L80004258:
    lwz r30, 0x8(r29)
    cmpwi r30, 0x0
    beq L80004298
    lwz r4, 0x0(r29)
    lwz r31, 0x4(r29)
    beq L80004290
    cmplw r31, r4
    beq L80004290
    mr r3, r31
    mr r5, r30
    bl memcpy
    mr r3, r31
    mr r4, r30
    bl __flush_cache
L80004290:
    addi r29, r29, 0xc
    b L80004258
L80004298:
    lis r29, _bss_init_info@ha
    addi r29, r29, _bss_init_info@l
L800042A0:
    lwz r5, 0x4(r29)
    cmpwi r5, 0x0
    beq L800042C4
    lwz r3, 0x0(r29)
    beq L800042BC
    li r4, 0x0
    bl memset
L800042BC:
    addi r29, r29, 0x8
    b L800042A0
L800042C4:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
}

__declspec(section ".init") asm void __init_hardware(void) {
    nofralloc
    mfmsr r0
    ori r0, r0, 0x2000
    mtmsr r0
    mflr r31
    bl __OSPSInit
    bl __OSFPRInit
    bl __OSCacheInit
    mtlr r31
    blr
}

__declspec(section ".init") asm void __flush_cache(void* addr, unsigned long size) {
    nofralloc
    lis r5, 0xffff
    ori r5, r5, 0xfff1
    and r5, r5, r3
    subf r3, r5, r3
    add r4, r4, r3
L80004318:
    dcbst r0, r5
    sync
    icbi r0, r5
    addic r5, r5, 0x8
    subic. r4, r4, 0x8
    bge L80004318
    isync
    blr
}
