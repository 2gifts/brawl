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

void ftParamCustomizeModuleImpl::applyThunder(float factor) {
    float* scale = g_ftCommonData.dataCommon->sizeCustomizeScale[1];
    for (int variation = 0; variation < 2; variation++) {
        {
            float& parameter = m_paramFloat[variation][4];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[4]);
        }
        {
            float& parameter = m_paramFloat[variation][7];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[5]);
        }
        {
            s32& parameter = m_paramInt[variation][1];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[6]);
        }
        {
            float& parameter = m_paramFloat[variation][12];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[7]);
        }
        {
            float& parameter = m_paramFloat[variation][15];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[8]);
        }
        {
            float& parameter = m_paramFloat[variation][22];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[9]);
        }
        {
            float& parameter = m_paramFloat[variation][23];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[10]);
        }
        {
            float& parameter = m_paramFloat[variation][26];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[11]);
        }
        {
            float& parameter = m_paramFloat[variation][27];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[12]);
        }
        {
            float& parameter = m_paramFloat[variation][28];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[13]);
        }
        {
            float& parameter = m_paramFloat[variation][30];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[14]);
        }
        {
            float& parameter = m_paramFloat[variation][40];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[15]);
        }
        {
            float& parameter = m_paramFloat[variation][46];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[16]);
        }
        {
            float& parameter = m_paramFloat[variation][56];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[17]);
        }
        {
            float& parameter = m_paramFloat[variation][57];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[18]);
        }
        {
            float& parameter = m_paramFloat[variation][64];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[19]);
        }
        {
            float& parameter = m_paramFloat[variation][65];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[20]);
        }
        {
            float& parameter = m_paramFloat[variation][94];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[21]);
        }
        {
            s32& parameter = m_paramInt[variation][11];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[22]);
        }
        {
            float& parameter = m_paramFloat[variation][72];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[23]);
        }
        {
            float& parameter = m_paramFloat[variation][73];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[24]);
        }
        {
            float& parameter = m_paramFloat[variation][74];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[25]);
        }
        {
            float& parameter = m_paramFloat[variation][75];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[26]);
        }
        {
            float& parameter = m_paramFloat[variation][76];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[27]);
        }
        {
            float& parameter = m_paramFloat[variation][77];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[28]);
        }
        {
            float& parameter = m_paramFloat[variation][78];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[29]);
        }
        {
            float& parameter = m_paramFloat[variation][110];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[30]);
        }
        {
            float& parameter = m_paramFloat[variation][111];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[31]);
        }
    }
    setAttackPowerMulScale(ftUtil::calcScalingParam(1.0f, factor, scale[1]));
}

void ftParamCustomizeModuleImpl::applyKinoko(float factor) {
    float* scale = g_ftCommonData.dataCommon->sizeCustomizeScale[0];
    for (int variation = 0; variation < 2; variation++) {
        {
            float& parameter = m_paramFloat[variation][4];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[4]);
        }
        {
            float& parameter = m_paramFloat[variation][7];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[5]);
        }
        {
            s32& parameter = m_paramInt[variation][1];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[6]);
        }
        {
            float& parameter = m_paramFloat[variation][12];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[7]);
        }
        {
            float& parameter = m_paramFloat[variation][15];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[8]);
        }
        {
            float& parameter = m_paramFloat[variation][22];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[9]);
        }
        {
            float& parameter = m_paramFloat[variation][23];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[10]);
        }
        {
            float& parameter = m_paramFloat[variation][26];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[11]);
        }
        {
            float& parameter = m_paramFloat[variation][27];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[12]);
        }
        {
            float& parameter = m_paramFloat[variation][28];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[13]);
        }
        {
            float& parameter = m_paramFloat[variation][30];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[14]);
        }
        {
            float& parameter = m_paramFloat[variation][40];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[15]);
        }
        {
            float& parameter = m_paramFloat[variation][46];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[16]);
        }
        {
            float& parameter = m_paramFloat[variation][56];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[17]);
        }
        {
            float& parameter = m_paramFloat[variation][57];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[18]);
        }
        {
            float& parameter = m_paramFloat[variation][64];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[19]);
        }
        {
            float& parameter = m_paramFloat[variation][65];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[20]);
        }
        {
            float& parameter = m_paramFloat[variation][94];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[21]);
        }
        {
            s32& parameter = m_paramInt[variation][11];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[22]);
        }
        {
            float& parameter = m_paramFloat[variation][72];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[23]);
        }
        {
            float& parameter = m_paramFloat[variation][73];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[24]);
        }
        {
            float& parameter = m_paramFloat[variation][74];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[25]);
        }
        {
            float& parameter = m_paramFloat[variation][75];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[26]);
        }
        {
            float& parameter = m_paramFloat[variation][76];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[27]);
        }
        {
            float& parameter = m_paramFloat[variation][77];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[28]);
        }
        {
            float& parameter = m_paramFloat[variation][78];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[29]);
        }
        {
            float& parameter = m_paramFloat[variation][110];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[30]);
        }
        {
            float& parameter = m_paramFloat[variation][111];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[31]);
        }
    }
    setAttackPowerMulScale(ftUtil::calcScalingParam(1.0f, factor, scale[1]));
    float walkSpeedScale = ftUtil::calcScalingParam(1.0f, factor, scale[3]);
    float walkSpeed = getWalkSpeedMul();
    m_walkSpeedMul = walkSpeed * walkSpeedScale;
}

