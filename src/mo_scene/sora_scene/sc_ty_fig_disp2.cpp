#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_photo.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_camera.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_set_rule.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <sr/sr_common.h>
#include <types.h>

class scTyFigDisp2;

// The camera animation of a scene animation and what it gives at a frame (main.dol; the shared g3d headers do not have
// them in a form that this unit can include). TODO: clean up - use the g3d ResAnmCamera / CameraAnmResult once the
// shared headers can be included here.
struct scCameraAnm {
    void* m_data;
};
struct scCameraAnmResult {
    u32 m_flags;                        // 0x00
    u32 m_projectionType;               // 0x04
    float m_posX;                       // 0x08
    float m_posY;                       // 0x0C
    float m_posZ;                       // 0x10
    float m_aspect;                     // 0x14
    float m_near;                       // 0x18
    float m_far;                        // 0x1C
    float m_targetX;                    // 0x20
    float m_targetY;                    // 0x24
    float m_targetZ;                    // 0x28
    float m_twist;                      // 0x2C
    float m_fovy;                       // 0x30
    float m_height;                     // 0x34
};
extern "C" scCameraAnm GetResAnmCamera__Q34nw4r3g3d9ResAnmScnCFi(const nw4r::g3d::ResAnmScn* scn, int index);
extern "C" void GetAnmResult__Q34nw4r3g3d12ResAnmCameraCFPQ34nw4r3g3d15CameraAnmResultf(scCameraAnm* camera,
                                                                                         scCameraAnmResult* result,
                                                                                         float frame);

// The task that shows the figures of the second figure display (sora_menu_main.rel).
class tyFigDisp2Task : public gfTask {
public:
    static tyFigDisp2Task* create(scTyFigDisp2* scene, void* data);
};

// Sets how the photo camera controller works (main.dol, the function is not named).
extern "C" void fn_800A8DA8(cmPhotoController* controller, int mode);

// The scene of the second figure display (module sora_scene): runs the task of sora_menu_main.rel on a camera that the
// scene sets up itself.
class scTyFigDisp2 : public gfScene {
public:
    // What start makes.
    struct Work {
        CameraController* m_camera;     // 0x00
        muObjProcessTask* m_objProcess; // 0x04 updates the objects of the menu
        tyFigDisp2Task* m_task;         // 0x08
        u8 m_isLayerHidden;             // 0x0C the layer of the models of the task is not updated when set
    };

    Work* m_work;                       // 0x08

    scTyFigDisp2(const char* name) : gfScene(name), m_work(NULL) { }
    static scTyFigDisp2* create();
    void setObjs(MuObject** objs, int num);
    void addMdlLayer2(nw4r::g3d::ScnMdl* mdl);
    void removeMdlLayer2(nw4r::g3d::ScnMdl* mdl);
    void addMdlLayer6(nw4r::g3d::ScnMdl* mdl);
    void removeMdlLayer6(nw4r::g3d::ScnMdl* mdl);
    void addMdlLayer8(nw4r::g3d::ScnMdl* mdl);
    void removeMdlLayer8(nw4r::g3d::ScnMdl* mdl);
    void setAnmScn(nw4r::g3d::ResAnmScn* anm);
    void setCameraFromScene();
    void removeAnmScn();
    void setInfoCamAnm(nw4r::g3d::ResAnmScn* anm);
    void updateFrame();
    void setRuleData(const void* data);
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scTyFigDisp2) == 0xC, "Class is wrong size!");

scTyFigDisp2* scTyFigDisp2::create() {
    return new (Heaps::GlobalMode) scTyFigDisp2("scTyFigDisp2");
}

void scTyFigDisp2::setModule() {
    setLoadModule("sora_menu_main.rel", false);
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_sel_stage.rel", false);
}

// TODO: clean up - the rule data of the game is reached by raw offsets (+0x34 the pad, +0x30 the data of the task); name
// the members of gmSetRule (_30) once the task is recovered.
void scTyFigDisp2::start() {
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_camera = CameraController::getInstance();
    m_work->m_camera->changeInput(CameraController::Photo);
    fn_800A8DA8(dynamic_cast<cmPhotoController*>(m_work->m_camera->m_currentController), 1);
    m_work->m_isLayerHidden = 0;
    int pad = gmGetMenuDecisionPad();
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(g_GameGlobal->m_setRule) + 0x34) = pad;
    m_work->m_objProcess = muObjProcessTask::create(Heaps::MenuInstance);
    m_work->m_task = tyFigDisp2Task::create(this, g_GameGlobal->m_setRule->_30);
}

s32 scTyFigDisp2::process() {
    return 0;
}

s32 scTyFigDisp2::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_objProcess->exit();
    m_work->m_task->exit();
    gfHeapManager::free(m_work);
    m_work = NULL;
    g_gfSceneRoot->removeInfoCamResAnm();
    g_gfSceneRoot->removeResAnmScn();
    return 0;
}

// TODO: clean up - muObjProcessTask only declares its size here; the object list and its count are at +0x40 and +0x44.
void scTyFigDisp2::setObjs(MuObject** objs, int num) {
    muObjProcessTask* objProcess = m_work->m_objProcess;
    *reinterpret_cast<MuObject***>(reinterpret_cast<u8*>(objProcess) + 0x40) = objs;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(objProcess) + 0x44) = num;
}

