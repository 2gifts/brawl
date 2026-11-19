#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: the Vec3f copies of this unit are aggregate word moves.
#define GR_GIMMICK_EFFECT_INFO_CTOR
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_motion_path.h>
#include <gf/gf_model.h>
#include <ec/ec_mgr.h>
#include <memory.h>
#include <st/stage.h>
#include <string.h>
#include <types.h>
#include <mu/mu_mat_color.h>
#include <nw4r/ut/ut_Color.h>

extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);

// HYPOTHESIS: the runtime type tag of nw4r's ScnMdl in main (the same cast the stage hit objects do).
extern int lbl_8040ABD8[];

static inline nw4r::g3d::ScnMdl* grDynamicCastScnMdl(nw4r::g3d::ScnMdl* obj) {
    bool isMdl = false;
    if (obj != NULL) {
        int* typeObj = lbl_8040ABD8;
        if (obj->IsDerivedFrom((int*)&typeObj)) {
            isMdl = true;
        }
    }
    return isMdl ? obj : NULL;
}

bool grVisibleProduction::isFinished() {
    return m_0x4 == VisibleProduction_Finished;
}

bool grVisibleProduction::isStandby() {
    return m_0x4 == VisibleProduction_Standby;
}

void grVisibleProduction::setStart() {
    setEnable(true);
    setState(1);
}

void grVisibleProduction::setEnable(const bool& enable) {
    m_enabled = enable;
}

void grGimmick::setAttachPathdataFrame(float frame) {
    if (m_gimmickMotionPath != NULL) {
        m_gimmickMotionPath->setFrame(frame);
    }
}

void grGimmick::setPos(float x, float y, float z) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_pos.m_x = x;
        data->m_pos.m_y = y;
        data->m_pos.m_z = z;
    }
}

void grGimmick::setRot(float x, float y, float z) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_rot.m_x = x;
        data->m_rot.m_y = y;
        data->m_rot.m_z = z;
    }
}

Vec3f grGimmick::getRot() {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        return data->m_rot;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

void grGimmick::setScale(float x, float y, float z) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_scale.m_x = x;
        data->m_scale.m_y = y;
        data->m_scale.m_z = z;
    }
}

Vec3f grGimmick::getScale() {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        return data->m_scale;
    }
    return Vec3f(1.0f, 1.0f, 1.0f);
}

void grGimmick::processUpdate() {
    Ground::processUpdate();
    if (m_resFile.ptr() != NULL) {
        Ground::updateG3dProcCalcWorld();
    } else {
        m_hasUpdatedG3dCalcWorld = true;
    }
    float ratio = m_motionRatio;
    float frame = g_stGameFrame.m_frameDelta;
    fixedPosition(frame * ratio);
}

void grGimmick::setTgtNode(const char* name) {
    if (name != NULL) {
        u32 len = strlen(name);
        if (len != 0) {
            // HYPOTHESIS: the original tests the name again before freeing the old copy.
            if (name != NULL) {
                delete m_tgtNode;
            }
            m_tgtNode = new (Heaps::StageInstance) char[len + 1];
            memset(m_tgtNode, 0, len + 1);
            strncpy(m_tgtNode, name, len);
        }
    }
}

char* grGimmick::getTgtNode() {
    return m_tgtNode;
}

void grGimmick::setMotionRatio(float ratio) {
    m_motionRatio = ratio;
    if (m_gimmickMotionPath != NULL) {
        m_gimmickMotionPath->setFrameUpdate(ratio);
    }
}

void grGimmick::setTransparencyFlag(char flag) {
    reinterpret_cast<char&>(m_transparencyFlag) = flag; // BYTES: the raw byte is stored, not a normalized bool
}

void grGimmick::dbDispInvalidatedByCameraClippingSphere() { }

void grGimmick::setTransparency(u32 flag, u32 settings) {
    if (settings & 0x10) {
        setVisibilityByClipping(flag);
    }
    if (settings & 1) {
        setEnableCollisionStatusByClipping(flag);
    }
    reinterpret_cast<char&>(m_transparencyFlag) = flag; // BYTES: the raw byte is stored, not a normalized bool
}

void grGimmick::setPos(Matrix* matrix) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = matrix->m[0][3];
        float y = matrix->m[1][3];
        data->m_pos.m_x = x;
        float z = matrix->m[2][3];
        data->m_pos.m_y = y;
        data->m_pos.m_z = z;
    }
}

void grGimmick::getMatrix(Matrix* matrix) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        *matrix = data->m_matrix;
    } else {
        matrix->setIdentity();
    }
}

void grGimmick::setMaterialAlpha(const char* name, u32 alpha) {
    nw4r::g3d::ResMdl resMdl;
    nw4r::g3d::ResMat resMat;
    nw4r::g3d::ResMatChan resMatChan;
    GXColor color = nw4r::ut::Color(0xFFFFFFFF);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        resMdl = scnMdl->m_resMdl;
        if (resMdl.IsValid()) {
            resMat = resMdl.GetResMat(name);
            if (resMat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat.ref().m_id);
                resMatChan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, false);
                if (resMatChan.IsValid()) {
                    resMatChan.GXGetChanMatColor(GX_ALPHA0, &color);
                    color.a = alpha;
                    resMatChan.GXSetChanMatColor(GX_ALPHA0, color);
                    resMat.DCStore(false);
                }
            }
        }
    }
}

