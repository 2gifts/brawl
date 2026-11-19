#include <cm/cm_camera_controller.h>
#include <ec/ec_mgr.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/math/math_triangular.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdDome* grHalberdDome::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdDome* ground = new (Heaps::StageInstance) grHalberdDome(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdDome::grHalberdDome(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_posTrainerWork = NULL;
    m_motion = 1;
    m_motionTimer = 0.0f;
    m_joint[0] = NULL;
    m_joint[1] = NULL;
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_node[3] = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    m_seId[0] = snd_se_stage_Halberd_Doc;
    m_seId[1] = snd_se_stage_Halberd_Doc_open;
    m_seId[2] = snd_se_stage_Halberd_Doc_finish;
    m_seData[0].id = snd_se_stage_Halberd_Doc;
    m_seData[0].unk4 = 0.0f;
    m_seData[0].unk8 = 0.0f;
    m_seData[0].unkC = 0.0f;
    m_seData[1].id = snd_se_stage_Halberd_Doc_open;
    m_seData[1].unk4 = 0.0f;
    m_seData[1].unk8 = 0.0f;
    m_seData[1].unkC = 480.0f;
    m_seData[2].id = snd_se_stage_Halberd_Doc_finish;
    m_seData[2].unk4 = 0.0f;
    m_seData[2].unk8 = 480.0f;
    m_seData[2].unkC = 0.0f;
    m_seSeq.registId(m_seId, 3);
    m_seSeq.registSeq(0, m_seData, 3, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
}

grHalberdDome::~grHalberdDome() {
}

void grHalberdDome::processAnim() {
    Ground::processAnim();
    if (m_mtxGimmickWork != NULL) {
        getNodeMatrix(&m_mtxGimmickWork[3], 0, "DomeWarningPosition");
    }
}

void grHalberdDome::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateCollision(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        switch (*m_stateWork) {
        case 2:
            if (*m_frameWork < 720.0f && m_posTrainerWork != NULL) {
                getNodePosition(&m_posTrainerWork[0], 0, m_node[0]);
                getNodePosition(&m_posTrainerWork[1], 0, m_node[1]);
                getNodePosition(&m_posTrainerWork[2], 0, m_node[2]);
                getNodePosition(&m_posTrainerWork[3], 0, m_node[3]);
            }
            break;
        }
    }
}

// Finds the two joints of the dome floor ("D_maindome" and the gate) the first time they exist, then opens and closes the
// ledges of the dome with the frame of the animation.
void grHalberdDome::updateJoint(float deltaFrame) {
    if (m_joint[0] == NULL || m_joint[1] == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 nodeGate;
            u32 nodeDome;
            if (getNodeIndex(&nodeDome, 0, "D_maindome") && getNodeIndex(&nodeGate, 0, "D_dodai_gate01")) {
                u32 jointNum = (u16)collision->m_jointLen;
                for (u32 i = 0; i != jointNum; i++) {
                    grCollisionJoint* joint = collision->getJoint(i);
                    if (joint == NULL) {
                        break;
                    }
                    if (joint->m_ground == this && joint->_0x4E == 0) {
                        if (nodeDome == joint->m_nodeIndex) {
                            joint->m_0x56_7 = true;
                            grHalberdJointDisable(joint);
                            m_joint[0] = joint;
                        } else if (nodeGate == joint->m_nodeIndex) {
                            joint->m_0x56_7 = true;
                            grHalberdJointDisable(joint);
                            m_joint[1] = joint;
                        }
                    }
                    if (m_joint[0] != NULL && m_joint[1] != NULL) {
                        return;
                    }
                }
            }
        }
    } else if (getStageData() != NULL) {
        if (*m_stateWork == 2) {
            if (*m_frameWork <= 170.0f) {
                m_joint[0]->m_0x55_3 = false;
                m_joint[1]->m_0x55_3 = false;
            } else {
                m_joint[0]->m_0x55_3 = true;
                m_joint[1]->m_0x55_3 = true;
            }
        } else {
            m_joint[0]->m_0x55_3 = false;
            m_joint[1]->m_0x55_3 = false;
        }
    }
}