// TODO: clean up, give variables real names
namespace jj_14B700 {
struct KindObj {
    unsigned char pad[0x3C];
    virtual void u0();
    virtual void u1();
    virtual void u2();
    virtual void u3();
    virtual void u4();
    virtual void u5();
    virtual void u6();
    virtual void u7();
    virtual void u8();
    virtual void u9();
    virtual void u10();
    virtual void u11();
    virtual void u12();
    virtual void u13();
    virtual void u14();
    virtual void u15();
    virtual void u16();
    virtual void u17();
    virtual void u18();
    virtual void u19();
    virtual void u20();
    virtual void u21();
    virtual void u22();
    virtual void u23();
    virtual void u24();
    virtual void u25();
    virtual void u26();
    virtual void u27();
    virtual void u28();
    virtual void u29();
    virtual void u30();
    virtual void u31();
    virtual void u32();
    virtual void u33();
    virtual void u34();
    virtual void u35();
    virtual void u36();
    virtual void u37();
    virtual void u38();
    virtual void u39();
    virtual int getKind();
};
struct Target {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10(float);
};
struct FOut { unsigned char pad[0x18]; float f[114]; };
struct IOut { unsigned char pad[0x3A8]; int i[27]; };
struct POut { unsigned char pad[0x480]; int p[23]; };
struct Data {
    int a0;
    int a4;
    int a8;
    int ac;
    int a10;
    int a14;
    int a18;
    int a1c;
    int a20;
    int a24;
    int a28;
    int a2c;
    int a30;
    int a34;
    int a38;
    int a3c;
    int a40;
    int a44;
    int a48;
};
struct ParamData {
    float w0;
    float w1;
    float w2;
    float w3;
    float w4;
    float w5;
    float w6;
    float w7;
    float w8;
    float w9;
    int w10;
    float w11;
    int w12;
    float w13;
    float w14;
    float w15;
    float w16;
    float w17;
    float w18;
    float w19;
    float w20;
    float w21;
    float w22;
    float w23;
    int w24;
    float w25;
    float w26;
    float w27;
    float w28;
    float w29;
    float w30;
    float w31;
    float w32;
    float w33;
    float w34;
    int w35;
    float w36;
    float w37;
    float w38;
    float w39;
    float w40;
    float w41;
    float w42;
    float w43;
    float w44;
    float w45;
    float w46;
    float w47;
    float w48;
    float w49;
    float w50;
    float w51;
    float w52;
    float w53;
    float w54;
    float w55;
    float w56;
    float w57;
    int w58;
    int w59;
    float w60;
    float w61;
    float w62;
    float w63;
    float w64;
    float w65;
    float w66;
    float w67;
    float w68;
    float w69;
    float w70;
    float w71;
    float w72;
    float w73;
    float w74;
    float w75;
    float w76;
    float w77;
    float w78;
    float w79;
    float w80;
    float w81;
    float w82;
    float w83;
    float w84;
    int w85;
    int w86;
    float w87;
    float w88;
    float w89;
    float w90;
    float w91;
    float w92;
    int w93;
    float w94;
    float w95;
    int w96;
    int w97;
    float w98;
    float w99;
    float w100;
    float w101;
    float w102;
    float w103;
    float w104;
    float w105;
    int w106;
    float w107;
    float w108;
    float w109;
    float w110;
    float w111;
    float w112;
    float w113;
    float w114;
    int w115;
    float w116;
    float w117;
    float w118;
    float w119;
    int w120;
    float w121;
    float w122;
    float w123;
    float w124;
    float w125;
    float w126;
    float w127;
    float w128;
    float w129;
    float w130;
    float w131;
    float w132;
    float w133;
    float w134;
    float w135;
    float w136;
    float w137;
    float w138;
    float w139;
    float w140;
    float w141;
    float w142;
    float w143;
    float w144;
    float w145;
    float w146;
    float w147;
    float w148;
    float w149;
    float w150;
    float w151;
    float w152;
    float w153;
    float w154;
    float w155;
    float w156;
    float w157;
    float w158;
    float w159;
    float w160;
    float w161;
    float w162;
    int w163;
    int w164;
    int w165;
    float w166;
    int w167;
    float w168;
    float w169;
    float w170;
    float w171;
    float w172;
    float w173;
    float w174;
    float w175;
    int w176;
    int w177;
    int w178;
    int w179;
    int w180;
    int w181;
    int w182;
    int w183;
    int w184;
};
extern "C" {
extern unsigned char g_ftCommonDataAccesser[];
Data* getParamEtc__20ftCommonDataAccesserCF6ftKind(void*, int);
ParamData* getParam__20ftCommonDataAccesserCF6ftKindi(void*, int, int);
extern const float lbl_27_rodata_18D0;
void fn_27_14B700(Target* obj) {
    FOut* fo;
    IOut* io;
    POut* po;
    void* P = *(void**)((char*)obj + 0x14);
    KindObj* ko = *(KindObj**)((char*)P + 8);
    int kind = ko->getKind();
    Data* data = getParamEtc__20ftCommonDataAccesserCF6ftKind(&g_ftCommonDataAccesser, kind);
    fo = (FOut*)obj;
    io = (IOut*)obj;
    po = (POut*)obj;
    int i = 0;
    do {
        ParamData* s = getParam__20ftCommonDataAccesserCF6ftKindi(&g_ftCommonDataAccesser, kind, i);
        fo->f[0]=s->w0;
        fo->f[1]=s->w1;
        fo->f[2]=s->w2;
        fo->f[3]=s->w3;
        fo->f[4]=s->w4;
        fo->f[5]=s->w5;
        fo->f[6]=s->w6;
        fo->f[7]=s->w7;
        fo->f[8]=s->w8;
        fo->f[9]=s->w9;
        fo->f[10]=s->w11;
        fo->f[11]=s->w13;
        fo->f[12]=s->w14;
        fo->f[13]=s->w15;
        fo->f[14]=s->w16;
        fo->f[15]=s->w17;
        fo->f[16]=s->w18;
        fo->f[17]=s->w19;
        fo->f[18]=s->w20;
        fo->f[19]=s->w21;
        fo->f[20]=s->w22;
        fo->f[21]=s->w23;
        fo->f[22]=s->w25;
        fo->f[23]=s->w26;
        fo->f[24]=s->w27;
        fo->f[25]=s->w28;
        fo->f[26]=s->w29;
        fo->f[27]=s->w30;
        fo->f[28]=s->w31;
        fo->f[29]=s->w32;
        fo->f[30]=s->w33;
        fo->f[31]=s->w34;
        fo->f[32]=s->w36;
        fo->f[33]=s->w37;
        fo->f[34]=s->w38;
        fo->f[35]=s->w39;
        fo->f[36]=s->w40;
        fo->f[37]=s->w41;
        fo->f[38]=s->w42;
        fo->f[39]=s->w43;
        fo->f[40]=s->w44;
        fo->f[41]=s->w45;
        fo->f[42]=s->w46;
        fo->f[43]=s->w47;
        fo->f[44]=s->w48;
        fo->f[45]=s->w49;
        fo->f[46]=s->w50;
        fo->f[47]=s->w51;
        fo->f[48]=s->w52;
        fo->f[49]=s->w53;
        fo->f[50]=s->w54;
        fo->f[51]=s->w55;
        fo->f[52]=s->w56;
        fo->f[53]=s->w57;
        fo->f[54]=s->w60;
        fo->f[55]=s->w61;
        fo->f[56]=s->w62;
        fo->f[57]=s->w63;
        fo->f[58]=s->w64;
        fo->f[59]=s->w65;
        fo->f[60]=s->w66;
        fo->f[61]=s->w67;
        fo->f[62]=s->w68;
        fo->f[63]=s->w69;
        fo->f[64]=s->w70;
        fo->f[65]=s->w71;
        fo->f[66]=s->w72;
        fo->f[67]=s->w73;
        fo->f[68]=s->w74;
        fo->f[69]=s->w75;
        fo->f[70]=s->w76;
        fo->f[71]=s->w77;
        fo->f[72]=s->w78;
        fo->f[73]=s->w79;
        fo->f[74]=s->w80;
        fo->f[75]=s->w81;
        fo->f[76]=s->w82;
        fo->f[77]=s->w83;
        fo->f[78]=s->w84;
        fo->f[79]=s->w87;
        fo->f[80]=s->w88;
        fo->f[81]=s->w89;
        fo->f[82]=s->w90;
        fo->f[83]=s->w91;
        fo->f[84]=s->w92;
        fo->f[85]=s->w94;
        fo->f[86]=s->w95;
        fo->f[87]=s->w98;
        fo->f[88]=s->w99;
        fo->f[89]=s->w100;
        fo->f[90]=s->w101;
        fo->f[91]=s->w102;
        fo->f[92]=s->w103;
        fo->f[93]=s->w104;
        fo->f[94]=s->w105;
        fo->f[95]=s->w107;
        fo->f[96]=s->w108;
        fo->f[97]=s->w109;
        fo->f[98]=s->w110;
        fo->f[99]=s->w111;
        fo->f[100]=s->w112;
        fo->f[101]=s->w113;
        fo->f[102]=s->w114;
        fo->f[103]=s->w116;
        fo->f[104]=s->w117;
        fo->f[105]=s->w118;
        fo->f[106]=s->w119;
        fo->f[107]=s->w121;
        fo->f[108]=s->w122;
        fo->f[109]=s->w123;
        fo->f[110]=s->w124;
        fo->f[111]=s->w125;
        fo->f[112]=s->w162;
        fo->f[113]=s->w166;
        io->i[0]=s->w10;
        io->i[1]=s->w12;
        io->i[2]=s->w24;
        io->i[3]=s->w35;
        io->i[4]=s->w58;
        io->i[5]=s->w59;
        io->i[6]=s->w85;
        io->i[7]=s->w86;
        io->i[8]=s->w93;
        io->i[9]=s->w96;
        io->i[10]=s->w97;
        io->i[11]=s->w106;
        io->i[12]=s->w115;
        io->i[13]=s->w120;
        io->i[14]=s->w163;
        io->i[15]=s->w164;
        io->i[16]=s->w165;
        io->i[17]=s->w167;
        io->i[18]=s->w176;
        io->i[19]=s->w177;
        io->i[20]=s->w178;
        io->i[21]=s->w179;
        io->i[22]=s->w180;
        io->i[23]=s->w181;
        io->i[24]=s->w182;
        io->i[25]=s->w183;
        io->i[26]=s->w184;
        po->p[0]=(int)&s->w126;
        po->p[1]=(int)&s->w131;
        po->p[2]=(int)&s->w136;
        po->p[3]=(int)&s->w141;
        po->p[4]=(int)&s->w146;
        po->p[5]=(int)&s->w151;
        po->p[6]=(int)&s->w159;
        po->p[7]=(int)&s->w168;
        po->p[8]=data->a0;
        po->p[9]=(int)&data->a4;
        po->p[10]=(int)&data->ac;
        po->p[11]=(int)&data->a14;
        i++;
        fo = (FOut*)((char*)fo + 0x1C8);
        po->p[12]=(int)&data->a1c;
        io = (IOut*)((char*)io + 0x6C);
        po->p[13]=data->a24;
        po->p[14]=data->a28;
        po->p[15]=data->a2c;
        po->p[16]=data->a30;
        po->p[17]=data->a34;
        po->p[18]=data->a38;
        po->p[19]=data->a3c;
        po->p[20]=data->a40;
        po->p[21]=data->a44;
        po->p[22]=data->a48;
        po = (POut*)((char*)po + 0x5C);
    } while (i < 2);
    obj->v10(lbl_27_rodata_18D0);
    *(float*)((char*)obj + 0x690) = lbl_27_rodata_18D0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_14CCAC {
extern "C" {
    extern const float lbl_27_rodata_18E0;
}

extern "C" float fn_27_14CCAC(void) {
    return lbl_27_rodata_18E0;
}
}

// TODO: clean up, give variables real names
namespace jj_14CCE0 {
extern "C" void fn_27_14CCE0(void) {}
}

// TODO: clean up, give variables real names
namespace jj_14CCE4 {
extern "C" void fn_27_14CCE4(void) {}
}

// TODO: clean up, give variables real names
namespace jj_14CCE8 {
extern "C" void fn_27_14CCE8(void) {}
}