void grGimmick::setPos(Vec3f* pos) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = pos->m_x;
        float y = pos->m_y;
        data->m_pos.m_x = x;
        float z = pos->m_z;
        data->m_pos.m_y = y;
        data->m_pos.m_z = z;
    }
}

void grGimmick::setRot(Vec3f* rot) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = rot->m_x;
        float y = rot->m_y;
        data->m_rot.m_x = x;
        float z = rot->m_z;
        data->m_rot.m_y = y;
        data->m_rot.m_z = z;
    }
}

void grGimmick::setScale(Vec3f* scale) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = scale->m_x;
        float y = scale->m_y;
        data->m_scale.m_x = x;
        float z = scale->m_z;
        data->m_scale.m_y = y;
        data->m_scale.m_z = z;
    }
}

void grGimmick::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    Ground::startup(archive, unk1, layerType);
}

Vec3f grGimmick::getPos() {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        if (data->m_flags & 8) {
            float z = data->m_matrix.m[2][3];
            float y = data->m_matrix.m[1][3];
            float x = data->m_matrix.m[0][3];
            data->m_pos.m_x = x;
            data->m_pos.m_y = y;
            data->m_pos.m_z = z;
        }
        return m_calcWorldCallBack.m_nodeCallbackDatas->m_pos;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

void grGimmick::setCalcuCallbackRoot(u32 index) {
    if ((int)m_calcWorldCallBack.m_numNodeCallbackData > 0) {
        m_calcWorldCallBack.m_nodeCallbackDatas->m_nodeIndex = 0;
        m_calcWorldCallBack.m_nodeCallbackDatas->m_flags = index;
    }
}

void grGimmick::fixedPosition(float deltaFrame) {
    updataGimmickSE(deltaFrame);
    updataGimmickEffect(deltaFrame);
}

void grGimmick::setMatrix(Matrix* matrix) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_matrix = *matrix;
    }
}

void grGimmick::createSimpleEffectData(SimpleEffectData* simpleEffectData, int effectId, const char* nodeName) {
    u32 nodeIndex = getNodeIndex(0, nodeName);
    simpleEffectData->m_id = effectId;
    simpleEffectData->m_repeatFrame = 0;
    simpleEffectData->m_nodeIndex = nodeIndex;
    simpleEffectData->m_endFrame = 0;
    simpleEffectData->m_0xc = 0;
}

void grGimmick::renderDebug() {
    gfCameraManager::getManager()->m_cameras[0].setGX();
}

void grGimmick::startGimmickEffect(u32 index) {
    if (m_effects[index].m_id != 0) {
        *(short*)&m_effects[index] = Effect_Info::State_Start; // HYPOTHESIS: halfword store
        m_effects[index].m_framesActive = 0.0f;
    }
}

void grGimmick::updateCallback(u32 index) {
    if ((int)m_calcWorldCallBack.m_numNodeCallbackData > 0) {
        nw4r::g3d::ScnMdl* model = m_sceneModels[index];
        if (model->m_calcWorldCallBack == NULL && model != NULL) {
            m_calcWorldCallBack.m_index = 0;
            model->m_calcWorldCallBack = &m_calcWorldCallBack;
            model->EnableScnMdlCallbackTiming(1);
            model->m_nodeIndex = m_calcWorldCallBack.m_nodeCallbackDatas[0].m_nodeIndex;
        }
    }
}

void grGimmick::stopGimmickSE(u32 index) {
    if (m_soundEffects[index].m_handleId >= 0) {
        m_soundGenerators[(u16)m_soundEffects[index].m_generatorIndex].stopSE(m_soundEffects[index].m_handleId, 0);
        Effect_Info* info = &m_soundEffects[index];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
    }
}

void grGimmick::stopGimmickEffect(u32 index) {
    if (m_effects[index].m_handleId >= 0) {
        g_ecMgr->endEffect(m_effects[index].m_handleId);
        Effect_Info* info = &m_effects[index];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
    }
}

void grGimmick::setSimpleEffectVisibleProduction() {
    static u32 visibleProductionIndex = 4;
    SimpleEffectData simpleEffectData;
    createSimpleEffectData(&simpleEffectData, 0x1030006, "effect_locator");
    createEffectVisibleProductionForExcel(&simpleEffectData, &visibleProductionIndex, m_visibleProductions);
}

void grGimmick::createEffectWork(int numEffects) {
    if (m_effects != NULL) {
        delete[] m_effects;
        m_effects = NULL;
    }
    m_effectNum = numEffects;
    m_effects = new (Heaps::StageInstance) Effect_Info[numEffects];
    for (u32 i = 0; i < m_effectNum; i++) {
        Effect_Info* info = &m_effects[i];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
        m_effects[i].m_repeatFrame = 0;
        m_effects[i].m_id = 0;
        m_effects[i].m_scale = 1.0f;
    }
}

