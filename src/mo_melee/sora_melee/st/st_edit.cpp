#include <gf/gf_heap_manager.h>
#include <st_stageedit/st_stageedit.h>
#include <string.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern u8 lbl_27_data_4B190[4][2];

void stEdit::initStageBlocks() {
    memset(m_startPos, 0, 0x1A28);
    if (m_itemPos) {
        gfHeapManager::free(m_itemPos);
        m_itemPos = 0;
        m_itemPosCount = 0;
    }
    if (m_fallBlocks) {
        gfHeapManager::free(m_fallBlocks);
        m_fallBlocks = 0;
        m_fallBlockCount = 0;
    }
    if (m_linkCollA) {
        gfHeapManager::free(m_linkCollA);
        m_linkCollA = 0;
    }
    if (m_linkCollB) {
        gfHeapManager::free(m_linkCollB);
        m_linkCollB = 0;
    }
    if (m_linkMaskA) {
        gfHeapManager::free(m_linkMaskA);
        m_linkMaskA = 0;
    }
    if (m_linkMaskB) {
        gfHeapManager::free(m_linkMaskB);
        m_linkMaskB = 0;
    }
    m_width = lbl_27_data_4B190[m_size][0];
    m_height = lbl_27_data_4B190[m_size][1];
}

void stEdit::enableBg(bool enableTrainer) {
    if (!(m_flags & 1)) {
        static_cast<grMadein*>(getGround(0))->startEntityAutoLoop();
        m_flags |= 1;
    }
    if (enableTrainer && m_pokeTrainerBase && !(m_flags & 2)) {
        static_cast<grMadein*>(m_pokeTrainerBase)->startEntityAutoLoop();
        if (m_pokeTrainerBaseColl) {
            m_pokeTrainerBaseColl->setEnable();
        }
        m_flags |= 2;
    }
}

void stEdit::disableBg() {
    if (m_flags & 1) {
        static_cast<grMadein*>(getGround(0))->endEntity();
        m_flags &= ~1;
    }
    if (m_pokeTrainerBase && (m_flags & 2)) {
        static_cast<grMadein*>(m_pokeTrainerBase)->endEntity();
        m_flags &= ~2;
    }
}

void stEdit::setScene(int scene) {
    nw4r::g3d::ResFileData* data = static_cast<nw4r::g3d::ResFileData*>(
        m_fileData->getData(Data_Type_Scene, scene, 0xFFFE));
    registScnAnim(data, 0);
}

float stEdit::getCameraParamDistanceMax() {
    if (m_cameraParam1 == NULL) {
        return 0.0f;
    } else {
        return m_cameraParam1->m_horizontalRotationFactor;
    }
}
