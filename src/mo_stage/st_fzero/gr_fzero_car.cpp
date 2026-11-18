#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define YK_STAGE_FULL // the hit objects of this REL also hold the observers of the stage object (their stubs are emitted with them)
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member
#include <st_fzero/gr_fzero.h>
#include <st_fzero/gr_fzero_anim.h>
#include <gr/gr_calc_world_callback.h>
#include <yk/yk_no_hit_normal.h>
#include <memory.h>
#include <string.h>

grFzeroCar::grFzeroCar(const char* taskName) : grFzero(taskName) {
    m_sceneWork = NULL;
    m_stateWork = NULL;
    m_carData = NULL;
    m_animId = 2;
    m_animFrames = 0.0f;
    m_hasYakumono = 0;
    m_attackEnabled = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    m_seHandle = -1;
    m_seHandleNear = -1;
}

grFzeroCar* grFzeroCar::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grFzeroCar* ground = new (Heaps::StageInstance) grFzeroCar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grFzeroCar::~grFzeroCar() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grFzeroCar::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Creates the hit (setHit) once the car exists.
void grFzeroCar::updateYakumono(float deltaFrame) {
    if (m_hasYakumono != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// The engine sound that goes with a car type (-1 for none).
static inline int grFzeroCarPassSound(u8 type) {
    int id;
    switch (type) {
    case 0:
        id = 0x1c66;
        break;
    case 1:
        id = 0x1c67;
        break;
    case 2:
        id = 0x1c68;
        break;
    case 3:
        id = 0x1c69;
        break;
    default:
        id = -1;
    }
    return id;
}

// The sound of a car going past the fighters.
static inline int grFzeroCarNearSound(u8 type) {
    int id;
    switch (type) {
    case 0:
        id = 0x1c6a;
        break;
    case 1:
        id = 0x1c6b;
        break;
    case 2:
        id = 0x1c6c;
        break;
    case 3:
        id = 0x1c6d;
        break;
    default:
        id = -1;
    }
    return id;
}

// Distance of the car from the origin of its matrix (HYPOTHESIS: the camera/ring centre), zero when it is there.
static inline float grFzeroCarDistance(grCalcWorldCallBack* callback) {
    Matrix* mtx = &callback->m_nodeCallbackDatas[0].m_matrix;
    Vec3f pos(mtx->m[0][3], mtx->m[1][3], mtx->m[2][3]);
    float distance;
    bool atOrigin = false;
    if (fzeroIsNearZero(pos.m_x) && fzeroIsNearZero(pos.m_y) && fzeroIsNearZero(pos.m_z)) {
        atOrigin = true;
    }
    if (atOrigin == true) {
        distance = 0.0f;
    } else {
        distance = pos.m_z * pos.m_z + (pos.m_x * pos.m_x + pos.m_y * pos.m_y);
        if ((float)fabs(distance) <= 1.17549435e-38f) {
            distance = 0.0f;
        } else {
            distance = distance * rsqrtf(distance);
        }
    }
    return distance;
}

void grFzeroCar::updateActive(float deltaFrame) {
    if (m_carData == NULL) {
        return;
    }
    switch (m_state) {
    case 2:
        break;
    case 0:
        setMotionCommon(2, 0, 1, 0);
        setVisibility(0);
        disableAttack(0);
        m_attackEnabled = 0;
        m_state = 1;
        // fall through
    case 1:
        if (m_carData->m_state == 7) {
            setMotionCommon(1, 1, 1, &m_animFrames);
            int id = grFzeroCarPassSound(m_carData->m_type);
            if (id != -1) {
                m_seHandle = m_sndGen.playSE(static_cast<SndID>(id), 0, 30, -1);
            }
            m_state = 3;
        }
        break;
    case 3: {
        if (!m_isVisible) {
            setVisibility(1);
        }
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback != NULL) {
            float distance = grFzeroCarDistance(callback);
            if (distance > 2048.0f) {
                if (m_attackEnabled == 1) {
                    disableAttack(0);
                }
                m_attackEnabled = 0;
            } else {
                setAttack();
            }
            if (m_seHandleNear == -1) {
                u8 scene = *m_sceneWork;
                bool close = true;
                if (scene == 1) {
                    close = false;
                }
                if (scene == 2) {
                    close = false;
                }
                if (scene == 3 && *m_stateWork == 0) {
                    close = false;
                }
                if (close == true) {
                    callback = &m_calcWorldCallBack;
                    if (callback == NULL) {
                        return;
                    }
                    float nearDistance = grFzeroCarDistance(callback);
                    if (nearDistance < 1000.0f) {
                        int id = grFzeroCarNearSound(m_carData->m_type);
                        if (id != -1) {
                            m_seHandleNear = m_sndGen.playSE(static_cast<SndID>(id), 0, 30, -1);
                        }
                    }
                }
            }
            if (m_carData->m_state == 8) {
                m_carData->m_mtx = NULL;
                if (m_seHandle != -1) {
                    m_sndGen.stopSE(m_seHandle, 30);
                }
                m_seHandle = -1;
                m_seHandleNear = -1;
                m_state = 0;
            }
        }
        break;
    }
    }
}

// The model follows the stage matrix of its car; it is hidden while it stands still.
void grFzeroCar::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_carData != NULL && m_carData->m_mtx != NULL) {
                Matrix* mtx = m_carData->m_mtx;
                bool same = false;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *mtx;
                Matrix* node = &calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix;
                Vec3f pos(node->m[0][3], node->m[1][3], node->m[2][3]);
                Vec3f diff;
                diff = pos - m_carData->m_pos;
                if (fzeroIsNearZero(diff.m_x) && fzeroIsNearZero(diff.m_y) && fzeroIsNearZero(diff.m_z)) {
                    same = true;
                }
                if (same == true && deltaFrame != 0.0f) {
                    if (m_isVisible == true) {
                        setVisibility(0);
                    }
                } else if (!m_isVisible) {
                    setVisibility(1);
                }
                m_carData->m_pos = pos;
                m_sndGen.setPos(&pos);
            }
        }
    }
}

