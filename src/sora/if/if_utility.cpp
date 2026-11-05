#include <GX.h>
#include <gf/gf_application.h>
#include <if/if_utility.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

utLoaderList lbl_8049E550 __attribute__((aligned(8)));

bool utility::Loader::isLoadFinish() {
    utListNode* node = lbl_8049E550.m_list.getHead();
    utListNode* end = 0;
    for (; node != end; node = node->m_next) {
        if (!static_cast<Loader*>(node)->loading()) return false;
    }
    return true;
}

bool utility::AutoResFile::loading() {
    if (!m_file.m_handle.isReady()) return false;
    if (!m_isInit && m_file.m_handle.getReturnStatus() == 0) {
        static_cast<nw4r::g3d::ResFile&>(*this) = nw4r::g3d::ResFile(m_file.m_handle.getBuffer());
        nw4r::g3d::ResFile::Init(this);
        m_isInit = true;
    }
    return true;
}




void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* object, u32 flag);
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* object, u32 timing);

utility::DepthScaler::DepthScaler(MuObject& object, float first, float second)
    : unk4(first), unk8(second) {
    nw4r::g3d::ScnObj* model = object.m_sceneModel;
    model->m_callback = this;
    ScnObj_EnableCallbackExecOp(model, 1);
    ScnObj_EnableCallbackExecOp(model, 4);
    ScnObj_EnableCallbackTiming(model, 0x10);
    ScnObj_EnableCallbackTiming(model, 0x20);
}

void utility::DepthScaler::setZScaleOffset(nw4r::g3d::ScnObj::Timing timing) const {
    switch (timing) {
    case nw4r::g3d::ScnObj::CALLBACK_TIMING_A:
        GXSetZScaleOffset(unk4, 1.0f - unk4);
        GXSetZMode(true, GX_LEQUAL, true);
        break;
    case nw4r::g3d::ScnObj::CALLBACK_TIMING_C:
        GXSetZScaleOffset(1.0f, 0.0f);
        GXSetZMode(false, GX_LEQUAL, false);
        break;
    case 3:
        break;
    }
}

void utility::DepthScaler::ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj*, u32, void*) {
    setZScaleOffset(timing);
}

void utility::DepthScaler::ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj*, u32, void*) {
    setZScaleOffset(timing);
}


utility::DepthClearer::DepthClearer(gfSceneRoot::LayerType layer) : unk0(layer) {
    g_gfSceneRoot->setClearDepthBufferState(layer, true);
}

utility::DepthClearer::~DepthClearer() {
    g_gfSceneRoot->setClearDepthBufferState(unk0, false);
}

void utility::FadeOutTask::processDefault() {
    --unk40;
}

void utility::AutoFile::clear() {
    if (m_isRequested) {
        if (m_handle.isReady()) {
            gfHeapManager::free(m_handle.getBuffer());
        } else {
            m_handle.cancelRequest();
        }
        m_handle.release();
        m_isRequested = false;
    }
}

utility::StandardTask::~StandardTask() {
    if (m_unk42_5) {
        g_gfApplication->unkD0.startKeepScreen();
    }
    g_gfSceneRoot->removeInfoCamResAnm();
}

utility::DepthScaler::~DepthScaler() { }