// The dome floor is solid while it is closed; while the dome opens (frame 170 - 435) the camera is lifted to the gate.
void grHalberdDome::updateCollision(float deltaFrame) {
    if (m_frameWork != NULL && m_stateWork != NULL && m_joint[0] != NULL && m_joint[1] != NULL) {
        int state = *m_stateWork;
        switch (state) {
        case 2:
            {
                float frame = *m_frameWork;
                if (frame > 720.0f) {
                    grHalberdJointDisable(m_joint[0]);
                    grHalberdJointDisable(m_joint[1]);
                } else if (frame > 435.0f) {
                    grHalberdJointEnable(m_joint[0]);
                    grHalberdJointEnable(m_joint[1]);
                } else if (frame > 170.0f) {
                    grHalberdJointEnable(m_joint[0]);
                    grHalberdJointDisable(m_joint[1]);
                    m_joint[0]->m_0x55_3 = true;
                    m_joint[1]->m_0x55_3 = true;
                } else {
                    grHalberdJointEnable(m_joint[0]);
                    grHalberdJointEnable(m_joint[1]);
                    m_joint[0]->m_0x55_3 = false;
                    m_joint[1]->m_0x55_3 = false;
                }
                // MATCH-ONLY: the camera controller keeps its current range at +0x148 and an extra lift at +0x18C; the
                // header names neither.
                if (*m_frameWork > 720.0f) {
                    CameraController* camera = CameraController::getInstance();
                    Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
                    CameraController::getInstance()->setCameraRange(&range);
                    camera = CameraController::getInstance();
                    *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x18C) = 0.0f;
                } else {
                    float lift = 0.0f;
                    Vec3f gate;
                    getNodePosition(&gate, 0, "D_dodai_gate01");
                    CameraController* camera = CameraController::getInstance();
                    Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
                    if (range.m_down < gate.m_y) {
                        range.m_down = gate.m_y;
                        cmStageParam* param = &CameraController::getInstance()->m_stageCameraParam;
                        if (param != NULL) {
                            float cosine = nw4r::math::CosFIdx(10.666667f);
                            float sine = nw4r::math::SinFIdx(10.666667f);
                            range.m_down = range.m_down - param->m_verticalRotationFactor * (sine / cosine) * 0.5f;
                        }
                        lift = gate.m_y - range.m_down;
                    }
                    CameraController::getInstance()->setCameraRange(&range);
                    *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x18C) = lift;
                }
            }
            break;
        case 3:
            grHalberdJointDisable(m_joint[0]);
            grHalberdJointDisable(m_joint[1]);
            break;
        case 4:
            grHalberdJointDisable(m_joint[0]);
            grHalberdJointDisable(m_joint[1]);
            break;
        }
    }
}

void grHalberdDome::updateMotion(float deltaFrame) {
    if (m_frameWork != NULL && m_stateWork != NULL) {
        m_motionTimer -= deltaFrame;
        if (m_motionTimer < 0.0f) {
            m_motionTimer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, 0, 1, NULL);
            getNodeIndex(&m_node[0], 0, "ptPosition01Dome");
            getNodeIndex(&m_node[1], 0, "ptPosition02Dome");
            getNodeIndex(&m_node[2], 0, "ptPosition03Dome");
            getNodeIndex(&m_node[3], 0, "ptPosition04Dome");
            m_state = 2;
            break;
        case 2:
            if (*m_stateWork == 2) {
                setMotion(0, 0, 1, &m_motionTimer);
                setMotionFrame(*m_frameWork, 0);
                m_state = 3;
            }
            break;
        case 3: {
            Vec3f pos(0.0f, 0.0f, 0.0f);
            m_snd.setPos(&pos);
            m_seSeq.playFrame(0, getMotionFrame(0));
            if (getMotionFrame(0) >= m_motionTimer) {
                m_state = 4;
            }
            break;
        }
        }
    }
}

