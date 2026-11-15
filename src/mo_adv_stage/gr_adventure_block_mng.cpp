#define MT_VEC3F_MUL_INLINE
#define SO_DAMAGE_STAGE_CTOR_OUT_OF_LINE // BYTES: the damage constructor is compiled after the unit's own functions (its 1.0 ends the pool)
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the stage RELs that also hold the damage module
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // soArrayVector<soCollisionGroup, 24> places its elements 4-aligned
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies it member by member
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL

#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>
#include <gf/gf_camera_controller.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <memory.h>
#include <st/stage.h>
#include <yk/yk_normal.h>
#include <sora_adv_stage/gr_adventure_block_mng.h>
#include <sora_adv_stage/gr_gimmick_block.h>

// Yakumono::setHitStatus (sora_melee, unnamed)
extern "C" void fn_27_2637E8(Yakumono* yakumono, int index, int status, int part);

grAdventureBlockMng* grAdventureBlockMng::create(int mdlIndex, u8 blockKind, const char* taskName) {
    grAdventureBlockMng* ground = new (Heaps::StageInstance) grAdventureBlockMng(blockKind, taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

grAdventureBlockMng::grAdventureBlockMng(u8 blockKind, const char* taskName) : grYakumono(taskName) {
    m_blockNum = 0;
    m_blocks = NULL;
    m_data = NULL;
    m_blockKind = blockKind;
    m_hitData = NULL;
    m_hitSets = NULL;
    m_dataGroups = NULL;
    m_ykData = NULL;
    m_isDamaged = 0;
}

grAdventureBlockMng::~grAdventureBlockMng() {
    if (m_blocks != NULL) {
        delete[] m_blocks;
        m_blocks = NULL;
    }
    if (m_hitData != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitData);
        m_hitData = NULL;
    }
    if (m_hitSets != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSets);
        m_hitSets = NULL;
    }
    if (m_dataGroups != NULL) {
        delete[] m_dataGroups;
        m_dataGroups = NULL;
    }
    if (m_ykData != NULL) {
        delete m_ykData;
        m_ykData = NULL;
    }
}

bool grAdventureBlockMng::createBlock(gfArchive* archive) {
    nw4r::g3d::ResMdl resMdl = m_sceneModels[0]->m_resMdl;
    m_blockNum = resMdl.GetResNodeNumEntries() - 1;
    m_blocks = new (Heaps::StageInstance) grGimmickBlock*[m_blockNum];
    grGimmickBlock* block;
    for (u32 i = 0; i < m_blockNum; i++) {
        if (m_blockKind == 0) {
            block = grGimmickBlock::create(static_cast<s16>(m_data->m_blockMdlIndex), "grGimmickBlock");
        }
        block->m_connectedTask = this;
        if (m_attachedTask == NULL) {
            m_attachedTask = block;
        } else {
            gfTask* last = m_attachedTask;
            while (last->m_nextTask != NULL) {
                last = last->m_nextTask;
            }
            last->m_nextTask = block;
        }
        m_blocks[i] = block;
        block->setGimmickData(m_data);
        block->unk14C = i + 1;
        block->startup(archive, 0, static_cast<gfSceneRoot::LayerType>(0));
        if (m_data->m_collIndex != 0xFF) {
            grCollData* collData = new (Heaps::StageInstance) grCollData;
            if (collData != NULL) {
                g_Stage->testCollDataInit(archive, collData, m_data->m_collIndex);
                grCollision* collision = grCollision::create(g_Stage, collData, block, 1);
                if (!(m_data->m_collFlags & 1)) {
                    collision->getJoint(0)->m_0x55_4 |= true;
                }
            }
            delete collData;
        }
        block->m_blockMng = this;
    }
    return true;
}