void scTyFigDisp2::addMdlLayer2(nw4r::g3d::ScnMdl* mdl) {
    g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(2), mdl);
}

void scTyFigDisp2::removeMdlLayer2(nw4r::g3d::ScnMdl* mdl) {
    g_gfSceneRoot->remove(mdl);
}

void scTyFigDisp2::addMdlLayer6(nw4r::g3d::ScnMdl* mdl) {
    g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(6), mdl);
}

void scTyFigDisp2::removeMdlLayer6(nw4r::g3d::ScnMdl* mdl) {
    g_gfSceneRoot->remove(mdl);
}

void scTyFigDisp2::addMdlLayer8(nw4r::g3d::ScnMdl* mdl) {
    g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(8), mdl);
}

void scTyFigDisp2::removeMdlLayer8(nw4r::g3d::ScnMdl* mdl) {
    g_gfSceneRoot->remove(mdl);
}

// Gives the scene the animation of the scene file and takes the aspect of its camera.
void scTyFigDisp2::setAnmScn(nw4r::g3d::ResAnmScn* anm) {
    // TODO: clean up - the two locals share one stack object in the original (the result sits right behind the camera).
    struct {
        scCameraAnm camera;
        scCameraAnmResult result;
    } local;
    local.camera.m_data = NULL;
    g_gfSceneRoot->setResAnmScn(anm, 0x2a);
    local.camera = GetResAnmCamera__Q34nw4r3g3d9ResAnmScnCFi(anm, 0);
    GetAnmResult__Q34nw4r3g3d12ResAnmCameraCFPQ34nw4r3g3d15CameraAnmResultf(&local.camera, &local.result, 0.0f);
    gfCameraManager* manager = gfCameraManager::getManager();
    manager->m_cameras[0].m_aspect = local.result.m_aspect;
}

// Takes the position and the posture of the camera of the scene file for the camera of the game.
void scTyFigDisp2::setCameraFromScene() {
    gfCamera* camera = &gfCameraManager::getManager()->m_cameras[0];
    nw4r::g3d::Camera nwCamera = g_gfSceneRoot->unk40->GetCamera(0);
    nw4r::math::VEC3 position;
    nwCamera.GetPosition(&position);
    camera->m_centerPos.m_x = position.x;
    camera->m_centerPos.m_y = position.y;
    camera->m_centerPos.m_z = position.z;
    u16 flags = 2;
    Vec3f up;
    Vec3f lookTarget;
    Vec3f aimTarget;
    Vec3f rotate;
    nw4r::g3d::Camera::PostureInfo posture;
    nwCamera.GetPosture(&posture);
    switch (posture.tp) {
    case nw4r::g3d::Camera::POSTURE_LOOKAT: {
        up.m_x = posture.cameraUp.x;
        up.m_y = posture.cameraUp.y;
        up.m_z = posture.cameraUp.z;
        camera->m_up.normalize(&up);
        camera->unkFA.m_mask |= 1;
        flags |= 9;
        lookTarget.m_x = posture.cameraTarget.x;
        lookTarget.m_y = posture.cameraTarget.y;
        lookTarget.m_z = posture.cameraTarget.z;
        camera->m_targetPos = lookTarget;
        camera->unkFA.m_mask |= 2;
        break;
    }
    case nw4r::g3d::Camera::POSTURE_ROTATE: {
        rotate.m_x = posture.cameraRotate.x;
        rotate.m_y = posture.cameraRotate.y;
        rotate.m_z = posture.cameraRotate.z;
        flags |= 0x60;
        camera->m_rot = rotate;
        camera->unkFA.m_mask |= 0x40;
        break;
    }
    case nw4r::g3d::Camera::POSTURE_AIM: {
        aimTarget.m_x = posture.cameraTarget.x;
        aimTarget.m_y = posture.cameraTarget.y;
        aimTarget.m_z = posture.cameraTarget.z;
        flags |= 0x41;
        camera->m_targetPos = aimTarget;
        camera->unkFA.m_mask |= 2;
        camera->m_rot.m_z = posture.cameraTwist;
        camera->unkFA.m_mask |= 0x40;
        break;
    }
    }
    camera->m_transformFlag.m_mask = flags;
    camera->calc();
}

void scTyFigDisp2::removeAnmScn() {
    g_gfSceneRoot->removeResAnmScn();
}

void scTyFigDisp2::setInfoCamAnm(nw4r::g3d::ResAnmScn* anm) {
    g_gfSceneRoot->setInfoCamResAnm(anm);
}

// Moves the scene (and its layers) one frame on.
void scTyFigDisp2::updateFrame() {
    gfSceneRoot* root = g_gfSceneRoot;
    root->unk40->UpdateFrame();
    root->unk44->UpdateFrame();
    if (m_work->m_isLayerHidden == 0) {
        root->layerUpdateFrame(2);
    }
    root->layerUpdateFrame(6);
    root->layerUpdateFrame(8);
}

void scTyFigDisp2::setRuleData(const void* data) {
    memcpy(g_GameGlobal->m_setRule->_30, data, 0x58);
}
