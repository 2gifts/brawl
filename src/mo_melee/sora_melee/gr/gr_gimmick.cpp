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
