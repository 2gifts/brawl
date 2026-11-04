#include <ft/ft_param_customize_module_impl.h>
#include <ft/ft_util.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>

ftParamCustomizeModuleImpl::ftParamCustomizeModuleImpl(soModuleAccesser* accsr) :
    m_moduleAccsr(accsr), m_customizeBits(0), m_walkSpeedMul(1.0f) {
    reconstruct();
}

ftParamCustomizeModuleImpl::~ftParamCustomizeModuleImpl() { }

void ftParamCustomizeModuleImpl::activate() {
    soParamCustomizeModuleImpl::activate();
}

float ftParamCustomizeModuleImpl::getAttackPowerMul(void* p1) {
    return soParamCustomizeModuleImpl::getAttackPowerMul(p1) * ftUtil::getAttackPowerMul(m_moduleAccsr);
}

float ftParamCustomizeModuleImpl::getAttackPowerMulArticle() {
    return soValueAccesser::getConstantFloat(m_moduleAccsr, 0xDAE, 0);
}

float ftParamCustomizeModuleImpl::getAttackReactionMul() {
    return ftUtil::getAttackReactionMul(m_moduleAccsr);
}

void ftParamCustomizeModuleImpl::setDefault() {
    reconstruct();
    m_customizeBits = 0;
    float scale = m_moduleAccsr->getPostureModule().getBaseScale();
    if (1.0f != scale) {
        applyKinoko(scale);
        m_customizeBits |= 1;
    }
}

void ftParamCustomizeModuleImpl::addCustomize(u8 kind) {
    m_customizeBits |= 1 << kind;
    calc();
}

void ftParamCustomizeModuleImpl::removeCustomize(u8 kind) {
    u32 bit = 1 << kind;
    if (m_customizeBits & bit) {
        m_customizeBits &= ~bit;
        calc();
    }
}

void ftParamCustomizeModuleImpl::calc() {
    reconstruct();
    float scale = m_moduleAccsr->getPostureModule().getBaseScale();
    if (m_customizeBits & 1) {
        applyKinoko(scale);
    } else if (m_customizeBits & 2) {
        applyThunder(scale);
    } else if (1.0f != scale) {
        applyKinoko(scale);
        m_customizeBits |= 1;
    }
    if (m_customizeBits & 4) {
        applyRabbitCap();
    }
    if (m_customizeBits & 8) {
        applyMetal();
    }
    m_moduleAccsr->getLinkModule().setNodeScalePowerMul(getAttackPowerMulScale());
}

int ftParamCustomizeModuleImpl::getParamInt(u32 variation, u32 id) {
    return m_paramInt[variation][id - 23001];
}

float ftParamCustomizeModuleImpl::getParamFloat(u32 variation, u32 id) {
    return m_paramFloat[variation][id - 3001];
}

void* ftParamCustomizeModuleImpl::getParamIndefinite(u32 variation, u32 id) {
    return m_paramIndefinite[variation][id - 43001];
}

void ftParamCustomizeModuleImpl::applyRabbitCap() {
    float* scale = g_ftCommonData.dataCommon->rabbitCapScale;
    for (int i = 0; i < 2; i++) {
        m_paramFloat[i][4] *= scale[1];
        m_paramFloat[i][5] *= scale[2];
        m_paramFloat[i][6] *= scale[3];
        m_paramFloat[i][7] *= scale[4];
        m_paramFloat[i][11] *= scale[5];
        m_paramFloat[i][12] *= scale[6];
        m_paramFloat[i][15] *= scale[7];
        m_paramFloat[i][14] *= scale[8];
        m_paramFloat[i][22] *= scale[9];
        m_paramFloat[i][23] *= scale[10];
        m_paramFloat[i][30] *= scale[11];
        m_paramFloat[i][56] *= scale[12];
        m_paramFloat[i][57] *= scale[13];
        m_paramFloat[i][81] *= scale[14];
        m_paramFloat[i][82] *= scale[15];
    }
    m_walkSpeedMul = scale[0] * getWalkSpeedMul();
}

void ftParamCustomizeModuleImpl::applyMetal() {
    float* scale = g_ftCommonData.dataCommon->metalScale;
    for (int i = 0; i < 2; i++) {
        m_paramFloat[i][12] *= scale[1];
        m_paramFloat[i][15] *= scale[2];
        m_paramFloat[i][22] *= scale[3];
        m_paramFloat[i][23] *= scale[4];
        m_paramFloat[i][30] *= scale[5];
        m_paramFloat[i][40] *= scale[6];
        m_paramFloat[i][57] *= scale[7];
        m_paramFloat[i][82] *= scale[8];
    }
    m_walkSpeedMul = scale[0] * getWalkSpeedMul();
}
