#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_motion_path.h>
#include <memory.h>
#include <st/stage.h>
#include <string.h>
#include <types.h>
#include <mu/mu_mat_color.h>
#include <nw4r/ut/ut_Color.h>

extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);

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
    update(g_stGameFrame.m_frameDelta * m_motionRatio);
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
    m_transparencyFlag = flag;
}

void grGimmick::dbDispInvalidatedByCameraClippingSphere() { }

void grGimmick::setTransparency(u32 flag, u32 settings) {
    if (settings & 0x10) {
        setVisibilityByClipping(flag);
    }
    if (settings & 1) {
        setEnableCollisionStatusByClipping(flag);
    }
    m_transparencyFlag = flag;
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
