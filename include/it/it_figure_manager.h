#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <it/it_figure_archive.h>
#include <so/so_array.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

// The trophy (figure) archives of the items (sora_melee it_figure_manager.o): the trophy list data and the loaded
// trophy texture archives. Only the members the decompiled functions use are described.
class itFigureManager {
public:
    virtual ~itFigureManager();

    tyFigListDataManager* m_figList; // 0x4
    soArrayList<itFigureArchive*, 64> m_archives; // 0x8
    int m_preloadMax;                // 0x310, HYPOTHESIS: how many archives may be preloaded at once

    itFigureManager(void* figListData, u32 figListSize);

    bool isValidFigureId(u32 id);
    int getFigureFileId(u32 id);
    bool isAllStarFigureId(u32 id);
    bool preloadFigureTex(u32 id, bool addRef);
    const char* getTexName(u32 id);
    bool getOffsetAndScale(u32 id, Vec2f* offset, Vec2f* scale);
    bool isCompFigureTexPreloadAll() const;
    bool isCompFigureTex(u32 id);
    void removeFigureTexAll();
    bool onProcessFixPosition();
    nw4r::g3d::ResFileData* getTexFile(u32 id);
    itFigureArchive* findArchive(u32 fileId) const;
    bool isExistFigureArchive(u32 id);
    void unloadFigureTex(u32 id, bool countDown);

};
static_assert(sizeof(itFigureManager) == 0x314, "Class is wrong size!");
