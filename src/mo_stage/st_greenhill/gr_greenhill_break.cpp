#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <st_greenhill/gr_greenhill.h>
#include <st_greenhill/gr_greenhill_anim.h>
#include <ec/ec_mgr.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>
#include <yk/yk_normal.h>

// MATCH-ONLY: passing the offset by value reproduces the original's temporary copies.
static inline void assignVec2f(Vec2f& dst, Vec2f src) {
    dst = src;
}

grGreenhillBreak* grGreenhillBreak::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grGreenhillBreak* ground = new (Heaps::StageInstance) grGreenhillBreak(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grGreenhillBreak::grGreenhillBreak(const char* taskName) : grGreenhill(taskName) {
    unk158 = 0.0f;
    unk15C = 0.0f;
    unk160 = 0.0f;
    unk164 = NULL;
    m_type = 3;
    unk170 = 0.0f;
    m_animId = 1;
    unk178 = 0.0f;
    m_nodeA = 0;
    m_nodeB = 0;
    unk184 = 0;
    m_joint = NULL;
    unk18C = 0;
    unk18D = 0;
    unk190[0] = NULL;
    unk190[1] = NULL;
    unk190[2] = NULL;
    unk190[3] = NULL;
    unk190[4] = NULL;
    createSoundWork(1, 1);
    m_soundEffects[0].m_id = 0x1d19;
    m_soundEffects[0].m_repeatFrame = 0;
    m_soundEffects[0].m_nodeIndex = 0;
    m_soundEffects[0].m_endFrame = 0;
    assignVec2f(m_soundEffects[0].m_offsetPos, Vec2f(0.0f, 0.0f));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGreenhillBreak::~grGreenhillBreak() {
    if (unk190[0] != NULL) {
        delete[] static_cast<u8*>(unk190[0]);
    }
    unk190[0] = NULL;
    if (unk190[1] != NULL) {
        delete[] static_cast<u8*>(unk190[1]);
    }
    unk190[1] = NULL;
    if (unk190[2] != NULL) {
        delete static_cast<u8*>(unk190[2]);
    }
    unk190[2] = NULL;
    if (unk190[3] != NULL) {
        delete static_cast<u8*>(unk190[3]);
    }
    unk190[3] = NULL;
    if (unk190[4] != NULL) {
        delete static_cast<u8*>(unk190[4]);
    }
    unk190[4] = NULL;
}

void grGreenhillBreak::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateYakumono(deltaFrame);
        updateBreak(deltaFrame);
    }
}

// Finds the collision joint this object owns the first time it exists.
void grGreenhillBreak::updateJoint(float deltaFrame) {
    if (m_joint == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u16 jointNum = collision->m_jointLen;
            u32 i;
            grCollisionJoint* joint;
            for (i = 0; i != jointNum; i++) {
                joint = collision->getJoint(i);
                if (joint != NULL && joint->m_ground == this) {
                    break;
                }
            }
            if (i != jointNum) {
                m_joint = joint;
            }
        }
    }
}

// Shows or hides the broken/whole model pieces according to the shared break info (type 0: both halves, 1 and 2: one
// piece) and tells the collision joint which ledges are gone.
void grGreenhillBreak::updateYakumono(float deltaFrame) {
    if (unk18C == 1) {
        if (m_joint != NULL) {
            u16 ledgeFlags = 0;
            switch (m_type) {
            case 0:
                if (unk168[1] == 1) {
                    setNodeVisibility(true, 0, m_nodeA, true, false);
                } else {
                    setNodeVisibility(false, 0, m_nodeA, true, false);
                    ledgeFlags |= 0x2000;
                }
                if (unk168[2] == 1) {
                    setNodeVisibility(true, 0, m_nodeB, true, false);
                } else {
                    setNodeVisibility(false, 0, m_nodeB, true, false);
                    ledgeFlags |= 0x4000;
                }
                break;
            case 1:
                if (unk168[0] == 1) {
                    setNodeVisibility(true, 0, m_nodeA, true, false);
                } else {
                    setNodeVisibility(false, 0, m_nodeA, true, false);
                    ledgeFlags |= 0x4000;
                }
                break;
            case 2:
                if (unk168[0] == 1) {
                    setNodeVisibility(true, 0, m_nodeA, true, false);
                } else {
                    setNodeVisibility(false, 0, m_nodeA, true, false);
                    ledgeFlags |= 0x2000;
                }
                break;
            }
            m_joint->m_0x52 = ledgeFlags;
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            unk18C = 1;
        }
    }
}

