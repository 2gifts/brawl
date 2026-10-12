#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// HYPOTHESIS: slot names are unverified; argument types inferred from register use. Collision hit module view (local). Slots: 0x3C (void*, u32), 0x44 (u32 x3), 0x50 (int, u32).
class ftCollisionHitModuleSlotView {
public:
    virtual ~ftCollisionHitModuleSlotView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void slotHit3c(void* value, u32 group);
    virtual void pad16();
    virtual void slotHit44(u32 a, u32 b, u32 c);
    virtual void pad18();
    virtual void pad19();
    virtual void slotHit50(int value, u32 group);
};

// HYPOTHESIS: slotGround54 is unnamed; the ground-module identity comes from enumeration slot +0x10. Ground module view (local). The module's vptr sits at +0x8, so the view derives from an 8-byte padding struct.
struct ftGroundModulePadding {
    u32 unk0;
    u32 unk4;
};

class ftGroundModuleSlotView : public ftGroundModulePadding {
public:
    virtual ~ftGroundModuleSlotView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void pad15();
    virtual void pad16();
    virtual void pad17();
    virtual void slotGround54(u32 a, u32 b);
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureDamageYoshi : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureDamageYoshi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureDamageYoshi g_ftStatusUniqProcessCaptureDamageYoshi;

void ftStatusUniqProcessCaptureDamageYoshi::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCollisionHitModuleSlotView*>(moduleAccesser->m_enumerationStart->m_collisionHitModule)->slotHit50(3, 0);
    reinterpret_cast<ftCollisionHitModuleSlotView*>(moduleAccesser->m_enumerationStart->m_collisionHitModule)->slotHit44(0, 0, 0);
    reinterpret_cast<ftGroundModuleSlotView*>(moduleAccesser->m_enumerationStart->m_groundModule)->slotGround54(0, 0);
}
void ftStatusUniqProcessCaptureDamageYoshi::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCaptureDamageYoshi::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}
bool ftStatusUniqProcessCaptureDamageYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(0x109);
}

// HYPOTHESIS: shared parameter table and getter, same as the Snake grenade status (names unverified).
extern u8 lbl_27_bss_7B0C[];
extern "C" int fn_27_30FA2C(void* table, int kind, int id, soModuleAccesser* acc, int);

float soModuleAccesser::getConstantFloatKirby(s32 id) {
    return soValueAccesser::getConstantFloat(this, fn_27_30FA2C(lbl_27_bss_7B0C, 1, id, this, 0), 0);
}


// HYPOTHESIS: shared parameter table and getter, same as the Snake grenade status (names unverified).
extern u8 lbl_27_bss_7B0C[];
extern "C" int fn_27_30FA2C(void* table, int kind, int id, soModuleAccesser* acc, int);

s32 soModuleAccesser::getConstantIntKirby(s32 id) {
    return soValueAccesser::getConstantInt(this, fn_27_30FA2C(lbl_27_bss_7B0C, 1, id, this, 0), 0);
}
