// MATCH-ONLY: the editor inlines its native paired-single vector multiplication.
#define MT_VEC3F_MUL_INLINE
#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <math.h>
#include <ms/ms_message.h>
#include <mu/mu_stage_edit_ui.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <st/st_data_container.h>
#include <if/if_snap_save.h>

// HYPOTHESIS: the RVL SDK texture-object accessor, which the GX headers do not declare.
extern "C" void* GXGetTexObjData(const _GXTexObj* texture);
extern "C" int fn_800C39D8(void* collection);
extern "C" int fn_6_308C(muCollectionViewer* viewer, int* cursor);
extern "C" void fn_5_B43C(muStageEditUI* ui);
extern gfKeepFrameBuffer* lbl_805A0008;

// HYPOTHESIS: descriptive table names inferred from the grid readers.
// Parameter tables indexed by stage size, then zoom level. Their native storage
// remains in the original module until the complete editor unit is recovered.
extern u32 g_stageEditColumns[3];
extern u32 g_stageEditRows[3];
extern float g_stageEditBaseScale[3];
extern float g_stageEditZoomScale[3][3];
extern u8 g_stageEditMaxZoom[3];

// Palette layout's rectangle test. The native scan skips entry zero and
// visits enabled entries preceding the candidate within the supplied group.
bool muStageEditUI::isPutOn(int count, Vec3f* position, int first) {
    if (position->m_y < 0.0f) return true;
    Vec3f otherPosition;
    Vec3f size;
    size = m_paletteSizes[count + first];
    int entry = count - 1;
    for (int remaining = entry; remaining != 0; --remaining, --entry) {
        if (m_paletteEnabled[entry]) {
            Vec3f otherSize;
            otherPosition = m_palettePositions[first + entry];
            otherSize = m_paletteSizes[first + entry];
            if (position->m_x <= otherPosition.m_x + otherSize.m_x &&
                otherPosition.m_x <= position->m_x + size.m_x &&
                position->m_y >= otherPosition.m_y + otherSize.m_y &&
                otherPosition.m_y >= position->m_y + size.m_y) return true;
        }
    }
    return false;
}

void muStageEditUI::setPalettCursorPos(int scrollIntoView) {
    Vec3f position = m_palettePositions[m_paletteSelection];
    position.m_x += 0.5f * m_paletteSizes[m_paletteSelection].m_x;
    position.m_y -= 0.5f * m_paletteSizes[m_paletteSelection].m_y;
    m_paletteCursor->setTrans(&position);
    if (scrollIntoView) {
        float top = -m_palettePositions[m_paletteSelection].m_y - 0.5f * m_paletteSizes[m_paletteSelection].m_y;
        if (m_paletteCategory == 3 && -top >= -3.0f) top = 0.0f;
        m_paletteCursor->getPos(&position, "underN");
        float bottom = (-m_palettePositions[m_paletteSelection].m_y + 0.5f * m_paletteSizes[m_paletteSelection].m_y) - position.m_y * 2.0f;
        if (bottom > m_paletteScrollLimit) bottom = m_paletteScrollLimit;
        if (top < m_paletteTranslation.m_y) {
            m_paletteScrollTarget = top;
            m_paletteScrolling = true;
        } else if (bottom > m_paletteTranslation.m_y + m_paletteViewportHeight) {
            m_paletteScrolling = true;
            m_paletteScrollTarget = bottom - m_paletteViewportHeight;
        }
    }
}

// Returning from the grid selects the closest enabled palette entry to the
// previous anchor, rather than resetting selection to the first part.
void muStageEditUI::setScroll2Palette() {
    m_paletteGroup->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_paletteCursor->m_scnMdl), "pos00");
    m_paletteCursorVisible->setFrameVisible(0.0f);
    float bestDistance = 3.402823466e+38f;
    u32 closest = 0;
    Vec3fSub(&m_paletteAnchor, &m_paletteAnchor, &m_paletteTranslation);
    for (u32 i = 0; i < m_paletteCount; ++i) {
        if (m_paletteEnabled[i]) {
            Vec3f delta;
            Vec3fSub(&delta, &m_palettePositions[i], &m_paletteAnchor);
            float square = delta.m_z * delta.m_z + (delta.m_x * delta.m_x + delta.m_y * delta.m_y);
            float distance;
            if ((float)__fabs(square) <= 1.17549435e-38f) distance = 0.0f;
            else distance = square * rsqrtf(square);
            if (bestDistance > distance) {
                closest = i;
                bestDistance = distance;
            }
        }
    }
    m_paletteSelection = closest;
    setPalettCursorPos(1);
    setGridPartsCursor(0);
}

// Keep the selected cell/part in the 40 x 32 viewport when changing zoom,
// then clamp the grid edges so panning does not expose empty space.
void muStageEditUI::setScaleGrid(int animate) {
    float scale = g_stageEditBaseScale[m_stageSize] * g_stageEditZoomScale[m_stageSize][m_zoomLevel];
    int halfRows = g_stageEditRows[m_stageSize] >> 1;
    int halfColumns = g_stageEditColumns[m_stageSize] >> 1;
    if (m_state != 6 && m_state != 7 && m_state != 11 && m_state != 12 &&
        m_state != 4 && m_state != 19 && m_state != 20) {
        m_gridGroup->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_gridPart->m_scnMdl));
    }
    float width, height, left, top;
    if (m_state == 6 || m_state == 7 || m_state == 1 || m_state == 2 ||
        m_state == 11 || m_state == 12 || m_partDragging) {
        width = m_partWidth * 4.0f * scale + 3.0f;
        top = (m_partRow + (float)(int)(m_partHeight * 0.5f)) * 4.0f + 1.5f;
        left = (m_partColumn - (float)(int)(m_partWidth * 0.5f)) * 4.0f - 1.5f;
        height = m_partHeight * 4.0f * scale + 3.0f;
    } else {
        width = scale * 4.0f;
        u32 row = m_gridCell / g_stageEditColumns[m_stageSize];
        left = ((float)(m_gridCell % g_stageEditColumns[m_stageSize]) - (float)halfColumns) * 4.0f;
        top = ((float)halfRows - (float)row) * 4.0f;
        height = width;
    }
    Vec3f selectedPosition(left, top, 0.0f);
    selectedPosition = selectedPosition * scale + m_gridTranslation;
    left = selectedPosition.m_x;
    top = selectedPosition.m_y;
    if (left >= -20.0f) {
        if (left + width > 20.0f) m_gridTranslation.m_x -= (left + width) - 20.0f;
    } else m_gridTranslation.m_x += -20.0f - left;
    if (top <= 16.0f) {
        if (top - height < -16.0f) m_gridTranslation.m_y += -16.0f - (top - height);
    } else m_gridTranslation.m_y -= top - 16.0f;

    float bottomEdge = (float)-halfRows * 4.0f;
    Vec3f upperLeft((float)-halfColumns * 4.0f, (float)halfRows * 4.0f, 0.0f);
    upperLeft = upperLeft * scale + m_gridTranslation;
    if (m_stageSize == 1) bottomEdge -= 4.0f;
    Vec3f lowerRight((float)halfColumns * 4.0f, bottomEdge, 0.0f);
    lowerRight = lowerRight * scale + m_gridTranslation;
    float gridLeft = upperLeft.m_x;
    float gridTop = upperLeft.m_y;
    float gridRight = lowerRight.m_x;
    float gridBottom = lowerRight.m_y;
    if (gridLeft <= -20.0f) {
        if (gridRight < 20.0f && gridLeft < -20.0f) m_gridTranslation.m_x += 20.0f - gridRight;
    } else if (gridRight > 20.0f) m_gridTranslation.m_x -= gridLeft + 20.0f;
    if (gridTop >= 16.0f) {
        if (gridBottom > -16.0f && gridTop > 16.0f) m_gridTranslation.m_y -= gridBottom + 16.0f;
    } else if (gridBottom < -16.0f) m_gridTranslation.m_y += 16.0f - gridTop;

    if (animate == 0) calcMoveGrid(1);
    else { m_gridScaling = true; m_gridPanning = true; }
    int state = (m_state == 19 || m_state == 20) ? m_previousState : m_state;
    bool active = (u32)(state - 10) <= 2;
    if (m_zoomLevel == 0) {
        if (active && m_hoverButton == 3) m_zoomOut->changeAnimN(2, 0x11, true, false);
        else m_zoomOut->changeAnimN(3, 0x11, false, false);
    } else if (active && m_hoverButton == 3) m_zoomOut->changeAnimN(1, 0x11, true, false);
    else m_zoomOut->changeAnimN(0, 0x11, false, false);
    if (m_zoomLevel == g_stageEditMaxZoom[m_stageSize]) {
        if (active && m_hoverButton == 4) m_zoomIn->changeAnimN(2, 0x11, true, false);
        else m_zoomIn->changeAnimN(3, 0x11, false, false);
    } else if (active && m_hoverButton == 4) m_zoomIn->changeAnimN(1, 0x11, true, false);
    else m_zoomIn->changeAnimN(0, 0x11, false, false);
    for (u32 i = 0; i < 4; ++i) {
        if (m_scrollState[i] != 3) setGridScroll(i, 0);
    }
}

