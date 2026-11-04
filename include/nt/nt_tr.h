#pragma once

#include <nt/nt_report.h>
#include <string.h>
#include <types.h>

class NtTr : public NtBase {
public:
    NtTr();
    virtual ~NtTr();
    void create();
    void init(bool);
    void openAID(u8);
    void closeAID(u8);
    void resetAllAID();
    static void handleRecvName(u8, const void*, u32);
    // HYPOTHESIS: parameter types follow register widths and output stores.
    bool refSendCmd(u8, void**, u16, u16*, u8);

    u8 unk8[0x10];
    u32 unk18;
    u32 unk1C;
    u32 unk20;
    u32 unk24;
    u32 unk28;
    u32 unk2C;
    u32 unk30;
    u8 unk34;
    u8 unk35[3];
    u32 unk38;
    u8 unk3C;
    u8 unk3D;
    u8 unk3E_b6 : 2;
    u8 unk3E_b5 : 1;
    u8 unk3E_b4 : 1;
    u8 unk3E_b2 : 2;
    u8 unk3E_b1 : 1;
    u8 unk3E_b0 : 1;
    u8 unk3F;
    u32 unk40;
    // HYPOTHESIS: per-AID queue fields follow reset and reservation accesses.
    struct {
        u8 unk0 : 8;
        u8 unk1;
        u8 unk2;
        u8 unk3;
        float unk4;
        u16 unk8;
        u8 unkA[6];
        u16 unk10;
        u8 unk12[2];
        s32 unk14;
        s32 unk18;
        u8 unk1C[4][0x38C];
        u8 unkE4C[4];
        // HYPOTHESIS: inline initialization shared by the per-AID reset paths.
        void init() {
            unk0 = 0;
            unk1 = 0;
            unk2 = 0;
            unk4 = 0.0f;
            unk8 = 0;
            unk10 = 0;
            unk18 = 0;
            unk14 = 0;
            memset(unk1C, 0, sizeof(unk1C));
        }
    } unk44[4];
    struct {
        u32 unk0;
        u32 unk4;
        void* unk8;
        u32 unkC;
        struct {
            u32 unk0;
            void* unk4;
            u8 unk8[0x388];
        } unk10[4];
    } unk3984;
    u32 unk47D4;
    u32 unk47D8;
    u32 unk47DC;
    u8 unk47E0[0x28];
    u8 unk4808;
    u8 unk4809[3];
    struct {
        u8 unk0;
        u8 unk1[3];
        u32 unk4;
        u32 unk8;
        u32 unkC;
        u32 unk10;
        u32 unk14;
    } unk480C[4];
};

extern NtTr* lbl_805A03A8;
