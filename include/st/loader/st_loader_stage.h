#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <st/loader/st_loader.h>
#include <types.h>

class Stage;

class stLoaderStage : public stLoader {
public:
    char m_fileName[0x40];          // 0x48
    const char* m_moduleName;       // 0x88
    u32 m_stageKind;                // 0x8C
    u32 m_subStageKind;             // 0x90
    int m_effectResourceId;         // 0x94
    u8 unk98;                       // 0x98
    u8 unk99;                       // 0x99
    gfArchive* m_archive;           // 0x9C
    gfArchive* m_secondaryArchive;  // 0xA0
    gfArchive* m_fileArchive;       // 0xA4
    gfArchive* m_effectArchive;     // 0xA8
    void* m_secondaryData;          // 0xAC
    void* m_fileData;               // 0xB0
    void* m_effectData;             // 0xB4
    void* m_heapBuffer;             // 0xB8
    u8* m_heapCursor;               // 0xBC
    void* m_readBuffer;             // 0xC0
    Stage* m_stage;                 // 0xC4
    stLoaderStage* m_commonLoader;  // 0xC8

    virtual void processBegin();
    virtual ~stLoaderStage();
    virtual void entryEntity();
    virtual void removeEntity();
    virtual bool isLoad();
    virtual bool isRemove();
    virtual void buildFileName();

    u32 getMemoryBlankSize();
    void entryEntitySuddenDeath();
    void enablePause();
    void disablePause();
    Stage* getStage();
};
static_assert(sizeof(stLoaderStage) == 0xCC, "Class is wrong size!");
