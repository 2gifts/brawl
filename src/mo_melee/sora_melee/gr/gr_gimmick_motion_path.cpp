#include <st/stage.h>
#include <string.h>
#include <types.h>
#include <gr/gr_gimmick_motion_path.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale);
nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView,
                                    void (*callback)());
extern "C" void fn_801B0A28();
extern const float lbl_27_rodata_43C0[2];

void grGimmickMotionPath::setTRS() {
    nw4r::g3d::ScnMdl* model = m_sceneModels[unk178];
    if (model) {
        Matrix matrix(true);
        if (model->GetScnMtxPos(&matrix, nw4r::g3d::ScnObj::MTX_WORLD, m_nodeIndex)) {
            Vec3f position = matrix.getTranslation();
            Vec3f rotation;
            Vec3f scale;
            matrix.getRotate(&rotation);
            getScalingFactor(&matrix, &scale);
            float rotationX, rotationY, rotationZ;
            rotationX = rotation.m_x;
            rotationY = rotation.m_y;
            rotationX = lbl_27_rodata_43C0[0] * rotationX;
            rotationZ = rotation.m_z;
            rotationY = lbl_27_rodata_43C0[0] * rotationY;
            rotationZ = lbl_27_rodata_43C0[0] * rotationZ;
            rotation.m_x = rotationX;
            rotation.m_y = rotationY;
            rotation.m_z = rotationZ;
            setPos(&position);
            setRot(&rotation);
            setScale(&scale);
        }
    }
}

void grGimmickMotionPath::playDefaultSE() {
    if (m_soundEffects) {
        for (int index = 0; index < 3; ++index) {
            startGimmickSE(index);
        }
    }
    if (m_effects) {
        for (int index = 0; index < 3; ++index) {
            startGimmickEffect(index);
        }
    }
}

grGimmickMotionPath* grGimmickMotionPath::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grGimmickMotionPath* ground = new (Heaps::StageInstance) grGimmickMotionPath(taskName, nodeName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

void grGimmickMotionPath::setFrameUpdate(float rate) {
    float frame = g_stGameFrame.m_frameDelta;
    float speed = m_motionRatio * (rate * frame);
    m_frameSpeed = speed;
    m_frameStep = speed;
}

grGimmickMotionPath::grGimmickMotionPath(const char* taskName, const char* nodeName) : grGimmick(taskName) {
    m_state = 0;
    m_frame = 0.0f;
    unk154 = 0.0f;
    m_frameCount = 0;
    unk168 = 1.0f;
    unk16C = 1.0f;
    unk170 = 1.0f;
    m_endThreshold = 0.001f;
    unk178 = 0;
    m_nodeIndex = 0;
    memset(m_nodeName, 0, sizeof(m_nodeName));
    strncpy(m_nodeName, nodeName, sizeof(m_nodeName) - 1);
    setFrameUpdate(0.0f);
    m_transparencySettings = 0;
}

grGimmickMotionPath::~grGimmickMotionPath() { }

void grGimmickMotionPath::createMotionPathSoundWork() {
    createSoundWork(0xb, 1);
}

void grGimmickMotionPath::createMotionPathEffectWork() {
    createEffectWork(0xb);
}

void grGimmickMotionPath::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    m_motionPathData = static_cast<grGimmickMotionPathData*>(getGimmickData());
    Ground::startup(data, unk1, layerType);
    m_frameCount = m_modelAnims[0]->getFrameCount();
    m_hasUpdatedG3dCalcWorld = true;
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(0);
    unk178 = 0;
    m_nodeIndex = Ground::getNodeIndex(0, m_nodeName);
    setFrameModelAnim();
    if (m_soundEffects) {
        for (int index = 0; index < 3; ++index) {
            startGimmickSE(index);
        }
    }
    if (m_effects) {
        for (int index = 0; index < 3; ++index) {
            startGimmickEffect(index);
        }
    }
}

void grGimmickMotionPath::update(float deltaFrame) {
    grGimmick::update(m_frameStep);
    switch (m_state) {
    case 1:
        m_frameSpeed = m_frameSpeed * unk168;
        break;
    }
}

void grGimmickMotionPath::fixedPosition(float unused) {
    grGimmick::fixedPosition(m_frameStep);
}

int grGimmickMotionPath::getModelCount() {
    return 1;
}

