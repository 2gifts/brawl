#define SO_MOTION_MODULE_IMPL_UNIT
#include <so/motion/so_motion_module_impl.h>

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
