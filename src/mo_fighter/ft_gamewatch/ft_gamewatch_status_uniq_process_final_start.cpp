#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: camera module (soModuleEnumeration::m_cameraModule); only the slots below are used.
class ftCameraModuleStub {
public:
    virtual ~ftCameraModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual float* unk44(int);
    virtual void unk48();
    virtual void unk4C(float*, int);
    virtual void unk50(float*, int);
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual void unk70();
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80();
    virtual void unk84();
    virtual void unk88();
    virtual void unk8C();
    virtual void unk90();
    virtual void unk94();
    virtual void unk98(float, int);
};

// HYPOTHESIS: posture module (soModuleEnumeration::m_postureModule); only the float getter at slot 0x60 is used.
class ftPostureModuleStub {
public:
    virtual ~ftPostureModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual float getUnk60();
};

class ftGamewatchStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessFinalStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGamewatchStatusUniqProcessFinalStart g_ftGamewatchStatusUniqProcessFinalStart;

static const float s_finalStartScale = 12.0f;
static const float s_finalStartTable[8] = { 12.0f, -30.0f, 20.0f, 50.0f, -50.0f, 30.0f, 0.0f, 3.0f };

void ftGamewatchStatusUniqProcessFinalStart::initStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    ftPostureModuleStub* posture = reinterpret_cast<ftPostureModuleStub*>(modules->m_postureModule);
    soWorkManageModule& workManage = *modules->m_workManageModule;
    workManage.setFloat(s_finalStartScale * posture->getUnk60(), 0x11000012);
}

void ftGamewatchStatusUniqProcessFinalStart::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    const float* t = s_finalStartTable;
    if (kind >= 0x11d && kind <= 0x138) {
        const float* p = reinterpret_cast<ftCameraModuleStub*>(moduleAccesser->m_enumerationStart->m_cameraModule)->unk44(0);
        float d4 = t[1] - p[4];
        float d3 = t[2] - p[3];
        float d2 = t[3] - p[1];
        float d0 = t[4] - p[2];
        float rect[4];
        rect[0] = d0;
        rect[1] = d2;
        rect[2] = d3;
        rect[3] = d4;
        reinterpret_cast<ftCameraModuleStub*>(moduleAccesser->m_enumerationStart->m_cameraModule)->unk50(rect, 0);
        float v1 = t[5] - p[0];
        float edge = t[6];
        float vec[3];
        vec[0] = edge;
        vec[1] = v1;
        vec[2] = edge;
        reinterpret_cast<ftCameraModuleStub*>(moduleAccesser->m_enumerationStart->m_cameraModule)->unk4C(vec, 0);
        reinterpret_cast<ftCameraModuleStub*>(moduleAccesser->m_enumerationStart->m_cameraModule)->unk98(t[7], 0);
    }
}
