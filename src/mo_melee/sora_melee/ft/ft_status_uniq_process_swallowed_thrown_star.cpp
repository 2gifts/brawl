#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <so/status/so_status_module_impl.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule, +0x54)
class ftLinkModuleView {
public:
    virtual ~ftLinkModuleView() { }
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
    virtual void unk98();
    virtual void unk9C();
    virtual void unkA0();
    virtual void unkA4();
    virtual float unkA8(int);
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void unkBC();
    virtual void unkC0();
    virtual void unkC4();
    virtual void unkC8();
    virtual void unkCC();
    virtual void unkD0();
    virtual void unkD4();
    virtual void unkD8();
    virtual void unkDC();
    virtual void unkE0();
    virtual void unkE4();
    virtual void unkE8();
    virtual void unkEC();
    virtual void unkF0();
    virtual void unkF4();
    virtual void unkF8();
    virtual void unkFC();
    virtual void unk100();
    virtual void unk104();
    virtual void unk108();
    virtual void unk10C();
    virtual void unk110();
    virtual void unk114();
    virtual void unk118();
    virtual void unk11C();
    virtual void unk120();
    virtual void unk124();
    virtual void unk128();
    virtual void unk12C();
    virtual void unk130();
    virtual void unk134();
    virtual void unk138();
    virtual void unk13C();
    virtual void unk140();
    virtual void unk144();
    virtual void unk148();
    virtual void unk14C();
    virtual void unk150();
    virtual void unk154();
    virtual void unk158();
    virtual void unk15C(Vec3f* first, Vec3f* second);
};
// HYPOTHESIS: controller module (soModuleEnumeration::m_controllerModule, +0x5C)
class ftControllerModuleView {
public:
    virtual ~ftControllerModuleView() { }
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
    virtual void unk98();
    virtual void unk9C();
    virtual void unkA0();
    virtual void unkA4();
    virtual void unkA8();
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void unkBC();
    virtual void unkC0();
    virtual void unkC4();
    virtual void unkC8();
    virtual void unkCC();
    virtual void unkD0();
    virtual float unkD4(int);
};

// HYPOTHESIS: work words (0xFF, 0, 0xFF, 1) stored ahead of the status vtable (+0x1C), as in ft_status_uniq_process_swallowed_thrown.cpp
class ftStatusUniqProcessSwallowedThrownStarData {
public:
    s32 m_unk00;
    s32 m_unk04;
    s32 m_unk08;
    s32 m_unk0C;
    u8 m_unk10[0xC];
};

class ftStatusUniqProcessSwallowedThrownStar : public ftStatusUniqProcessSwallowedThrownStarData, public soStatusUniqProcess {
public:
    ftStatusUniqProcessSwallowedThrownStar() {
        m_unk00 = 0xff;
        m_unk04 = 0;
        m_unk08 = 0xff;
        m_unk0C = 1;
    }
    virtual ~ftStatusUniqProcessSwallowedThrownStar() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessSwallowedThrownStar g_ftStatusUniqProcessSwallowedThrownStar;

void ftStatusUniqProcessSwallowedThrownStar::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessSwallowedThrownStar::execStatus(soModuleAccesser* moduleAccesser) {
    struct {
        Vec3f m_second;
        Vec3f m_first;
    } zeros;
    zeros.m_second.m_x = 0.0f;
    zeros.m_second.m_y = 0.0f;
    zeros.m_second.m_z = 0.0f;
    zeros.m_first.m_x = 0.0f;
    zeros.m_first.m_y = 0.0f;
    zeros.m_first.m_z = 0.0f;
    reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk15C(&zeros.m_first, &zeros.m_second);
    // HYPOTHESIS: threshold constant lbl_27_rodata_24A8 (value taken as 0.0f)
    if (reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->unkD4(0) <= 0.0f) {
        moduleAccesser->getStatusModule().changeStatusRequest(0xd4, moduleAccesser);
    }
}

void ftStatusUniqProcessSwallowedThrownStar::exitStatus(soModuleAccesser* moduleAccesser, int status) {
}

bool ftStatusUniqProcessSwallowedThrownStar::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return true;
}