void grGimmick::createSoundWork(u32 unk1, u32 unk2) {
    if (m_soundGenerators != NULL) {
        delete[] m_soundGenerators;
        m_soundGenerators = NULL;
    }
    if (m_soundEffects != NULL) {
        delete[] m_soundEffects;
        m_soundEffects = NULL;
    }
    m_soundGeneratorNum = unk2;
    m_soundGenerators = new (Heaps::StageInstance) snd3DGenerator[unk2];
    m_soundEffectNum = unk1;
    m_soundEffects = new (Heaps::StageInstance) Effect_Info[unk1];
    for (u32 i = 0; i < m_soundEffectNum; i++) {
        Effect_Info* info = &m_soundEffects[i];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
        m_soundEffects[i].m_repeatFrame = 0;
        m_soundEffects[i].m_id = -1;
        m_soundEffects[i].m_scale = 1.0f;
    }
}

static inline void loadChrRes(gfModelAnimation* modelAnim, nw4r::g3d::ResMdl resMdl, u32 index) {
    if (index < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        int instanceSize;
        nw4r::g3d::ResAnmChr anim = modelAnim->m_resFile.GetResAnmChr(index);
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
        if (anim.IsValid()) {
            nw4r::g3d::AnmObjChrRes* anmObj = nw4r::g3d::AnmObjChrRes::Construct(allocator, &instanceSize, anim, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjChrRes != NULL) {
                    modelAnim->m_anmObjChrRes->Destroy();
                }
                modelAnim->m_anmObjChrRes = anmObj;
            }
        }
    }
}

static inline void loadChr(gfModelAnimation* modelAnim, nw4r::g3d::ScnMdl* sceneModel, u32 index) {
    loadChrRes(modelAnim, static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl, index);
}

void grGimmick::changeNodeAnim(u32 index, u32 modelAnimIndex) {
    if (m_modelAnims != NULL) {
        loadChr(m_modelAnims[modelAnimIndex], m_sceneModels[modelAnimIndex], index);
        m_modelAnims[modelAnimIndex]->bindNodeAnim(m_sceneModels[modelAnimIndex]);
        m_modelAnims[modelAnimIndex]->setFrame(0.0f);
    }
}

static inline void loadVisRes(gfModelAnimation* modelAnim, nw4r::g3d::ResMdl resMdl, u32 index) {
    if (index < modelAnim->m_resFile.GetResAnmVisNumEntries()) {
        int instanceSize;
        nw4r::g3d::ResAnmVis anim = modelAnim->m_resFile.GetResAnmVis(index);
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
        if (anim.IsValid()) {
            nw4r::g3d::AnmObjVisRes* anmObj = nw4r::g3d::AnmObjVisRes::Construct(allocator, &instanceSize, anim, resMdl);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjVisRes != NULL) {
                    modelAnim->m_anmObjVisRes->Destroy();
                }
                modelAnim->m_anmObjVisRes = anmObj;
            }
        }
    }
}

static inline void loadVis(gfModelAnimation* modelAnim, nw4r::g3d::ScnMdl* sceneModel, u32 index) {
    loadVisRes(modelAnim, static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl, index);
}

void grGimmick::changeVisibleAnim(u32 index, u32 modelAnimIndex) {
    if (m_modelAnims != NULL) {
        loadVis(m_modelAnims[modelAnimIndex], m_sceneModels[modelAnimIndex], index);
        m_modelAnims[modelAnimIndex]->bindVisibleAnim(m_sceneModels[modelAnimIndex]);
        m_modelAnims[modelAnimIndex]->setFrame(0.0f);
    }
}

static inline void loadTexPatRes(gfModelAnimation* modelAnim, nw4r::g3d::ResMdl resMdl, u32 index) {
    if (index < modelAnim->m_resFile.GetResAnmTexPatNumEntries()) {
        int instanceSize;
        nw4r::g3d::ResAnmTexPat anim = modelAnim->m_resFile.GetResAnmTexPat(index);
        if (anim.IsValid()) {
            MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
            nw4r::g3d::AnmObjTexPatRes* anmObj = nw4r::g3d::AnmObjTexPatRes::Construct(allocator, &instanceSize, anim, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjTexPatRes != NULL) {
                    modelAnim->m_anmObjTexPatRes->Destroy();
                }
                modelAnim->m_anmObjTexPatRes = anmObj;
            }
        }
    }
}

static inline void loadTexPat(gfModelAnimation* modelAnim, nw4r::g3d::ScnMdl* sceneModel, u32 index) {
    loadTexPatRes(modelAnim, static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl, index);
}

void grGimmick::changeTexAnim(u32 index, u32 modelAnimIndex) {
    if (m_modelAnims != NULL) {
        loadTexPat(m_modelAnims[modelAnimIndex], m_sceneModels[modelAnimIndex], index);
        m_modelAnims[modelAnimIndex]->bindTexAnim(m_sceneModels[modelAnimIndex]);
        m_modelAnims[modelAnimIndex]->setFrame(0.0f);
    }
}

static inline void loadTexSrtRes(gfModelAnimation* modelAnim, nw4r::g3d::ResMdl resMdl, u32 index) {
    if (index < modelAnim->m_resFile.GetResAnmTexSrtNumEntries()) {
        int instanceSize;
        nw4r::g3d::ResAnmTexSrt anim = modelAnim->m_resFile.GetResAnmTexSrt(index);
        if (anim.IsValid()) {
            MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
            nw4r::g3d::AnmObjTexSrtRes* anmObj = nw4r::g3d::AnmObjTexSrtRes::Construct(allocator, &instanceSize, anim, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjTexSrtRes != NULL) {
                    modelAnim->m_anmObjTexSrtRes->Destroy();
                }
                modelAnim->m_anmObjTexSrtRes = anmObj;
            }
        }
    }
}

