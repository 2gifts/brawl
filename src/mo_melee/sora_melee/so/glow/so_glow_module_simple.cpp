#include <so/so_null.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <types.h>

class soCollisionAttackData;

// The glow module of an article (soGlowModuleSimple): it keeps only an attack level and a glow flag and, while it
// references a link connection (m_refLinkNo != -1), takes the glow of the linked parent instead. See
// so_glow_module_impl.cpp for the full module.
// HYPOTHESIS: method names (as in so_glow_module_impl.cpp). The classes are declared here because
// so/glow/so_glow_module_simple.h only declares the constructor and the first slots.
// TODO: clean up: share the soGlowModule declaration with so_glow_module_impl.cpp through a header.
class soGlowModule : public soNullable {
public:
    virtual ~soGlowModule() { }
    virtual void clear() = 0;                                                    // 0x10
    virtual void update() = 0;                                                   // 0x14
    virtual int getGlowAttack() const = 0;                                             // 0x18
    virtual void setGlowAttack(int attack) = 0;                                  // 0x1C
    virtual bool isGlow() const = 0;                                                   // 0x20
    virtual int getGlowDefense() = 0;                                            // 0x24
    virtual void setGlowDefense(int defense) = 0;                                // 0x28
    virtual void setGlowEffect(float eff, float min, float max) = 0;             // 0x2C
    virtual float getGlowEffect() = 0;                                           // 0x30
    virtual float getGlowDamageMin() = 0;                                        // 0x34
    virtual float getGlowDamageMax() = 0;                                        // 0x38
    virtual void setGlowAttackMax(int attackMax) = 0;                            // 0x3C
    virtual int getGlowAttackMax() = 0;                                          // 0x40
    virtual void unk44() = 0;                                                    // 0x44
    virtual void unk48() = 0;                                                    // 0x48
    virtual void unk4C() = 0;                                                    // 0x4C
    virtual void unk50() = 0;                                                    // 0x50
    virtual void unk54() = 0;                                                    // 0x54
    virtual int unk58() = 0;                                                     // 0x58
    virtual int getGlowAttack(soCollisionAttackData* attackData) const = 0;            // 0x5C
    virtual void unk60() = 0;                                                    // 0x60
    virtual int getWeaponGlowAttack() = 0;                                       // 0x64
    virtual void unk68() = 0;                                                    // 0x68
    virtual int getWeaponGlowDefense() = 0;                                      // 0x6C
    virtual float modifyPower(float power, soCollisionAttackData* attackData, soGlowModule* attacker) = 0; // 0x70
    virtual float calcPower(float power, soCollisionAttackData* attackData, int attack) = 0;            // 0x74
    virtual void setRefLinkNo(int refLinkNo) = 0;                                // 0x78
    virtual void debugPrint() = 0;                                               // 0x7C
    virtual void unk80() = 0;                                                    // 0x80
};

class soGlowModuleSimple : public soGlowModule {
public:
    soModuleAccesser* m_moduleAccesser; // 0x08
    int m_refLinkNo;                    // 0x0C: the link connection whose parent glow is used, or -1
    int m_glowAttack;                   // 0x10
    bool m_isGlow;                      // 0x14
    u32 unk18;                          // 0x18

    soGlowModuleSimple(soModuleAccesser* moduleAccesser);
    virtual ~soGlowModuleSimple();
    virtual void clear();
    virtual void update() { }
    virtual int getGlowAttack() const { return m_glowAttack; }
    virtual void setGlowAttack(int attack);
    virtual bool isGlow() const;
    virtual int getGlowDefense();
    virtual void setGlowDefense(int defense) { }
    virtual void setGlowEffect(float eff, float min, float max) { }
    virtual float getGlowEffect() { return 1.0f; }
    virtual float getGlowDamageMin() { return 1.0f; }
    virtual float getGlowDamageMax() { return 1.0f; }
    virtual void setGlowAttackMax(int attackMax) { }
    virtual int getGlowAttackMax() { return 999; }
    virtual void unk44() { }
    virtual void unk48() { }
    virtual void unk4C() { }
    virtual void unk50() { }
    virtual void unk54() { }
    virtual int unk58() { return 0; }
    virtual int getGlowAttack(soCollisionAttackData* attackData) const;
    virtual void unk60() { }
    virtual int getWeaponGlowAttack() { return getGlowAttack(); }
    virtual void unk68() { }
    virtual int getWeaponGlowDefense() { return getGlowDefense(); }
    virtual float modifyPower(float power, soCollisionAttackData* attackData, soGlowModule* attacker) { return power; }
    virtual float calcPower(float power, soCollisionAttackData* attackData, int attack) { return power; }
    virtual void setRefLinkNo(int refLinkNo) { m_refLinkNo = refLinkNo; }
    virtual void debugPrint() { }
    virtual void unk80() { }
};

soGlowModuleSimple::soGlowModuleSimple(soModuleAccesser* moduleAccesser) :
    m_moduleAccesser(moduleAccesser), m_refLinkNo(-1), m_glowAttack(12), m_isGlow(false) { }

soGlowModuleSimple::~soGlowModuleSimple() { }

void soGlowModuleSimple::clear() {
    m_refLinkNo = -1;
    m_isGlow = false;
    m_glowAttack = 12;
}

bool soGlowModuleSimple::isGlow() const {
    if (m_refLinkNo != -1 && m_moduleAccesser->getLinkModule().isLink(m_refLinkNo) == true) {
        return m_moduleAccesser->getLinkModule().isParentGlow(m_refLinkNo);
    }
    return m_isGlow;
}

int soGlowModuleSimple::getGlowAttack(soCollisionAttackData* attackData) const {
    if (m_refLinkNo != -1 && m_moduleAccesser->getLinkModule().isLink(m_refLinkNo) == true) {
        return m_moduleAccesser->getLinkModule().getParentGlowAttack(m_refLinkNo, attackData);
    }
    return m_glowAttack + m_moduleAccesser->getLinkModule().getParentGlowAttackAdd(attackData);
}

void soGlowModuleSimple::setGlowAttack(int attack) {
    m_glowAttack = attack;
    m_isGlow = true;
}

int soGlowModuleSimple::getGlowDefense() {
    return 8;
}
