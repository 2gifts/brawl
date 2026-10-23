#pragma once
// Local BrawlHeaders shadow: names the setup-request state behind the loader pointers and adds the sequenced
// setup calls (sora_melee 0x23A508, 0x23A528). The loader pointers are unchanged.

#include <StaticAssert.h>
#include <st/loader/st_loader_player.h>
#include <gm/gm_lib.h>
#include <types.h>

class stLoaderManager {
public:
    stLoaderPlayer* m_loaderPlayers[MAX_PLAYERS];
    void* m_loaderStage;
    void* m_loaderStageAlt;
    void* m_loaderStageSe;
    void* m_loaderPokemonSe;
    void* m_loaderInfo;
    void* m_loaderSandbag;
    void* m_loaderStageAdvCommon;
    // HYPOTHESIS: the bit set of created managers, the set the current setup wants, the set already set up and the
    // last requested set; the two flags mark a pending request and a finished setup, and the last two words are the
    // position of the setup sequence.
    u32 unk38;
    u32 unk3C;
    u32* unk40;
    u32* m_request;
    bool m_isRequested;
    bool m_isSetupDone;
    const void* unk4C;
    int unk50;

    // HYPOTHESIS: parameter types from the callers' register use.
    void sqSetupRequest(const u32* request);
    bool sqSetupSync(bool wait);
};
static_assert(sizeof(stLoaderManager) == 0x54, "Class is wrong size!");

extern stLoaderManager* g_stLoaderManager;