static inline void loadTexSrt(gfModelAnimation* modelAnim, nw4r::g3d::ScnMdl* sceneModel, u32 index) {
    loadTexSrtRes(modelAnim, static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl, index);
}

void grGimmick::changeTexSrtAnim(u32 index, u32 modelAnimIndex) {
    if (m_modelAnims != NULL) {
        loadTexSrt(m_modelAnims[modelAnimIndex], m_sceneModels[modelAnimIndex], index);
        m_modelAnims[modelAnimIndex]->bindTexSrtAnim(m_sceneModels[modelAnimIndex]);
        m_modelAnims[modelAnimIndex]->setFrame(0.0f);
    }
}

static inline void loadClrRes(gfModelAnimation* modelAnim, nw4r::g3d::ResMdl resMdl, u32 index) {
    if (index < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        int instanceSize;
        nw4r::g3d::ResAnmClr anim = modelAnim->m_resFile.GetResAnmClr(index);
        if (anim.IsValid()) {
            MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
            nw4r::g3d::AnmObjMatClrRes* anmObj = nw4r::g3d::AnmObjMatClrRes::Construct(allocator, &instanceSize, anim, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjMatClrRes != NULL) {
                    modelAnim->m_anmObjMatClrRes->Destroy();
                }
                modelAnim->m_anmObjMatClrRes = anmObj;
            }
        }
    }
}

static inline void loadClr(gfModelAnimation* modelAnim, nw4r::g3d::ScnMdl* sceneModel, u32 index) {
    loadClrRes(modelAnim, static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl, index);
}

void grGimmick::changeMatColAnim(u32 index, u32 modelAnimIndex) {
    if (m_modelAnims != NULL) {
        loadClr(m_modelAnims[modelAnimIndex], m_sceneModels[modelAnimIndex], index);
        m_modelAnims[modelAnimIndex]->bindMatColAnim(m_sceneModels[modelAnimIndex]);
        m_modelAnims[modelAnimIndex]->setFrame(0.0f);
    }
}

static inline void loadShpRes(gfModelAnimation* modelAnim, nw4r::g3d::ResMdl resMdl, u32 index) {
    if (index < modelAnim->m_resFile.GetResAnmShpNumEntries()) {
        int instanceSize;
        nw4r::g3d::ResAnmShp anim = modelAnim->m_resFile.GetResAnmShp(index);
        if (anim.IsValid()) {
            MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::StageInstance);
            nw4r::g3d::AnmObjShpRes* anmObj = nw4r::g3d::AnmObjShpRes::Construct(allocator, &instanceSize, anim, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjShpRes != NULL) {
                    anmObj->Destroy();
                }
                modelAnim->m_anmObjShpRes = anmObj;
            }
        }
    }
}

static inline void loadShp(gfModelAnimation* modelAnim, nw4r::g3d::ScnMdl* sceneModel, u32 index) {
    loadShpRes(modelAnim, static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl, index);
}

void grGimmick::changeShapeAnim(u32 index, u32 modelAnimIndex) {
    if (m_modelAnims != NULL) {
        m_modelAnims[modelAnimIndex]->unbindShapeAnim(m_sceneModels[0]);
        loadShp(m_modelAnims[modelAnimIndex], m_sceneModels[modelAnimIndex], index);
        m_modelAnims[modelAnimIndex]->bindShapeAnim(m_sceneModels[modelAnimIndex]);
        m_modelAnims[modelAnimIndex]->setFrame(0.0f);
        m_modelAnims[modelAnimIndex]->m_anmObjShpRes->SetUpdateRate(1.0f);
    }
}

// The base colour of a channel of a material of one scene model.

static inline bool grFindResMat(nw4r::g3d::ScnMdl* scnMdl, const char* name, nw4r::g3d::ResMat* out) {
    bool found;
    if (scnMdl != NULL) {
        nw4r::g3d::ResMdl resMdl = scnMdl->m_resMdl;
        *out = resMdl.GetResMat(name);
        found = true;
    } else {
        found = false;
    }
    return found;
}

u32 grGimmick::getMaterialColor(int* channel, const char* name, u32* sceneModelIndex) {
    GXColor color = {0, 0, 0, 0};
    nw4r::g3d::ScnMdl* scnMdl = grDynamicCastScnMdl(m_sceneModels[*sceneModelIndex]);
    nw4r::g3d::ResMat resMat;
    if (scnMdl != NULL) {
        {
            if (grFindResMat(scnMdl, name, &resMat)) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat.ref().m_id);
                nw4r::g3d::ResMatChan resMatChan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, false);
                resMatChan.GXGetChanMatColor((GXChannelID)*channel, &color);
                resMat.DCStore(false);
                return *reinterpret_cast<u32*>(&color);
            }
        }
    }
    return *reinterpret_cast<u32*>(&color);
}

u32 grGimmick::getMaterialTevColor(int* reg, const char* name, u32* sceneModelIndex) {
    GXColor color = {0, 0, 0, 0};
    nw4r::g3d::ScnMdl* scnMdl = grDynamicCastScnMdl(m_sceneModels[*sceneModelIndex]);
    nw4r::g3d::ResMat resMat;
    if (scnMdl != NULL) {
        {
            if (grFindResMat(scnMdl, name, &resMat)) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat.ref().m_id);
                nw4r::g3d::ResMatTevColor tevColor = access.GetResMatTevColor(false);
                tevColor.GXGetTevColor((GXTevRegID)*reg, &color);
                tevColor.DCStore(false);
                resMat.DCStore(false);
                return *reinterpret_cast<u32*>(&color);
            }
        }
    }
    return *reinterpret_cast<u32*>(&color);
}

