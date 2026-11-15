#include <it/it_figure_manager.h>
#include <it/it_manager.h>
#include <types.h>

// HYPOTHESIS: the item manager's game mode word (3 in All-Star, 2 in the mode that limits the trophy range).
static inline int itFigureManagerGetMode(itManager* manager) {
    return *reinterpret_cast<int*>(reinterpret_cast<u8*>(manager) + 0x50);
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
        if (archive != NULL && fileId == archive->m_fileId) {
            return archive;
        }
    }
    return NULL;
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
                archive->m_isRemove = true;
            }
        }
    }
}

bool itFigureManager::onProcessFixPosition() {
    int pending = 0;
    for (int i = 0; i < m_archives.size(); i++) {
        itFigureArchive*& archive = m_archives.at(i);
        if (archive != NULL && archive->m_isRemove) {
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

bool itFigureManager::isExistFigureArchive(u32 id) {
    return findArchive(itFigureFileId(id)) != NULL;
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
        if (archive != NULL && fileId == archive->m_fileId) {
            if (countDown && archive->m_refCount > 0) {
                if (--archive->m_refCount > 0) {
                    return;
                }
            }
            if (archive->m_isRemove) {
                return;
            }
            if (archive->isRemovable()) {
                delete archive;
                m_archives.erase(i);
            } else {
                archive->m_isRemove = true;
            }
            return;
        }
    }
}

void* itFigureManager::getTexFile(u32 id) {
    int fileId = itFigureFileId(id);
    u32 texId = 0;
    if ((u32)fileId == 0xFFFFFFF0 || (u32)fileId == 0xFFFFFFF1) {
        texId = id;
    }
    itFigureArchive* archive = findArchive(fileId);
    if (archive == NULL) {
        return NULL;
    }
    if (!archive->isComp() || archive->m_isRemove) {
        return NULL;
    }
    return archive->getTexFile(texId);
}
