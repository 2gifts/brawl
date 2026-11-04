#include <so/collision/so_collision_hit_group.h>
#include <so/collision/so_collision_hit_part.h>
#include <types.h>

soCollisionHitGroup::soCollisionHitGroup() {
    m_index = -1;
    m_partStart = 0;
    m_partSize = 0;
    m_multiSituation = 0;
    m_globalOffset = 0;
}

soCollisionHitGroup::~soCollisionHitGroup() { }

void soCollisionHitGroup::init() {
    m_partStart = -1;
    m_partSize = 0;
    m_index = 0;
}

void soCollisionHitGroup::resetStatusAll(soArray<soCollisionHitPart>* parts) {
    if (m_51 == 1) {
        for (int i = 0; i < m_partSize; i++) {
            soCollisionHitPart& part = parts->at(m_partStart + i);
            part.m_status = part.unk4;
        }
        m_51 = 0;
    }
}

void soCollisionHitGroup::setStatus(soArray<soCollisionHitPart>* parts, u32 index, int status) {
    parts->at(m_partStart + index).m_status = status;
    m_51 = 1;
}

void soCollisionHitGroup::setStatusNode(soArray<soCollisionHitPart>* parts, int nodeIndex, int status) {
    for (int i = 0; i < m_partSize; i++) {
        if (parts->at(m_partStart + i).m_hitData.m_nodeIndex == nodeIndex) {
            parts->at(m_partStart + i).m_status = status;
            m_51 = 1;
            return;
        }
    }
}

void soCollisionHitGroup::setStatusNodeDefault(soArray<soCollisionHitPart>* parts, int nodeIndex, int status) {
    for (int i = 0; i < m_partSize; i++) {
        if (parts->at(m_partStart + i).m_hitData.m_nodeIndex == nodeIndex) {
            parts->at(m_partStart + i).unk4 = status;
            m_51 = 1;
            return;
        }
    }
}

void soCollisionHitGroup::setStatusAll(soArray<soCollisionHitPart>* parts, int status) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).m_status = status;
    }
    m_51 = 1;
}

void soCollisionHitGroup::setWhole(int whole) {
    m_whole = whole;
}

int soCollisionHitGroup::getWhole() {
    return m_whole;
}

void soCollisionHitGroup::resetStatusGlobal() {
    if (m_49 == true || m_xluFrameGlobal != 0) {
        m_global = 2;
    } else if (m_50 == true || m_invincibleFrameGlobal != 0) {
        m_global = 1;
    } else {
        m_global = 0;
    }
}

void soCollisionHitGroup::setXluGlobal() {
    m_49 = true;
    resetStatusGlobal();
}

void soCollisionHitGroup::cancelXluGlobal() {
    m_49 = false;
    resetStatusGlobal();
}

void soCollisionHitGroup::setXluFrameGlobal(int xluFrameGlobal) {
    m_xluFrameGlobal = xluFrameGlobal;
    resetStatusGlobal();
}

void soCollisionHitGroup::setInvincibleGlobal() {
    m_50 = true;
    resetStatusGlobal();
}

void soCollisionHitGroup::cancelInvincibleGlobal() {
    m_50 = false;
    resetStatusGlobal();
}

void soCollisionHitGroup::setInvincibleFrameGlobal(int invincibleFrameGlobal, bool override) {
    if (override == true) {
        m_invincibleFrameGlobal = invincibleFrameGlobal;
    } else if (m_invincibleFrameGlobal < invincibleFrameGlobal) {
        m_invincibleFrameGlobal = invincibleFrameGlobal;
    }
    resetStatusGlobal();
}

void soCollisionHitGroup::updateStatusGlobal() {
    if (m_xluFrameGlobal > 0) {
        m_xluFrameGlobal--;
        if (m_xluFrameGlobal == 0) {
            resetStatusGlobal();
        }
    }
    if (m_invincibleFrameGlobal > 0) {
        m_invincibleFrameGlobal--;
        if (m_invincibleFrameGlobal == 0) {
            resetStatusGlobal();
        }
    }
}

int soCollisionHitGroup::getGlobal() {
    return m_global;
}

int soCollisionHitGroup::getPartStatus(soArray<soCollisionHitPart>* parts, u32 index) {
    return parts->at(m_partStart + index).m_status;
}

void soCollisionHitGroup::initMultiSituation() {
    m_multiSituation = 0;
}

void soCollisionHitGroup::setMultiSituation(SituationKind situation) {
    m_multiSituation = 1 << soCollision::getSituation(situation);
}

void soCollisionHitGroup::setOpponentCategory(soArray<soCollisionHitPart>* parts, u32 categoryMask, bool enable) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setOpponentCategory(categoryMask, enable);
    }
}

void soCollisionHitGroup::setSelfCategory(soArray<soCollisionHitPart>* parts, soCollision::Category category) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setSelfCategory(category);
    }
}

void soCollisionHitGroup::reset(soCollision* collision, soArray<soCollisionHitPart>* parts, float posX, float scale, float lr, SituationKind situation) {
    m_posX = posX;
    m_scale = scale;
    m_situationKind = situation;
    m_lr = lr;
    m_multiSituation = 0;
    if (m_index != -1) {
        unk24 = 0.0f;
        m_whole = 0;
        m_global = 0;
        m_49 = false;
        m_xluFrameGlobal = 0;
        m_50 = false;
        m_invincibleFrameGlobal = 0;
        m_51 = 1;
        unk48 = 0;
        soCollision::getSituation(situation);
        soCollisionHitPart* part;
        for (int i = 0; i < m_partSize; i++) {
            part = &parts->at(m_partStart + i);
            part->reset(collision->getCollision(i, m_index));
        }
    }
}

void soCollisionHitGroup::set(soArray<soCollisionHitPart>* parts, u32 index, soCollisionHitData* data) {
    parts->at(m_partStart + index).setData(data);
}

void soCollisionHitGroup::set(soArray<soCollisionHitPart>* parts, soSet<soCollisionHitData>* data) {
    for (int i = 0; i < (int)data->m_size; i++) {
        parts->at(m_partStart + i).setData(&data->m_elements[i]);
    }
}

void soCollisionHitGroup::setCheckCatch(soArray<soCollisionHitPart>* parts, bool checkCatch) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setCheckCatch(checkCatch);
    }
}
