#include <gm/gm_global.h>
#include <st/loader/st_loader_local_adventure.h>
#include <st/stage.h>
#include <types.h>

static stLoaderSandBagMarker sMarkerA(0xFF, 0);
static stLoaderSandBagMarker sMarkerB(0xFF, 1);

stLoaderLocalAdventure::stLoaderLocalAdventure() : stLoaderSandBag(4), unk6C(-1), m_advData(g_GameGlobal->m_advData) { }

stLoaderLocalAdventure::~stLoaderLocalAdventure() { }

void stLoaderLocalAdventure::processBegin() {
    if ((u8)_0x45[1] != 0) {
        Stage* stage = g_Stage;
        switch (m_state) {
            case 0:
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else if (stage != NULL) {
                    m_state = 1;
                }
                break;
            case 1:
                if ((u8)_0x45[2] == 1) {
                    m_state = 5;
                } else if (stage != NULL) {
                    unk6C = *reinterpret_cast<int*>(reinterpret_cast<u8*>(m_advData) + 0x620);
                    stage->startLoadLocalData();
                    m_state = 2;
                }
                break;
            case 2:
                if (stage->isLoadLocalData()) {
                    m_state = 3;
                }
                break;
            case 3:
                if (isRemove()) {
                    m_state = 5;
                }
                break;
            case 5:
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else {
                    m_state = 0;
                }
                break;
        }
    }
}

void stLoaderLocalAdventure::entryEntity() {
    Stage* stage = g_Stage;
    stLoader::entryEntity();
    stage->entryLocalData();
}

void stLoaderLocalAdventure::removeEntity() {
    Stage* stage = g_Stage;
    if (isLoad()) {
        stLoader::removeEntity();
        stage->removeLocalData();
    }
}

bool stLoaderLocalAdventure::isEntry() {
    return true;
}

void stLoaderLocalAdventure::lockEntity() { }

void stLoaderLocalAdventure::unlockEntity() { }

bool stLoaderLocalAdventure::isRemove() {
    if ((u8)_0x45[2] == 1) {
        return true;
    }
    return g_Stage == NULL;
}
