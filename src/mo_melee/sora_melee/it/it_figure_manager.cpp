#include <it/it_figure_manager.h>
#include <it/it_manager.h>
#include <stdio.h>
#include <types.h>

// HYPOTHESIS: the item manager's game mode word (3 in All-Star, 2 in the mode that limits the trophy range).
static inline int itFigureManagerGetMode(itManager* manager) {
    return *reinterpret_cast<int*>(reinterpret_cast<u8*>(manager) + 0x50);
}

itFigureManager::itFigureManager(void* figListData, u32 figListSize) {
    bool loadFile = figListData == NULL;
    m_figList = new (Heaps::System) tyFigListDataManager(loadFile);
    if (figListData != NULL) {
        m_figList->setData(figListData, figListSize);
    }
    m_preloadMax = m_archives.capacity();
    if (m_preloadMax > m_archives.capacity()) {
        m_preloadMax = m_archives.capacity();
    } else if (m_preloadMax < 0) {
        m_preloadMax = 0;
    }
}

itFigureManager::~itFigureManager() {
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive*& archive = m_archives.at(i);
        if (archive != NULL) {
            if (archive->isRemovable()) {
                delete archive;
                m_archives.erase(i);
                i--;
            } else {
                archive->unk8 = true;
            }
        }
    }
    delete m_figList;
    m_figList = NULL;
}

bool itFigureManager::isValidFigureId(u32 id) {
    if (id == 5000 || (id > 5001 && id < 5008)) {
        return false;
    }
    if (m_figList == NULL) {
        return false;
    }
    return m_figList->getFigDataId(id) != NULL;
}

// The odd trophy ids 1..91 except a few are the All-Star trophies.
static inline bool itIsAllStarFigureId(u32 id) {
    if (id >= 1 && id <= 0x5B && (id & 1) == 1) {
        switch (id) {
        case 51:
        case 53:
        case 55:
        case 57:
        case 59:
        case 67:
        case 69:
        case 71:
        case 81:
            return false;
        default:
            return true;
        }
    }
    return false;
}

// The archive file of a trophy id: the All-Star / limited trophies share one file (negative ids).
static inline int itFigureFileId(u32 id) {
    switch (itFigureManagerGetMode(itManager::getInstance())) {
    case 3:
        if (itIsAllStarFigureId(id)) {
            return -16;
        }
        break;
    case 2: {
        bool limited;
        if (id >= 0x147 && id <= 0x187) {
            if (id == 0x155 || id == 0x183) {
                limited = false;
            } else {
                limited = true;
            }
        } else {
            limited = false;
        }
        if (limited) {
            return -15;
        }
        break;
    }
    }
    return id;
}

// MATCH-ONLY: the same lookup as itFigureFileId, written out so the callers get the inlined form and this copy the
// early returns.
int itFigureManager::getFigureFileId(u32 id) {
    switch (itFigureManagerGetMode(itManager::getInstance())) {
    case 3:
        if (itIsAllStarFigureId(id)) {
            return -16;
        }
        break;
    case 2: {
        bool limited;
        if (id >= 0x147 && id <= 0x187) {
            if (id == 0x155 || id == 0x183) {
                limited = false;
            } else {
                limited = true;
            }
        } else {
            limited = false;
        }
        if (limited) {
            return -15;
        }
        break;
    }
    }
    return id;
}

bool itFigureManager::isAllStarFigureId(u32 id) {
    return itIsAllStarFigureId(id);
}

inline itFigureArchive* itFigureManager::findArchive(u32 fileId) const {
    if (m_figList == NULL) {
        return NULL;
    }
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive* archive = m_archives.at(i);
        if (archive != NULL && fileId == archive->m_kind) {
            return archive;
        }
    }
    return NULL;
}

