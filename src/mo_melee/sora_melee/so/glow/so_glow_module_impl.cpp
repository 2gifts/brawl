#include <so/so_null.h>
#include <revolution/OS/OSError.h>
#include <types.h>

class soModuleAccesser;
class soCollisionAttackData;

// The glow ("aura") module: an attack and a defense level, an attack cap and the damage multiplier range used when an
// attacker's glow attack differs from the defender's glow defense. soGlowModuleSimple (so_glow_module_simple.cpp) is the
// variant for articles, which can take the glow of the fighter they are linked to.
// HYPOTHESIS: all method names except the constructor and the debug print strings ("glow attack:%d attack_max:%d
// defense:%d", "glow dmg_min:%f dmg_max:%f dmg_eff:%f"); the empty slots keep placeholder names. The class is declared
// here because so/glow/so_glow_module_impl.h and so/glow/so_glow_module_simple.h only declare the constructors.
// TODO: clean up: move the full class into so/glow/so_glow_module_impl.h once its users (ftGlowModuleImpl, the enemy and
// article module builders) are checked against the full vtable.
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

class soGlowModuleImpl : public soGlowModule {
public:
    int m_glowAttack;       // 0x08
    int m_glowDefense;      // 0x0C
    int m_glowAttackMax;    // 0x10
    float m_glowEffect;     // 0x14
    float m_glowDamageMin;  // 0x18
    float m_glowDamageMax;  // 0x1C
    bool m_isGlow;          // 0x20

    soGlowModuleImpl(soModuleAccesser* moduleAccesser);
    virtual ~soGlowModuleImpl();
    virtual void clear();
    virtual void update() { }
    virtual int getGlowAttack() const { return m_glowAttack; }
    virtual void setGlowAttack(int attack);
    virtual bool isGlow() const { return m_isGlow; }
    virtual int getGlowDefense();
    virtual void setGlowDefense(int defense);
    virtual void setGlowEffect(float eff, float min, float max) {
        m_glowEffect = eff;
        m_glowDamageMin = min;
        m_glowDamageMax = max;
    }
    virtual float getGlowEffect() { return m_glowEffect; }
    virtual float getGlowDamageMin() { return m_glowDamageMin; }
    virtual float getGlowDamageMax() { return m_glowDamageMax; }
    virtual void setGlowAttackMax(int attackMax) { m_glowAttackMax = attackMax; }
    virtual int getGlowAttackMax() { return m_glowAttackMax; }
    virtual void unk44() { }
    virtual void unk48() { }
    virtual void unk4C() { }
    virtual void unk50() { }
    virtual void unk54() { }
    virtual int unk58() { return 0; }
    virtual int getGlowAttack(soCollisionAttackData* attackData) const { return getGlowAttack(); }
    virtual void unk60() { }
    virtual int getWeaponGlowAttack() { return getGlowAttack(); }
    virtual void unk68() { }
    virtual int getWeaponGlowDefense();
    virtual float modifyPower(float power, soCollisionAttackData* attackData, soGlowModule* attacker);
    virtual float calcPower(float power, soCollisionAttackData* attackData, int attack);
    virtual void setRefLinkNo(int refLinkNo) { }
    virtual void debugPrint();
    virtual void unk80() { }
};

soGlowModuleImpl::soGlowModuleImpl(soModuleAccesser* moduleAccesser) { }

soGlowModuleImpl::~soGlowModuleImpl() { }

void soGlowModuleImpl::clear() {
    m_isGlow = false;
    m_glowAttack = 12;
    m_glowDefense = 8;
    m_glowEffect = 0.0f;
    m_glowDamageMin = 0.0f;
    m_glowDamageMax = 0.0f;
    m_glowAttackMax = 999;
}

void soGlowModuleImpl::setGlowAttack(int attack) {
    m_glowAttack = attack;
    m_isGlow = true;
}

void soGlowModuleImpl::setGlowDefense(int defense) {
    m_glowDefense = defense;
    m_isGlow = true;
}

float soGlowModuleImpl::modifyPower(float power, soCollisionAttackData* attackData, soGlowModule* attacker) {
    int attack = attacker->getGlowAttack(attackData);
    if (attack > attacker->getGlowAttackMax()) {
        attack = attacker->getGlowAttackMax();
    }
    return calcPower(power, attackData, attack);
}

// The damage is scaled by 1 + (attack - defense) / 100 * effect and kept inside [min, max] (the limits only apply when
// the scaling crosses them).
float soGlowModuleImpl::calcPower(float power, soCollisionAttackData* attackData, int attack) {
    int diff = attack - getWeaponGlowDefense();
    if (diff == 0) {
        return power;
    }
    float result = power * (1.0f + (diff / 100.0f) * m_glowEffect);
    if (result <= 0.0f && power > 0.0f) {
        result = m_glowDamageMin;
    }
    if (result > m_glowDamageMax && power < m_glowDamageMax) {
        result = m_glowDamageMax;
    }
    return result;
}

int soGlowModuleImpl::getWeaponGlowDefense() {
    return getGlowDefense();
}

int soGlowModuleImpl::getGlowDefense() {
    return m_glowDefense;
}

void soGlowModuleImpl::debugPrint() {
    OSReport("glow dmg_min:%f dmg_max:%f dmg_eff:%f \n", m_glowDamageMin, m_glowDamageMax, m_glowEffect);
    OSReport("glow attack:%d attack_max:%d defense:%d \n", m_glowAttack, m_glowAttackMax, m_glowDefense);
}
