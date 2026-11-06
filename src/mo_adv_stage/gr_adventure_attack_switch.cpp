#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL
#define YK_DATA_INLINE_CTOR
#define SO_COLLISION_GROUP_ALIGNED

#include <memory.h>
#include <yk/yk_normal.h>
#include <sora_adv_stage/gr_adventure_attack_switch.h>

grAdventureAttackSwitch* grAdventureAttackSwitch::create(int mdlIndex, bool* isOn, const char* taskName) {
    grAdventureAttackSwitch* ground = new (Heaps::StageInstance) grAdventureAttackSwitch(isOn, taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureAttackSwitch::grAdventureAttackSwitch(bool* isOn, const char* taskName) : grYakumonoSwitch(isOn, taskName) {
    m_coolTime = 0.0f;
    m_data = NULL;
    unk190[0] = 0;
    unk190[1] = 0;
    unk190[2] = 0;
    unk190[3] = 0;
    unk190[4] = 0;
}

grAdventureAttackSwitch::~grAdventureAttackSwitch() { }

void grAdventureAttackSwitch::update(float deltaFrame) {
    switch (m_switchState) {
    case 1:
        if (0.0f != static_cast<grGimmickAttackSwitchData*>(m_switchData)->m_onTime) {
            m_onTimer += deltaFrame;
            if (m_onTimer >= static_cast<grGimmickAttackSwitchData*>(m_switchData)->m_onTime) {
                setOff(1);
            }
        }
        break;
    default:
        break;
    }
    if (m_coolTime > 0.0f) {
        m_coolTime -= deltaFrame;
    }
    grYakumonoSwitch::update(deltaFrame);
}

struct HitFlags {
    u32 m_nodeIndex : 9;
    u32 m_part : 2;
    u32 m_height : 2;
    u32 m_unused : 2;
    u32 m_shape : 1;
    u32 m_rest : 16;
};

void grAdventureAttackSwitch::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_data = static_cast<grGimmickAttackSwitchData*>(m_switchData);

    // HYPOTHESIS: 0x168 holds a soCollisionHitData built from the record, 0x188 the set that points at it, 0x190 the data
    // group and 0x19c the ykData.
    soCollisionHitData* hitData = reinterpret_cast<soCollisionHitData*>(&unk168[0]);
    hitData->m_startOffsetPos = m_data->m_hit.m_startOffsetPos;
    hitData->m_endOffsetPos = m_data->m_hit.m_endOffsetPos;
    hitData->m_size = m_data->m_hit.m_size;
    // HYPOTHESIS: the word at 0x1c of the hit data is the node index (9 bits), part (2), height (2), two unused bits, the shape
    // (1) and 16 more bits; the node and part are cleared, the height is low and the shape is set when the record's is.
    HitFlags* flags = reinterpret_cast<HitFlags*>(reinterpret_cast<u8*>(hitData) + 0x1C);
    flags->m_nodeIndex = 0;
    flags->m_part = 0;
    flags->m_height = 1;
    flags->m_unused = 0;
    flags->m_shape = static_cast<s8>(reinterpret_cast<u8*>(&m_data->m_hit)[0x1C]) != 0;
    reinterpret_cast<soSet<soCollisionHitData>*>(&unk168[0x20])->m_elements = hitData;
    reinterpret_cast<soSet<soCollisionHitData>*>(&unk168[0x20])->m_size = 1;
    reinterpret_cast<ykDataGroup*>(&unk190[0])->m_hitDataSimpleSet = NULL;
    reinterpret_cast<ykDataGroup*>(&unk190[0])->m_hitDataSet = reinterpret_cast<soSet<soCollisionHitData>*>(&unk168[0x20]);
    reinterpret_cast<ykDataGroup*>(&unk190[0])->m_hitGroupIndex = 0;
    reinterpret_cast<ykData*>(&unk190[3])->m_dataGroupNum = 1;
    reinterpret_cast<ykData*>(&unk190[3])->m_dataGroups = reinterpret_cast<ykDataGroup*>(&unk190[0]);

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = reinterpret_cast<ykData*>(&unk190[3]);
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    setYakumono(yakumono);
    if (m_resFile.GetResAnmClrNumEntries() != 0) {
        changeMatColAnim(0, 0);
    }
}

void grAdventureAttackSwitch::setOn(int isTrigger) {
    grYakumonoSwitch::setOn(isTrigger);
    disableHit(0, 0);
    if (m_resFile.GetResAnmClrNumEntries() > 1) {
        changeMatColAnim(1, 0);
        if (isTrigger == 0) {
            gfModelAnimation* modelAnim = m_modelAnims[0];
            modelAnim->setFrame(modelAnim->getFrameCount());
        }
    }
}

void grAdventureAttackSwitch::setOff(int isTrigger) {
    grYakumonoSwitch::setOff(isTrigger);
    enableHit(0, 0);
    m_coolTime = 5.0f;
}

void grAdventureAttackSwitch::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (!(m_coolTime > 0.0f)) {
        if (grYakumono::isEnableAttackPersonAttacked((u32)damage, (u32)attackerInfo, (u32)&m_data->m_hit) &&
            damage->unk4 > 0.0f) {
            setOn(true);
        }
    }
}
