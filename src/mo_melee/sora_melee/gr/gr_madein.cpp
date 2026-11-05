#include <gr/gr_madein.h>
#include <types.h>

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