void muStageEditUI::calcMoveGrid(int immediate) {
    float target = g_stageEditBaseScale[m_stageSize] * g_stageEditZoomScale[m_stageSize][m_zoomLevel];
    const char* names[3] = {"syou", "chu", "dai"};
    Vec3f scale;
    Vec3f translation;
    Vec3f cursor;
    if (immediate) {
        if ((u32)m_state < 4) setGridPartsCursor(1);
        scale.m_x = target; scale.m_y = target; scale.m_z = 1.0f;
        // The native API writes the node rotation, even though the feedback
        // comes from getAnimScale. Preserve this asset-dependent behavior.
        m_grid->setRotate(names[m_stageSize], &scale);
        m_grid->setTrans(&m_gridTranslation);
        m_gridScaling = false;
        m_gridPanning = false;
    } else {
        bool panReady = false;
        bool scaleReady = false;
        if (!m_gridScaling) scaleReady = true;
        else {
            m_grid->getAnimScale(&scale, names[m_stageSize]);
            float step = (target - scale.m_x) * 0.125f;
            scale.m_x += step;
            if ((float)__fabs(target - scale.m_x) < 0.001f) {
                scale.m_x = target; scale.m_y = target;
                scaleReady = true;
                m_gridScaling = false;
            } else {
                scale.m_y += step;
                if ((float)__fabs(target - (scale.m_x + step)) < 0.1f) scaleReady = true;
            }
            m_grid->setRotate(names[m_stageSize], &scale);
        }
        if (!m_gridPanning) panReady = true;
        else {
            translation.m_z = m_grid->m_trans.m_z;
            float x = m_gridTranslation.m_x;
            translation.m_x = m_grid->m_trans.m_x + (x - m_grid->m_trans.m_x) * 0.125f;
            bool xReady = (float)__fabs(x - translation.m_x) < 0.05f;
            if (xReady) translation.m_x = x;
            float y = m_gridTranslation.m_y;
            translation.m_y = m_grid->m_trans.m_y + (y - m_grid->m_trans.m_y) * 0.125f;
            if ((float)__fabs(y - translation.m_y) < 0.05f) {
                translation.m_y = y;
                if (xReady) { panReady = true; m_gridPanning = false; }
            }
            m_grid->setTrans(&translation);
        }
        if (panReady && scaleReady && (u32)(m_state - 1) < 3) setGridPartsCursor(1);
        u32 columns = g_stageEditColumns[m_stageSize];
        u32 rows = g_stageEditRows[m_stageSize];
        u32 cell = m_gridCell;
        m_grid->getAnimScale(&scale, names[m_stageSize]);
        translation = m_grid->m_trans;
        cursor.m_x = (float)(int)(cell % columns - (columns >> 1) + 1) * 4.0f;
        cursor.m_y = (float)(int)((rows >> 1) - cell / columns - 1) * 4.0f;
        cursor.m_z = 0.0f;
        cursor = cursor * scale.m_x + translation;
        m_gridCursor->setTrans(&cursor);
    }
}

// Arrow states: 0 unavailable, 1 can scroll, 2 selected cell requests scrolling;
// state 3 is reserved by the surrounding input handler.
void muStageEditUI::setGridScroll(int direction, int clearOpposite) {
    u32 columns = g_stageEditColumns[m_stageSize];
    u32 rows = g_stageEditRows[m_stageSize];
    u32 row = m_gridCell / columns;
    u32 halfColumns = columns >> 1;
    u32 halfRows = rows >> 1;
    int x = m_gridCell % columns - halfColumns;
    int y = halfRows - row;
    float scale = g_stageEditBaseScale[m_stageSize] * g_stageEditZoomScale[m_stageSize][m_zoomLevel];
    float cellSize = scale * 4.0f;
    float width = (float)(int)halfColumns * cellSize;
    float xLimit = 20.0f - cellSize * 0.75f;
    float yLimit = 16.0f - cellSize * 0.75f;
    float cellX = (float)x * 4.0f * scale + m_gridTranslation.m_x;
    float cellY = (float)y * 4.0f * scale + m_gridTranslation.m_y;
    float height = (float)(int)halfRows * cellSize;
    int state = 0;
    switch (direction) {
    case 2:
        if (width + m_gridTranslation.m_x <= 20.0f) m_scrollArrows[2]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[2] != 1) m_scrollArrows[2]->changeAnimN(1, 0x13, true, false);
            state = (x == halfColumns - 1 || cellX + cellSize > xLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[1] = 0;
        break;
    case 0:
        if (height + m_gridTranslation.m_y <= 16.0f) m_scrollArrows[0]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[0] != 1) m_scrollArrows[0]->changeAnimN(1, 0x13, true, false);
            state = (y == halfRows || cellY > yLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[3] = 0;
        break;
    case 1:
        if (-width + m_gridTranslation.m_x >= -20.0f) m_scrollArrows[1]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[1] != 1) m_scrollArrows[1]->changeAnimN(1, 0x13, true, false);
            state = (x == -halfColumns || cellX < -xLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[2] = 0;
        break;
    case 3: {
        float bottom = -height + m_gridTranslation.m_y;
        if ((rows & 1) == 1) bottom -= cellSize;
        if (bottom >= -16.0f) m_scrollArrows[3]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[3] != 1) m_scrollArrows[3]->changeAnimN(1, 0x13, true, false);
            state = (y == 1 - halfRows || cellY - cellSize < -yLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[0] = 0;
        break;
    }
    }
    m_scrollState[direction] = state;
}

s16 muStageEditUI::getSettingDataNum() {
    return m_settingDataNum;
}

void muStageEditUI::setStageThumbnail(_GXTexObj* texture) {
    m_thumbnail->changeMaterialTex("thumbnail_mat", texture);
    m_thumbnailData = GXGetTexObjData(texture);
}

void muStageEditUI::ctrlSetup(gfPadStatus* output, int repeat) {
    u32 buttons = 0;
    for (int i = 0; i < 8; ++i) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error < 0) continue;
        switch (status.m_controllerType) {
        case gfPadType::WIIMOTE:
            buttons |= status.m_buttonsPressedThisFrame2.bits | unk74;
            if (status.m_buttonsPressedThisFrame.bits & 0x200000) {
                output->m_buttonsPressedThisFrame.bits |= 0x400;
            } else if (status.m_buttonsPressedThisFrame.bits & 0x400000) {
                output->m_buttonsPressedThisFrame.bits |= 0x10;
                output->m_buttonsPressedThisFrame.bits &= ~0x40;
            } else if (status.m_buttonsCurrentFrame2.bits & 0x80000) {
                buttons = 0;
                if (status.m_buttonsPressedThisFrame.bits & 4) {
                    output->m_buttonsPressedThisFrame.bits |= 0x20;
                } else if (status.m_buttonsPressedThisFrame.bits & 8) {
                    output->m_buttonsPressedThisFrame.bits |= 0x40;
                }
            }
            break;
        case gfPadType::NUNCHUK:
            buttons |= unk74;
            if (repeat) buttons |= status.m_buttonsPressedThisFrame2.bits;
            if (status.m_buttonsPressedThisFrame.bits & 0x80000) {
                output->m_buttonsPressedThisFrame.bits |= 0x400;
            } else if (status.m_buttonsPressedThisFrame.bits & 0x20000) {
                output->m_buttonsPressedThisFrame.bits |= 0x800;
            } else if (status.m_buttonsPressedThisFrame.bits & 4) {
                output->m_buttonsPressedThisFrame.bits |= 0x20;
            } else if (status.m_buttonsPressedThisFrame.bits & 8) {
                output->m_buttonsPressedThisFrame.bits |= 0x40;
            } else if (status.m_buttonsPressedThisFrame2.bits & 0x40000) {
                unk68 = 1;
                buttons = 0;
                output->m_subStickY = output->m_stickY;
                output->m_buttonsPressedThisFrame.bits &= ~0x400;
            }
            break;
        default:
            buttons |= status.m_buttonsPressedThisFrame2.bits | unk74;
            break;
        }
    }
    output->m_buttonsPressedThisFrame2.bits = buttons;
}

extern const char lbl_5_data_600[];

muStageEditDataManager::muStageEditDataManager(int size, int load)
    : unk5D0(size), m_loaded(false), unk608(true), unk60C(NULL) {
    if (load && !unk604.m_request) {
        m_loaded = false;
        unk608 = false;
        unk604.readRequest(lbl_5_data_600, static_cast<HeapType>(0x2B), 0, 0);
    }
}

