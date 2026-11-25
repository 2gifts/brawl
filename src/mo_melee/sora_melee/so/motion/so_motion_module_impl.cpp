#define SO_MOTION_MODULE_IMPL_UNIT
#define SO_ATTRIBUTE_FLAG_TRIVIAL_DTOR
#define MT_VEC3F_DIV_SCALE
#include <so/motion/so_motion_module_impl.h>
#include <so/so_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_simple.h>
#include <so/posture/so_posture_module_simple.h>

soPartialAnim::soPartialAnim() : m_kind(-1), m_secondaryKind(-1) {}
soPartialAnim::~soPartialAnim() {}
soOtherAnim::soOtherAnim() : m_kind(-1) {}
soOtherAnim::~soOtherAnim() {}
soMotionModule::~soMotionModule() {}

void soMotionModuleImpl::setData(void* data) {
    m_dataAccesser.setData(static_cast<soSet<const soMotionData>*>(data));
}
int soMotionModuleImpl::getKind() { return m_kind; }
float soMotionModuleImpl::getRate() { return m_mainAnim.m_rate; }
void soMotionModuleImpl::setRate(float rate) { m_mainAnim.m_rate = rate; }
void soMotionModuleImpl::setLoopFlag(bool loop) { m_mainAnim.m_loopFlag = loop; }
bool soMotionModuleImpl::isLoopFlag() { return m_mainAnim.m_loopFlag; }
float soMotionModuleImpl::getFrame() { return m_mainAnim.m_animFrame; }
bool soMotionModuleImpl::isEnd() { return m_mainAnim.isEnd(); }
bool soMotionModuleImpl::isLooped() { return m_mainAnim.isLooped(); }
void soMotionModuleImpl::setWeightRate(float rate) { m_weightRate = rate; }
void soMotionModuleImpl::disableNode(int node) { m_mainAnim.disableNode(node); }
float soMotionModuleImpl::getEndFrame() { return m_mainAnim.getEndFrame(m_mainAnim.m_anmChrRes); }
void soMotionEventObserver::notifyEventChangeMotion(int, int, void*, soModuleAccesser*) {}
void soMotionModuleImpl::setShareAnimObjPartialInfo(int kind, int slot) {
    m_shareAnimObjPartialKind = kind;
    m_shareAnimObjPartialSlot = slot;
}
void soMotionModuleImpl::set2ndRate(float rate) { m_secondaryAnim.m_rate = rate; }
float soMotionModuleImpl::get2ndFrame() { return m_secondaryAnim.m_animFrame; }
bool soMotionModuleImpl::is2ndEnd() { return m_secondaryAnim.isEnd(); }
void soMotionModuleImpl::setPartialRate(float rate, int part) { m_partialAnims->at(part).m_anim.m_rate = rate; }
int soMotionModuleImpl::getOtherAnimSize() { return m_otherAnims->size(); }
bool soMotionModuleImpl::isTransMove() { return m_transMove; }
bool soMotionModuleImpl::is2ndTransMove() { return m_secondaryTransMove; }
bool soMotionModuleImpl::isObserv(char event) { return event == 4; }

char* soMotionModuleImpl::getName(int kind, bool useOffset) {
    return m_dataAccesser.get(kind, useOffset).m_name;
}
void soMotionModuleImpl::removeOtherAnim(int part) {
    soOtherAnim& anim = m_otherAnims->at(part);
    anim.m_anim.reset();
    anim.m_kind = -1;
}

void soMotionModuleImpl::setOtherAnimRate(int part, float rate) {
    if (!m_otherAnims->at(part).m_anim.m_emptyFlag)
        m_otherAnims->at(part).m_anim.m_rate = rate;
}
void soMotionModuleImpl::setOtherAnimFrame(float frame, int part) {
    if (!m_otherAnims->at(part).m_anim.m_emptyFlag)
        m_otherAnims->at(part).m_anim.setFrame(frame);
}

void soMotionModuleImpl::setFrame(float frame) {
    m_mainAnim.setFrame(frame);
    if (m_mainAnim.m_anmChrRes)
        m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
    m_otherAnim.setFrame(frame);
}
void soMotionModuleImpl::setPartialFrame(float frame, int part) {
    m_partialAnims->at(part).m_anim.setFrame(frame);
    if (m_partialAnims->at(part).m_anim.m_anmChrRes)
        m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
}


void soMotionModuleImpl::setShareAnimObjKind(int kind) {
    if (kind == -1 && unk357_7 == 1) {
        removePartialAnimChr(m_shareAnimObjPartialSlot);
        unk357_7 = false;
    }
    m_shareAnimObjKind = kind;
}
void soMotionModuleImpl::begin() {
    m_mainAnim.m_startFrame = m_mainAnim.m_animFrame;
    if (m_secondaryKind != -1) {
        m_secondaryAnim.m_startFrame = m_secondaryAnim.m_animFrame;
        m_prevWeight = m_weight;
    }
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        soPartialAnim& anim = m_partialAnims->at(i);
        if (anim.m_kind != -1) anim.m_anim.m_startFrame = anim.m_anim.m_animFrame;
    }
    size = m_otherAnims->size();
    for (int i = 0; i < size; i++) {
        soOtherAnim& anim = m_otherAnims->at(i);
        if (anim.m_kind != -1) anim.m_anim.m_startFrame = anim.m_anim.m_animFrame;
    }
    unk356_4 = false;
    unk357_6 = false;
}
void soMotionModuleImpl::rewind() {
    m_mainAnim.rewind();
    if (m_secondaryKind != -1) {
        m_secondaryAnim.rewind();
        m_saveWeight = m_weight;
        setWeight(m_prevWeight);
    }
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_kind != -1) m_partialAnims->at(i).m_anim.rewind();
    }
    size = m_otherAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_otherAnims->at(i).m_kind != -1) m_otherAnims->at(i).m_anim.rewind();
    }
}
void soMotionModuleImpl::turnBack() {
    m_mainAnim.turnBack();
    if (m_secondaryKind != -1) {
        m_secondaryAnim.turnBack();
        setWeight(m_saveWeight);
    }
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_kind != -1) m_partialAnims->at(i).m_anim.turnBack();
    }
    size = m_otherAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_otherAnims->at(i).m_kind != -1) m_otherAnims->at(i).m_anim.turnBack();
    }
    if (m_mainAnim.m_anmChrRes)
        m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
}
void soMotionModuleImpl::setWeight(float weight) {
    m_weight = weight;
    float secondaryWeight = 1.0f - weight;
    m_anmObjChrBlend->SetWeight(0, weight);
    m_anmObjChrBlend->SetWeight(1, secondaryWeight);
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_secondaryKind != -1) {
            if (m_partialAnims->at(i).unk44 == 1)
                m_anmObjChrBlend->SetWeight(i + 2, 1.0f);
            else
                m_anmObjChrBlend->SetWeight(i + 2, secondaryWeight);
        }
    }
    m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
}