void grGimmickMotionPath::setFrameModelAnim() {
    nw4r::g3d::ScnMdl* model = m_sceneModels[0];
    gfModelAnimation* animation = m_modelAnims[0];
    if (model != NULL && animation != NULL) {
        animation->setFrame(m_frame);
        animation->setUpdateRate(0.0f);
        static_cast<nw4r::g3d::ScnMdlSimple*>(model)->UpdateFrame();
        m_hasUpdatedG3dCalcWorld = false;
        updateG3dProcCalcWorld();
        setTRS();
    }
}

bool grGimmickMotionPath::isMoveEnd() {
    return __fabs(m_frameSpeed) < m_endThreshold;
}

bool grGimmickMotionPath::isReturnEnd() {
    switch (*((u8*)m_motionPathData + 5)) { // MATCH-ONLY: byte read of the path mode
    case 1:
    case 2:
        return isMoveEnd();
    }
    return m_state == 0;
}

void grGimmickMotionPath::setFrame(float frame) {
    m_frame = frame;
}

float grGimmickMotionPath::getFrame() {
    return m_frame;
}

void grGimmickMotionPath::getTRS(Vec3f* pos, Vec3f* rotation, float* scale) {
    setFrameModelAnim();
    Vec3f currentPos = getPos();
    pos->m_x = currentPos.m_x;
    pos->m_y = currentPos.m_y;
    pos->m_z = currentPos.m_z;
    Vec3f currentRot = getRot();
    rotation->m_x = currentRot.m_x;
    rotation->m_y = currentRot.m_y;
    rotation->m_z = currentRot.m_z;
    Vec3f currentScale = getScale();
    *scale = currentScale.m_x;
}

void grGimmickMotionPath::startMove() {
    float ratio = m_motionPathData->m_motionRatio;
    m_motionRatio = ratio;
    float frame = g_stGameFrame.m_frameDelta;
    float speed = ratio * frame;
    m_state = 1;
    m_frameSpeed = speed;
    m_frameStep = speed;
    if (m_soundEffects) {
        for (int index = 0; index < 5; ++index) {
            startGimmickSE(index + 3);
        }
    }
    if (m_effects) {
        for (int index = 0; index < 5; ++index) {
            startGimmickEffect(index + 3);
        }
    }
}

void grGimmickMotionPath::startMove(float deltaFrame) {
    float speed = m_motionRatio * (deltaFrame * g_stGameFrame.m_frameDelta);
    m_state = 1;
    m_frameSpeed = speed;
    m_frameStep = speed;
    if (m_soundEffects) {
        for (int index = 0; index < 5; ++index) {
            startGimmickSE(index + 3);
        }
    }
    if (m_effects) {
        for (int index = 0; index < 5; ++index) {
            startGimmickEffect(index + 3);
        }
    }
}

void grGimmickMotionPath::stopStartSE() {
    if (m_soundEffects) {
        for (u32 index = 0; index < 5; ++index) {
            stopGimmickSE(index + 3);
        }
    }
    if (m_effects) {
        for (u32 index = 0; index < 5; ++index) {
            stopGimmickEffect(index + 3);
        }
    }
}

void grGimmickMotionPath::applyEndFrame() {
    m_frameCount = m_modelAnims[0]->getFrameCount();
}

bool grGimmickMotionPath::isStartFrame() {
    return m_frame <= m_frameSpeed;
}

bool grGimmickMotionPath::isEndFrame() {
    return m_frame + m_frameSpeed >= (float)m_frameCount;
}

void grGimmickMotionPath::clearFrame() {
    m_state = 0;
    m_frame = 0.0f;
    m_frameSpeed = 0.0f;
    setFrameModelAnim();
}

// The motion path loads one model of the resource file (the one its data names) with its animation.
void grGimmickMotionPath::loadGroundData(nw4r::g3d::ResFile resFile, u32 flags, gfSceneRoot::LayerType layerType) {
    grGimmickMotionPathData* data = static_cast<grGimmickMotionPathData*>(getGimmickData());
    if (data != NULL) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
        m_resFile = resFile;
        m_modelAnimNum = m_resFile.GetResMdlNumEntries();
        m_sceneModels = new (Heaps::StageInstance) nw4r::g3d::ScnMdl*;
        m_modelAnims = new (Heaps::StageInstance) gfModelAnimation*;
        nw4r::g3d::ResMdl resMdl = resFile.GetResMdl((u8)data->m_index);
        u32 size;
        nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, resMdl, flags, 1, fn_801B0A28);
        nw4r::g3d::ResMdl animMdl = resMdl;
        gfModelAnimation* modelAnim = new (Heaps::StageInstance) gfModelAnimation(&resFile, &animMdl, true, static_cast<u32>((u8)data->m_index), Heaps::StageInstance);
        modelAnim->_spacer[0] = 0;
        gfModelAnimation::bind(scnMdl, modelAnim);
        m_sceneModels[0] = scnMdl;
        m_modelAnims[0] = modelAnim;
    }
}

