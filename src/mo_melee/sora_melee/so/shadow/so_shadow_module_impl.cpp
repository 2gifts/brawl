#include <so/so_module_accesser.h>
#include <so/shadow/so_shadow_module_impl.h>
#include <st/stage.h>
#include <types.h>
#include <gf/gf_application.h>
#include <st/st_shadow.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale);

soShadowModuleImpl::soShadowModuleImpl(soModuleAccesser* moduleAccesser, float size, int node) :
    soStatusEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    soAnimCmdEventObserver(5, static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId()),
    soModelEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1) {
    m_moduleAccesser = moduleAccesser;
    _0x2C = size;
    _0x30 = 4.0f;
    _0x34 = 100.0f;
    _0x38 = 0.0f;
    _0x3C = 1.5f;
    _0x40 = node;
    m_active = true;
    m_activeStatus = false;
    m_groundRef = false;
    m_groundPokemon = false;
    m_ignoreMode = 3;
    m_dispProjection = false;
}

soShadowModuleImpl::~soShadowModuleImpl() { }

void soShadowModuleImpl::activate() {
    m_activeStatus = true;
    m_dispProjection = false;
}

void soShadowModuleImpl::deactivate() {
    m_activeStatus = false;
}

void soShadowModuleImpl::setIgnoreMode(int mode) {
    m_ignoreMode = mode;
}

bool soShadowModuleImpl::isObserv(char group) {
    return group == 0x19;
}

void soShadowModuleImpl::setActiveStatus(int status) {
    m_activeStatus = status;
}

void soShadowModuleImpl::setSize(float size) {
    _0x2C = size;
}

void soShadowModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                 soModuleAccesser* moduleAccesser) {
    setActiveStatus(true);
}

void soShadowModuleImpl::notifyEventDestructInstance(soModuleAccesser* moduleAccesser) {
    stShadow* shadow = g_Stage->m_shadow;
    shadow->eraseShadowObject(moduleAccesser->getModelModule().getScnMdl(true));
}

bool soShadowModuleImpl::isDispProjection() {
    return m_dispProjection;
}

void soShadowModuleImpl::setGroundPokemon(int groundPokemon) {
    m_groundPokemon = groundPokemon;
}

void soShadowModuleImpl::setSizeMul(float sizeMul) {
    _0x3C = sizeMul;
}

void soShadowModuleImpl::setGroundRef(int groundRef) {
    m_groundRef = groundRef;
}

void soShadowModuleImpl::setYOffset(float yOffset) {
    _0x38 = yOffset;
}

void soShadowModuleImpl::setSizeMinMax(float sizeMin, float sizeMax) {
    _0x30 = sizeMin;
    _0x34 = sizeMax;
}

void soShadowModuleImpl::setNode(int node) {
    _0x40 = node;
}

void soShadowModuleImpl::setActive(bool active) {
    m_active = active;
}

void soShadowModuleImpl::update() {
    m_dispProjection = false;
    if (m_activeStatus != 1) {
        return;
    }
    if (m_active != 1) {
        return;
    }
    if (m_moduleAccesser->getVisibilityModule().isVisible() != 1) {
        return;
    }
    int mode = getMode();
    if (m_ignoreMode != -1 && m_ignoreMode == mode) {
        return;
    }
    if (mode == 0 && g_gfApplication->m_skipLowEffect == 1) {
        return;
    }
    Matrix matrix(true);
    m_moduleAccesser->getModelModule().getNodeGlobalMtx(_0x40, &matrix, false);
    Vec3f position = matrix.getTranslation();
    Vec3f scale;
    getScalingFactor(&matrix, &scale);
    float sizeScale = 0.5f * (scale.m_x + scale.m_z);
    u32 shadowObject = 0;
    float size = 1.0f;
    switch (mode) {
    case 0:
        shadowObject = (u32)m_moduleAccesser->getModelModule().getScnMdl(true);
        m_dispProjection = true;
        break;
    case 1:
        if (m_groundRef == 1) {
            size = _0x3C * m_moduleAccesser->getGroundModule().getWidth(0);
            float minimum = _0x30 * sizeScale;
            if (size < minimum) size = minimum;
            float maximum = _0x34 * sizeScale;
            if (size > maximum) size = maximum;
        } else {
            size = _0x2C;
            if (size < _0x30) size = _0x30;
            if (size > _0x34) size = _0x34;
            size *= sizeScale;
        }
        break;
    }
    float yOffset = _0x38 * sizeScale;
    if (size > 0.0f) {
        soGroundModule& ground = m_moduleAccesser->getGroundModule();
        bool groundPokemon = m_groundPokemon == 1;
        Vec3f shadowPosition(position);
        stShadow* shadow = g_Stage->m_shadow;
        shadow->addShadowObject(shadowObject, &shadowPosition, size,
            (u32)ground.getGrCollision(), yOffset, groundPokemon);
    }
}
