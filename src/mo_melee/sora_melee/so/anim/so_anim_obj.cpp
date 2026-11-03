#include <so/anim/so_anim_obj.h>

void soAnimObj::reset() {
    destroy();
    m_anmVisRes = 0;
    m_anmMatClrRes = 0;
    m_anmTexPatRes = 0;
    m_anmTexSrtRes = 0;
    m_anmShpRes = 0;
    m_animFrame = 0.0f;
    m_startFrame = 0.0f;
    m_saveFrame = 0.0f;
    m_rate = 1.0f;
    m_loopFlag = 0;
    m_reverseFlag = 0;
    m_emptyFlag = 1;
}

void soAnimObj::setFrame(float frame) {
    if (m_emptyFlag == 0) {
        if (m_anmVisRes) {
            m_anmVisRes->SetFrame(frame);
        }
        if (m_anmMatClrRes) {
            m_anmMatClrRes->SetFrame(frame);
        }
        if (m_anmTexPatRes) {
            m_anmTexPatRes->SetFrame(frame);
        }
        if (m_anmTexSrtRes) {
            m_anmTexSrtRes->SetFrame(frame);
        }
        if (m_anmShpRes) {
            m_anmShpRes->SetFrame(frame);
        }
        m_animFrame = frame;
    }
}

float soAnimObj::getEndFrame() const {
    if (m_emptyFlag == 0) {
        if (m_anmVisRes) {
            return (s32)m_anmVisRes->m_anmVisFile->m_animLength - 1.0f;
        }
        if (m_anmMatClrRes) {
            return (s32)m_anmMatClrRes->m_anmMatClrFile->m_animLength - 1.0f;
        }
        if (m_anmTexPatRes) {
            return (s32)m_anmTexPatRes->m_anmTexPatFile->m_animLength - 1.0f;
        }
        if (m_anmTexSrtRes) {
            return (s32)m_anmTexSrtRes->m_anmTexSrtFile->m_animLength - 1.0f;
        }
        if (m_anmShpRes) {
            return (s32)m_anmShpRes->m_anmShpFile->m_animLength - 1.0f;
        }
    }
    return 0.0f;
}

bool soAnimObj::isEnd() const {
    if (m_emptyFlag == 0 && m_loopFlag == 0 && m_animFrame >= getEndFrame()) {
        return true;
    }
    return false;
}

bool soAnimObj::isLooped() const {
    if (m_emptyFlag == 0 && m_loopFlag == 1 && m_animFrame >= 0.0f && m_animFrame < m_rate && m_animFrame < m_startFrame) {
        return true;
    }
    return false;
}

void soAnimObj::rewind() {
    m_saveFrame = m_animFrame;
    setFrame(m_startFrame);
}

void soAnimObj::turnBack() {
    setFrame(m_saveFrame);
}

void soAnimObj::setAnimObj(float startFrame, float rate, nw4r::g3d::ResFile resFile, nw4r::g3d::ResMdl resMdl, Heaps::HeapType heapType, u8 loopFlag, u8 reverseFlag, const char* animationName) {
    create(nw4r::g3d::ResFile(resFile), nw4r::g3d::ResMdl(resMdl), heapType, animationName);
    m_rate = rate;
    m_loopFlag = loopFlag;
    setFrame(startFrame);
    m_startFrame = startFrame;
    m_reverseFlag = reverseFlag;
}