void grGimmickMotionPath::processAnim() {
    float frameDelta = g_stGameFrame.m_frameDelta;
    if (m_state != 0) {
        nw4r::g3d::ScnMdl* model = m_sceneModels[0];
        gfModelAnimation* animation = m_modelAnims[0];
        if (model != NULL && animation != NULL) {
            switch (m_state) {
            case 1:
                m_frame = m_frame + m_frameSpeed;
                switch (*((u8*)m_motionPathData + 5)) { // MATCH-ONLY: byte read of the path mode
                case 1:
                    if (m_frame >= (float)m_frameCount) {
                        if (m_soundEffects) {
                            for (int index = 0; index < 3; ++index) {
                                startGimmickSE(index + 8);
                            }
                        }
                        if (m_effects) {
                            for (int index = 0; index < 3; ++index) {
                                startGimmickEffect(index + 8);
                            }
                        }
                        m_frame = m_frame - (float)m_frameCount;
                        if (m_soundEffects) {
                            for (int index = 0; index < 5; ++index) {
                                startGimmickSE(index + 3);
                            }
                        }
                        if (m_effects) {
                            for (int index = 0; index < 5; ++index) {
                                startGimmickEffect(index + 3);
                            }
                        }
                    } else if (m_frame <= 0.0f) {
                        m_frame = m_frame + (float)m_frameCount;
                    }
                    break;
                case 0:
                    if (m_frame >= (float)m_frameCount || isMoveEnd() == 1) {
                        if (m_frame > (float)m_frameCount) {
                            m_frame = (float)m_frameCount;
                        }
                        m_state = 2;
                        m_frameSpeed = 0.0f;
                        unk154 = unk16C;
                        if (m_soundEffects) {
                            for (int index = 0; index < 3; ++index) {
                                startGimmickSE(index + 8);
                            }
                        }
                        if (m_effects) {
                            for (int index = 0; index < 3; ++index) {
                                startGimmickEffect(index + 8);
                            }
                        }
                    }
                    break;
                case 2:
                    if (m_frame >= (float)m_frameCount || isMoveEnd() == 1) {
                        m_state = 0;
                        m_frameSpeed = 0.0f;
                        if (m_frame > (float)m_frameCount) {
                            m_frame = (float)m_frameCount;
                        }
                        if (m_soundEffects) {
                            for (int index = 0; index < 3; ++index) {
                                startGimmickSE(index + 8);
                            }
                        }
                        if (m_effects) {
                            for (int index = 0; index < 3; ++index) {
                                startGimmickEffect(index + 8);
                            }
                        }
                    } else if (m_frame <= 0.0f) {
                        m_frameSpeed = 0.0f;
                        m_state = 0;
                        m_frame = 0.0f;
                    }
                    break;
                }
                animation->setFrame(m_frame);
                animation->setUpdateRate(0.0f);
                static_cast<nw4r::g3d::ScnMdlSimple*>(model)->UpdateFrame();
                break;
            case 2:
                unk154 = unk154 - frameDelta * m_motionRatio;
                if (unk154 <= 0.0f) {
                    m_state = 3;
                    m_frameSpeed = m_motionRatio * (unk170 * g_stGameFrame.m_frameDelta);
                    m_frameStep = m_frameSpeed;
                }
                break;
            case 3:
                m_frame = m_frame - m_frameSpeed;
                if (m_frame <= 0.0f) {
                    m_frameSpeed = 0.0f;
                    m_frame = 0.0f;
                    m_state = 0;
                }
                animation->setFrame(m_frame);
                animation->setUpdateRate(0.0f);
                static_cast<nw4r::g3d::ScnMdlSimple*>(model)->UpdateFrame();
                break;
            }
            m_hasUpdatedG3dCalcWorld = false;
            updateG3dProcCalcWorld();
            setTRS();
        }
    }
}
