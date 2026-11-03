#include <so/collision/so_collision.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <so/collision/so_collision_shield_part.h>
#include <so/so_module_accesser.h>
#include <types.h>

soCollisionShieldModule::~soCollisionShieldModule() { }

void soCollisionShieldModuleImpl::clean() {
    for (int i = 0; i < m_collisionShieldGroupArray->size(); i++) {
        m_collisionShieldGroupArray->at(i).init();
    }
    _24 = 0;
    m_collision.clean();
}

void soCollisionShieldModuleImpl::deactivate() {
    if (_52 == 0) {
        clean();
    } else {
        m_collision.initLogAll();
    }
    m_collision.m_taskId = -1;
}

void soCollisionShieldModuleImpl::add(soCollisionShieldGroupData* data, u32 collisionShieldGroupIndex) {
    _24 += m_collisionShieldGroupArray->at(collisionShieldGroupIndex).add(&m_collision, m_partArray, data, collisionShieldGroupIndex, _24);
}

void soCollisionShieldModuleImpl::add(soCollisionReflectorGroupData* data, u32 collisionShieldGroupIndex) {
    _24 += m_collisionShieldGroupArray->at(collisionShieldGroupIndex).add(&m_collision, m_partArray, data, collisionShieldGroupIndex, _24);
}

void soCollisionShieldModuleImpl::update(double posX, double scale, double lr, u32 collisionShieldGroupIndex) {
    soModelModule& model = m_moduleAccesser->getModelModule();
    soCollisionShieldGroup& group = m_collisionShieldGroupArray->at(collisionShieldGroupIndex);
    if (group.update(&m_collision, m_partArray, &model, posX, scale, lr) == true && _50 == 0) {
        m_collision.addCheckList();
        _50 = 1;
    }
}

void soCollisionShieldModuleImpl::update(double posX, double scale, double lr) {
    int num = getGroupNum();
    for (int i = 0; i < num; i++) {
        update(posX, scale, lr, i);
    }
}

void soCollisionShieldModuleImpl::begin() {
    _50 = 0;
}

void soCollisionShieldModuleImpl::set(u32 index, soCollisionShieldData* data, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).set(m_partArray, index, data);
}

void soCollisionShieldModuleImpl::setReflectorData(soCollisionReflectorData* data, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).setReflectorData(data);
}

bool soCollisionShieldModuleImpl::isShield(u32 index, u32 collisionShieldGroupIndex) {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).isShield(m_partArray, index);
}

void soCollisionShieldModuleImpl::setStatus(u32 index, u32 status, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).setStatus(m_partArray, index, status);
}

void soCollisionShieldModuleImpl::setStatusAll(u32 status, u32 collisionShieldGroupIndex) {
    for (int i = 0; i < getPartSize(collisionShieldGroupIndex); i++) {
        setStatus(i, status, collisionShieldGroupIndex);
    }
}

u8 soCollisionShieldModuleImpl::isTurn(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isTurn;
}

u8 soCollisionShieldModuleImpl::isFront(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isFront;
}

bool soCollisionShieldModuleImpl::isHop(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isHop;
}

float soCollisionShieldModuleImpl::getHopAngle(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_hopAngle;
}

bool soCollisionShieldModuleImpl::isNoHop(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isNoHop;
}

void soCollisionShieldModuleImpl::setTurn(bool isTurn, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isTurn = isTurn;
}

void soCollisionShieldModuleImpl::setFront(bool isFront, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isFront = isFront;
}

void soCollisionShieldModuleImpl::setHop(float hopAngle, bool isHop, u32 collisionShieldGroupIndex) {
    soCollisionShieldGroup& group = m_collisionShieldGroupArray->at(collisionShieldGroupIndex);
    group.m_isHop = isHop;
    group.m_hopAngle = hopAngle;
}

double soCollisionShieldModuleImpl::getAttackMul(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_attackMul;
}

double soCollisionShieldModuleImpl::getSpeedMul(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_speedMul;
}

double soCollisionShieldModuleImpl::getLifeMul(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_lifeMul;
}

float soCollisionShieldModuleImpl::getAttackLimit(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_attackLimit;
}

u8 soCollisionShieldModuleImpl::isNoMBall(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isNoMBall;
}

int soCollisionShieldModuleImpl::getPartSize(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_partSize;
}

void soCollisionShieldModuleImpl::clearAll() {
    int num = m_collisionShieldGroupArray->size();
    for (int i = 0; i < num; i++) {
        m_collisionShieldGroupArray->at(i).clearAll(m_partArray);
    }
}

void soCollisionShieldModuleImpl::clearAll(u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).clearAll(m_partArray);
}

void soCollisionShieldModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    int num = m_partArray->size();
    for (int i = 0; i < num; i++) {
        if (m_partArray->at(i).m_isActive == 1) {
            m_partArray->at(i).m_isActive = 0;
        }
    }
}

bool soCollisionShieldModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

u32 soCollisionShieldModuleImpl::getGroupNum() {
    return m_collisionShieldGroupArray->size();
}

double soCollisionShieldModuleImpl::getPosX(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_posX;
}

void soCollisionShieldModuleImpl::setTargetProperty(u32 property, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).setTargetProperty(m_partArray, property);
}
