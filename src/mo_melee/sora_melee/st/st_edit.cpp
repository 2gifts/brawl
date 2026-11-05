#include <gf/gf_heap_manager.h>
#include <st_stageedit/st_stageedit.h>
#include <string.h>
#include <types.h>
#include <st/st_area_manager.h>
#include <snd/snd_system.h>

grGimmickEditSpring::~grGimmickEditSpring() { }

void grGimmickEditSpring::setMotionOn() {
    m_motionRatio = 1.0f;
    changeNodeAnim(1, 0);
    m_state = 1;
    m_animFrame = 0.0f;
}

void grGimmickEditSpring::setMotionOff() {
    changeNodeAnim(0, 0);
    g_sndSystem->playSE(static_cast<SndID>(0x1D9A), -1, 0, 0, -1);
    m_state = 2;
    m_animFrame = 0.0f;
}

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

void stEdit::getFighterReStartPos(Vec3f* startPos, int fighterIndex) {
    *startPos = m_restartPos;
}

void stEdit::getFighterStartPos(Vec3f* startPos, int fighterIndex) {
    *startPos = m_startPos[fighterIndex];
}

stEdit* stEdit::create() {
    return new (Heaps::StageInstance) stEdit;
}

void stEdit::getItemPos(Vec3f* pos, Vec3f* end, u32 index) {
    if (index < m_itemPosCount && m_itemPos) {
        Vec3f* item = &m_itemPos[index * 2];
        pos->m_x = item[0].m_x;
        pos->m_y = item[0].m_y;
        pos->m_z = item[0].m_z;
        end->m_x = item[1].m_x;
        end->m_y = item[1].m_y;
        end->m_z = item[1].m_z;
    }
}

GXColor stEdit::getFinalTechniqColor() {
    u32 packed;
    switch (m_theme) {
        case 0:
            packed = 0x140004B8;
            break;
        case 1:
            packed = 0x14000496;
            break;
        case 2:
            packed = 0x1400047D;
            break;
        default:
            packed = 0;
            break;
    }
    return *reinterpret_cast<GXColor*>(&packed);
}

u32 stEdit::getModeFlag(u32 category, u32 type) {
    u32 flags;
    if (category == 1) {
        flags = 0x10;
    } else if (category == 3 && type == 9) {
        flags = 0x10;
    } else {
        flags = 0;
    }
    if (category == 0 && type == 5) {
        flags |= 0x80;
    }
    if (category == 1) {
        flags |= 0x800;
    }
    if (category != 3) {
        return flags;
    }
    switch (type) {
        case 0: return flags | 0x1020;
        case 1: return flags | 0x20;
        case 2: return flags | 3;
        case 3: return flags | 0x420;
        case 4: return flags | 0x21;
        case 5: return flags | 0x40;
        case 6: return flags | 2;
        case 7:
        case 8:
        case 9:
            break;
    }
    return flags;
}

// stTriggerMng::eraseObsTrigger (the map name); src/mo_stage/st_stageedit/st_stageedit.cpp calls it by this stand-in name.
extern "C" void fn_27_232374(stTriggerMng* mng, void* trigger);

stEdit::stEdit() : stMelee("stEdit", Stages::Edit) {
    m_theme = 0;
    m_size = 0;
    m_bgmId = 0x2C;
    m_loaded = 0;
    m_itemPosCount = 0;
    m_fallBlockCount = 0;
    m_linkCollA = NULL;
    m_linkCollB = NULL;
    m_linkMaskA = NULL;
    m_linkMaskB = NULL;
    m_flags = 0;
    m_pokeTrainerBase = NULL;
    m_pokeTrainerBaseColl = NULL;
    m_itemPos = NULL;
    m_fallBlocks = NULL;
    m_seCount = 0;
    memset(m_startPos, 0, 0x1A28);
    stAreaManager::getInstance()->initMng(0x20);
}

void stEdit::deleteBlock(stEditBlock* block) {
    if (block->m_collision != NULL) {
        delete block->m_collision;
        block->m_collision = NULL;
    }
    Ground* ground = block->m_ground;
    if (ground != NULL) {
        grMadein* madein = dynamic_cast<grMadein*>(ground);
        if (madein != NULL) {
            madein->endEntity();
        }
        removeGround(ground);
        ground->preExit();
        ground->exit();
        block->m_ground = NULL;
    }
    if (block->m_trigger != NULL) {
        fn_27_232374(m_triggerMng, block->m_trigger);
        block->m_trigger = NULL;
    }
    if (block->m_data != NULL) {
        gfHeapManager::free(block->m_data);
        block->m_data = NULL;
    }
    block->m_desc = 0;
}

void stEdit::createMoveController(u32 countA, u32 countB) {
    if (m_linkCollA != NULL || m_linkCollB != NULL) {
        if (m_linkCollA != NULL) {
            gfHeapManager::free(m_linkCollA);
            m_linkCollA = NULL;
        }
        if (m_linkCollB != NULL) {
            gfHeapManager::free(m_linkCollB);
            m_linkCollB = NULL;
        }
        if (m_linkMaskA != NULL) {
            gfHeapManager::free(m_linkMaskA);
            m_linkMaskA = NULL;
        }
        if (m_linkMaskB != NULL) {
            gfHeapManager::free(m_linkMaskB);
            m_linkMaskB = NULL;
        }
    }
    if (countA != 0) {
        u32 bytes = (countA & 0xFF) << 2;
        m_linkCollA = static_cast<grCollision**>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkCollA != NULL) {
            memset(m_linkCollA, 0, bytes);
        }
        m_linkMaskA = static_cast<u32*>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkMaskA != NULL) {
            memset(m_linkMaskA, 0, bytes);
        }
    }
    if (countB != 0) {
        u32 bytes = (countB & 0xFF) << 2;
        m_linkCollB = static_cast<grCollision**>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkCollB != NULL) {
            memset(m_linkCollB, 0, bytes);
        }
        m_linkMaskB = static_cast<u32*>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkMaskB != NULL) {
            memset(m_linkMaskB, 0, bytes);
        }
    }
}