void grGimmick::setMaterialColor(u32* channel, const char* name, u32* color, u32* sceneModelIndex) {
    nw4r::g3d::ScnMdl* scnMdl = grDynamicCastScnMdl(m_sceneModels[*sceneModelIndex]);
    nw4r::g3d::ResMat resMat;
    if (scnMdl != NULL) {
        {
            if (grFindResMat(scnMdl, name, &resMat)) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat.ref().m_id);
                nw4r::g3d::ResMatChan resMatChan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, false);
                resMatChan.GXSetChanMatColor((GXChannelID)*channel, *reinterpret_cast<GXColor*>(color));
                resMat.DCStore(false);
            }
        }
    }
}

void grGimmick::setMaterialTevColor(u32* reg, const char* name, u32* color, u32* sceneModelIndex) {
    nw4r::g3d::ScnMdl* scnMdl = grDynamicCastScnMdl(m_sceneModels[*sceneModelIndex]);
    nw4r::g3d::ResMat resMat;
    if (scnMdl != NULL) {
        {
            if (grFindResMat(scnMdl, name, &resMat)) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat.ref().m_id);
                nw4r::g3d::ResMatTevColor tevColor = access.GetResMatTevColor(false);
                tevColor.GXSetTevColor((GXTevRegID)*reg, *reinterpret_cast<GXColor*>(color));
                tevColor.DCStore(false);
                resMat.DCStore(false);
            }
        }
    }
}

void grGimmick::setPosGimmickEffect(u32 index, Vec3f* pos) {
    if (m_effects[index].m_id != 0) {
        Vec3f effectPos = *pos;
        g_ecMgr->setPos(m_effects[index].m_handleId, &effectPos);
    }
}

grGimmick::grGimmick(const char* taskName) :
    Ground(taskName), m_soundGeneratorNum(0), m_soundGenerators(NULL), m_soundEffectNum(0), m_soundEffects(NULL), m_effectNum(0),
    m_effects(NULL), unkB4(0.0f), m_tgtNode(NULL), m_gimmickMotionPath(NULL) {
    m_motionPathFlags = 0;
    m_transparencyFlag = true;
    m_transparencySettings = 0x1F;
    Vec3f clipCenter(0.0f, 0.0f, 0.0f);
    m_clipSphereCenter = clipCenter;
    unkF8 = 100.0f;
    m_gimmickData = NULL;
    m_unk1 = -1;
    m_nodeIndex = -1;
    m_isUpdate = false;
    m_isUseMdlNameForStartAnim = true;
    for (int i = 0; i < 10; i++) {
        m_visibleProductions[i] = NULL;
        m_visibleProductions[i + 10] = NULL;
    }
}

grGimmick::~grGimmick() {
    if (m_soundGenerators != NULL) {
        delete[] m_soundGenerators;
        m_soundGenerators = NULL;
    }
    if (m_soundEffects != NULL) {
        delete[] m_soundEffects;
        m_soundEffects = NULL;
    }
    if (m_effects != NULL) {
        delete[] m_effects;
        m_effects = NULL;
    }
    for (int i = 0; i < 10; i++) {
        if (m_visibleProductions[i] != NULL) {
            delete m_visibleProductions[i];
        }
        m_visibleProductions[i] = NULL;
        if (m_visibleProductions[i + 10] != NULL) {
            delete m_visibleProductions[i + 10];
        }
        m_visibleProductions[i + 10] = NULL;
    }
    if (m_tgtNode != NULL) {
        delete m_tgtNode;
    }
}

void grGimmick::update(float deltaFrame) {
    if (m_gimmickMotionPath != NULL) {
        if (m_motionPathFlags & 1) {
            setPos(&m_gimmickMotionPath->getPos());
        }
        if (m_motionPathFlags & 2) {
            setRot(&m_gimmickMotionPath->getRot());
        }
    }
    if (m_isSetup != true) {
        updateProduction(deltaFrame);
    }
}