muStageEditDataManager::~muStageEditDataManager() {
    if (!unk608) {
        gfHeapManager::free(unk604.getBuffer());
        unk604.release();
    }
    if (unk60C) {
        delete unk60C;
        unk60C = NULL;
    }
}

muStageEditDataManager::Data* muStageEditDataManager::getDataId(u32 id) {
    if (!m_loaded) {
        return NULL;
    }
    for (u32 i = 0; i < m_numData; i++) {
        if (m_data[i].m_id == id) {
            return &m_data[i];
        }
    }
    return NULL;
}

muStageEditSelectTask::~muStageEditSelectTask() { }

void muStageEditSelectTask::playBGM() {
    if ((u32)g_sndSystem->getBGMId() != 0x270F) {
        g_sndSystem->playBGM(0x270F, 0, true);
    }
}

void muStageEditSelectTask::addObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Insert(root->sceneItemsCount, static_cast<nw4r::g3d::ScnObj*>(obj));
}

void muStageEditSelectTask::removeObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Remove(static_cast<nw4r::g3d::ScnObj*>(obj));
}

void* muStageEditSelectTask::getScnRoot() {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    if (holder->m_hasScnRoot) {
        return holder->m_scnRoot;
    }
    return NULL;
}

void muStageEditSelectTask::controlExit() {
    if (fn_800C39D8(m_work->m_collection)) {
        int exitMode = getWork()->m_exitMode;
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = exitMode;
        manager->processStep = 2;
    }
}

void muStageEditSelectTask::controlLoadFinish() {
    m_work->m_exitRequest = 1;
}

void muStageEditSelectTask::onLoadFailed() {
    removeObjCore(m_work->m_objs[30]->m_sceneModel);
}

int muStageEditSelectTask::getNumPic(int media) {
    return m_work->m_media[media].m_numPic;
}

muCollectionFileName muStageEditSelectTask::getFileHandle(int index) {
    return muCollectionFileName(m_work->m_media[m_work->m_curMedia].m_names, index);
}

void muStageEditSelectTask::controlPopUpCheckStart() {
    muStageEditCollection* collection = static_cast<muStageEditCollection*>(m_work->m_collection);
    if (collection->unk350 == 0) {
        reinterpret_cast<muCollection*>(collection)->checkLimit(getWork()->m_curMedia);
        m_work->m_state = 0x1A;
    }
}

void muStageEditSelectTask::controlPopUpChecking() {
    muStageEditCollection* collection = static_cast<muStageEditCollection*>(m_work->m_collection);
    if (collection->unk350 == 0) {
        if (collection->unk354 != 0) {
            getWork()->m_exitMode = -1;
            m_work->m_state = getWork()->m_unk1D48;
        } else if (showNoFreeBlock() == 0) {
            m_work->m_state = getWork()->m_unk1D44;
        }
    }
}

void muStageEditSelectTask::controlNewDataExit() {
    if (m_work->m_objs[26]->isNodeAnimFinished()) {
        removeObjCore(m_work->m_objs[37]);
        if (getWork()->m_exitMode < 0) {
            m_work->m_state = 0;
        } else {
            m_work->m_exitRequest = 1;
        }
    }
}

void muStageEditSelectTask::cancelAndExit() {
    getWork()->m_exitMode = 3;
    m_work->m_exitRequest = 1;
}

void muStageEditSelectTask::erasePreviewCore() {
    removeObjCore(m_work->m_objs[36]);
}

bool muStageEditSelectTask::preLoad() {
    return true;
}

void muStageEditSelectTask::controlNewDataChecking() { }

void muStageEditSelectTask::controlBrowsingLoading() { }

void muStageEditSelectTask::controlBrowsing() { }

int muStageEditSelectTask::dispThumbnail(MuObject* object, void* summary) {
    return 0;
}

bool muStageEditDataManager::isLoadFinish() {
    if (!m_loaded) {
        if (!unk604.isReady()) {
            return false;
        }
        setData(static_cast<stDataContainerData*>(unk604.getBuffer()));
        m_loaded = true;
        return true;
    }
    return true;
}

sndBgmTitleData* muStageEditSelectTask::NewData::getBgmData() const {
    return lbl_805A01D8->getSeriesTitleData(unk8, unkC);
}

void muStageEditSelectTask::getMineLabel(muCollectionMsgBuf* buf) {
    void* data = m_work->m_msgFile[0].m_handle.getBuffer();
    u32 size;
    char* text;
    Message::getPrintIndexData(data, 6, &text, &size);
    u32 length = size;
    int used = buf->m_size;
    memcpy(buf->m_data + used, text, length);
    buf->m_size = used + length;
}

u32 stDataContainer::createResId(const char* name) {
    return *reinterpret_cast<const u32*>(name);
}

// HYPOTHESIS: clears the first word of one file handle. The editor constructor
// builds an array of handles through this.
extern "C" void fn_5_918(int* word) {
    *word = 0;
}

void muStageEditSelectTask::getNewLabel(muCollectionMsgBuf* buf) {
    void* data = m_work->m_msgFile[0].m_handle.getBuffer();
    u32 size;
    char* text;
    Message::getPrintIndexData(data, 7, &text, &size);
    u32 length = size;
    int used = buf->m_size;
    memcpy(buf->m_data + used, text, length);
    buf->m_size = used + length;
}

void muStageEditSelectTask::onPreCheckFinished() {
    // HYPOTHESIS: the stage-edit record keeps the collection cursor at 0x24A0.
    gmStageEditData* data = g_GameGlobal->m_stageEditData;
    int count = fn_6_308C(this, &data->m_cursor);
    if (count >= 0) {
        m_work->m_curMedia = data->m_cursor;
        m_work->m_numFiles = m_work->m_extraPanel + count;
    }
    if ((u32)g_sndSystem->getBGMId() != 0x270F) {
        g_sndSystem->playBGM(0x270F, 0, true);
    }
}

void muStageEditSelectTask::onDecided(int pad) {
    if (m_work->m_numFiles > 0) {
        getWork()->m_unk1D4C = 0;
        m_work->m_state = 0x17;
    } else {
        getWork()->m_pad = pad;
        getWork()->m_exitMode = -1;
        getWork()->m_unk1D44 = 0x1B;
        getWork()->m_unk1D48 = 0;
        m_work->m_state = 0x19;
    }
    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
}

void muStageEditUI::setSaveProcess() {
    // HYPOTHESIS: word 0x44 is a sub-state, and the scene animation lives in the resource file at 0x7D69C.
    int state = 13;
    m_state = state;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x44) = 0;
    nw4r::g3d::ResFile* sceneRes = reinterpret_cast<nw4r::g3d::ResFile*>(reinterpret_cast<char*>(this) + 0x7D69C);
    gfSceneRoot* root = g_gfSceneRoot;
    nw4r::g3d::ResAnmScn anm;
    anm = sceneRes->GetResAnmScn(0);
    anm.Bind(anm);
    root->setUseCameraAnim(true);
    root->setResAnmScn(&anm, 0x2A);
    root->setInfoCamResAnm(&anm);
    fn_5_B43C(this);
    lbl_805A0008->endKeepScreen();
}

#include <nw4r/g3d/g3d_scnmdl.h>
#include <mu/mu_object.h>
#include <string.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_file_io_handle.h>
#include <ut/ut_list.h>
#include <stdio.h>
#include <mt/mt_prng.h>
#include <mu/mu_msg.h>
// TODO: clean up, give variables real names
namespace jj_924 {
extern "C" {
extern void* g_GameGlobal;
extern void* g_sndSystem;
extern char g_stageEditColumns[];
void* getGlobalRecordMenuDatap__10GameGlobalFv(void*);
void readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(void*, const char*, int, int, int);
void fn_800151FC(void*, const char*, int, int, int, int);
int loadSoundGroup__9sndSystemF10SndGroupIDib(void*, int, int, int);

void fn_5_924(char* self) {
    char* d = g_stageEditColumns;
    char* s = self;
    switch (*(int*)(s + 0x104)) {
    case 0:
        if ((*(unsigned char*)((char*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal) + 0x28) >> 7) & 1)
            readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC476C, d + 0x5c, 0x2b, 0, 0);
        else
            readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC476C, d + 0x90, 0x2b, 0, 0);
        break;
    case 1:
        if ((*(unsigned char*)((char*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal) + 0x28) >> 7) & 1)
            readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC476C, d + 0xc0, 0x2b, 0, 0);
        else
            readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC476C, d + 0xf4, 0x2b, 0, 0);
        break;
    case 2:
        if ((*(unsigned char*)((char*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal) + 0x28) >> 7) & 1)
            readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC476C, d + 0x124, 0x2b, 0, 0);
        else
            readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC476C, d + 0x158, 0x2b, 0, 0);
        break;
    }
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC4770, d + 0x188, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC477C, d + 0x1b0, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC4774, d + 0x1d0, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC4778, d + 0x1e8, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC4780, d + 0x204, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(s + 0xC4784, d + 0x21c, 0x2b, 0, 0);
    fn_800151FC(*(void**)(s + 0xC4788), d + 0x238, 0x2b, 0, 0, 0xff);
    *(int*)(s + 0x10c) = loadSoundGroup__9sndSystemF10SndGroupIDib(g_sndSystem, 0x69, 1, 1);
}
}
}