// Drives the breakable piece: it waits a random time, breaks (hit boxes off, collision gone), stays away for a while,
// comes back blinking and then goes idle again.
void grGreenhillBreak::updateBreak(float deltaFrame) {
    grGreenhillBreakParam* data = (grGreenhillBreakParam*)getStageData();
    if (data == NULL) {
        return;
    }

    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    unk15C -= deltaFrame;
    if (unk15C < 0.0f) {
        unk15C = 0.0f;
    }
    unk160 -= deltaFrame;
    if (unk160 < 0.0f) {
        unk160 = 0.0f;
    }

    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(true);
        setEnableCollisionStatus(true);
        enableHit(0, 0);
        enableHit(1, 0);
        enableHit(2, 0);
        switch (m_type) {
        case 0:
            getNodeIndex(&m_nodeA, 0, "DanmenBrk02");
            getNodeIndex(&m_nodeB, 0, "DanmenBrk03");
            break;
        case 1:
            getNodeIndex(&m_nodeA, 0, "DanmenBrk01");
            break;
        case 2:
            getNodeIndex(&m_nodeA, 0, "DanmenBrk01");
            break;
        }
        switch (m_type) {
        case 0:
            unk168[0] = 0;
            break;
        case 1:
            unk168[1] = 0;
            break;
        case 2:
            unk168[2] = 0;
            break;
        }
        *unk164 = 5;
        unk170 = data->floorHealth;
        m_timer = data->m_waitMin + (data->m_waitMax - data->m_waitMin) * randf();
        m_state = 1;
        break;
    case 1:
        if (0.0f == unk15C && unk18D == 1) {
            disableAttack(0);
            disableAttack(1);
            disableAttack(2);
            unk18D = 0;
        }
        if (*unk164 == 2 || 0.0f == m_timer) {
            *unk164 = 2;
            setMotion(0, false, true, &unk178);
            disableHit(0, 0);
            disableHit(1, 0);
            disableHit(2, 0);
            switch (m_type) {
            case 0:
                unk168[0] = 1;
                break;
            case 1:
                unk168[1] = 1;
                break;
            case 2:
                unk168[2] = 1;
                break;
            }
            m_timer = 25.0f;
            startGimmickSE(0);
            m_state = 2;
        }
        break;
    case 2:
        if (0.0f == m_timer) {
            setEnableCollisionStatus(false);
            m_timer = data->floorRespawnFrames;
            m_state = 3;
        }
        break;
    case 3:
        if (m_timer <= data->floorRespawnAnimFrames) {
            setMotion(1, false, true, NULL);
            setEnableCollisionStatus(true);
            setAttack();
            m_state = 4;
            unk158 = 4.0f;
        }
        break;
    case 4:
        if (0.0f == m_timer) {
            m_state = 0;
            unk15C = 30.0f;
        } else {
            unk158 -= deltaFrame;
            if (unk158 < 0.0f) {
                unk158 = 0.0f;
            }
            if (unk158 > 2.0f) {
                setVisibility(true);
            } else {
                setVisibility(false);
            }
            if (0.0f == unk158) {
                unk158 = 4.0f;
            }
        }
        break;
    }
}

// MATCH-ONLY: the stage's node name strings ("DanmenBrk02", "DanmenBrk03", "DanmenBrk01", then one per piece type).
extern const char g_greenhillBreakNodeNames[][12];

// Looks up the node the hit boxes of this piece are attached to.
bool grGreenhillBreak::setNode() {
    const char (*names)[12] = g_greenhillBreakNodeNames;
    bool result = grGimmick::setNode();
    switch (m_type) {
    case 0:
        getNodeIndex(&unk184, 0, names[3]);
        break;
    case 1:
        getNodeIndex(&unk184, 0, names[4]);
        break;
    case 2:
        getNodeIndex(&unk184, 0, names[5]);
        break;
    }
    return result;
}

// MATCH-ONLY: the hit sphere is copied to its simple form as plain words (the two offsets, the size, the shape byte).
struct BreakHitWords {
    u32 m_x, m_y, m_z;
};
struct BreakHitCopy {
    BreakHitWords m_start;
    BreakHitWords m_end;
    float m_size;
    u8 m_shape;
};

