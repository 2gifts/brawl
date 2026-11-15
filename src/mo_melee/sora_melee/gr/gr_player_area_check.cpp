#include <ft/ft_manager.h>
#include <gf/gf_memory_util.h>
#include <gr/gr_norfair_player_area_check.h>

// An area gimmick that only records which players stood in it during the frame.

grPlayerAreaCheck* grPlayerAreaCheck::create(int mdlIndex, const char* taskName) {
    grPlayerAreaCheck* ground = new (Heaps::StageInstance) grPlayerAreaCheck(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    ground->setupMelee();
    return ground;
}

grPlayerAreaCheck::~grPlayerAreaCheck() { }

void grPlayerAreaCheck::update(float deltaFrame) {
    m_playerChecks[0] = false;
    m_playerChecks[1] = false;
    m_playerChecks[2] = false;
    m_playerChecks[3] = false;
    grGimmick::update(deltaFrame);
    updateCallback(0);
}

void grPlayerAreaCheck::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmickAreaCommon::startup(archive, unk1, layerType);
}

void grPlayerAreaCheck::setArea(int posX, int posY, int rangeX, int rangeY) {
    gfMemFill(&_playerAreaCheckData, 0, sizeof(grPlayerAreaCheckData));
    _playerAreaCheckData.m_areaData.m_offsetPos.m_x = posX;
    _playerAreaCheckData.m_areaData.m_offsetPos.m_y = posY;
    _playerAreaCheckData.m_areaData.m_range.m_x = rangeX;
    _playerAreaCheckData.m_areaData.m_range.m_y = rangeY;
    m_playerAreaCheckData = &_playerAreaCheckData;
}

void grPlayerAreaCheck::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    ftManager* manager = g_ftManager;
    int entryId = manager->getEntryIdFromTaskId(*taskId, NULL);
    if (entryId != -1) {
        int playerNo = manager->getPlayerNo(entryId);
        if (playerNo < 4) {
            m_playerChecks[playerNo] = true;
        }
    }
}
