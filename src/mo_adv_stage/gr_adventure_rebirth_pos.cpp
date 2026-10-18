#include <cstdio>
#include <sora_adv_stage/gr_adventure_rebirth_pos.h>

static const char sRebirthNodePrefix[] = "AdvRebirth";
static const char sRebirthNodePosSuffix[] = "P";
static const char sRebirthNodeTopLeftSuffix[] = "TL";
static const char sRebirthNodeBottomRightSuffix[] = "BR";

grAdventureRebirthPos* grAdventureRebirthPos::create(int mdlIndex, const char* taskName) {
    grAdventureRebirthPos* ground = new (Heaps::StageInstance) grAdventureRebirthPos(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureRebirthPos::~grAdventureRebirthPos() { }

void grAdventureRebirthPos::update(float deltaFrame) { }

grAdventureRebirthPosData* grAdventureRebirthPos::getRebirthPos(int index) {
    return m_rebirthPosList.get(index);
}

void grAdventureRebirthPos::setRebirthPosData() {
    Vec3f pos;
    char nodeName[0x10];
    grAdventureRebirthPosData data;
    m_rebirthPosList.clear();
    for (u32 i = 1; i <= 20; i++) {
        sprintf(nodeName, "%s%02d%s", sRebirthNodePrefix, i, sRebirthNodePosSuffix);
        if (!getLocaterNodePosition(&pos, nodeName)) {
            continue;
        }
        data.m_pos[0] = pos.m_x;
        data.m_pos[1] = pos.m_y;
        sprintf(nodeName, "%s%02d%s", sRebirthNodePrefix, i, sRebirthNodeTopLeftSuffix);
        if (!getLocaterNodePosition(&pos, nodeName)) {
            continue;
        }
        data.m_topLeft[0] = pos.m_x;
        data.m_topLeft[1] = pos.m_y;
        sprintf(nodeName, "%s%02d%s", sRebirthNodePrefix, i, sRebirthNodeBottomRightSuffix);
        if (!getLocaterNodePosition(&pos, nodeName)) {
            continue;
        }
        data.m_bottomRight[0] = pos.m_x;
        data.m_bottomRight[1] = pos.m_y;
        m_rebirthPosList.add(data);
    }
}
