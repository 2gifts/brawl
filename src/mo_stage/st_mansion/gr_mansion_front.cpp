#include <ec/ec_mgr.h>
#include <ft/ft_manager.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <st_mansion/gr_mansion_front.h>
#include <st_mansion/gr_mansion_util.h>

grMansionFront* grMansionFront::create(int modelIndex, const char* nodeName, const char* taskName) {
    grMansionFront* ground = new (Heaps::StageInstance) grMansionFront(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(modelIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grMansionFront::~grMansionFront() { }

void grMansionFront::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateArea(deltaFrame);
        updateDestroy(deltaFrame);
    }
}

// While a fighter is inside the front (the box between the two locators) the wall stays open for five more frames.
void grMansionFront::updateArea(float deltaFrame) {
    int ids[9];
    memset(ids, 0, 0x24);
    float halfHeight = (unk158[0].m_y - unk158[1].m_y) * 0.5f;
    float halfWidth = (unk158[1].m_x - unk158[0].m_x) * 0.5f;
    float centerY = unk158[1].m_y + halfHeight;
    float centerX = 0.0f;
    Rect2D area;
    area.m_left = centerX - halfWidth;
    area.m_right = centerX + halfWidth;
    area.m_up = centerY + halfHeight;
    area.m_down = centerY - halfHeight;
    int num = enumIncludeEntryId__9ftManagerCFi(g_ftManager, &area, ids, NULL, 1);
    if (num > 0) {
        unk164 = 5.0f;
    }
}

// The wall opens (the animation plays backward to its start) when a fighter is near and closes when nobody is; an area that
// came back shows the effect of the repair.
void grMansionFront::updateDestroy(float deltaFrame) {
    unk154 -= deltaFrame;
    if (unk154 < 0.0f) {
        unk154 = 0.0f;
    }
    switch (unk150) {
    case 0:
        setMotion(0, false, true, &unk170);
        setMotionFrame(unk170, 0);
        unk150 = 2;
        unk16C = unk170;
        // fall through
    case 2: {
        u8* areas = unk15C;
        if (areas[0] == 2 || areas[1] == 2 || areas[2] == 2 || areas[3] == 2) {
            if (m_motionRatio < 0.0f) {
                m_motionRatio *= -1.0f;
            }
            setMotionLoop(false, 0);
            setMotionFrame(unk16C, 0);
            unk150 = 4;
            break;
        }
        if (unk164 == 0.0f) {
            if (unk168 == 0) {
                unk169 = 0;
            }
            unk168 = 1;
            switch (unk169) {
            case 0:
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                setMotionLoop(true, 0);
                if (getMotionFrame(0) > unk170) {
                    setMotionFrame(unk170 - 1.0f, 0);
                }
                unk16C = getMotionFrame(0);
                unk169++;
                // fall through
            case 1:
                if (getMotionFrame(0) > unk16C) {
                    setMotionFrame(0.0f, 0);
                    setMotionLoop(false, 0);
                    unk16C = 0.0f;
                    unk169++;
                } else {
                    unk16C = getMotionFrame(0);
                }
                break;
            }
        } else {
            if (unk168 == 1) {
                unk169 = 0;
            }
            unk168 = 0;
            switch (unk169) {
            case 0:
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                setMotionLoop(false, 0);
                setMotionFrame(unk16C, 0);
                unk169++;
                // fall through
            case 1:
                if (getMotionFrame(0) >= unk170) {
                    unk16C = unk170;
                    unk169++;
                }
                break;
            }
        }
        unk164 -= deltaFrame;
        if (unk164 < 0.0f) {
            unk164 = 0.0f;
        }
        break;
    }
    case 4: {
        u8* areas = unk15C;
        if (areas[0] != 4 && areas[1] != 4 && areas[2] != 4 && areas[3] != 4) {
            break;
        }
        u32 effect = g_ecMgr->setEffect(ef_ptc_stg_mansion_revival);
        g_ecMgr->setParent(effect, m_sceneModels[0], "StgManshon00Front", true);
        unk174.playSE(snd_se_stage_Mansion_07, 0, 0, -1);
        Vec3f soundPos(0.0f, 0.0f, 0.0f);
        unk174.setPos(&soundPos);
        unk150 = 5;
        break;
    }
    case 5:
        if (*unk15C == 0) {
            unk150 = 0;
        }
        break;
    case 1:
    case 3:
        break;
    }
}

void grMansionFront::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    MANSION_SET_MOTION_VIS(unk16A, animId, loop, force, frameCount)
}