// TODO: clean up, give variables real names
namespace jj_4080 {

extern "C" {
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
f32 fn_5_42FC(void*, u32, s32, f32);
void setPalettCursorPos__13muStageEditUIFi(void*, s32);
void setTrans__8MuObjectFP5Vec3f(void*, f32*);
void getPos__8MuObjectFP5Vec3fPCc(void*, f32*, char*);
void setPos__8MuObjectFP5Vec3fPCc(void*, f32*, char*, f32, f32);
extern const f32 lbl_5_rodata_0[];
extern char g_stageEditColumns[];

inline f32 multiply(f32 a, f32 b) { return a * b; }

inline s32 getCount(char* p, u32 index) {
    if (*(u8*)(p + 0x600) != 0)
        return *(s32*)(p + index * 4 + 0x1c);
    return 0;
}

void fn_5_4080(void* arg0) {
    char* d = g_stageEditColumns;
    const f32* c = lbl_5_rodata_0;
    char* self = (char*)arg0;
    s32 r28;
    char* far;
    char* hi;
    char* p5 = *(char**)(self + 0xC4758);
    s32 r0;
    f32 f31;
    f32 f30;
    f32 f29;
    f32 f28 = c[3];
    f32 buf[3];
    f32 v[3];
    f32 scale[3];

    r0 = getCount(p5, *(s32*)(self + 0x7D164));
    hi = self + 0x80000;
    *(s32*)(hi - 0x2e8c) = r0;

    if (*(s32*)(hi - 0x2e9c) == 3) {
        f29 = c[6];
        f30 = c[3];
        f31 = c[7];
        char* r25 = self;
        s32 r24 = 0;
        u32 r23 = 0;
        r28 = 0;
        far = self + 0xc0000;
        do {
            Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
                *(void**)(hi + 0x7D684 - 0x80000),
                *(void**)(*(char**)(r25 + 0x7D640) + 0xC),
                d + 0x258);
            buf[0] = f29;
            buf[1] = f28;
            buf[2] = f30;
            setTrans__8MuObjectFP5Vec3f(*(void**)(r25 + 0x7D640), buf);
            f28 -= f31;
            f28 = fn_5_42FC(self, r23, r24, f28) - f31;
            p5 = *(char**)(far + 0x4758);
            if (*(u8*)(p5 + 0x600) != 0) {
                r0 = *(s32*)(p5 + r28 + 0x1c);
            } else {
                r0 = 0;
            }
            r23 += 1;
            r24 += r0;
            r25 += 4;
            r28 += 4;
        } while (r23 < 3);
    } else {
        f28 = fn_5_42FC(self, *(s32*)(self + 0x7D164), 0, c[3]);
    }

    setPalettCursorPos__13muStageEditUIFi(self, 0);
    *(f32*)(self + 0x7D51C) = -f28;
    getPos__8MuObjectFP5Vec3fPCc(*(void**)(self + 0x7D5AC), v, d + 0x498);
    f32 t = *(f32*)(self + 0x7D51C) - v[1];
    *(f32*)(self + 0x7D51C) = t;

    if (t > *(f32*)(self + 0x7D518)) {
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
            *(void**)(self + 0x7D680),
            *(void**)(*(char**)(self + 0x7D64C) + 0xC),
            d + 0x258);
        f32 a = c[11] * *(f32*)(self + 0x7D518);
        f32 q = a / *(f32*)(self + 0x7D51C);
        scale[0] = c[3];
        scale[1] = -q;
        scale[2] = c[3];
        setPos__8MuObjectFP5Vec3fPCc(*(void**)(self + 0x7D64C), scale, d + 0x260, q, a);
        f32 b = multiply(c[11], *(f32*)(self + 0x7D500));
        f32 q2 = b / *(f32*)(self + 0x7D51C);
        scale[0] = c[3];
        scale[1] = -q2;
        scale[2] = c[3];
        setPos__8MuObjectFP5Vec3fPCc(*(void**)(self + 0x7D64C), scale, d + 0x268, q2, b);
        return;
    }
    nw4r::g3d::ScnGroup* obj = *(nw4r::g3d::ScnGroup**)(self + 0x7D680);
    nw4r::g3d::ScnObj* arg = *(nw4r::g3d::ScnObj**)(*(char**)(self + 0x7D64C) + 0xC);
    obj->Remove(arg);
}
}
}

// TODO: clean up, give variables real names
namespace jj_61E4 {
typedef unsigned int u32;

struct PadSt {
    u32 w[3];
    u32 w3;
    u32 w4;
    u32 w5;
    char pad[0x33 - 0x18];
    unsigned char stickY;
    char rest[0x40 - 0x34];
};

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13(int);
};

struct Ptr0C {
    char pad[0xC];
    int v;
};

struct Menu {
    char p0[0x40];
    int state;
    char p1[0x7D500 - 0x44];
    float cur;
    char p2[0x7D518 - 0x7D504];
    float step;
    float maxv;
    bool flag;
    float newv;
    int done;
    char p3[0x7D560 - 0x7D52C];
    Ptr0C* sel;
    char p4[0x7D580 - 0x7D564];
    int obj580;
    char p5[0x7D64C - 0x7D584];
    void* muObj;
    char p6[0x7D688 - 0x7D650];
    VObj* vobj;
    char p7[0xC4500 - 0x7D68C];
    int mode;
};

extern "C" {
extern void* g_gfPadSystem;
extern void* g_sndSystem;
extern const float lbl_5_rodata_C;
extern const float lbl_5_rodata_1E4;

void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void*, int, PadSt*);
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
void setFrameVisible__8MuObjectFf(void*, float);
void ctrlSetup__13muStageEditUIFP11gfPadStatusi(Menu*, PadSt*, int);
void setScroll2Palette__13muStageEditUIFv(Menu*);
void fn_5_CBE8(Menu*, int, int);
void changeAnimN__8MuObjectFiUlbb(int, int, int, int, int);