#define BREAK_HIT (static_cast<soCollisionHitData*>(unk190[0]))
#define BREAK_HIT_SIMPLE (static_cast<soCollisionHitData::Simple*>(unk190[1]))
#define BREAK_HIT_SET (static_cast<soSet<soCollisionHitData::Simple>*>(unk190[2]))
#define BREAK_DATA_GROUP (static_cast<ykDataGroup*>(unk190[3]))
#define BREAK_DATA (static_cast<ykData*>(unk190[4]))
#define BREAK_COPY(i)     *reinterpret_cast<BreakHitCopy*>(&BREAK_HIT_SIMPLE[i]) = *reinterpret_cast<BreakHitCopy*>(&BREAK_HIT[i]);     BREAK_HIT_SIMPLE[i].m_height = soCollisionHitData::Height_Low;     BREAK_HIT_SIMPLE[i].m_nodeIndex = unk184;

// The hit object of the piece: three hit spheres side by side along it (the height of each depends on how the piece is
// mounted) and the damage module (it reports its damage to onDamage).
void grGreenhillBreak::setHit() {
    unk190[0] = new (Heaps::StageInstance) u8[0x60];
    unk190[1] = new (Heaps::StageInstance) u8[0x78];
    unk190[2] = new (Heaps::StageInstance) soSet<soCollisionHitData::Simple>;
    unk190[3] = new (Heaps::StageInstance) ykDataGroup;
    unk190[4] = new (Heaps::StageInstance) ykData;

    {
    Vec3f nodePos;
    getNodePosition(&nodePos, 0, unk184);
    BREAK_HIT[0].m_startOffsetPos.m_x = -10.0f;
    BREAK_HIT[0].m_startOffsetPos.m_y = 5.0f;
    BREAK_HIT[0].m_startOffsetPos.m_z = -nodePos.m_z;
    BREAK_HIT[0].m_endOffsetPos.m_x = -10.0f;
    BREAK_HIT[0].m_endOffsetPos.m_y = 5.0f;
    BREAK_HIT[0].m_endOffsetPos.m_z = -nodePos.m_z;
    BREAK_HIT[0].m_size = 10.0f;
    BREAK_HIT[0].m_shapeType = soCollision::Shape_Capsule;
    BREAK_HIT[1].m_startOffsetPos.m_x = 0.0f;
    BREAK_HIT[1].m_startOffsetPos.m_y = 5.0f;
    BREAK_HIT[1].m_startOffsetPos.m_z = -nodePos.m_z;
    BREAK_HIT[1].m_endOffsetPos.m_x = 0.0f;
    BREAK_HIT[1].m_endOffsetPos.m_y = 5.0f;
    BREAK_HIT[1].m_endOffsetPos.m_z = -nodePos.m_z;
    BREAK_HIT[1].m_size = 10.0f;
    BREAK_HIT[1].m_shapeType = soCollision::Shape_Capsule;
    BREAK_HIT[2].m_startOffsetPos.m_x = 10.0f;
    BREAK_HIT[2].m_startOffsetPos.m_y = 5.0f;
    BREAK_HIT[2].m_startOffsetPos.m_z = -nodePos.m_z;
    BREAK_HIT[2].m_endOffsetPos.m_x = 10.0f;
    BREAK_HIT[2].m_endOffsetPos.m_y = 5.0f;
    BREAK_HIT[2].m_endOffsetPos.m_z = -nodePos.m_z;
    BREAK_HIT[2].m_size = 10.0f;
    BREAK_HIT[2].m_shapeType = soCollision::Shape_Capsule;
    switch (m_type) {
    case 0:
        BREAK_HIT[0].m_startOffsetPos.m_y = 7.5f;
        BREAK_HIT[0].m_endOffsetPos.m_y = 7.5f;
        BREAK_HIT[1].m_startOffsetPos.m_y = 7.0f;
        BREAK_HIT[1].m_endOffsetPos.m_y = 7.0f;
        BREAK_HIT[2].m_startOffsetPos.m_y = 7.5f;
        BREAK_HIT[2].m_endOffsetPos.m_y = 7.5f;
        break;
    case 1:
        BREAK_HIT[0].m_startOffsetPos.m_y = 8.5f;
        BREAK_HIT[0].m_endOffsetPos.m_y = 8.5f;
        BREAK_HIT[1].m_startOffsetPos.m_y = 5.5f;
        BREAK_HIT[1].m_endOffsetPos.m_y = 5.5f;
        BREAK_HIT[2].m_startOffsetPos.m_y = 2.5f;
        BREAK_HIT[2].m_endOffsetPos.m_y = 2.5f;
        break;
    case 2:
        BREAK_HIT[0].m_startOffsetPos.m_y = 2.5f;
        BREAK_HIT[0].m_endOffsetPos.m_y = 2.5f;
        BREAK_HIT[1].m_startOffsetPos.m_y = 5.5f;
        BREAK_HIT[1].m_endOffsetPos.m_y = 5.5f;
        BREAK_HIT[2].m_startOffsetPos.m_y = 9.5f;
        BREAK_HIT[2].m_endOffsetPos.m_y = 9.5f;
        break;
    }
    BREAK_COPY(0)
    BREAK_COPY(1)
    BREAK_COPY(2)
    }

    BREAK_HIT_SET->m_elements = BREAK_HIT_SIMPLE;
    BREAK_HIT_SET->m_size = 3;
    BREAK_DATA_GROUP->m_hitDataSimpleSet = BREAK_HIT_SET;
    BREAK_DATA_GROUP->m_hitGroupIndex = 0;
    BREAK_DATA->m_dataGroupNum = 1;
    BREAK_DATA->m_dataGroups = BREAK_DATA_GROUP;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = BREAK_DATA;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 3, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Wall, 3, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    setYakumono(yakumono);
}