// Builds the car's hit object: one attack part, one collision group and no hit module (a ykNoHitNormal), at the position
// the car's model reports.
void grFzeroCar::setHit() {
    m_work = new (Heaps::StageInstance) grFzeroAttackWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x10, NULL, NULL};
    info.m_ground = this;
    nw4r::g3d::ScnMdl* model = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_node = model;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// The car hurts fighters it hits: a ball of 20 units around the car, enabled once.
void grFzeroCar::setAttack() {
    if (m_attackEnabled == 1) {
        return;
    }

    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = 5.0f;
    offset.m_z = 0.0f;

    setAttackGimmickDetails(&attack, 20.0f, 1.0f, 0.5f, 1.0f,
        20, &offset, 90, 100, 0, 70, 0,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
        soCollisionAttackData::Sound_Attribute_Cutup,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, false);
    m_yakumono->setAttack(0, 0, &attack);
    m_attackEnabled = 1;
}

// Binds the two animations of the car (0 and 1); animation 2 means "none".
void grFzeroCar::setMotionCommon(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_animId == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_animId = animId;

    if (animId >= 2) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grFzeroSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grFzeroSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grFzeroSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grFzeroSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grFzeroSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

grFzeroNode* grFzeroNode::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grFzeroNode* ground = new (Heaps::StageInstance) grFzeroNode(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grFzeroNode::grFzeroNode(const char* taskName) : grFzero(taskName) {
    m_sceneWork = NULL;
    m_frameSceneWork = NULL;
    m_motionTimer = 0.0f;
    m_stateWork = NULL;
    m_motionWork = NULL;
    m_motionId = 5;
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    memset(m_node, 0, sizeof(m_node));
    m_animId = 5;
    m_animFrames = 0.0f;
    memset(unk1BC, 0, sizeof(unk1BC));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grFzeroNode::~grFzeroNode() {
}

void grFzeroNode::processAnim() {
    Ground::processAnim();
}

void grFzeroNode::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        if (m_mtxGimmickWork != NULL) {
            getNodeMatrix(m_mtxGimmickWork + 24, 0, m_node[0]);
            getNodeMatrix(m_mtxGimmickWork + 25, 0, m_node[1]);
            getNodeMatrix(m_mtxGimmickWork + 26, 0, m_node[2]);
            getNodeMatrix(m_mtxGimmickWork + 27, 0, m_node[3]);
            getNodeMatrix(m_mtxGimmickWork + 28, 0, m_node[4]);
            getNodeMatrix(m_mtxGimmickWork + 29, 0, m_node[5]);
            getNodeMatrix(m_mtxGimmickWork + 30, 0, m_node[6]);
            getNodeMatrix(m_mtxGimmickWork + 31, 0, m_node[7]);
            getNodeMatrix(m_mtxGimmickWork + 32, 0, m_node[8]);
            getNodeMatrix(m_mtxGimmickWork + 33, 0, m_node[9]);
            getNodeMatrix(m_mtxGimmickWork + 34, 0, m_node[10]);
            getNodeMatrix(m_mtxGimmickWork + 35, 0, m_node[11]);
            getNodeMatrix(m_mtxGimmickWork + 36, 0, m_node[12]);
            getNodeMatrix(m_mtxGimmickWork + 37, 0, m_node[13]);
            getNodeMatrix(m_mtxGimmickWork + 38, 0, m_node[14]);
        }
    }
}

// The animation that belongs to a course section (5 = the parked pose).
static inline u32 grFzeroNodeMotionOfScene(u32 scene) {
    if (scene == 0) {
        scene = 1;
    } else if (scene == 2) {
        scene = 2;
    } else if (scene == 3) {
        scene = 2;
    } else if (scene == 4) {
        scene = 3;
    } else if (scene == 5) {
        scene = 4;
    } else {
        scene = (scene == 6) ? 0 : 5;
    }
    return scene;
}

void grFzeroNode::updateActive(float deltaFrame) {
    u8 state = m_state;
    m_motionTimer += deltaFrame;
    switch (state) {
    case 0:
        setMotion(5, 0, 1, 0);
        m_state = 1;
        // fall through
    case 1: {
        u8* stateWork = m_stateWork;
        if (*stateWork == 6) {
            u8 motion = grFzeroNodeMotionOfScene(*m_sceneWork);
            bool keep = false;
            if (motion == 5) {
                keep = true;
            } else {
                u8 current = m_motionId;
                if (motion == current) {
                    keep = true;
                } else if (motion == 0) {
                    if (m_motionTimer < 3600.0f && current == 4) {
                        keep = true;
                    }
                } else if (m_motionTimer < 3600.0f && current == (u8)(motion - 1)) {
                    keep = true;
                }
            }
            if (!keep) {
                *stateWork = 7;
                m_state = 2;
            } else {
                *stateWork = 8;
            }
        }
        break;
    }
    case 2:
        if (*m_stateWork == 7) {
            u8 motion = grFzeroNodeMotionOfScene(*m_sceneWork);
            setMotion(motion, 0, 1, &m_animFrames);
            *m_motionWork = motion;
            m_motionId = motion;
            m_motionTimer = 0.0f;
            m_state = 3;
        }
        break;
    case 3:
        if (getMotionFrame(0) >= m_animFrames) {
            setMotion(5, 0, 1, 0);
            *m_motionWork = 5;
            *m_stateWork = 8;
            m_state = 1;
        }
        break;
    }
}

// The model follows the stage matrix.
void grFzeroNode::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_mtxWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

bool grFzeroNode::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "FZmachine01");
    getNodeIndex(&m_node[1], 0, "FZmachine02");
    getNodeIndex(&m_node[2], 0, "FZmachine03");
    getNodeIndex(&m_node[3], 0, "FZmachine04");
    getNodeIndex(&m_node[4], 0, "FZmachine05");
    getNodeIndex(&m_node[5], 0, "FZmachine06");
    getNodeIndex(&m_node[6], 0, "FZmachine07");
    getNodeIndex(&m_node[7], 0, "FZmachine08");
    getNodeIndex(&m_node[8], 0, "FZmachine09");
    getNodeIndex(&m_node[9], 0, "FZmachine10");
    getNodeIndex(&m_node[10], 0, "FZmachine11");
    getNodeIndex(&m_node[11], 0, "FZmachine12");
    getNodeIndex(&m_node[12], 0, "FZmachine13");
    getNodeIndex(&m_node[13], 0, "FZmachine14");
    getNodeIndex(&m_node[14], 0, "FZmachine15");
    return result;
}

// Binds the animation of the machines (five of them: the four course sections and the parked pose).
void grFzeroNode::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_animId == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_animId = animId;

    if (animId >= 5) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grFzeroSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grFzeroSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grFzeroSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grFzeroSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grFzeroSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
