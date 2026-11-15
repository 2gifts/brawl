#include <gr/gr_calc_world_callback.h>
#include <gr/gr_poke_trainer.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>

// The Pokemon Trainer standing on the stage background: the stage reads the positions of its two nodes every frame and
// the stage hands it the position it follows.

inline grPokeTrainer::grPokeTrainer(const char* taskName) : grYakumono(taskName) {
    setupMelee();
    m_posWork = NULL;
    m_posDstWork = NULL;
    m_nNode = 0;
    m_eNode = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grPokeTrainer* grPokeTrainer::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPokeTrainer* ground = new (Heaps::StageInstance) grPokeTrainer(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPokeTrainer::~grPokeTrainer() { }

void grPokeTrainer::processAnim() {
    Ground::processAnim();
    if (m_posDstWork != NULL) {
        getNodePosition(m_posDstWork, 0, m_nNode);
        getNodePosition(m_posDstWork + 1, 0, m_eNode);
    }
}

void grPokeTrainer::update(float deltaFrame) {
    Ground::update(deltaFrame);
    if (m_isUpdate) {
        updateCallBack(deltaFrame);
    }
}

void grPokeTrainer::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        nw4r::g3d::ScnMdlSimple* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                callback->m_index = 0;
                callback->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = callback;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = callback->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_posWork != NULL) {
                callback->m_nodeCallbackDatas[0].m_pos = *m_posWork;
            }
        }
    }
}

bool grPokeTrainer::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(reinterpret_cast<u32*>(&m_nNode), 0, "PokeTrainer0N");
    getNodeIndex(reinterpret_cast<u32*>(&m_eNode), 0, "PokeTrainer0E");
    return result;
}