// Arms the three hit pieces once.
void grGreenhillBreak::setAttack() {
    if (unk18D != 1) {
        setAttack(0);
        setAttack(1);
        setAttack(2);
        unk18D = 1;
    }
}

// One of the three hitboxes of the piece: its offset along the piece depends on the index and on which way the piece
// is mounted (m_type), the box is centred on the node in unk184.
void grGreenhillBreak::setAttack(int index) {
    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    getNodePosition(&offset, 0, unk184);

    switch (index) {
    case 0:
        offset.m_x = -10.0f;
        offset.m_y = 5.0f;
        offset.m_z = -offset.m_z;
        break;
    case 1:
        offset.m_x = 0.0f;
        offset.m_y = 5.0f;
        offset.m_z = -offset.m_z;
        break;
    case 2:
        offset.m_x = 10.0f;
        offset.m_y = 5.0f;
        offset.m_z = -offset.m_z;
        break;
    }

    switch (m_type) {
    case 0:
        if (index == 0) {
            offset.m_y = 7.5f;
        } else if (index == 1) {
            offset.m_y = 7.0f;
        } else if (index == 2) {
            offset.m_y = 7.5f;
        }
        break;
    case 1:
        if (index == 0) {
            offset.m_y = 8.5f;
        } else if (index == 1) {
            offset.m_y = 5.5f;
        } else if (index == 2) {
            offset.m_y = 2.5f;
        }
        break;
    case 2:
        if (index == 0) {
            offset.m_y = 2.5f;
        } else if (index == 1) {
            offset.m_y = 5.5f;
        } else if (index == 2) {
            offset.m_y = 8.5f;
        }
        break;
    }

    float zero = 1.0f;
    setAttackGimmickDetails(&attack, 10.0f, zero, zero, zero,
        0, &offset, 90, 50, 100, 80, unk184,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
        soCollisionAttackData::Sound_Attribute_None,
        false, false, false, false, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(index, 0, &attack);
}

// Only animation 0 exists on the breakable pieces.
void grGreenhillBreak::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    GR_GREENHILL_SET_MOTION(m_animId, 1)
}

// A fighter hit the piece: the damage wears the piece down, and while it is hit the dust effect is replayed.
void grGreenhillBreak::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float damageAdd = damage->unk4;
    fn_27_26399C(m_yakumono);

    unk170 -= damageAdd;
    if (unk170 < 0.0f) {
        unk170 = 0.0f;
    }
    if (0.0f == unk170 && *unk164 != 2) {
        *unk164 = 2;
    }
    if (0.0f == unk160) {
        u32 effect = g_ecMgr->setEffect(ef_ptc_stg_greenhill_zimen_damage);
        g_ecMgr->setParent(effect, m_sceneModels[0], (u16)unk184, false);
        unk160 = 10.0f + 5.0f * randf();
    }
}
