#pragma once

#include <StaticAssert.h>
#include <st/loader/st_loader.h>
#include <types.h>

// The loaders of the stage sound groups (sora_melee st_loader_stage_se.o): each picks a sound group, loads it and frees
// it again when the stage changes.
class stLoaderStageSe : public stLoader {
public:
    int m_groupHandle; // 0x48, what loadSoundGroup returned (-1: nothing loaded)
    int m_groupId;     // 0x4C, the group buildSoundGroup picked (-1: none)
    int m_heapLevel;   // 0x50

    stLoaderStageSe();

    virtual void processBegin();
    virtual ~stLoaderStageSe();
    virtual bool isLoad();
    virtual int buildSoundGroup();
};
static_assert(sizeof(stLoaderStageSe) == 0x54, "Class is wrong size!");

// The common sound group of the Subspace Emissary.
class stLoaderCommonSeAdventure : public stLoaderStageSe {
public:
    stLoaderCommonSeAdventure();
    virtual ~stLoaderCommonSeAdventure();
    virtual int buildSoundGroup();
};

// The sound group of the Pokemon Trainer's Pokemon.
class stLoaderPokemonSe : public stLoader {
public:
    int m_groupHandle; // 0x48
    int m_groupId;     // 0x4C
    int m_heapLevel;   // 0x50

    stLoaderPokemonSe();

    virtual void processBegin();
    virtual ~stLoaderPokemonSe();
    virtual int buildSoundGroup();
};
static_assert(sizeof(stLoaderPokemonSe) == 0x54, "Class is wrong size!");

// The stage sound group of a Subspace Emissary stage, looked up from the stage id.
class stLoaderStageSeAdventure : public stLoaderPokemonSe {
public:
    stLoaderStageSeAdventure();
    virtual ~stLoaderStageSeAdventure();
    virtual int buildSoundGroup();
};
