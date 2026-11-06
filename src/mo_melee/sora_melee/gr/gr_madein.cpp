#include <gr/gr_madein.h>
#include <types.h>

// normalize/[Matrix]/(mt_matrix.o) (the map name); other files call it by this stand-in name.
extern "C" void fn_8003E5B4(Matrix* mtx);

void grMadein::setTgtNode(const char* unk1) { }

char* grMadein::getTgtNode() {
    return NULL;
}

float grMadein::getEntityFrame() {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim == NULL) {
        return 0.0f;
    } else {
        return anim->getFrame();
    }
}

grMadein::StageHitData::StageHitData() {
    m_30 = 0;
    m_34 = 0;
    m_38 = 0;
}

void grMadein::switchToMatrix() {
    m_calcWorldCallBack.m_nodeCallbackDatas->m_flags &= ~7;
    m_calcWorldCallBack.m_nodeCallbackDatas->m_flags |= 8;
}

void grMadein::pauseEntity(bool isPause) {
    if (isPause == true) {
        m_motionRatio = 0.0f;
    } else {
        m_motionRatio = 1.0f;
    }
}

void grMadein::setMatrix(Matrix* matrix, bool followAttack) {
    m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix = *matrix;
    if (!followAttack) {
        fn_8003E5B4(&m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix);
    }
    setPos(matrix);
}