bool itFigureManager::preloadFigureTex(u32 id, bool addRef) {
    static char s_path[0x100];
    if (id == 5000 || (id > 5001 && id < 5008)) {
        return false;
    }
    int fileId = itFigureFileId(id);
    itFigureArchive* archive = findArchive(fileId);
    if (addRef && archive != NULL) {
        archive->unkC++;
    }
    if (archive != NULL) {
        return true;
    }
    bool full = false;
    int capacity = m_archives.capacity();
    if (m_archives.size() >= capacity || m_archives.size() >= m_preloadMax) {
        full = true;
    }
    if (full) {
        return false;
    }
    switch (fileId) {
    case -16:
        snprintf(s_path, sizeof(s_path), "/item/figure/ItmFigureAllstar.pac");
        break;
    case -15:
        snprintf(s_path, sizeof(s_path), "/item/figure/ItmFigureEnemy.pac");
        break;
    default: {
        if (m_figList == NULL) {
            return false;
        }
        tyFigListData* data = m_figList->getFigDataId(id);
        if (data == NULL) {
            return false;
        }
        snprintf(s_path, sizeof(s_path), "/item/figure/figdisp/FigTex%03d.pac", data->bilTexIdx);
        break;
    }
    }
    itFigureArchive* newArchive = new (Heaps::HeapType(0x24)) itFigureArchive(fileId, s_path);
    if (newArchive == NULL) {
        return false;
    }
    m_archives.push(newArchive);
    newArchive->unkC++;
    return true;
}

void itFigureManager::unloadFigureTex(u32 id, bool countDown) {
    if (id == 5000 || (id > 5001 && id < 5008)) {
        return;
    }
    if (m_figList == NULL) {
        return;
    }
    u32 fileId = itFigureFileId(id);
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive* archive = m_archives.at(i);
        if (archive != NULL && fileId == archive->m_kind) {
            // the count is signed here (the archive header keeps it as a u32)
            if (countDown && (int)archive->unkC > 0) {
                if ((int)--archive->unkC > 0) {
                    return;
                }
            }
            if (archive->unk8) {
                return;
            }
            if (archive->isRemovable()) {
                delete archive;
                m_archives.erase(i);
            } else {
                archive->unk8 = true;
            }
            return;
        }
    }
}

bool itFigureManager::isExistFigureArchive(u32 id) {
    return findArchive(itFigureFileId(id)) != NULL;
}

bool itFigureManager::isCompFigureTexPreloadAll() const {
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive* archive = m_archives.at(i);
        if (archive != NULL && !archive->isComp()) {
            return false;
        }
    }
    return true;
}

bool itFigureManager::isCompFigureTex(u32 id) {
    itFigureArchive* archive = findArchive(itFigureFileId(id));
    if (archive != NULL) {
        return archive->isComp();
    }
    return false;
}

void itFigureManager::removeFigureTexAll() {
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive*& archive = m_archives.at(i);
        if (archive != NULL) {
            if (archive->isRemovable()) {
                delete archive;
                m_archives.erase(i);
                i--;
            } else {
                archive->unk8 = true;
            }
        }
    }
}

bool itFigureManager::onProcessFixPosition() {
    int pending = 0;
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive*& archive = m_archives.at(i);
        if (archive != NULL && archive->unk8) {
            pending++;
            if (archive->isRemovable()) {
                delete archive;
                m_archives.erase(i);
                i--;
            }
        }
    }
    return pending > 0 ? false : true;
}

nw4r::g3d::ResFileData* itFigureManager::getTexFile(u32 id) {
    int fileId = itFigureFileId(id);
    u32 texId = 0;
    if ((u32)fileId == 0xFFFFFFF0 || (u32)fileId == 0xFFFFFFF1) {
        texId = id;
    }
    itFigureArchive* archive = findArchive(fileId);
    if (archive == NULL) {
        return NULL;
    }
    if (!archive->isComp() || archive->unk8) {
        return NULL;
    }
    return archive->getTexFile(texId);
}

const char* itFigureManager::getTexName(u32 id) {
    static char s_texName[0x20];
    m_figList->getBilTexNameFigId(id, s_texName);
    return s_texName;
}

bool itFigureManager::getOffsetAndScale(u32 id, Vec2f* offset, Vec2f* scale) {
    offset->m_y = 0.0f;
    offset->m_x = 0.0f;
    scale->m_y = 0.0f;
    scale->m_x = 0.0f;
    if (!m_figList->isLoadFinish()) {
        return false;
    }
    tyFigListData* data = m_figList->getFigDataId(id);
    if (data == NULL) {
        return false;
    }
    float* values = reinterpret_cast<float*>(data->unk50);
    offset->m_x = values[0];
    offset->m_y = values[1];
    scale->m_x = values[2];
    scale->m_y = values[3];
    return true;
}