void grAdventureBlockMng::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_data = static_cast<grAdventureBlockMngData*>(m_gimmickData);
    if (m_data == NULL) {
        return;
    }
    createBlock(data);
    // BYTES: raw storage (no element construction, no array cookie) for the hit spheres and their sets.
    m_hitData = reinterpret_cast<grAdventureBlockHitData*>(new (Heaps::StageInstance) u8[sizeof(grAdventureBlockHitData) * m_blockNum]);
    m_hitSets = reinterpret_cast<soSet<grAdventureBlockHitData>*>(
        new (Heaps::StageInstance) u8[sizeof(soSet<grAdventureBlockHitData>) * m_blockNum]);
    m_dataGroups = new (Heaps::StageInstance) ykDataGroup[m_blockNum];
    m_ykData = new (Heaps::StageInstance) ykData;
    for (u32 i = 0; i < m_blockNum; i++) {
        m_hitData[i].m_startOffsetPos = m_data->m_hitStartOffset;
        m_hitData[i].m_endOffsetPos = m_data->m_hitEndOffset;
        m_hitData[i].m_size = m_data->m_hitSize;
        m_hitData[i].m_nodeIndex = i + 1;
        m_hitData[i].m_part = 0;
        m_hitData[i].m_height = 1;
        m_hitData[i].m_0x1c_3 = 0;
        m_hitData[i].m_shapeType = m_data->m_hitShape != 0;
        m_hitSets[i].m_elements = &m_hitData[i];
        m_hitSets[i].m_size = 1;
        m_dataGroups[i].m_hitDataSimpleSet = NULL;
        m_dataGroups[i].m_hitDataSet = reinterpret_cast<soSet<soCollisionHitData>*>(&m_hitSets[i]);
        m_dataGroups[i].m_hitGroupIndex = i;
    }
    m_ykData->m_dataGroupNum = m_blockNum;
    m_ykData->m_dataGroups = m_dataGroups;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_ykData;
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 24, 24, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    setYakumono(yakumono);
    if (m_data->m_situationODD != 0) {
        setSituationODD();
    }
    nw4r::g3d::ResMdl resMdl = m_sceneModels[0]->m_resMdl;
    for (u32 i = 0; i < m_blockNum; i++) {
        if (m_blocks[i] != NULL) {
            nw4r::g3d::ResNode node = resMdl.GetResNode(m_blocks[i]->unk14C);
            Vec3f blockPos;
            blockPos.m_x = node->m_translation.m_x;
            blockPos.m_y = node->m_translation.m_y;
            blockPos.m_z = node->m_translation.m_z;
            m_blocks[i]->setPos(&blockPos);
            m_blocks[i]->updateCallback(0);
            m_blocks[i]->updateG3dProcCalcWorld();
        }
    }
    createInvalidCameraClippingSphere();
}

// HYPOTHESIS: the stage passes the saved state of the group as a pointer in the int argument.
void grAdventureBlockMng::setInitializeInfo(int initializeInfo) {
    grAdventureBlockInitializeInfo* info = reinterpret_cast<grAdventureBlockInitializeInfo*>(initializeInfo);
    if (info->m_kind == 0x30) {
        u64 mask = info->m_brokenMask;
        for (u32 i = 0; i < m_blockNum; i++) {
            if (mask & (1ULL << i)) {
                m_blocks[i]->breakBlock();
                requestBreak(i);
            }
        }
    }
}

bool grAdventureBlockMng::getInitializeInfo(int initializeInfo) {
    grAdventureBlockInitializeInfo* info = reinterpret_cast<grAdventureBlockInitializeInfo*>(initializeInfo);
    info->m_kind = 0x30;
    info->m_brokenMask = 0;
    if (m_data->m_noSave == 1) {
        return false;
    }
    for (u32 i = 0; i < m_blockNum; i++) {
        if (m_blocks[i]->m_isBroken == 1) {
            info->m_brokenMask = info->m_brokenMask | (1ULL << i);
        }
    }
    return true;
}

void grAdventureBlockMng::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isDamaged) {
        m_isDamaged = 0;
        clearAttackerInfo();
    }
}

void grAdventureBlockMng::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    m_isDamaged = 1;
    if (grYakumono::isEnableAttackPersonAttacked(reinterpret_cast<u32>(damage), reinterpret_cast<u32>(attackerInfo),
                                     reinterpret_cast<u32>(m_data))) {
        m_blocks[index]->requestBreak(damage->unk4);
        if (m_blocks[index]->m_isBroken == 1) {
            requestBreak(index);
        }
    }
}