#include <revolution/OS.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <snd/snd_3d_generator.h>
// TODO: clean up, give variables real names
namespace jj_26A5A4 {
extern "C" void __dl__FPv(void* p);

extern "C" void* fn_27_26A5A4(void* p, s32 n) {
    if (p != nullptr && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_26A664 {

struct SimpleSoundData {
    u32 id;
    u16 repeatFrame;
};

extern "C" grVisibleProduction* fn_27_271170(grGimmick* owner, const SimpleSoundData* data);

extern "C" void createSoundEffectVisibleProductionForExcel__9grGimmickFUlUlUl(
    grGimmick* owner, const SimpleSoundData* data, const u32* index, grVisibleProduction** outputs) {
    if (data->id != 0xFFFFFFFFUL) {
        outputs[*index] = fn_27_271170(owner, data);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_26ACA8 {

extern "C" {
extern unsigned char g_stIsJustGameFrame;
extern const float lbl_27_rodata_4388;
extern char lbl_27_data_56744[];

int isExistEffect__5ecMgrFUl(ecMgr*, int);
void fn_27_26D35C(void*, unsigned int);
}

struct Pair {
    float x, y;

    Pair(float a, float b) : x(a), y(b) {}
};

struct GimmickEffect {
    unsigned short state;
    unsigned char pad02[2];
    float timer;
    int handle;
    int effectID;
    int duration;
    int unk14;
    unsigned char pad18[2];
    unsigned short unk1A;
    Pair offset;
    unsigned char pad24[4];
};

struct Gimmick {
    void* unk00;
    unsigned char pad04[0xA8];
    unsigned int effectCount;
    GimmickEffect* effects;
};

extern "C" void updataGimmickEffect__9grGimmickFf(Gimmick* self, float dt) {
    if (g_stIsJustGameFrame == 0)
        return;

    int offset;
    unsigned int i = 0;
    offset = 0;

#define EFFECT ((GimmickEffect*)((char*)self->effects + offset))

    while (i < self->effectCount) {
        if (EFFECT->effectID != 0) {
            if (EFFECT->state == 1 &&
                (float)EFFECT->duration <= EFFECT->timer) {
                EFFECT->handle = g_ecMgr->setEffect((EfID)EFFECT->effectID);

                if (EFFECT->handle == 0)
                    OSReport(lbl_27_data_56744, self->unk00, EFFECT->effectID);

                EFFECT->state = 2;
                EFFECT->timer = lbl_27_rodata_4388;
            }

            if (EFFECT->handle >= 0 && EFFECT->state == 2) {
                fn_27_26D35C(self, i);

                if (isExistEffect__5ecMgrFUl(g_ecMgr, EFFECT->handle) == 0 &&
                    EFFECT->handle >= 0) {
                    g_ecMgr->endEffect(EFFECT->handle);
                    GimmickEffect& effect = *EFFECT;
                    effect.state = 0;
                    effect.timer = lbl_27_rodata_4388;
                    effect.handle = -1;
                    effect.unk1A = 0;
                    effect.offset = Pair(lbl_27_rodata_4388, lbl_27_rodata_4388);
                }

                if (EFFECT->unk14 != 0 &&
                    (float)EFFECT->unk14 <= EFFECT->timer &&
                    EFFECT->handle >= 0) {
                    g_ecMgr->endEffect(EFFECT->handle);
                    GimmickEffect& effect = *EFFECT;
                    effect.state = 0;
                    effect.timer = lbl_27_rodata_4388;
                    effect.handle = -1;
                    effect.unk1A = 0;
                    effect.offset = Pair(lbl_27_rodata_4388, lbl_27_rodata_4388);
                }
            }

            EFFECT->timer += dt;
        }

        offset += 0x28;
        ++i;
    }

#undef EFFECT
}
}

// TODO: clean up, give variables real names
namespace jj_26AF10 {

extern int lbl_27_data_5677C;
extern int lbl_27_data_56780;

class AttachedEffect {
public:
    virtual void v0();
    virtual void v1(float frame);
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual int v11();
    int field;
};

extern "C" void updateProduction__9grGimmickFf(grGimmick* self, float frame) {
    char* p = reinterpret_cast<char*>(self);
    int hasB = 0;
    int allOk = 1;
    for (int i = 0; i < 10; i++) {
        if (*reinterpret_cast<AttachedEffect**>(p + 0xFC) != 0) {
            (*reinterpret_cast<AttachedEffect**>(p + 0xFC))->v1(frame);
            if ((*reinterpret_cast<AttachedEffect**>(p + 0xFC))->v11() != 0) {
                (*reinterpret_cast<AttachedEffect**>(p + 0xFC))->field = lbl_27_data_5677C;
            }
        }
        if (*reinterpret_cast<AttachedEffect**>(p + 0x124) != 0) {
            hasB = 1;
            (*reinterpret_cast<AttachedEffect**>(p + 0x124))->v1(frame);
            if ((*reinterpret_cast<AttachedEffect**>(p + 0x124))->v11() != 0) {
                (*reinterpret_cast<AttachedEffect**>(p + 0x124))->field = lbl_27_data_56780;
            } else {
                allOk = 0;
            }
        }
        p += 4;
    }
    if (hasB != 0 && allOk != 0) {
        self->Ground::setVisibilityAttachedEffect(0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_26B02C {
struct Object {
    int unknown;
    int index;
};

typedef void (Object::*Callback)();

struct Table {
    Callback entries[4];
};

extern const Table lbl_27_rodata_4358;

extern "C" void fn_27_26B02C(Object* self) {
    Table table = lbl_27_rodata_4358;
    (self->*table.entries[self->index])();
}
}

// TODO: clean up, give variables real names
namespace jj_26B25C {
typedef unsigned char u8;
struct GimmickBase { char pad[0x3c]; };
class grGimmick : public GimmickBase {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual void v46();
    virtual void v47();
    virtual void v48();
    virtual void v49();
    virtual void v50();
    virtual void v51();
    virtual void v52();
    virtual void v53();
    virtual void v54();
    virtual void v55();
    virtual void v56();
    virtual void v57();
    virtual void v58();
    virtual void v59();
    virtual void v60();
    virtual void v61();
    virtual void v62();
    virtual void v63();
    virtual void v64();
    virtual void v65();
    virtual void v66();
    virtual void v67();
    virtual void v68();
    virtual void v69();
    virtual void v70();
    virtual void v71();
    virtual void v72();
    virtual void v73();
    virtual void v74();
    virtual void v75();
    virtual void v76();
    virtual void v77();
    virtual void v78();
    virtual const char* getTgtNode();
    char pad40[4];
    void** models;
    char pad48[0x78];
    int nodeIndex;
    int targetNodeIndex;
    u8 nodeSet;
};
extern "C" u8 setNode__9grGimmickFv(grGimmick* self) {
    if (self->nodeSet == 1) return 1;
    int valid = 0;
    int nodeId = valid;
    nw4r::g3d::ResMdl resource;
    void* model = *self->models;
    if (model == 0) return 0;
    resource = nw4r::g3d::ResMdl(*(void**)((char*)model + 0xe8));
    if (!resource.IsValid()) return 0;
    self->nodeIndex = nodeId;
    nw4r::g3d::ResNode node = resource.GetResNode(self->getTgtNode());
    if (node.IsValid()) {
        nodeId = node.IsValid() ? node.ptr()->m_nodeIndex : nodeId;
        self->targetNodeIndex = nodeId;
    }
    if (self->nodeIndex != 0xffffffff && self->targetNodeIndex != 0xffffffff) valid = 1;
    else valid = 0;
    if (valid != 0) self->nodeSet = 1;
    return self->nodeSet;
}
}

// TODO: clean up, give variables real names
namespace jj_26BB1C {
extern "C" void __dla__FPv(void*);
extern "C" void* __nwa__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void __ct__18grNodeCallbackDataFv();
extern "C" void* __construct_new_array(
    void*, void (*)(), void*, unsigned long, int);
extern "C" void setIdentity__6MatrixFv(void*);

extern const float lbl_27_rodata_4388;
extern const float lbl_27_rodata_4398;

struct Data {
    float a, b, c, d, e, f, g, h, i;
    float j, k, l, m, n, o, p, q, r;

    Data()
        : a(lbl_27_rodata_4388),
          b(lbl_27_rodata_4388),
          c(lbl_27_rodata_4388),
          d(lbl_27_rodata_4388),
          e(lbl_27_rodata_4388),
          f(lbl_27_rodata_4388),
          g(lbl_27_rodata_4398),
          h(lbl_27_rodata_4398),
          i(lbl_27_rodata_4398),
          j(lbl_27_rodata_4388),
          k(lbl_27_rodata_4388),
          l(lbl_27_rodata_4388),
          m(lbl_27_rodata_4388),
          n(lbl_27_rodata_4388),
          o(lbl_27_rodata_4388),
          p(lbl_27_rodata_4398),
          q(lbl_27_rodata_4398),
          r(lbl_27_rodata_4398) {}
};

struct Callback {
    int a, b;
    Data data;
    char matrix[48];
};

struct Gimmick {
    char pad[0xD4];
    Callback* callbacks;
    unsigned long count;
    int used;
};

extern "C" void makeCalcuCallback__9grGimmickFUlQ25Heaps8HeapType(
    Gimmick* self, unsigned long count, int heap)
{
    if (self->callbacks) {
        if (self->callbacks)
            __dla__FPv((char*)self->callbacks - 16);
    }

    self->callbacks = 0;
    self->count = count;
    self->used = 0;
    self->callbacks = (Callback*)__construct_new_array(
        __nwa__FUlQ25Heaps8HeapType(count * 128 + 16, heap),
        __ct__18grNodeCallbackDataFv, 0, 128, count);

    for (int i = 0; i < (int)count; i++) {
        Callback* cb = &self->callbacks[i];
        cb->a = 0;
        cb->b = 0;
        cb->data = Data();
        setIdentity__6MatrixFv(cb->matrix);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_26CE10 {
struct Vec3f;

struct Vec3f {
    float x, y, z;
};

struct Vec2f {
    float x, y;

    Vec2f(float a, float b) : x(a), y(b) {}
};

struct SEInfo {
    short state;
    short pad2;
    float frames;
    int handle;
    unsigned int sndId;
    char pad10[8];
    unsigned short unk18;
    unsigned short generator;
    Vec2f offset;
    float scale;
};

struct GimmickPos {
    int unk0;
    unsigned int flags;
    Vec3f pos;
    char pad14[0x48];
    float prevX;
    char pad60[0xC];
    float prevY;
    char pad70[0xC];
    float prevZ;
    void updatePos() {
        if (flags & 8) {
            float z = prevZ;
            float y = prevY;
            float x = prevX;
            setPos(z, y, x);
        }
    }
    void setPos(float z, float y, float x) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
};

struct Ground_base {
    int unk0;
    char pad4[0x38];
};

class Ground : public Ground_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void slot2();
    virtual void slot3();
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
    virtual void slot20();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
    virtual void loadGroundData();
    virtual void slot25();
    virtual void setupMelee();
    virtual void slot27();
    virtual void setVisibilityByClipping();
    virtual void slot29();
    virtual void receiveCollMsg();
    virtual void slot31();
    virtual void receiveCollMsg_Heading();
    virtual void receiveCollMsg_Wall();
    virtual void receiveCollMsg_Attack();
    virtual void unloadData();
    virtual void getModelCount();
    virtual void slot37();
    virtual void slot38();
    virtual void setStageData();
    virtual void getStageData();
    virtual void initStageData();
    virtual void setMdlIndex();
    virtual void getMdlIndex();
    virtual void isEnableCalcCollision();
    virtual void enableCalcCollision();
    virtual void disableCalcCollision();
    virtual void getNodeIndex();
    virtual void getNodePosition();
    virtual void getNodePosition_2();
    virtual void getNodeMatrix(float (*mtx)[4], unsigned long unk, unsigned long node);
};

class grGimmick : public Ground {
public:
    char pad40[0x60];
    snd3DGenerator* generators;
    char padA4[4];
    SEInfo* sounds;
    char padAC[0x28];
    GimmickPos* unkD4;
};

extern "C" {
void stopSE__14snd3DGeneratorFll(snd3DGenerator*, int, int);
int prepareSE__14snd3DGeneratorF5SndIDl(snd3DGenerator*, unsigned int, int);
void setPos__14snd3DGeneratorFP5Vec3f(snd3DGenerator*, Vec3f*);
int isPlay__14snd3DGeneratorFl(snd3DGenerator*, int);
void OSReport(const char*, ...);
}

extern const float lbl_27_rodata_4388;
extern char lbl_27_data_568B0[];

inline void updateSoundPos(grGimmick* self, unsigned long index)
{
        if (self->sounds[(int)index].sndId != -1) {
            unsigned short node = self->sounds[(int)index].unk18;
            if (node == 0) {
                Vec3f pos;
                GimmickPos* p = self->unkD4;

                if (p != 0) {
                    p->updatePos();
                    __memcpy(&pos, &self->unkD4->pos, sizeof(pos));
                } else {
                    pos.x = lbl_27_rodata_4388;
                    pos.y = lbl_27_rodata_4388;
                    pos.z = lbl_27_rodata_4388;
                }

                setPos__14snd3DGeneratorFP5Vec3f(
                    self->generators + self->sounds[(int)index].generator,
                    &pos);
            } else {
                float mtx[3][4];
                self->getNodeMatrix(mtx, 0, node);

                Vec3f pos;
                float z = mtx[2][3];
                float y = mtx[1][3];
                float x = mtx[0][3];
                pos.y = y;
                pos.x = x;
                pos.z = z;

                setPos__14snd3DGeneratorFP5Vec3f(
                    self->generators + self->sounds[(int)index].generator,
                    &pos);
            }
        }
}

extern "C" void startGimmickSE__9grGimmickFUl(grGimmick* self, unsigned long index)
{
    if (self->sounds[index].sndId != -1) {
        if (self->sounds[index].handle != -1 &&
            isPlay__14snd3DGeneratorFl(
                self->generators + self->sounds[index].generator,
                self->sounds[index].handle) != 0) {
            if (self->sounds[(int)index].handle >= 0) {
                stopSE__14snd3DGeneratorFll(
                    self->generators + self->sounds[(int)index].generator,
                    self->sounds[(int)index].handle,
                    0);

                SEInfo& sound = self->sounds[(int)index];
                sound.state = 0;
                sound.frames = lbl_27_rodata_4388;
                sound.handle = -1;
                sound.generator = 0;
                sound.offset = Vec2f(lbl_27_rodata_4388, lbl_27_rodata_4388);
            }
        }

        self->sounds[index].state = 1;
        self->sounds[index].frames = lbl_27_rodata_4388;
        self->sounds[index].handle = prepareSE__14snd3DGeneratorF5SndIDl(
            self->generators + self->sounds[index].generator,
            self->sounds[index].sndId,
            -1);

        if (self->sounds[index].handle == -1) {
            self->sounds[index].state = 0;
            OSReport(lbl_27_data_568B0, self->sounds[index].sndId, self->unk0);
            return;
        }

        updateSoundPos(self, index);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_26D798 {
extern "C" int lbl_27_data_568E0;
extern "C" int lbl_27_data_568E4;
extern "C" void setVisibilityAttachedEffect__6GroundFUl(void*, unsigned long);

class AttachedEffect {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void* v12();
    int field;
};

struct grGimmick {
    unsigned char pad[0xFC];
    AttachedEffect* effects_a[10];
    AttachedEffect* effects_b[10];
};

extern "C" void setVisibilityAttachedEffect__9grGimmickFUl(grGimmick* self, unsigned long visible) {
    int found = 1;
    if (visible != 0) {
        for (int i = 0; i < 10; i++) {
            if (self->effects_a[i] != 0) {
                if (self->effects_a[i]->v12() != 0) {
                    AttachedEffect* other = self->effects_b[i];
                    if (other != 0) {
                        other->field = lbl_27_data_568E0;
                    }
                    self->effects_a[i]->v4();
                }
            }
        }
        setVisibilityAttachedEffect__6GroundFUl(self, 1);
        return;
    }
    if (visible == 0) {
        for (int i = 0; i < 10; i++) {
            if (self->effects_b[i] != 0) {
                if (self->effects_b[i]->v12() != 0) {
                    AttachedEffect* other = self->effects_a[i];
                    if (other != 0) {
                        other->field = lbl_27_data_568E4;
                    }
                    self->effects_b[i]->v4();
                    found = 0;
                }
            }
        }
        if (found != 0) {
            setVisibilityAttachedEffect__6GroundFUl(self, 0);
        }
    }
}
}

