#include <gr/gr_visible_production_material_motion.h>
#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <types.h>

grVisibleProductionMaterialMotion* grVisibleProductionMaterialMotion::create(nw4r::g3d::ScnMdl* sceneModel, gfModelAnimation* modelAnim, u32* animIndex) {
    return new (Heaps::StageInstance) grVisibleProductionMaterialMotion(sceneModel, modelAnim, animIndex);
}

void grVisibleProductionMaterialMotion::initialize() { }

void grVisibleProductionMaterialMotion::remove() { }

void grVisibleProductionMaterialMotion::productFlowStandby() { }

void grVisibleProductionMaterialMotion::productFlowStart() {
    int size;
    nw4r::g3d::ResMdl resMdl = m_sceneModel->m_resMdl;
    gfModelAnimation* modelAnim = m_modelAnim;
    u32 animIndex = m_animIndex;
    if (animIndex < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        nw4r::g3d::ResAnmClr anmClr = modelAnim->m_resFile.GetResAnmClr(animIndex);
        if (anmClr.IsValid()) {
            nw4r::g3d::AnmObjMatClrRes* anmObj = nw4r::g3d::AnmObjMatClrRes::Construct(gfHeapManager::getMEMAllocator(Heaps::StageInstance), &size, anmClr, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjMatClrRes != NULL) {
                    modelAnim->m_anmObjMatClrRes->Destroy();
                }
                modelAnim->m_anmObjMatClrRes = anmObj;
            }
        }
    }
    m_modelAnim->bindMatColAnim(m_sceneModel);
    m_modelAnim->setFrame(0.0f);
    setState(VisibleProduction_Executing);
}

void grVisibleProductionMaterialMotion::productFlowProducing(float deltaFrame) {
    nw4r::g3d::AnmObjMatClrRes* anmObj = m_modelAnim->m_anmObjMatClrRes;
    u16 animLength = anmObj->m_anmMatClrFile.ref().m_animLength;
    if (anmObj->GetFrame() >= animLength) {
        setState(VisibleProduction_Finished);
        m_visiblePermission = true;
    }
}

void grVisibleProductionMaterialMotion::productFlowEnd() { }

void grVisibleProductionMaterialMotion::setForcedEnding() { }

// TODO: clean up, give variables real names
namespace jj_271578 {
typedef unsigned long u32;

struct ResMdl {
    void* data;
};

struct ResAnmClr {
    void* data;
};

struct AnmObj {
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
    virtual void Bind(const ResMdl*);
};

struct Model {
    char pad[0xE8];
    ResMdl resource;
};

struct ModelAnimation {
    char pad[0x18];
    AnmObj* animation;
};

struct Obj {
    char pad0[4];
    u32 state;
    char pad8[8];
    Model* model;
    ModelAnimation* animation;
    u32 index;
};

extern "C" {
u32 GetResAnmClrNumEntries__Q34nw4r3g3d7ResFileFv(ModelAnimation*);
void* GetResAnmClr__Q34nw4r3g3d7ResFileCFUl(ModelAnimation*, u32);
void* getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(int);
AnmObj* Construct__Q34nw4r3g3d15AnmObjMatClrResFP12MEMAllocatorPiQ34nw4r3g3d9ResAnmClrQ34nw4r3g3d6ResMdlb(
    void*, int*, const ResAnmClr*, const ResMdl*, bool);
void Destroy__Q34nw4r3g3d6G3dObjFv(AnmObj*);
void bindMatColAnim__16gfModelAnimationFPQ34nw4r3g3d6ScnMdl(ModelAnimation*, Model*);
void setFrame__16gfModelAnimationFd(ModelAnimation*, double);

extern const float lbl_27_rodata_4470;
extern u32 lbl_27_data_57EE8;

void fn_27_271578(Obj* self) {
    Model* mdl = self->model;
    ModelAnimation* animation = self->animation;
    u32 index = self->index;
    ResMdl model = mdl->resource;

    if (index < GetResAnmClrNumEntries__Q34nw4r3g3d7ResFileFv(animation)) {
        struct Params {
            int size;
            ResAnmClr anim;
            ResMdl model;
            ResMdl bindModel;
        } params;

        ResAnmClr anim;
        anim.data = GetResAnmClr__Q34nw4r3g3d7ResFileCFUl(animation, index);
        if (anim.data) {
            void* allocator = getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(15);
            params.model = model;
            params.anim = anim;
            AnmObj* object =
                Construct__Q34nw4r3g3d15AnmObjMatClrResFP12MEMAllocatorPiQ34nw4r3g3d9ResAnmClrQ34nw4r3g3d6ResMdlb(
                    allocator, &params.size, &params.anim, &params.model, false);
            if (object) {
                params.bindModel = model;
                object->Bind(&params.bindModel);
                if (animation->animation)
                    Destroy__Q34nw4r3g3d6G3dObjFv(animation->animation);
                animation->animation = object;
            }
        }
    }

    bindMatColAnim__16gfModelAnimationFPQ34nw4r3g3d6ScnMdl(self->animation, self->model);
    setFrame__16gfModelAnimationFd(self->animation, lbl_27_rodata_4470);
    self->state = lbl_27_data_57EE8;
}
}
}

