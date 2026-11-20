#pragma once

// What the melee scenes of the scene module (scEdit, scVsResult, scCharacterRoll, ...) share for their start: the copy of
// the melee mode, the scMelee helpers and the operators made inline.

#include <StaticAssert.h>
#include <gm/gm_global_mode_melee.h>
#include <sc/sc_melee.h>
#include <sr/sr_common.h>
#include <st/loader/st_loader_player.h>
#include <st/operator/st_operator_controller.h>
#include <st/operator/st_operator_info.h>
#include <st/operator/st_operator_network.h>
#include <types.h>

// scMelee helpers of the scene module that have no names yet.
// TODO: clean up - name them as scMelee members.
extern "C" void fn_1_139E0(scMelee* scene);
extern "C" void fn_1_1A4DC(scMelee* scene);
extern "C" void fn_1_194A4(scMelee* scene, int unk);
extern "C" void fn_1_19968(scMelee* scene, int unk);
// sora_melee: a member of stLoaderPlayer and the maker of the stage event work of the edit stage.
// TODO: clean up - name them.
extern "C" void fn_27_23BB10(stLoaderPlayer* loader);
extern "C" void* fn_27_27CBE0();

// The melee mode the scenes copy from GameGlobal (the copy is a static of sc_melee.cpp).
// TODO: clean up - the shared gmGlobalModeMelee header has a union of 8 bytes in front; this layout gives the memberwise
// copy (3 bytes, the init data and the players) of the original.
struct scMeleeModeCopy {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    gmMeleeInitData m_meleeInitData;
    gmPlayerInitData m_playersInitData[MAX_PLAYERS];
};
extern scMeleeModeCopy lbl_1_bss_90;

// The bits of a player loader the melee scenes change (byte 0x5D, bit 0x80).
// TODO: clean up - name the field in stLoaderPlayer.
struct scLoaderPlayerFlags {
    u8 m_unk0[0x5D];
    u8 m_unk5D_b7 : 1;
    u8 m_unk5D_b6 : 7;
};

// The stage event work of the melee scenes (made by sora_melee 0x27CBE0 for the edit stage).
// HYPOTHESIS: only the operators it gets are known.
struct scStageEventWork {
    u8 m_unk0[0x10];
    void* m_rebirths[MAX_PLAYERS];      // 0x10
    stOperatorInfo* m_info;             // 0x2C
    void* m_rule;                       // 0x30
};

// TODO: clean up - stOperatorController's constructor and startController are called without the name and with a flag in
// the original (the map names say otherwise); these declarations give the same calls.
extern "C" stOperatorController* __ct__20stOperatorControllerFPCc(stOperatorController* controller);
extern "C" void startController__20stOperatorControllerFv(stOperatorController* controller, bool flag);
static inline stOperatorController* newOperatorController(Heaps::HeapType heap) {
    stOperatorController* controller = static_cast<stOperatorController*>(operator new(sizeof(stOperatorController), heap));
    if (controller != NULL) {
        controller = __ct__20stOperatorControllerFPCc(controller);
    }
    return controller;
}

// The network operator of a scene without a network (every request is done at once).
class stOperatorNetworkNull : public stOperatorNetwork {
public:
    stOperatorNetworkNull() : stOperatorNetwork("StOperatorNetworkNull") { }
    virtual ~stOperatorNetworkNull() { }
};

