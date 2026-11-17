#include <so/jostle/so_jostle.h>
#include <types.h>

class soModuleAccesser;
struct soStatusData;

// HYPOTHESIS: only the members the accessors below use are named; the bases (soJostleModule at 0x0, the status event
// observer at 0xC) are not declared here.
class soJostleModuleImpl {
    char m_base[0x18];
    soModuleAccesser* m_moduleAccesser;
    soJostle m_jostle;
    s16 m_0x44;
    s8 m_layer;
    u8 m_status;
    u8 m_fix;
    u8 m_refer;
    u8 m_sleep;
    u8 m_ignoreSpeedX;

public:
    float getSpeedX();
    float getPosZ();
    void setPushSpeedX(float speed);
    void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    void setForceGap(u8 gap);
    void setJostleData(s32 data);
    void setPriority(u8 priority);
    void setRefer(u8 refer);
    void sleep(u8 sleep);
    void setIgnoreSpeedX(u8 ignore);
    void setFix(u8 fix);
    void setLayer(s8 layer);
    void setStatus(u8 status);
};

float soJostleModuleImpl::getSpeedX() {
    if (m_fix == 0 && m_ignoreSpeedX == 0) {
        return m_jostle.m_0x8;
    }
    return 0.0f;
}

float soJostleModuleImpl::getPosZ() {
    if (m_fix == 0) {
        return m_jostle.m_0x10;
    }
    return 0.0f;
}

void soJostleModuleImpl::setPushSpeedX(float speed) {
    m_jostle.m_0xC = speed;
}

void soJostleModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    m_status = (*(u32*)((char*)statusData + 0xC) >> 1) & 1;
}

void soJostleModuleImpl::setForceGap(u8 gap) {
    m_jostle.m_0x1A = gap;
}

void soJostleModuleImpl::setJostleData(s32 data) {
    m_jostle.m_0x4 = data;
}

void soJostleModuleImpl::setPriority(u8 priority) {
    m_jostle.setPriority(priority);
}

void soJostleModuleImpl::setRefer(u8 refer) {
    m_refer = refer;
}

void soJostleModuleImpl::sleep(u8 sleep) {
    m_sleep = sleep;
}

void soJostleModuleImpl::setIgnoreSpeedX(u8 ignore) {
    m_ignoreSpeedX = ignore;
}

void soJostleModuleImpl::setFix(u8 fix) {
    m_fix = fix;
}

void soJostleModuleImpl::setLayer(s8 layer) {
    m_layer = layer;
}

void soJostleModuleImpl::setStatus(u8 status) {
    m_status = status;
}
