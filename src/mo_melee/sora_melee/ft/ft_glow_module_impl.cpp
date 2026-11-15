#include <so/glow/so_glow_module_impl.h>
#include <types.h>

class soCollisionAttackData;

// The fighter glow module: per-attribute and per-region attack glow, defense glow and queued attack statuses on top of
// the common glow module.
// HYPOTHESIS: the field roles follow the setters; the array lengths follow the field offsets.
// TODO: activate, setup, getGlowAttack, getGlowDefense, notifyEventChangeStatus and the weapon glow queries.
class ftGlowModuleImpl : public soGlowModuleImpl {
public:
    int m_attackStatusAdd;       // 0x24
    int m_attackStatus[4][2];    // 0x28
    int m_attackStatusCount;     // 0x48
    int m_weaponAttack;          // 0x4C
    int m_attackRegion[26];      // 0x50
    int m_attackAttr[24];        // 0xB8
    int m_defenseAttr[24];       // 0x118
    int unk178;                  // 0x178

    ftGlowModuleImpl(soModuleAccesser* acc);
    virtual ~ftGlowModuleImpl();
    void setGlowAttackAttr(int attr, int add);
    void setGlowAttackRegion(int region, int add);
    void setGlowAttackStatus(int status, int value);
    int getGlowAttackAdd(soCollisionAttackData* attackData);
    void setGlowDefenseAttr(int attr, int add);
    void setWeaponGlowAttack(int add);
    void addGlowAttackStatus(int add);
    void clearGlowAttackStatus();
};

ftGlowModuleImpl::ftGlowModuleImpl(soModuleAccesser* acc) : soGlowModuleImpl(acc) {
    unk178 = 0;
}

ftGlowModuleImpl::~ftGlowModuleImpl() { }

void ftGlowModuleImpl::setGlowAttackAttr(int attr, int add) {
    m_attackAttr[attr] += add;
}

void ftGlowModuleImpl::setGlowAttackRegion(int region, int add) {
    m_attackRegion[region] += add;
}

void ftGlowModuleImpl::setGlowAttackStatus(int status, int value) {
    int* entry = m_attackStatus[m_attackStatusCount];
    entry[0] = status;
    entry[1] = value;
    m_attackStatusCount++;
}

// HYPOTHESIS: the attack data words at 0x30 and 0x38 carry the attribute and the region in their low five bits.
int ftGlowModuleImpl::getGlowAttackAdd(soCollisionAttackData* attackData) {
    u32 attr = reinterpret_cast<u32*>(attackData)[0x30 / 4] & 0x1f;
    u32 region = reinterpret_cast<u32*>(attackData)[0x38 / 4] & 0x1f;
    return m_attackRegion[region] + m_attackAttr[attr];
}

void ftGlowModuleImpl::setGlowDefenseAttr(int attr, int add) {
    m_defenseAttr[attr] += add;
}

void ftGlowModuleImpl::setWeaponGlowAttack(int add) {
    m_weaponAttack += add;
}

void ftGlowModuleImpl::addGlowAttackStatus(int add) {
    m_attackStatusAdd += add;
}

void ftGlowModuleImpl::clearGlowAttackStatus() {
    m_attackStatusCount = 0;
}
