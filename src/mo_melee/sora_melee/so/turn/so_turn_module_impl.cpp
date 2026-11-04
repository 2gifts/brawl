#define SO_TURN_MODULE_IMPL_FULL
#include <so/turn/so_turn_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>
#include <types.h>

void soTurnModuleImpl::activate() {
    end();
}

void soTurnModuleImpl::set(const soTurnData* data, bool changeLr, bool isExtern, float lr) {
    m_state = 1;
    m_turnData = data;
    m_changeLr = changeLr;
    unk24 = 0;
    m_lrReversed = false;
    m_lr = lr;
    m_isExtern = isExtern;
}

bool soTurnModuleImpl::isObserv(char group) {
    return group == 0x20;
}

soTurnModule::~soTurnModule() { }

void soTurnModuleImpl::end() {
    m_state = 0;
    m_rotY = unk30 = 0.0f;
    Vec3f rot(0.0f, 0.0f, 0.0f);
    m_moduleAccesser->getPostureModule().setRot(&rot, 0);
    m_isExtern = 0;
}
