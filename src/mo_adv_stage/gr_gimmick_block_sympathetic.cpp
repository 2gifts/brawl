#include <cm/cm_quake.h>
#include <sora_adv_stage/gr_gimmick_block_sympathetic.h>
#include <mt/mt_prng.h>

// HYPOTHESIS: getInstance/[cmQuake] and reqQuake/[cmQuake] (main.dol map), not decompiled yet.
extern "C" cmQuake* fn_8009D2DC();

grGimmickBlockSympathetic* grGimmickBlockSympathetic::create(int mdlIndex, const char* taskName) {
    grGimmickBlockSympathetic* ground = new (Heaps::StageInstance) grGimmickBlockSympathetic(taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

void grGimmickBlockSympathetic::update(float deltaFrame) {
    m_hasUpdatedG3dCalcWorld = true;
}

void grGimmickBlockSympathetic::requestBreak() {
    setVisibility(0);
    setEnableCollisionStatus(false);
    startGimmickSE(0);
    startGimmickEffect(0);
    Vec3f pos = getPos();
    fn_8009D2DC()->reqQuake(cmQuake::Amplitude_S, &pos);
}

void grGimmickBlockSympathetic::invalidatedByCameraClipping() { }

void grGimmickBlockSympathetic::dbDispInvalidatedByCameraClippingSphere() { }

void grGimmickBlockSympathetic::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    unk14C = static_cast<unkData*>(m_gimmickData);
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(1);
    createSoundWork(2, 1);
    createEffectWork(2);
    float choice = randf();
    if (unk14C->unk30 > 0) {
        setSoundEffect(0, unk14C->unk30);
    } else if (unk14C->unk2F == 0) {
        if (choice < 0.5f) {
            setSoundEffect(0, 0x2263);
        } else {
            setSoundEffect(0, 0x2264);
        }
    } else if (unk14C->unk2F == 1) {
        if (choice < 0.333f) {
            setSoundEffect(0, 0x2265);
        } else if (randf() < 0.666f) {
            setSoundEffect(0, 0x2266);
        } else {
            setSoundEffect(0, 0x2267);
        }
    } else if (unk14C->unk2F == 2) {
        setSoundEffect(0, 0x2275);
    }
    setSoundEffect(1, unk14C->unk34);
    if (unk14C->unk38 > 0) {
        setEffect(0, unk14C->unk38);
    } else {
        setEffect(0, 0x0103001E);
    }
    setEffect(1, unk14C->unk3C);
}
