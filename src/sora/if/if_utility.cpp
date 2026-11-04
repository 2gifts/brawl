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