void grHalberdDome::updateCallBack(float deltaFrame) {
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
                m_mtxWork->m[1][3] += 42.3864f;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

// The animation of the dome (only the first one is played).
void grHalberdDome::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
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
    m_motion = animId;

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// TODO: clean up, give variables real names
namespace jj_5AF4 {
typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

class grCollisionJoint {
public:
    u8 pad0[0x4e];
    u16 unk4E;
    u16 unk50;
    u8 pad52[2];
    u8 b0 : 1;
    u8 b1 : 1;
    u8 b2 : 1;
    u8 b3 : 1;
    u8 rest : 4;
    u8 unk55;
    u8 unk56;
    u8 pad57;
    void* unk58;
};

class grCollision {
public:
    u8 pad0[6];
    u16 unk6;
};

extern "C" grCollisionJoint* getJoint__11grCollisionFUs(grCollision*, unsigned int);

struct Ground_base { char pad[0x3c]; };
class Ground : public Ground_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim__13grHalberdDomeFv();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
    virtual void __dt__13grHalberdDomeFv();
    virtual void update__13grHalberdDomeFf();
    virtual void loadGroundData();
    virtual void bindData();
    virtual void setupMelee();
    virtual void setVisibility();
    virtual void setVisibilityByClipping();
    virtual void setVisibilityAttachedEffect();
    virtual void receiveCollMsg();
    virtual void receiveCollMsg_Landing();
    virtual void receiveCollMsg_Heading();
    virtual void receiveCollMsg_Wall();
    virtual void receiveCollMsg_Attack();
    virtual void unloadData();
    virtual void getModelCount__6GroundFv();
    virtual void startup();
    virtual void setNode();
    virtual void setStageData__6GroundFPv();
    virtual int getStageData__6GroundFv();
    int getNodeIndex(unsigned long* out, unsigned long flag, const char* name);

    char pad0[0xC];
    grCollision* collision;
    char pad1[0x108];
    f32* unk158;
    s8* unk15C;
    char pad2[0x14];
    grCollisionJoint* joint1;
    grCollisionJoint* joint2;
};

extern const char lbl_57_data_E6C[];
extern const char lbl_57_data_E78[];
extern f32 lbl_57_rodata_1A4[4];

extern "C" void updateJoint__13grHalberdDomeFf(Ground* self) {
    unsigned long nodeIndex2;
    unsigned long nodeIndex1;

    if (self->joint1 != 0 && self->joint2 != 0) {
        if (self->getStageData__6GroundFv() != 0) {
            switch (*self->unk15C) {
            case 2:
                if (*self->unk158 > lbl_57_rodata_1A4[0]) {
                    self->joint1->unk55 |= 8;
                    self->joint2->unk55 |= 8;
                } else {
                    self->joint1->unk55 &= ~8;
                    self->joint2->unk55 &= ~8;
                }
                break;
            default:
                self->joint1->unk55 &= ~8;
                self->joint2->unk55 &= ~8;
                break;
            }
        }
    } else {
        grCollision* collision = self->collision;
        if (collision != 0
            && self->getNodeIndex(&nodeIndex1, 0, lbl_57_data_E6C) != 0
            && self->getNodeIndex(&nodeIndex2, 0, lbl_57_data_E78) != 0) {
            unsigned int i;
            unsigned int count = collision->unk6;
            for (i = 0; i != count; i++) {
                grCollisionJoint* joint = getJoint__11grCollisionFUs(collision, i);
                if (joint == 0) {
                    return;
                }
                if (joint->unk58 == self && joint->unk4E == 0) {
                    if (nodeIndex1 == joint->unk50) {
                        joint->unk56 |= 0x80;
                        joint->b0 = 0;
                        joint->b3 = joint->b0;
                        joint->b1 = joint->b3;
                        self->joint1 = joint;
                    } else if (nodeIndex2 == joint->unk50) {
                        joint->unk56 |= 0x80;
                        joint->b0 = 0;
                        joint->b3 = joint->b0;
                        joint->b1 = joint->b3;
                        self->joint2 = joint;
                    }
                }
                if (self->joint1 != 0 && self->joint2 != 0) {
                    break;
                }
            }
        }
    }
}
}

