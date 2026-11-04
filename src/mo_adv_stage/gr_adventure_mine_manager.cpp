#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_mine.h>
#include <sora_adv_stage/gr_adventure_mine_manager.h>

// Matrix::copy (the map name); mt_matrix.cpp defines it by this stand-in name.
extern "C" void fn_8003E388(const Matrix* src, Matrix* dst);

void grAdventureMineManager::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    void (grAdventureMineManager::*flows[4])(const int&, float) = {
        &grAdventureMineManager::frowStandby,
        &grAdventureMineManager::frowExplosion,
        &grAdventureMineManager::frowExploding,
        &grAdventureMineManager::frowEnding
    };
    for (u8 i = 0; i < unk1A8; i++) {
        unk174[i]->m_isUpdate = true;
        if (unk174[i]->unk15C == 0) {
            Matrix copied(true);
            Matrix matrix(true);
            m_sceneModels[0]->GetScnMtxPos(&matrix, nw4r::g3d::ScnObj::MTX_WORLD, i + 1);
            fn_8003E388(&matrix, &copied);
            unk174[static_cast<int>(i)]->setMatrix(&copied);
        }
        (this->*flows[unk174[i]->unk15C])(i, deltaFrame);
    }
    chSympatheticDetonation();
    chWarning();
    if (unk170) {
        unk170 = false;
        clearAttackerInfo();
    }
}

void grAdventureMineManager::onInflict(soCollisionLog* collisionLog, u32 unk2, float power) {
    grAdventureMine* mine = unk174[collisionLog->m_damageIndex];
    if (mine->unk15C == 0) {
        mine->unk15C = 1;
    }
}

void grAdventureMineManager::frowExploding(const int& index, float deltaFrame) {
    float& time = unk174[index]->unk168;
    time -= deltaFrame;
    if (time < 0.0f) {
        unk174[index]->unk15C = 3;
    }
    if (time < 16.0f) {
        grYakumono::disableAttack(index);
        grYakumono::disableHit(index, 0);
        grYakumono::setTeamYakumono(0x12, -1);
    }
}