void grAdventureBlockMng::requestBuild(u8 index) {
    fn_27_2637E8(m_yakumono, 0, 0, index);
}

void grAdventureBlockMng::requestBuild(grGimmickBlock* block) {
    for (u32 i = 0; i < m_blockNum; i++) {
        if (m_blocks[i] == block) {
            fn_27_2637E8(m_yakumono, 0, 0, i);
            return;
        }
    }
}

void grAdventureBlockMng::requestBreak(u8 index) {
    fn_27_2637E8(m_yakumono, 0, 3, index);
}

void grAdventureBlockMng::invalidatedByCameraClipping() {
    gfCameraController* controller = CameraController::getInstance()->m_currentController;
    if (controller != NULL && controller->getCameraManager() != NULL) {
    float x, y, z, radius;
    radius = unkF8;
    z = m_clipSphereCenter.m_z;
    y = m_clipSphereCenter.m_y;
    x = m_clipSphereCenter.m_x;
    gfCamera::ClipSphere sphere;
    sphere.center.m_x = x;
    sphere.center.m_y = y;
    sphere.center.m_z = z;
    sphere.radius = radius;
    if (!controller->getCameraManager()->getCamera(0)->isClipIn(&sphere)) {
        if (getTransparencyFlag()) {
            for (u32 i = 0; i < m_blockNum; i++) {
                m_blocks[i]->setTransparency(0, m_transparencySettings);
            }
            setSleepHit(true);
            setTransparencyFlag(0);
        }
    } else if (!getTransparencyFlag()) {
        for (u32 i = 0; i < m_blockNum; i++) {
            m_blocks[i]->setTransparency(1, m_transparencySettings);
        }
        setSleepHit(false);
        setTransparencyFlag(1);
    }
    }
}

// BYTES: Vec3f::length, inline in this unit
static inline float blockMngLength(Vec3f& v) {
    float lengthSquared = v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
    // MATCH-ONLY: retain the original fabs double temporary and frsp at conversion
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

void grAdventureBlockMng::createInvalidCameraClippingSphere() {
    Vec3f max(0.0f, 0.0f, 0.0f);
    Vec3f min(0.0f, 0.0f, 0.0f);
    for (u32 i = 0; i < m_blockNum; i++) {
        if (m_blocks[i] != NULL) {
            Vec3f pos = m_blocks[i]->getPos();
            if (i == 0) {
                max.m_x = pos.m_x;
                max.m_y = pos.m_y;
                min.m_x = pos.m_x;
                min.m_y = pos.m_y;
            } else {
                if (pos.m_x > max.m_x) {
                    max.m_x = pos.m_x;
                }
                if (pos.m_y > max.m_y) {
                    max.m_y = pos.m_y;
                }
                if (pos.m_x < min.m_x) {
                    min.m_x = pos.m_x;
                }
                if (pos.m_y < min.m_y) {
                    min.m_y = pos.m_y;
                }
            }
        }
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    Vec3f diff = max - min;
    float radius = 0.5f * blockMngLength(diff);
    diff.normalize();
    center = min + diff * radius;
    m_clipSphereCenter = center;
    if (m_data->m_situationODD != 0) {
        unkF8 = 100.0f + radius;
    } else {
        unkF8 = 50.0f + radius;
    }
}

void grAdventureBlockMng::messageBreak(grGimmickBlock* block) {
    for (u32 i = 0; i < m_blockNum; i++) {
        if (m_blocks[i] == block) {
            requestBreak(i);
            return;
        }
    }
}

void grAdventureBlockMng::dbDispInvalidatedByCameraClippingSphere() {
    grGimmick::dbDispInvalidatedByCameraClippingSphere();
}

soDamage::soDamage() {
    unk60 = 1.0f;
    unk64 = 1.0f;
    unk68 = 1.0f;
    unk74 = unk74 & ~0x1F;
}