void fn_5_61E4(Menu* self) {
    PadSt st;
    float v = self->cur;
    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, 0xF0, &st);
    ctrlSetup__13muStageEditUIFP11gfPadStatusi(self, &st, 0);
    float old = self->newv;
    if (st.w5 & 8) {
        v -= self->step;
        if (v < lbl_5_rodata_C) v = lbl_5_rodata_C;
        self->newv = v;
        self->flag = true;
    } else if (st.w5 & 4) {
        v += self->step;
        if (v + self->step > self->maxv) v = self->maxv - self->step;
        self->newv = v;
        self->flag = true;
    } else if (st.w5 & 1) {
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
        self->state = 1;
        setScroll2Palette__13muStageEditUIFv(self);
    } else if (st.w3 & 0x800) {
        if (self->mode != 3) {
            self->state = 0x13;
        } else {
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 3, -1, 0, 0, -1);
        }
    }
    int y = (signed char)st.stickY;
    if (y < -0x1E) {
        v -= (float)(y - 0x1E) / lbl_5_rodata_1E4;
        if (v + self->step > self->maxv) v = self->maxv - self->step;
        self->newv = v;
        self->flag = true;
    } else if (y > 0x1E) {
        v -= (float)(y + 0x1E) / lbl_5_rodata_1E4;
        if (v < lbl_5_rodata_C) v = lbl_5_rodata_C;
        self->newv = v;
        self->flag = true;
    } else if (st.w3 & 0x1000) {
        setFrameVisible__8MuObjectFf(self->muObj, lbl_5_rodata_C);
        self->vobj->v13(self->sel->v);
        fn_5_CBE8(self, 0, 0);
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, -1, 0, 0, -1);
        self->state = 10;
        changeAnimN__8MuObjectFiUlbb(self->obj580, 1, 0x11, 1, 0);
        self->done = 1;
    }
    if (old != self->newv) {
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 7, -1, 0, 0, -1);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_8D44 {

struct Self {
    char pad0[0x40];
    int state;
    int result;
    char pad48[0xF8-0x48];
    void* window;
    void* scn;
    char pad100[0x7D160-0x100];
    int index;
    int sort;
    int toggle;
    int mode;
    int reset;
    char pad7D174[0x7D528-0x7D174];
    int selection;
    char pad7D52C[2];
    u8 level;
    char pad7D52F[0x7D554-0x7D52F];
    MuObject* objects[20];
    char pad7D5A4[0x7D684-0x7D5A4];
    nw4r::g3d::ScnGroup* group;
    char pad7D688[0x7D6A0-0x7D688];
    char res[0xC4500-0x7D6A0];
    int error;
    char padC4504[0xC4588-0xC4504];
    int condition;
    char padC458C[0xC4644-0xC458C];
    int stateFlag;
    void* msg;
};
struct Tbl { int v[7]; };
extern "C" {
extern gfKeepFrameBuffer* lbl_805A0008;
extern const unsigned char g_stageEditMaxZoom[];
extern const Tbl lbl_5_rodata_228;
void ctrlSetup__13muStageEditUIFP11gfPadStatusi(void*, gfPadStatus*, int);
void changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
void fn_5_4080(void*);
void setPalettCursorPos__13muStageEditUIFi(void*, int);
void setScaleGrid__13muStageEditUIFi(void*, int, int);
void fn_5_718C(void*);
void setGridScroll__13muStageEditUIFii(void*, int, int);
int fn_5_11624(void*);
void setUseCameraAnim__11gfSceneRootFb(void*, int);
void removeInfoCamResAnm__11gfSceneRootFv(void*);
void setAButtonPushedControllerData__16muMenuControllerFv();
void* fn_801AB6CC(void*, unsigned int*, int);
void* getMsgData__5MuMsgFUlPPv(void*, unsigned long, u32*);
void* fn_800BF048(int, void*, unsigned long, void*, void*, int, int, int, int, int, int, int);
}
template<class T> static inline T& at(void* p, unsigned long off) { return *(T*)((char*)p + off); }
static inline void se(int id) { g_sndSystem->playSE((SndID)id, -1, 0, 0, -1); }
static inline void drainNode(nw4r::g3d::ScnGroup* p) {
    int c;
    while ((c = p->sceneItemsCount) != 0) {
        if (c != 0) p->Remove((u32)(c - 1));
    }
}
extern "C" void fn_5_8D44(Self* self) {
    int var31 = 0;
    int var30 = 0;
    int var29;
    int var28 = 0;
    unsigned int heapTmp;
    u32 msgSize;
    Tbl tbl = lbl_5_rodata_228;
    gfPadStatus status;
    char buf[0x100];
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    ctrlSetup__13muStageEditUIFP11gfPadStatusi(self, &status, 0);
    u32 pb = status.m_buttonsPressedThisFrame.bits;
    if (pb & 0x200) {
        se(0);
        changeAnimN__8MuObjectFiUlbb(self->objects[10], 1, 0x11, 1, 0);
        self->state = 8;
        var31 = 1;
    } else if (pb & 0x10) {
        se(0x1D98);
        int nc = (self->sort + 1) % 4;
        self->sort = nc;
        changeAnimN__8MuObjectFiUlbb(self->objects[19], nc, 0x13, 1, 0);
        self->reset = 0;
        drainNode(self->group);
        fn_5_4080(self);
        setPalettCursorPos__13muStageEditUIFi(self, 1);
    } else {
        u32 pb2 = status.m_buttonsPressedThisFrame2.bits;
        if (pb2 & 8) {
            se(0);
            fn_5_718C(self);
            setGridScroll__13muStageEditUIFii(self, 3, 0);
            self->state = 4;
            var31 = 1;
        } else if (pb2 & 1) {
            if (self->selection != 0) {
                se(0);
                var30 = 1;
                var29 = self->selection - 1;
            }
        } else if (pb2 & 2) {
            if (self->selection != 6) {
                se(0);
                var30 = 1;
                var29 = self->selection + 1;
            }
        } else if (pb & 0x800) {
            if (self->error != 3) {
                int state = 0x13;
                if (self->selection == 2) state = 0x14;
                self->state = state;
            } else se(3);
        } else if (pb & 0x100) {
            switch (self->selection) {
            case 0: case 1:
                se(1); var28 = 1; break;
            case 2:
                if (self->error != 3) self->state = 0x14;
                else se(3);
                break;
            case 3:
                if (self->level != 0) {
                    se(0x1D95);
                    unsigned char old = self->level;
                    self->level = old - 1;
                    setScaleGrid__13muStageEditUIFi(self, 1, old);
                }
                break;
            case 4:
                if (self->level < g_stageEditMaxZoom[self->index]) {
                    se(0x1D94);
                    unsigned char old = self->level;
                    self->level = old + 1;
                    setScaleGrid__13muStageEditUIFi(self, 1, old);
                }
                break;
            case 5:
                se(0x1D97);
                self->toggle = (self->toggle == 0);
                self->reset = 0;
                drainNode(self->group);
                fn_5_4080(self);
                setPalettCursorPos__13muStageEditUIFi(self, 1);
                break;
            case 6:
                if (self->mode == 2) {
                    se(0x1DA1);
                    self->mode = 0;
                } else {
                    se(0x1D96);
                    self->mode = self->mode + 1;
                }
                self->objects[16]->setFrameVisible((float)self->mode);
                self->reset = 0;
                drainNode(self->group);
                fn_5_4080(self);
                setPalettCursorPos__13muStageEditUIFi(self, 1);
                setPalettCursorPos__13muStageEditUIFi(self, 1);
                break;
            }
        } else if (pb & 0x1000) {
            se(1);
            if (self->selection == 1) var28 = 1;
            else { var29 = 1; var30 = 1; }
        }
    }
    if (var28 != 0) {
        if (fn_5_11624(self) == 0) {
            self->state = 0x16;
            memset(buf, 0, 0x100);
            void* msgData = getMsgData__5MuMsgFUlPPv(self->msg, 4, &msgSize);
            memcpy(buf, msgData, msgSize);
            void* heap = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2A);
            void* scn = fn_801AB6CC(heap, &heapTmp, 1);
            self->scn = scn;
            g_gfSceneRoot->add((gfSceneRoot::LayerType)8, (nw4r::g3d::ScnMdl*)scn);
            self->window = fn_800BF048(0, buf, msgSize + 1, self->res,
                self->scn, 0x1A, 0, 0x2A, 0x2B, 0, 0, 0xF0);
        } else {
            if (self->selection == 0) se(1);
            else se(0x1D99);
            setAButtonPushedControllerData__16muMenuControllerFv();
            drainNode(at<nw4r::g3d::ScnGroup*>(g_gfSceneRoot, 0x450));
            drainNode(at<nw4r::g3d::ScnGroup*>(g_gfSceneRoot, 0x3C0));
            if (self->selection == 0) {
                self->stateFlag = 4;
                self->result = 3;
                self->state = 0x19;
                lbl_805A0008->startKeepScreen();
                setUseCameraAnim__11gfSceneRootFb(g_gfSceneRoot, 0);
                g_gfSceneRoot->removeResAnmScn();
                removeInfoCamResAnm__11gfSceneRootFv(g_gfSceneRoot);
            } else {
                self->result = 2;
                self->state = 0x19;
                setUseCameraAnim__11gfSceneRootFb(g_gfSceneRoot, 0);
                g_gfSceneRoot->removeResAnmScn();
                removeInfoCamResAnm__11gfSceneRootFv(g_gfSceneRoot);
            }
        }
    }
    if (var31 != 0 || var30 != 0) {
        int m = self->selection;
        int r4;
        if (m == 2) r4 = (self->condition == 2) ? 2 : 0;
        else if ((m == 3 && self->level == 0) ||
                 (m == 4 && self->level == g_stageEditMaxZoom[self->index])) r4 = 3;
        else r4 = 0;
        changeAnimN__8MuObjectFiUlbb(self->objects[tbl.v[m]], r4, 0x11, 0, 0);
    }
    if (var30 != 0) {
        self->selection = var29;
        if (var29 == 6)
            self->objects[16]->setFrameVisible((float)self->mode);
        int m = self->selection;
        int r4;
        if (m == 2) r4 = (self->condition == 2) ? 3 : 1;
        else if ((m == 3 && self->level == 0) ||
                 (m == 4 && self->level == g_stageEditMaxZoom[self->index])) r4 = 2;
        else r4 = 1;
        changeAnimN__8MuObjectFiUlbb(self->objects[tbl.v[m]], r4, 0x11, 1, 0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_C4E0 {

extern "C" {
extern void* g_gfPadSystem;

void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void*, int, gfPadStatus*);
void ctrlSetup__13muStageEditUIFP11gfPadStatusi(void*, gfPadStatus*, int);
int main__19MuSelctChrNameEntryFP11gfPadStatusPcbb(void*, gfPadStatus*, char*, int, int);
void* memcpy(void*, const void*, unsigned long);
unsigned long strlen(const char*);
void printData__5MuMsgFUlPCvi(void*, unsigned long, char*, unsigned long);
int changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
void setFontColor__5MuMsgFUlUcUcUcUc(void*, unsigned long, unsigned char, unsigned char, unsigned char, unsigned char);
void printIndex__5MuMsgFUlUlPv(void*, unsigned long, unsigned long, void*);
void close__19MuSelctChrNameEntryFv(void*);
void setFrameNode__8MuObjectFf(void*, float);
}

struct MsgObj {
    virtual void n00();
    virtual void n01();
    virtual void n02();
    virtual void n03();
    virtual void n04();
    virtual void n05();
    virtual void n06();
    virtual void n07();
    virtual void n08();
    virtual void n09();
    virtual void n10();
    virtual void n11();
    virtual void n12();
    virtual void n13(void*);
};

extern "C" void fn_5_C4E0(void* input) {
    char* self = (char*)input;
    gfPadStatus status;
    char* str;
    int index, size, mode;

    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, 0xF0, &status);
    ctrlSetup__13muStageEditUIFP11gfPadStatusi(self, &status, 1);

    if (*(int*)(self + 0x40) == 14) {
        str = self + 0xC45AA;
        index = 0;
        size = 0x41;
        mode = 2;
    } else {
        str = self + 0xC45EB;
        index = 1;
        size = 0x51;
        mode = 4;
    }

    switch (main__19MuSelctChrNameEntryFP11gfPadStatusPcbb(self + 0xC4660, &status, self + 0xC46F5, 1, 1)) {
    case 0:
        memcpy(str, self + 0xC46F5, size);
    case 1:
        if (strlen(str) != 0)
            printData__5MuMsgFUlPCvi(*(void**)(self + 0xC464C), index, str, strlen(str));
        ((MsgObj*)*(void**)(self + 0x7D68C))
            ->n13(*(void**)((char*)*(void**)(self + 0x7D65C) + 0xC));
        changeAnimN__8MuObjectFiUlbb(*(void**)(self + 0x7D654), mode, 0x11, 1, 0);
        setFontColor__5MuMsgFUlUcUcUcUc(*(void**)(self + 0xC4648), 2, 0xFF, 0xFF, 0xFF, 0xFF);
        printIndex__5MuMsgFUlUlPv(*(void**)(self + 0xC4648), 2, 3, 0);
        *(int*)(self + 0x40) = 13;
        close__19MuSelctChrNameEntryFv(self + 0xC4660);
        break;
    case 2:
        strlen(self + 0xC46F5);
        printData__5MuMsgFUlPCvi(*(void**)(self + 0xC464C), index, self + 0xC46F5, strlen(self + 0xC46F5));
        setFrameNode__8MuObjectFf(*(void**)(self + 0x7D65C), (float)*(s32*)(self + 0xC46E4));
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_11624 {
extern "C" {
void* getDataId__22muStageEditDataManagerFUl(void* t, u32 key);
extern s32 g_stageEditColumns[];
extern s32 g_stageEditRows[];

s32 fn_5_11624(u8* self) {
    s32 r30;
    u8* base;
    u32 r28;
    u32 r27;
    s32 r26;
    s32 r25;
    r26 = 0;
    r28 = 0;
    r30 = 0;
    while (r28 < (u32)g_stageEditColumns[*(s32*)(self + 0x7D160)]) {
        base = self + r30;
        r25 = -1;
        r27 = 0;
        while (r27 < (u32)g_stageEditRows[*(s32*)(self + 0x7D160)]) {
            s16 s = *(s16*)(base + 0x7D8A0);
            if (s != -1) {
                u32 key = reinterpret_cast<u32*>(self)[(s << 3) + 0x1F6A6];
                void* e = getDataId__22muStageEditDataManagerFUl(*(void**)(self + 0xC4758), key);
                u32 id = *(u32*)e;
                u16 v = *(u16*)((u8*)e + 0x42);
                u32 q = id / 10;
                if (q == 0x5B || q == 0x5C || q == 0x5D || q == 0x63) {
                    v = 4;
                } else if (q - 0x5F <= 2) {
                    v = 0;
                }
                if (v <= 1) {
                    if ((s32)(r27 - 1) == r25) {
                        r26++;
                        if (r26 >= 4) return 1;
                        break;
                    }
                } else if (v == 4) {
                    r25 = (s32)r27;
                }
            } else {
                r25 = (s32)r27;
            }
            base += 0x24;
            r27++;
        }
        r30 += 2;
        r28++;
    }
    return 0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_122BC {
typedef unsigned char u8;

struct gfFileIOHandle {
    void release();
    ~gfFileIOHandle();
};

struct MenuEditObject {
    u8 pad0[8];
    void* node;
    void* vtable;
    gfFileIOHandle handle;
};

extern "C" void clear__Q27utility8AutoFileFv(void*);
extern "C" void removeExist__6utListFP10utListNode(void*, void*);
extern "C" void __dl__FPv(void*);
extern "C" u8 lbl_5_data_CD8[];
extern "C" u8 lbl_8049E550[];

extern "C" MenuEditObject* fn_5_122BC(MenuEditObject* self, int deleting) {
    if (self != 0) {
        self->vtable = lbl_5_data_CD8;
        clear__Q27utility8AutoFileFv(self);
        self->handle.release();
        self->handle.~gfFileIOHandle();
        if (self != 0) {
            if (self != 0) {
                removeExist__6utListFP10utListNode(lbl_8049E550, self);
                self->node = 0;
                if (self != 0) {
                    if (self->node != 0) {
                        removeExist__6utListFP10utListNode(0, self);
                        self->node = 0;
                    }
                }
            }
        }
        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_1237C {

typedef unsigned char u8;

struct Sub {
    utListNode links;
    void* node;
    void* vtable;
    gfFileIOHandle handle;
};

struct Obj {
    u8 pad0[0x1D0C];
    Sub subA;
    u8 pad1[4];
    Sub subB;
};

extern "C" {
void clear__Q27utility8AutoFileFv(void*);
void __dt__Q218muCollectionViewer4WorkFv(void*, int);
void __dl__FPv(void*);
extern u8 lbl_5_data_CD8[];
}
extern utList lbl_8049E550;

extern "C" void* fn_5_1237C(Obj* object, int deleting) {
    if (object) {
        if (&object->subB != 0) {
            object->subB.vtable = lbl_5_data_CD8;
            clear__Q27utility8AutoFileFv(&object->subB);
            object->subB.handle.release();
            object->subB.handle.~gfFileIOHandle();
            if (&object->subB != 0) {
                if (&object->subB != 0) {
                    lbl_8049E550.removeExist(&object->subB.links);
                    int zero = 0;
                    object->subB.node = (void*)zero;
                    if (&object->subB != 0) {
                        if (object->subB.node != 0) {
                            ((utList*)0)->removeExist(&object->subB.links);
                            object->subB.node = (void*)zero;
                        }
                    }
                }
            }
        }
        if (&object->subA != 0) {
            object->subA.vtable = lbl_5_data_CD8;
            clear__Q27utility8AutoFileFv(&object->subA);
            object->subA.handle.release();
            object->subA.handle.~gfFileIOHandle();
            if (&object->subA != 0) {
                if (&object->subA != 0) {
                    lbl_8049E550.removeExist(&object->subA.links);
                    int zero = 0;
                    object->subA.node = (void*)zero;
                    if (&object->subA != 0) {
                        if (object->subA.node != 0) {
                            ((utList*)0)->removeExist(&object->subA.links);
                            object->subA.node = (void*)zero;
                        }
                    }
                }
            }
        }
        __dt__Q218muCollectionViewer4WorkFv(object, 0);
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_12500 {
#pragma opt_propagation off

extern "C" {
extern char lbl_5_data_660[];
void initProc__18muCollectionViewerFv(void*);
void clear__Q27utility8AutoFileFv(void*);
void request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(void*, const char*, int, int);
void loadMenuSound__6muMenuFv();
}

struct Menu {
    char pad[0x1AC];
    char* data;
};

extern "C" void fn_5_12500(Menu* self) {
    char* strings = lbl_5_data_660;
    initProc__18muCollectionViewerFv(self);
    char* resource = self->data;
    const char* path = strings + 0x1C;
    clear__Q27utility8AutoFileFv(resource + 0x14);
    *(int*)resource = 0;
    resource[0x2C] = 0;
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(resource + 0x14, path, 0x2B, 0);
    char* resource2 = self->data + 0x9C;
    const char* path2 = strings + 0x40;
    clear__Q27utility8AutoFileFv(resource2 + 0x14);
    *(int*)resource2 = 0;
    resource2[0x2C] = 0;
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(resource2 + 0x14, path2, 0x2B, 0);
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(self->data + 0x1C80, strings + 0x58, 0x2B, 0);
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(self->data + 0x1C98, strings + 0x74, 0x2B, 0);
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(self->data + 0x1D0C, strings + 0x90, 0x2B, 0);
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(self->data + 0x1D24, strings + 0xA8, 0x2B, 0);
    loadMenuSound__6muMenuFv();
}
}

// TODO: clean up, give variables real names
namespace jj_12E90 {
typedef unsigned long u32;

namespace nw4r {
namespace g3d {
class ScnMdlSimple;
}
}

class MuMsg {
public:
    void attachScnMdlSimple(u32, nw4r::g3d::ScnMdlSimple*, u32, float);
    void setFontColor(u32, unsigned char, unsigned char, unsigned char, unsigned char);
};

struct Model {
    char pad[0x10];
    nw4r::g3d::ScnMdlSimple* mdl;
};

struct Data {
    char pad[0x168];
    Model* a;
    Model* b;
    char pad2[0x1c7c - 0x170];
    MuMsg* msg;
};

struct Object {
    char pad[0x1ac];
    Data* data;
};

struct Ids {
    u32 id[2];
};

extern const Ids lbl_5_rodata_308;
extern const float lbl_5_rodata_310;

extern "C" void fn_6_1ECC(Object*, u32);

extern "C" void fn_5_12E90(Object* self, u32 flags) {
    fn_6_1ECC(self, flags | 4);

    self->data->msg->attachScnMdlSimple(0x11, self->data->a->mdl, 0, lbl_5_rodata_310);
    self->data->msg->setFontColor(0x11, 255, 255, 255, 255);

    self->data->msg->attachScnMdlSimple(0x12, self->data->a->mdl, 1, lbl_5_rodata_310);
    self->data->msg->setFontColor(0x12, 0x45, 0x46, 0x5e, 255);

    self->data->msg->attachScnMdlSimple(0x13, self->data->a->mdl, 2, lbl_5_rodata_310);
    self->data->msg->setFontColor(0x13, 0x45, 0x46, 0x5e, 255);

    Ids ids = lbl_5_rodata_308;
    for (u32 i = 0; i < 2; i++) {
        self->data->msg->attachScnMdlSimple(ids.id[i], self->data->b->mdl, i, lbl_5_rodata_310);
        self->data->msg->setFontColor(ids.id[i], 0, 0, 0, 255);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_13194 {
extern "C" {
extern char lbl_5_bss_11;
extern int lbl_5_data_988;
extern int lbl_5_data_9A8;
void __ct__Q218muCollectionViewer16PanelMotionTableFPCcPCc(void*, int, int);
void setAnimName__8MuObjectFP14MuAnimNameDatab(void*, const void*, int);

struct AnimInfo {
    float start;
    float end;
    float rate;
    const char* name;
    unsigned char flags[4];
};
struct AnimTable {
    AnimInfo first[11];
    AnimInfo second[11];
    unsigned char ids[12];
};
extern AnimTable lbl_5_bss_14;

void fn_5_13194(void* arg0, int arg1, int arg2) {
    if (lbl_5_bss_11 == 0) {
        __ct__Q218muCollectionViewer16PanelMotionTableFPCcPCc(&lbl_5_bss_14, lbl_5_data_988, lbl_5_data_9A8);
        lbl_5_bss_11 = 1;
    }
    setAnimName__8MuObjectFP14MuAnimNameDatab(*(void**)((char*)*(void**)((char*)arg0 + 0x1ac) + 0x108),
                &lbl_5_bss_14.first[lbl_5_bss_14.ids[arg1]], 0);
    if (arg2 != 0) {
        setAnimName__8MuObjectFP14MuAnimNameDatab(*(void**)((char*)*(void**)((char*)arg0 + 0x1ac) + 0x160),
                    &lbl_5_bss_14.second[lbl_5_bss_14.ids[arg1]], 0);
    }
    *(int*)((char*)*(void**)((char*)arg0 + 0x1ac) + 0x1bf0) = arg1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_1335C {

extern "C" {
extern char lbl_5_data_E24[];
extern char lbl_5_data_E28[];
extern f32 lbl_5_rodata_300;
void createNoticeWindow__18muCollectionViewerFv(void* self, void* buf, int len);
void* fn_6_8180(void* self, void* out);
}

struct Result {
    char data[0x44];
};

struct Child {
    char pad0[8];
    u8 kind;
};

struct Entry {
    char pad0[0xC];
    Child* child;
    char pad10[4];
    u8 flag;
};

struct Name {
    char s[65];
};

struct OutBuf {
    char text[0x200];
    int len;
};

struct AnimSlots {
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8(float f);
};

struct Data {
    char pad0[0x17C];
    MuObject* object;
    char pad180[0x1C8 - 0x180];
    char sub1C8[0x1BFC - 0x1C8];
    int state;
    char pad1C00[0x1C90 - 0x1C00];
    gfFileIOHandle handle1C90;
    char pad1C94[0x1CC0 - 0x1C94];
    gfFileIOHandle handle1CC0;
    char pad1CC4[0x1D40 - 0x1CC4];
    int a1D40;
    int a1D44;
    int a1D48;
};

struct Base {
    char pad[0x3c];
};

struct Menu : Base {
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual void f21();
    virtual void f22();
    virtual void f23();
    virtual void f24();
    virtual void f25();
    virtual void f26();
    virtual void f27();
    virtual void f28();
    virtual void f29();
    virtual void f30();
    virtual void f31();
    virtual void f32();
    virtual void f33();
    virtual void f34();
    virtual void f35();
    virtual void f36();
    virtual void f37();
    virtual void f38();
    virtual void f39();
    virtual void f40();
    virtual int f41();
    virtual Result f42(int);
    virtual void f43();
    virtual void f44(int);

    char pad40[0x16c];
    Data* data;
};

extern "C" void fn_5_1335C(Menu* self) {
    OutBuf out;
    int ret = self->f41();
    if (ret < 0) {
        self->data->state = 0;
        return;
    }
    const Result& buf = self->f42(ret);
    Entry* entry = (Entry*)fn_6_8180(self->data->sub1C8, (void*)&buf);
    if (entry->flag == 0) {
        return;
    }
    out.len = 0;
    Child* child = entry->child;
    if (child == NULL) {
        u32 slen;
        char* str;
        Message::getPrintIndexData(self->data->handle1CC0.getBuffer(), 0x3A, &str, &slen);
        u32 n = slen;
        int len = out.len;
        memcpy(out.text + len, str, n);
        int total = len + n;
        out.len = total;
        createNoticeWindow__18muCollectionViewerFv(self, out.text, total);
        return;
    }
    int kind = child->kind;
    switch (kind) {
    case 0: {
        MuObject* obj = self->data->object;
        self->f44((int)obj->m_sceneModel);
        char* mdl = *(char**)((char*)obj + 8);
        int off = *(int*)(mdl + 0x3c);
        const char* src = off ? mdl + off : 0;
        Name n;
        n.s[0] = 0;
        strcat(n.s, src);
        Name n2 = n;
        strcat(n2.s, lbl_5_data_E28);
        Name n3 = n2;
        sprintf(n3.s + strlen(n3.s), lbl_5_data_E24, 0);
        Name n4 = n3;
        obj->changeClrAnimNIf(n4.s);
        char* mdl2 = *(char**)((char*)obj + 8);
        int off2 = *(int*)(mdl2 + 0x3c);
        const char* src2 = off2 ? mdl2 + off2 : 0;
        Name n5;
        n5.s[0] = 0;
        strcat(n5.s, src2);
        Name n6 = n5;
        strcat(n6.s, lbl_5_data_E28);
        Name n7 = n6;
        sprintf(n7.s + strlen(n7.s), lbl_5_data_E24, 0);
        Name n8 = n7;
        obj->changeNodeAnimNIf(n8.s);
        AnimSlots* anim = (AnimSlots*)obj->m_modelAnim->m_anmObjChrRes;
        anim->s8(lbl_5_rodata_300);
        obj->setFrameNode(lbl_5_rodata_300);
        self->data->state = 0x18;
        break;
    }
    case 1:
    case 2: {
        u32 slen;
        char* str;
        Message::getPrintIndexData(self->data->handle1C90.getBuffer(), 5, &str, &slen);
        u32 n = slen;
        int len = out.len;
        memcpy(out.text + len, str, n);
        int total = len + n;
        out.len = total;
        createNoticeWindow__18muCollectionViewerFv(self, out.text, total);
        break;
    }
    case 3:
        self->data->a1D40 = 1;
        self->data->a1D44 = 4;
        self->data->a1D48 = 0;
        self->data->state = 0x19;
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_13730 {
typedef unsigned int u32;
struct Data {
    char pad0[0x1a0];
    u32 arg;
    char pad1[0x1b40 - 0x1a4];
    u32 index;
    char pad2[0x1cee - 0x1b44];
    bool flag;
};
struct Base { char padding[0x3c]; };
struct Object : Base {
    virtual void m0();virtual void m1();virtual void m2();virtual void m3();virtual void m4();
    virtual void m5();virtual void m6();virtual void m7();virtual void m8();virtual void m9();
    virtual void m10();virtual void m11();virtual void m12();virtual void m13();virtual void m14();
    virtual void m15();virtual void m16();virtual void m17();virtual void m18();virtual void m19();
    virtual void m20();virtual void m21();virtual void m22();virtual void m23();virtual void m24();
    virtual void m25();virtual void m26();virtual void m27();virtual void m28();virtual void m29();
    virtual void m30();virtual void m31();virtual void m32();virtual void m33();virtual void m34();
    virtual void m35();virtual void m36();virtual void m37();virtual void m38();virtual void m39();
    virtual void m40();virtual void m41();virtual void m42();virtual void m43();
    virtual u32 count(u32) const;
    char padding2[0x1ac - 0x40];
    Data* data;
};
extern "C" u32 fn_5_13730(const Object* self) {
    Data* d = self->data;
    u32 index;
    if (d->flag) {
        index = d->index - 1;
    } else {
        index = d->index;
    }
    u32 n = self->count(d->arg);
    return index < n ? index : 0x80000000;
}
}

// TODO: clean up, give variables real names
namespace jj_13A74 {

extern "C" {
extern char lbl_5_data_E24[];
extern char lbl_5_data_E28[];
}

struct Global {
    char pad[0x19c];
    int counts[24];

    int getCount(unsigned int index) {
        return index >= 24 ? 0 : counts[index];
    }
};

extern Global* lbl_805A01D8;

struct Data {
    char pad0[0x16c];
    MuObject* object;
    char pad170[0x28];
    int value;
    char pad19c[0x1bfc - 0x19c];
    int state;
    char pad1c00[0x150];
    int a;
    char pad1d54[4];
    int b;
    int c;
    unsigned int d;
    unsigned int e;

    inline void selectRandom() {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
        e = 0;

        Global* g = lbl_805A01D8;
        int count = 0;
        for (unsigned int i = 0; i < 21; ++i) {
            if (g->getCount(i) != 0)
                ++count;
        }

        d = randi(count);
        e = randi(g->getCount(d));
    }
};

struct Base {
    char pad[0x3c];
};

struct Menu : Base {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual void f21();
    virtual void f22();
    virtual void f23();
    virtual void f24();
    virtual void f25();
    virtual void f26();
    virtual void f27();
    virtual void f28();
    virtual void f29();
    virtual void f30();
    virtual void f31();
    virtual void f32();
    virtual void f33();
    virtual void f34();
    virtual void f35();
    virtual void f36();
    virtual void f37();
    virtual void f38();
    virtual void f39();
    virtual void f40();
    virtual void f41();
    virtual void fB0(int);
    virtual void f43();
    virtual void f44();
    virtual void f45();
    virtual void f46();
    virtual void f47();
    virtual void f48();
    virtual void f49();
    virtual void f50();
    virtual void f51();
    virtual void f52();
    virtual void f53();
    virtual void f54();
    virtual void f55();
    virtual void f56();
    virtual void f57();
    virtual void f58();
    virtual void f59();
    virtual void f60();
    virtual void f61();
    virtual void f62();
    virtual void f63();
    virtual void f64();
    virtual void f65();
    virtual void f66();
    virtual void f67();
    virtual void f68();
    virtual void f69();
    virtual void f70();
    virtual void f71();
    virtual void f72();
    virtual void f73();
    virtual void f74();
    virtual void f75();
    virtual void f76();
    virtual void f77();
    virtual void f78();
    virtual void f79();
    virtual void f80();
    virtual void f81();
    virtual void f82();
    virtual void f83();
    virtual void f84();
    virtual void f85();
    virtual void f86();
    virtual void f87();
    virtual void f88();
    virtual void f89();
    virtual void f90();
    virtual void f91();
    virtual void f92();
    virtual void f93();
    virtual void f94();
    virtual void f95();
    virtual void f96();
    virtual void f97();
    virtual void f98();
    virtual void f99();
    virtual void f100();
    virtual void f101();
    virtual void f102();
    virtual void f103();
    virtual void f104();
    virtual void f105();
    virtual void f106();
    virtual void f107();
    virtual void f108();
    virtual void f109();
    virtual void f110();
    virtual void f111();
    virtual void f112();
    virtual void f1CC();
    virtual void f1D0(int, int);

    char pad40[0x16c];
    Data* data;
};

struct Name {
    char s[65];
};

extern "C" void fn_5_13A74(Menu* self) {
    self->data->selectRandom();

    self->f1CC();
    self->f1D0(0, 0);

    MuObject* obj = self->data->object;
    char* mdl = *(char**)((char*)obj + 8);
    int offset = *(int*)(mdl + 0x3c);
    const char* name = offset ? mdl + offset : 0;

    Name n;
    n.s[0] = 0;
    strcat(n.s, name);
    Name n2 = n;
    strcat(n2.s, lbl_5_data_E28);
    Name n3 = n2;
    sprintf(n3.s + strlen(n3.s), lbl_5_data_E24, 0);
    Name n4 = n3;
    obj->changeNodeAnimNIf(n4.s);

    self->fB0(self->data->value);
    self->data->state = 0x1c;
}
}

// TODO: clean up, give variables real names
namespace jj_13D64 {
struct gfPadStatus {
    unsigned char data[0x38];
    signed char flag;
    int type;
};

extern "C" {
extern const float lbl_5_rodata_2F8[];
extern void* g_gfPadSystem;

void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void* self, int idx, gfPadStatus* st);
void setFrameFlg__8MuObjectFfUc(void* a, int b);
}

extern "C" void fn_5_13D64(char* self) {
    const float* c = lbl_5_rodata_2F8;
    gfPadStatus st;
    float f;
    void* obj;

    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, *(int*)(*(char**)(self + 0x1ac) + 0x1d68), &st);
    obj = *(void**)(*(char**)(self + 0x1ac) + 0x178);
    if (st.flag != 0) {
        f = c[10];
    } else {
        switch (st.type) {
        case 0: f = c[2]; break;
        case 1: f = c[11]; break;
        case 2: f = c[12]; break;
        case 3: f = c[13]; break;
        default: f = c[14]; break;
        }
    }
    if (f >= c[2]) {
        setFrameFlg__8MuObjectFfUc(obj, 4);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_14A1C {
extern "C" void utf16to8__7MessageFPcPCw(char*, const void*);
struct Entry { char pad[0x10]; unsigned long index; };
extern "C" Entry* getBgmTitleDataPlayId__16sndBgmRateSystemFUl(void*, unsigned int);
extern "C" void* lbl_805A01D8;
struct Context { char pad[0x194]; void* field194; char pad2[0x1c7c-0x198]; MuMsg* msg; char pad3[0x1d34-0x1c80]; gfFileIOHandle handle; };
struct Base { char pad[0x3c]; };
struct Self : Base {
virtual void v0();
virtual void v1();
virtual void v2();
virtual void v3();
virtual void v4();
virtual void v5();
virtual void v6();
virtual void v7();
virtual void v8();
virtual void v9();
virtual void v10();
virtual void v11();
virtual void v12();
virtual void v13();
virtual void v14();
virtual void v15();
virtual void v16();
virtual void v17();
virtual void v18();
virtual void v19();
virtual void v20();
virtual void v21();
virtual void v22();
virtual void v23();
virtual void v24();
virtual void v25();
virtual void v26();
virtual void v27();
virtual void v28();
virtual void v29();
virtual void v30();
virtual void v31();
virtual void v32();
virtual void v33();
virtual void v34();
virtual void v35();
virtual void v36();
virtual void v37();
virtual void v38();
virtual void v39();
virtual void v40();
virtual void v41();
virtual void first(void*);
virtual void second(void*);
char pad2[0x1ac-0x40]; Context* context;
};
extern "C" void fn_5_14A1C(Self* self, char* arg) {
 char output1[0x4c]; char output2[0x54]; char converted1[0x44]; char converted2[0x54]; short name1[17]; short name2[21];
 self->second(self->context->field194);
 self->first(self->context->field194);
 char* p = arg + 0x54;
 memcpy(name1,p,0x20); name1[16]=0; output1[0]=0;
 utf16to8__7MessageFPcPCw(converted1,name1); strcat(output1,converted1); self->context->msg->printf(0x11,output1);
 memcpy(name2,arg+0x28,0x28); name2[20]=0; output2[0]=0;
 utf16to8__7MessageFPcPCw(converted2,name2); strcat(output2,converted2); self->context->msg->printf(0x12,output2);
 Entry* entry=getBgmTitleDataPlayId__16sndBgmRateSystemFUl(lbl_805A01D8,*(unsigned short*)(p+0x20));
 void* buffer=self->context->handle.getBuffer();
 self->context->msg->printIndex(0x13,entry->index,buffer);
}
}

