#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_array.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

// HYPOTHESIS: a loaded trophy texture archive (it_figure_archive, not decompiled): the file id first, a byte that marks it
// for removal at 0x8, a vtable at 0x10.
class itFigureArchive {
public:
    u32 m_fileId;     // 0x0
    u8 _4[4];
    u8 m_isRemove;    // 0x8
    u8 _9[3];
    int m_refCount;   // 0xC, HYPOTHESIS: preload requests still holding the archive

    virtual ~itFigureArchive();
    bool isComp();                 // HYPOTHESIS: the archive finished loading
    bool isRemovable();            // HYPOTHESIS: nothing uses it any more
    void* getTexFile(u32 figureId);
};

// The trophy (figure) archives of the items (sora_melee it_figure_manager.o): the trophy list data and the loaded
// trophy texture archives. Only the members the decompiled functions use are described.
class itFigureManager {
public:
    virtual ~itFigureManager();

    tyFigListDataManager* m_figList; // 0x4
    soArrayList<itFigureArchive*, 64> m_archives; // 0x8
    int m_preloadIndex;              // 0x310

    bool isValidFigureId(u32 id);
    int getFigureFileId(u32 id);
    bool isAllStarFigureId(u32 id);
    const char* getTexName(u32 id);
    bool getOffsetAndScale(u32 id, Vec2f* offset, Vec2f* scale);
    bool isCompFigureTexPreloadAll() const;
    bool isCompFigureTex(u32 id);
    void removeFigureTexAll();
    bool onProcessFixPosition();
    void* getTexFile(u32 id);
    itFigureArchive* findArchive(u32 fileId) const;
    bool isExistFigureArchive(u32 id);
    void unloadFigureTex(u32 id, bool countDown);

};
static_assert(sizeof(itFigureManager) == 0x314, "Class is wrong size!");
