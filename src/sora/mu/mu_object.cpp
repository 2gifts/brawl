#include <gf/gf_heap_manager.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_model.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/math/math_arithmetic.h>
#include <ut/ut_nw.h>
#include <sr/sr_common.h>
#include <types.h>
#include <string.h>

// Animation frame policy function table (lives in gf_model_animation.cpp).
extern void* lbl_8059C648[2];

// "%s__%d"
extern char lbl_8059DF20[7];
extern "C" int sprintf(char* str, const char* fmt, ...);

// ResFile name lookups that are not declared in the BrawlHeaders ResFile class.

void ResNode_SetRotate(nw4r::g3d::ResNode* node, float x, float y, float z);
void ResNode_SetScale(nw4r::g3d::ResNode* node, float x, float y, float z);

struct ChrAnmResult {
    u32 m_flags;
    Vec3f m_scale;
    u8 m_10[0x40];
};
void ChrAnmResult_GetScale(const ChrAnmResult* res, Vec3f* out);

nw4r::g3d::ResTexObjData* CopiedMatAccess_GetResTexObj(nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);
extern "C" void GXGetTexObjAll(const GXTexObj* obj, void** image, u16* w, u16* h, GXTexFmt* fmt, GXTexWrapMode* wrapS, GXTexWrapMode* wrapT, GXBool* mipmap);

extern "C" {
void fn_801B0A28();
gfModelAnimation* __ct__16gfModelAnimationFd(void* self, nw4r::g3d::ResFile* file, nw4r::g3d::ResMdl* mdl, bool doBind, int animIndex, Heaps::HeapType heap);
gfModelAnimation* __ct1__16gfModelAnimationFd(void* self, nw4r::g3d::ResFile* file, nw4r::g3d::ResMdl* mdl, bool doBind, const char* animName, Heaps::HeapType heap);
}
nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView, void (*callback)());

extern "C" {
bool fn_80192B00(nw4r::g3d::ResTex* tex, void** image, u16* w, u16* h, GXTexFmt* fmt, f32* minLod, f32* maxLod, GXBool* mipmap);
bool fn_80192A44(nw4r::g3d::ResTex* tex, void** image, u16* w, u16* h, GXTexFmt* fmt, f32* minLod, f32* maxLod, GXBool* mipmap);
void* fn_801AFF94(nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);
GXTlutObj* fn_80190DC8(void** tlutObj, int id);
}

struct ResPlttRef {
    u8* m_data;
};
extern "C" u8* fn_8018D310(nw4r::g3d::ResFile* file, const char* name);

void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* mdl, u32 nodeId, const Matrix* mtx);

struct MuAnimIdxData {
    float m_frame;      // 0x00
    u8 m_04[8];         // 0x04
    u32 m_index;        // 0x0c
    u8 m_flags;         // 0x10
};

struct MuAnimNameData {
    float m_frame;      // 0x00
    u8 m_04[8];         // 0x04
    const char* m_name; // 0x0c
    u8 m_flags;         // 0x10
};

struct ZeroWord {
    u32 m_word;
    ZeroWord() { m_word = 0; }
};

class MuObjProcessTask {
public:
    virtual ~MuObjProcessTask();
};

struct MuObjTaskHolder {
    MuObjProcessTask* m_task;
    MuObjTaskHolder() { m_task = NULL; }
    ~MuObjTaskHolder() {
        delete m_task;
    }
};

struct MuObjBase {
    u32 m_flag : 1;
    u32 m_rest : 31;
    MuObjBase() { m_flag = 1; }
};

class MuObject : public MuObjBase {
public:
    nw4r::g3d::ResFile m_resFile;    // 0x04
    nw4r::g3d::ResMdl m_resMdl;      // 0x08
    nw4r::g3d::ScnMdl* m_scnMdl;     // 0x0c
    nw4r::g3d::ScnMdl* m_sceneModel; // 0x10
    gfModelAnimation* m_modelAnim;   // 0x14
    u32 m_18;                        // 0x18
    MuAnimIdxData* m_animIdxData;    // 0x1c
    MuAnimNameData* m_animNameData;  // 0x20
    ZeroWord m_24[4];                // 0x24
    u32 m_baseNode;                  // 0x34
    u8 m_38;                         // 0x38
    Vec3f m_trans;                   // 0x3c
    Vec3f m_rot;                     // 0x48
    Heaps::HeapType m_heapType;      // 0x54
    MuObjTaskHolder m_task;          // 0x58

    MuObject(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, bool isByName, Heaps::HeapType heapType);
    MuObject(nw4r::g3d::ResFile* modelSource, int animNode, int modelNode, nw4r::g3d::ResFile* textureSource, bool isByIndex, Heaps::HeapType heapType);

    static MuObject* create(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, Heaps::HeapType type);
    static MuObject* create(nw4r::g3d::ResFile* modelSource, int modelNode, nw4r::g3d::ResFile* textureSource, int animNode, Heaps::HeapType type);

    static MuObject* createAlt(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, Heaps::HeapType type);

    void calcMatrix();

    void changeNodeAnimN(const char* animName);
    bool changeNodeAnimNIf(const char* animName);
    void changeVisAnimN(const char* animName);
    bool changeVisAnimNIf(const char* animName);
    void changeTexPatAnim(u32 index);
    void changeTexPatAnimN(const char* animName);
    bool changeTexPatAnimNIf(const char* animName);
    void changeTexSrtAnimN(const char* animName);
    bool changeTexSrtAnimNIf(const char* animName);
    void changeClrAnimN(const char* animName);
    bool changeClrAnimNIf(const char* animName);
    void changeAnimN(const char* animName);
    u16 getNodeAnimLength();
    const char* getNodeAnimName();
    const char* getVisAnimName();
    const char* getTexPatAnimName();
    const char* getTexSrtAnimName();
    const char* getClrAnimName();
    bool isNodeAnimFinished();
    bool isVisAnimFinished();
    bool isClrAnimFinished();
    bool isTexPatAnimFinished();
    bool isTexSrtAnimFinished();
    bool isAnimFinished();
    bool isNodeAnimLoop();
    void getPos(Vec3f* pos);
    void getPos(Vec3f* pos, const char* nodeName);
    void setPos(Vec3f* pos);
    void setPos(Vec3f* pos, const char* nodeName);
    void setTrans(Vec3f* trans);
    void getRect3D(Rect2D* rect, const char* nodeNameA, const char* nodeNameB);
    void getRect3D(Rect2D* rect, int nodeA, int nodeB);
    nw4r::g3d::ResNode getNode(const char* nodeName);
    u32 getNodeID(const char* nodeName);
    Matrix getNodeMatrix(const char* nodeName);
    Vec3f getGlobalPosition(int resNode);
    Vec3f getGlobalPosition(const char* nodeName);
    Vec3f getGlobalPosition();
    void getAnimScale(Vec3f* scl, const char* name);
    Vec3f getScale(const char* nodeName);
    void setObjScale(Vec3f* scale);
    Vec3f getObjScale();
    void setRotate(Vec3f* rot);
    void setRotate(const char* nodeName, Vec3f* rot);
    void setObjRotate(Vec3f* rot);
    void getRotate(Vec3f* rot, const char* nodeName);
    void setScale(const char* nodeName, Vec3f* scale);
    void changeMaterialTex(const char* matName, GXTexObj* srcObj);
    void changeMaterialTex(const char* matName, void* image, u16 width, u16 height);
    void changeMaterialTex(const char* matName, int texIndex, nw4r::g3d::ResFile* texFile);
    void changeMaterialTex(u32 matId, const char* texName, nw4r::g3d::ResFile* texFile);
    void changeAnimN(int animId, u32 flags, bool play, bool reset);
    void setAnimIdx(MuAnimIdxData* data, bool force);
    void setAnimName(MuAnimNameData* data, bool force);
    void setFrame(float frame);
    void setFrameNode(float frame);
    void setFrameVisible(float frame);
    void setFrameTex(float frame);
    void setFrameTexSrt(float frame);
    void setFrameMatCol(float frame);

    MuObject(const char* path, int drawPriority, int node, MuObject* src, nw4r::g3d::ResFile* textureSource, Heaps::HeapType heapType);
    void initFromFile(const char* path, int drawPriority, int node, nw4r::g3d::ResFile* textureSource, Heaps::HeapType heapType);
    void initFromObject(MuObject* src, int drawPriority, int node, Heaps::HeapType heapType);
    virtual ~MuObject();
    virtual void vfunc1();
};

static inline gfModelAnimation* newModelAnimIdx(void* mem, nw4r::g3d::ResMdl mdl, nw4r::g3d::ResFile file, int index, Heaps::HeapType heap) {
    return __ct__16gfModelAnimationFd(mem, &file, &mdl, true, index, heap);
}

static inline gfModelAnimation* newModelAnimIdxCopy(void* mem, nw4r::g3d::ResMdl mdl, const nw4r::g3d::ResFile* filep, int index, Heaps::HeapType heap) {
    nw4r::g3d::ResFile file = *filep;
    return __ct__16gfModelAnimationFd(mem, &file, &mdl, true, index, heap);
}

static inline gfModelAnimation* newModelAnimName(void* mem, nw4r::g3d::ResMdl mdl, nw4r::g3d::ResFile file, const char* name, Heaps::HeapType heap) {
    return __ct1__16gfModelAnimationFd(mem, &file, &mdl, true, name, heap);
}

static inline u32 getScnMdlBufferFlags(nw4r::g3d::ResFile* file) {
    u32 flags = 0;
    if (file->HasResAnmClr()) {
        flags |= 0x10b;
    }
    if (file->HasResAnmTexPat()) {
        flags |= 0x3;
    }
    if (file->HasResAnmTexSrt()) {
        flags |= 0x204;
    }
    if (file->HasResAnmVis()) {
        flags |= 0x40;
    }
    if (file->HasResAnmShp()) {
        flags |= 0x7000;
    }
    return flags;
}

static inline u32 getRootNodeId(nw4r::g3d::ResMdl* mdl) {
    nw4r::g3d::ResNodeData* cur = mdl->GetResNode(0).ptr();
    while (true) {
        u32 off = cur->m_toParent;
        nw4r::g3d::ResNodeData* parent;
        if (off != 0) {
            parent = (nw4r::g3d::ResNodeData*)((u8*)cur + off);
        } else {
            parent = NULL;
        }
        if (parent == NULL) {
            break;
        }
        cur = parent;
    }
    return cur != NULL ? cur->m_nodeIndex : 0;
}

MuObject::MuObject(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, bool isByName, Heaps::HeapType heapType)
    : m_scnMdl(NULL), m_sceneModel(NULL), m_modelAnim(NULL), m_18(0), m_animIdxData(NULL), m_animNameData(NULL), m_baseNode(-1), m_38(0xff), m_heapType((Heaps::HeapType)0x2b) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heapType);
    m_heapType = heapType;
    m_resFile = *modelSource;
    if (textureSource != NULL) {
        m_resFile.Bind(*textureSource);
    } else {
        m_resFile.Bind(m_resFile);
    }
    m_resMdl = m_resFile.GetResMdl(modelNode);
    u32 flags;
    if (isByName) {
        flags = getScnMdlBufferFlags(&m_resFile) | 0x10b;
    } else {
        flags = 0;
    }
    u32 size;
    nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, m_resMdl, flags, 1, fn_801B0A28);
    if (drawPriority != 0) {
        scnMdl->SetPriorityDrawOpa(drawPriority);
        scnMdl->SetPriorityDrawXlu(drawPriority);
    }
    m_scnMdl = scnMdl;
    m_sceneModel = scnMdl;
    m_trans.m_x = m_trans.m_y = m_trans.m_z = 0.0f;
    m_rot.m_x = m_rot.m_y = m_rot.m_z = 0.0f;
    m_baseNode = getRootNodeId(&m_resMdl);
    if (!isByName) {
        return;
    }
    {
        gfModelAnimation* anim = (gfModelAnimation*)operator new(0x20, heapType);
        if (anim != NULL) {
            anim = newModelAnimName(anim, m_resMdl, m_resFile, modelNode, heapType);
        }
        gfModelAnimation::bind(scnMdl, anim);
        m_modelAnim = anim;
        *(u8*)((u8*)anim + 4) = 0;
        anim->setFrame(0.0f);
    }
}

MuObject::MuObject(nw4r::g3d::ResFile* modelSource, int node, int drawPriority, nw4r::g3d::ResFile* textureSource, bool isByIndex, Heaps::HeapType heapType)
    : m_scnMdl(NULL), m_sceneModel(NULL), m_modelAnim(NULL), m_18(0), m_animIdxData(NULL), m_animNameData(NULL), m_baseNode(-1), m_38(0xff), m_heapType((Heaps::HeapType)0x2b) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heapType);
    m_heapType = heapType;
    m_resFile = *modelSource;
    if (textureSource != NULL) {
        m_resFile.Bind(*textureSource);
    } else {
        m_resFile.Bind(m_resFile);
    }
    m_resMdl = m_resFile.GetResMdl(node);
    u32 flags;
    if (isByIndex) {
        flags = getScnMdlBufferFlags(&m_resFile) | 0x10b;
    } else {
        flags = 0;
    }
    u32 size;
    nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, m_resMdl, flags, 1, fn_801B0A28);
    if (drawPriority != 0) {
        scnMdl->SetPriorityDrawOpa(drawPriority);
        scnMdl->SetPriorityDrawXlu(drawPriority);
    }
    m_scnMdl = scnMdl;
    m_sceneModel = scnMdl;
    m_trans.m_x = m_trans.m_y = m_trans.m_z = 0.0f;
    m_rot.m_x = m_rot.m_y = m_rot.m_z = 0.0f;
    m_baseNode = getRootNodeId(&m_resMdl);
    if (!isByIndex) {
        return;
    }
    {
        gfModelAnimation* anim = (gfModelAnimation*)operator new(0x20, heapType);
        if (anim != NULL) {
            anim = newModelAnimIdx(anim, m_resMdl, m_resFile, node, heapType);
        }
        gfModelAnimation::bind(scnMdl, anim);
        m_modelAnim = anim;
        *(u8*)((u8*)anim + 4) = 0;
        anim->setFrame(0.0f);
    }
}

static inline void destroyModelAnim(gfModelAnimation* anim) {
    if (anim->m_anmObjChrRes != NULL) {
        anim->m_anmObjChrRes->Destroy();
    }
    if (anim->m_anmObjVisRes != NULL) {
        anim->m_anmObjVisRes->Destroy();
    }
    if (anim->m_anmObjTexPatRes != NULL) {
        anim->m_anmObjTexPatRes->Destroy();
    }
    if (anim->m_anmObjTexSrtRes != NULL) {
        anim->m_anmObjTexSrtRes->Destroy();
    }
    if (anim->m_anmObjMatClrRes != NULL) {
        anim->m_anmObjMatClrRes->Destroy();
    }
    if (anim->m_anmObjShpRes != NULL) {
        anim->m_anmObjShpRes->Destroy();
    }
    if (anim->m_resFile.ptr() != NULL) {
        if (*(u8*)((u8*)anim + 4) != 0) {
            gfHeapManager::free(anim->m_resFile.ptr());
            anim->m_resFile = nw4r::g3d::ResFile((void*)NULL);
        }
    }
    anim->m_anmObjChrRes = NULL;
    anim->m_anmObjVisRes = NULL;
    anim->m_anmObjTexPatRes = NULL;
    anim->m_anmObjTexSrtRes = NULL;
    anim->m_anmObjMatClrRes = NULL;
    anim->m_anmObjShpRes = NULL;
}

MuObject::MuObject(const char* path, int drawPriority, int node, MuObject* src, nw4r::g3d::ResFile* textureSource, Heaps::HeapType heapType)
    : m_scnMdl(NULL), m_sceneModel(NULL), m_modelAnim(NULL), m_18(0), m_animIdxData(NULL), m_animNameData(NULL), m_baseNode(-1), m_38(0xff), m_heapType(heapType) {
    if (src == NULL) {
        initFromFile(path, drawPriority, node, textureSource, heapType);
    } else {
        initFromObject(src, drawPriority, node, heapType);
    }
    m_trans.m_x = m_trans.m_y = m_trans.m_z = 0.0f;
    m_rot.m_x = m_rot.m_y = m_rot.m_z = 0.0f;
    m_baseNode = getRootNodeId(&m_resMdl);
}

MuObject::~MuObject() {
    gfModelAnimation* anim = m_modelAnim;
    if (anim != NULL) {
        destroyModelAnim(anim);
        gfHeapManager::free(m_modelAnim);
        m_modelAnim = NULL;
    }
    if (m_sceneModel != NULL) {
        ((nw4r::g3d::G3dObj*)m_sceneModel)->Destroy();
        m_sceneModel = NULL;
    }
}

void MuObject::vfunc1() {
}

static inline void* readFileBuffer(const char* path, Heaps::HeapType heapType) {
    void* buffer = NULL;
    gfFileIOHandle handle;
    if (handle.read(path, heapType, 0)) {
        buffer = handle.getBuffer();
        handle.getSize();
    }
    return buffer;
}

void MuObject::initFromFile(const char* path, int drawPriority, int node, nw4r::g3d::ResFile* textureSource, Heaps::HeapType heapType) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heapType);
    void* buffer = readFileBuffer(path, heapType);
    m_resFile = nw4r::g3d::ResFile(buffer);
    if (buffer == NULL) {
        gfHeapManager::dumpAll();
    }
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_resFile.Bind(textureSource != NULL ? *textureSource : m_resFile);
    m_resMdl = m_resFile.GetResMdl(node);
    u32 flags = getScnMdlBufferFlags(&m_resFile);
    u32 size;
    nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, m_resMdl, flags | 0x10b, 1, fn_801B0A28);
    if (drawPriority != 0) {
        scnMdl->SetPriorityDrawOpa(drawPriority);
        scnMdl->SetPriorityDrawXlu(drawPriority);
    }
    gfModelAnimation* anim = (gfModelAnimation*)operator new(0x20, heapType);
    if (anim != NULL) {
        anim = newModelAnimIdx(anim, m_resMdl, m_resFile, node, heapType);
    }
    gfModelAnimation::bind(scnMdl, anim);
    m_scnMdl = scnMdl;
    m_sceneModel = scnMdl;
    m_modelAnim = anim;
    anim->setFrame(0.0f);
}

void MuObject::initFromObject(MuObject* src, int drawPriority, int node, Heaps::HeapType heapType) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heapType);
    m_resFile = src->m_resFile;
    m_resMdl = src->m_resMdl;
    u32 flags = getScnMdlBufferFlags(&m_resFile);
    u32 size;
    nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, m_resMdl, flags | 0x10b, 1, fn_801B0A28);
    if (drawPriority != 0) {
        scnMdl->SetPriorityDrawOpa(drawPriority);
        scnMdl->SetPriorityDrawXlu(drawPriority);
    }
    gfModelAnimation* anim = (gfModelAnimation*)operator new(0x20, heapType);
    if (anim != NULL) {
        anim = newModelAnimIdxCopy(anim, m_resMdl, &src->m_resFile, node, heapType);
    }
    gfModelAnimation::bind(scnMdl, anim);
    m_scnMdl = scnMdl;
    m_sceneModel = scnMdl;
    m_modelAnim = anim;
    *(u8*)((u8*)anim + 4) = 0;
    anim->setFrame(0.0f);
}

MuObject* MuObject::create(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, Heaps::HeapType type) {
    return new (type) MuObject(modelSource, modelNode, drawPriority, textureSource, true, type);
}

MuObject* MuObject::create(nw4r::g3d::ResFile* modelSource, int modelNode, nw4r::g3d::ResFile* textureSource, int animNode, Heaps::HeapType type) {
    return new (type) MuObject(modelSource, animNode, modelNode, textureSource, true, type);
}

MuObject* MuObject::createAlt(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, Heaps::HeapType type) {
    return new (type) MuObject(modelSource, modelNode, drawPriority, textureSource, false, type);
}

// Frame policy used when a binding is replaced: clamp the frame into [start, end - epsilon].
float muObjPlayPolicyOneTime(float frame, float end, float start) {
    float d = start - frame;
    float last = end - 1.0f;
    float v = nw4r::math::FSelect(d, start, frame);
    return nw4r::math::FSelect(v - last, last, v);
}

static inline void setPolicy(void* obj, u32 policy) {
    void* fn;
    if (policy == 0) {
        fn = (void*)muObjPlayPolicyOneTime;
    } else {
        fn = lbl_8059C648[policy];
    }
    *(void**)((u8*)obj + 0x28) = fn;
}

static inline void setChrAnim(nw4r::g3d::ResAnmChr anim, gfModelAnimation* modelAnim, nw4r::g3d::ResMdl model, Heaps::HeapType heap) {
    int instanceSize;
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
    if (anim.IsValid()) {
        nw4r::g3d::AnmObjChrRes* anmObj = nw4r::g3d::AnmObjChrRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);
            if (modelAnim->m_anmObjChrRes != NULL) {
                modelAnim->m_anmObjChrRes->Destroy();
            }
            modelAnim->m_anmObjChrRes = anmObj;
        }
    }
}

static inline void setVisAnim(nw4r::g3d::ResAnmVis anim, gfModelAnimation* modelAnim, nw4r::g3d::ResMdl model, Heaps::HeapType heap) {
    int instanceSize;
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
    if (anim.IsValid()) {
        nw4r::g3d::AnmObjVisRes* anmObj = nw4r::g3d::AnmObjVisRes::Construct(allocator, &instanceSize, anim, model);
        if (anmObj != NULL) {
            anmObj->Bind(model);
            if (modelAnim->m_anmObjVisRes != NULL) {
                modelAnim->m_anmObjVisRes->Destroy();
            }
            modelAnim->m_anmObjVisRes = anmObj;
        }
    }
}

static inline void setTexPatAnim(nw4r::g3d::ResAnmTexPat anim, gfModelAnimation* modelAnim, nw4r::g3d::ResMdl model, Heaps::HeapType heap) {
    int instanceSize;
    if (anim.IsValid()) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
        nw4r::g3d::AnmObjTexPatRes* anmObj = nw4r::g3d::AnmObjTexPatRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);
            if (modelAnim->m_anmObjTexPatRes != NULL) {
                modelAnim->m_anmObjTexPatRes->Destroy();
            }
            modelAnim->m_anmObjTexPatRes = anmObj;
        }
    }
}

static inline void setTexSrtAnim(nw4r::g3d::ResAnmTexSrt anim, gfModelAnimation* modelAnim, nw4r::g3d::ResMdl model, Heaps::HeapType heap) {
    int instanceSize;
    if (anim.IsValid()) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
        nw4r::g3d::AnmObjTexSrtRes* anmObj = nw4r::g3d::AnmObjTexSrtRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);
            if (modelAnim->m_anmObjTexSrtRes != NULL) {
                modelAnim->m_anmObjTexSrtRes->Destroy();
            }
            modelAnim->m_anmObjTexSrtRes = anmObj;
        }
    }
}

static inline void setClrAnim(nw4r::g3d::ResAnmClr anim, gfModelAnimation* modelAnim, nw4r::g3d::ResMdl model, Heaps::HeapType heap) {
    int instanceSize;
    if (anim.IsValid()) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
        nw4r::g3d::AnmObjMatClrRes* anmObj = nw4r::g3d::AnmObjMatClrRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);
            if (modelAnim->m_anmObjMatClrRes != NULL) {
                modelAnim->m_anmObjMatClrRes->Destroy();
            }
            modelAnim->m_anmObjMatClrRes = anmObj;
        }
    }
}

static inline void setTexPatAnimIdx(u32 animId, nw4r::g3d::ResMdl model, Heaps::HeapType heap, gfModelAnimation* modelAnim) {
    if (animId < modelAnim->m_resFile.GetResAnmTexPatNumEntries()) {
        int instanceSize;
        nw4r::g3d::AnmObjTexPatRes* anmObj;
        nw4r::g3d::ResAnmTexPat anim = modelAnim->m_resFile.GetResAnmTexPat(animId);
        if (anim.IsValid()) {
            MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
            anmObj = nw4r::g3d::AnmObjTexPatRes::Construct(allocator, &instanceSize, anim, model, false);
            if (anmObj != NULL) {
                anmObj->Bind(model);
                if (modelAnim->m_anmObjTexPatRes != NULL) {
                    modelAnim->m_anmObjTexPatRes->Destroy();
                }
                modelAnim->m_anmObjTexPatRes = anmObj;
            }
        }
    }
}

static inline void setChrAnimIdx(u32 animId, nw4r::g3d::ResMdl model, Heaps::HeapType heap, gfModelAnimation* modelAnim) {
    if (animId < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        setChrAnim(modelAnim->m_resFile.GetResAnmChr(animId), modelAnim, model, heap);
    }
}

static inline void setVisAnimIdx(u32 animId, nw4r::g3d::ResMdl model, Heaps::HeapType heap, gfModelAnimation* modelAnim) {
    if (animId < modelAnim->m_resFile.GetResAnmVisNumEntries()) {
        setVisAnim(modelAnim->m_resFile.GetResAnmVis(animId), modelAnim, model, heap);
    }
}

static inline void setTexSrtAnimIdx(u32 animId, nw4r::g3d::ResMdl model, Heaps::HeapType heap, gfModelAnimation* modelAnim) {
    if (animId < modelAnim->m_resFile.GetResAnmTexSrtNumEntries()) {
        setTexSrtAnim(modelAnim->m_resFile.GetResAnmTexSrt(animId), modelAnim, model, heap);
    }
}

static inline void setClrAnimIdx(u32 animId, nw4r::g3d::ResMdl model, Heaps::HeapType heap, gfModelAnimation* modelAnim) {
    if (animId < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        setClrAnim(modelAnim->m_resFile.GetResAnmClr(animId), modelAnim, model, heap);
    }
}

static inline void bindNodeAnimImpl(MuObject* self, nw4r::g3d::ResAnmChr anim) {
    self->m_modelAnim->unbindNodeAnim(self->m_sceneModel);
    setChrAnim(anim, self->m_modelAnim, self->m_resMdl, self->m_heapType);
    self->m_modelAnim->bindNodeAnim(self->m_sceneModel);
    nw4r::g3d::AnmObjChrRes* o = self->m_modelAnim->m_anmObjChrRes;
    setPolicy(o, o->m_anmChrFile->m_loopMode);
    nw4r::g3d::ScnMdl* sceneModel = self->m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
}

void MuObject::changeNodeAnimN(const char* animName) {
    nw4r::g3d::ResAnmChr anim = m_resFile.GetResAnmChr(animName);
    if (anim.IsValid()) {
        bindNodeAnimImpl(this, anim);
    }
}

bool MuObject::changeNodeAnimNIf(const char* animName) {
    nw4r::g3d::ResAnmChr anim = m_resFile.GetResAnmChr(animName);
    if (!anim.IsValid()) {
        return false;
    }
    m_modelAnim->unbindNodeAnim(m_sceneModel);
    setChrAnim(anim, m_modelAnim, m_resMdl, m_heapType);
    m_modelAnim->bindNodeAnim(m_sceneModel);
    nw4r::g3d::AnmObjChrRes* o = m_modelAnim->m_anmObjChrRes;
    setPolicy(o, o->m_anmChrFile->m_loopMode);
    nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
    return true;
}

static inline void bindVisAnimImpl(MuObject* self, nw4r::g3d::ResAnmVis anim) {
    self->m_modelAnim->unbindVisibleAnim(self->m_sceneModel);
    setVisAnim(anim, self->m_modelAnim, self->m_resMdl, self->m_heapType);
    self->m_modelAnim->bindVisibleAnim(self->m_sceneModel);
    nw4r::g3d::AnmObjVisRes* o = self->m_modelAnim->m_anmObjVisRes;
    setPolicy(o, o->m_anmVisFile->m_loopMode);
    nw4r::g3d::ScnMdl* sceneModel = self->m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
}

void MuObject::changeVisAnimN(const char* animName) {
    nw4r::g3d::ResAnmVis anim = m_resFile.GetResAnmVis(animName);
    if (anim.IsValid()) {
        bindVisAnimImpl(this, anim);
    }
}

bool MuObject::changeVisAnimNIf(const char* animName) {
    nw4r::g3d::ResAnmVis anim = m_resFile.GetResAnmVis(animName);
    if (!anim.IsValid()) {
        return false;
    }
    m_modelAnim->unbindVisibleAnim(m_sceneModel);
    setVisAnim(anim, m_modelAnim, m_resMdl, m_heapType);
    m_modelAnim->bindVisibleAnim(m_sceneModel);
    nw4r::g3d::AnmObjVisRes* o = m_modelAnim->m_anmObjVisRes;
    setPolicy(o, o->m_anmVisFile->m_loopMode);
    nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
    return true;
}


static inline void changeNodeAnimIdx(MuObject* self, u32 index) {
    if (index < self->m_resFile.GetResAnmChrNumEntries()) {
        self->m_modelAnim->unbindNodeAnim(self->m_sceneModel);
        setChrAnimIdx(index, self->m_resMdl, self->m_heapType, self->m_modelAnim);
        self->m_modelAnim->bindNodeAnim(self->m_sceneModel);
        nw4r::g3d::AnmObjChrRes* o = self->m_modelAnim->m_anmObjChrRes;
        setPolicy(o, o->m_anmChrFile->m_loopMode);
        nw4r::g3d::ScnMdl* sceneModel = self->m_sceneModel;
        sceneModel->SetScnObjOption(2, 0);
        sceneModel->SetScnObjOption(5, 0);
    }
}

static inline void changeVisAnimIdx(MuObject* self, u32 index) {
    if (index < self->m_resFile.GetResAnmVisNumEntries()) {
        self->m_modelAnim->unbindVisibleAnim(self->m_sceneModel);
        setVisAnimIdx(index, self->m_resMdl, self->m_heapType, self->m_modelAnim);
        self->m_modelAnim->bindVisibleAnim(self->m_sceneModel);
        nw4r::g3d::AnmObjVisRes* o = self->m_modelAnim->m_anmObjVisRes;
        setPolicy(o, o->m_anmVisFile->m_loopMode);
        nw4r::g3d::ScnMdl* sceneModel = self->m_sceneModel;
        sceneModel->SetScnObjOption(2, 0);
        sceneModel->SetScnObjOption(5, 0);
    }
}

static inline void changeTexSrtAnimIdx(MuObject* self, u32 index) {
    if (index < self->m_resFile.GetResAnmTexSrtNumEntries()) {
        self->m_modelAnim->unbindTexSrtAnim(self->m_sceneModel);
        setTexSrtAnimIdx(index, self->m_resMdl, self->m_heapType, self->m_modelAnim);
        self->m_modelAnim->bindTexSrtAnim(self->m_sceneModel);
        nw4r::g3d::AnmObjTexSrtRes* o = self->m_modelAnim->m_anmObjTexSrtRes;
        setPolicy(o, o->m_anmTexSrtFile->m_loopMode);
        self->m_sceneModel->SetScnObjOption(3, 0);
    }
}

static inline void changeClrAnimIdx(MuObject* self, u32 index) {
    if (index < self->m_resFile.GetResAnmClrNumEntries()) {
        self->m_modelAnim->unbindMatColAnim(self->m_sceneModel);
        setClrAnimIdx(index, self->m_resMdl, self->m_heapType, self->m_modelAnim);
        self->m_modelAnim->bindMatColAnim(self->m_sceneModel);
        nw4r::g3d::AnmObjMatClrRes* o = self->m_modelAnim->m_anmObjMatClrRes;
        setPolicy(o, o->m_anmMatClrFile->m_loopMode);
        self->m_sceneModel->SetScnObjOption(3, 0);
    }
}

static inline void bindTexPatAnimIdxImpl(MuObject* self, u32 index) {
    self->m_modelAnim->unbindTexAnim(self->m_sceneModel);
    setTexPatAnimIdx(index, self->m_resMdl, self->m_heapType, self->m_modelAnim);
    self->m_modelAnim->bindTexAnim(self->m_sceneModel);
    nw4r::g3d::AnmObjTexPatRes* o = self->m_modelAnim->m_anmObjTexPatRes;
    setPolicy(o, o->m_anmTexPatFile->m_loopMode);
    self->m_sceneModel->SetScnObjOption(3, 0);
}

void MuObject::changeTexPatAnim(u32 index) {
    if (index < m_resFile.GetResAnmTexPatNumEntries()) {
        bindTexPatAnimIdxImpl(this, index);
    }
}

static inline void bindTexPatAnimImpl(MuObject* self, nw4r::g3d::ResAnmTexPat anim) {
    self->m_modelAnim->unbindTexAnim(self->m_sceneModel);
    setTexPatAnim(anim, self->m_modelAnim, self->m_resMdl, self->m_heapType);
    self->m_modelAnim->bindTexAnim(self->m_sceneModel);
    nw4r::g3d::AnmObjTexPatRes* o = self->m_modelAnim->m_anmObjTexPatRes;
    setPolicy(o, o->m_anmTexPatFile->m_loopMode);
    self->m_sceneModel->SetScnObjOption(3, 0);
}

void MuObject::changeTexPatAnimN(const char* animName) {
    nw4r::g3d::ResAnmTexPat anim = m_resFile.GetResAnmTexPat(animName);
    if (anim.IsValid()) {
        bindTexPatAnimImpl(this, anim);
    }
}

bool MuObject::changeTexPatAnimNIf(const char* animName) {
    nw4r::g3d::ResAnmTexPat anim = m_resFile.GetResAnmTexPat(animName);
    if (!anim.IsValid()) {
        return false;
    }
    m_modelAnim->unbindTexAnim(m_sceneModel);
    setTexPatAnim(anim, m_modelAnim, m_resMdl, m_heapType);
    m_modelAnim->bindTexAnim(m_sceneModel);
    nw4r::g3d::AnmObjTexPatRes* o = m_modelAnim->m_anmObjTexPatRes;
    setPolicy(o, o->m_anmTexPatFile->m_loopMode);
    m_sceneModel->SetScnObjOption(3, 0);
    return true;
}

static inline void bindTexSrtAnimImpl(MuObject* self, nw4r::g3d::ResAnmTexSrt anim) {
    self->m_modelAnim->unbindTexSrtAnim(self->m_sceneModel);
    setTexSrtAnim(anim, self->m_modelAnim, self->m_resMdl, self->m_heapType);
    self->m_modelAnim->bindTexSrtAnim(self->m_sceneModel);
    nw4r::g3d::AnmObjTexSrtRes* o = self->m_modelAnim->m_anmObjTexSrtRes;
    setPolicy(o, o->m_anmTexSrtFile->m_loopMode);
    self->m_sceneModel->SetScnObjOption(3, 0);
}

void MuObject::changeTexSrtAnimN(const char* animName) {
    nw4r::g3d::ResAnmTexSrt anim = m_resFile.GetResAnmTexSrt(animName);
    if (anim.IsValid()) {
        bindTexSrtAnimImpl(this, anim);
    }
}

bool MuObject::changeTexSrtAnimNIf(const char* animName) {
    nw4r::g3d::ResAnmTexSrt anim = m_resFile.GetResAnmTexSrt(animName);
    if (!anim.IsValid()) {
        return false;
    }
    m_modelAnim->unbindTexSrtAnim(m_sceneModel);
    setTexSrtAnim(anim, m_modelAnim, m_resMdl, m_heapType);
    m_modelAnim->bindTexSrtAnim(m_sceneModel);
    nw4r::g3d::AnmObjTexSrtRes* o = m_modelAnim->m_anmObjTexSrtRes;
    setPolicy(o, o->m_anmTexSrtFile->m_loopMode);
    m_sceneModel->SetScnObjOption(3, 0);
    return true;
}

static inline void bindClrAnimImpl(MuObject* self, nw4r::g3d::ResAnmClr anim) {
    self->m_modelAnim->unbindMatColAnim(self->m_sceneModel);
    setClrAnim(anim, self->m_modelAnim, self->m_resMdl, self->m_heapType);
    self->m_modelAnim->bindMatColAnim(self->m_sceneModel);
    nw4r::g3d::AnmObjMatClrRes* o = self->m_modelAnim->m_anmObjMatClrRes;
    setPolicy(o, o->m_anmMatClrFile->m_loopMode);
    self->m_sceneModel->SetScnObjOption(3, 0);
}

void MuObject::changeClrAnimN(const char* animName) {
    nw4r::g3d::ResAnmClr anim = m_resFile.GetResAnmClr(animName);
    if (anim.IsValid()) {
        bindClrAnimImpl(this, anim);
    }
}

bool MuObject::changeClrAnimNIf(const char* animName) {
    nw4r::g3d::ResAnmClr anim = m_resFile.GetResAnmClr(animName);
    if (!anim.IsValid()) {
        return false;
    }
    m_modelAnim->unbindMatColAnim(m_sceneModel);
    setClrAnim(anim, m_modelAnim, m_resMdl, m_heapType);
    m_modelAnim->bindMatColAnim(m_sceneModel);
    nw4r::g3d::AnmObjMatClrRes* o = m_modelAnim->m_anmObjMatClrRes;
    setPolicy(o, o->m_anmMatClrFile->m_loopMode);
    m_sceneModel->SetScnObjOption(3, 0);
    return true;
}

u16 MuObject::getNodeAnimLength() {
    nw4r::g3d::AnmObjChrRes* o = m_modelAnim->m_anmObjChrRes;
    if (o == NULL) {
        return 0;
    }
    return o->m_anmChrFile.ptr()->m_animLength;
}

void MuObject::changeAnimN(const char* animName) {
    {
        nw4r::g3d::ResAnmChr anim = m_resFile.GetResAnmChr(animName);
        if (anim.IsValid()) {
            bindNodeAnimImpl(this, anim);
        }
    }
    {
        nw4r::g3d::ResAnmVis anim = m_resFile.GetResAnmVis(animName);
        if (anim.IsValid()) {
            bindVisAnimImpl(this, anim);
        }
    }
    {
        nw4r::g3d::ResAnmTexPat anim = m_resFile.GetResAnmTexPat(animName);
        if (anim.IsValid()) {
            bindTexPatAnimImpl(this, anim);
        }
    }
    {
        nw4r::g3d::ResAnmTexSrt anim = m_resFile.GetResAnmTexSrt(animName);
        if (anim.IsValid()) {
            bindTexSrtAnimImpl(this, anim);
        }
    }
    {
        nw4r::g3d::ResAnmClr anim = m_resFile.GetResAnmClr(animName);
        if (anim.IsValid()) {
            bindClrAnimImpl(this, anim);
        }
    }
}

const char* MuObject::getNodeAnimName() {
    nw4r::g3d::AnmObjChrRes* o = m_modelAnim->m_anmObjChrRes;
    if (o == NULL) {
        return NULL;
    }
    nw4r::g3d::ResAnmChrData* d = o->m_anmChrFile.ptr();
    u32 offset = d->m_stringOffset;
    if (offset != 0) {
        return (const char*)((u8*)d + offset);
    }
    return NULL;
}

const char* MuObject::getVisAnimName() {
    nw4r::g3d::AnmObjVisRes* o = m_modelAnim->m_anmObjVisRes;
    if (o == NULL) {
        return NULL;
    }
    nw4r::g3d::ResAnmVisData* d = o->m_anmVisFile.ptr();
    u32 offset = d->m_stringOffset;
    if (offset != 0) {
        return (const char*)((u8*)d + offset);
    }
    return NULL;
}

const char* MuObject::getTexPatAnimName() {
    nw4r::g3d::AnmObjTexPatRes* o = m_modelAnim->m_anmObjTexPatRes;
    if (o == NULL) {
        return NULL;
    }
    nw4r::g3d::ResAnmTexPatData* d = o->m_anmTexPatFile.ptr();
    u32 offset = d->m_stringOffset;
    if (offset != 0) {
        return (const char*)((u8*)d + offset);
    }
    return NULL;
}

const char* MuObject::getTexSrtAnimName() {
    nw4r::g3d::AnmObjTexSrtRes* o = m_modelAnim->m_anmObjTexSrtRes;
    if (o == NULL) {
        return NULL;
    }
    nw4r::g3d::ResAnmTexSrtData* d = o->m_anmTexSrtFile.ptr();
    u32 offset = d->m_stringOffset;
    if (offset != 0) {
        return (const char*)((u8*)d + offset);
    }
    return NULL;
}

const char* MuObject::getClrAnimName() {
    nw4r::g3d::AnmObjMatClrRes* o = m_modelAnim->m_anmObjMatClrRes;
    if (o == NULL) {
        return NULL;
    }
    nw4r::g3d::ResAnmClrData* d = o->m_anmMatClrFile.ptr();
    u32 offset = d->m_stringOffset;
    if (offset != 0) {
        return (const char*)((u8*)d + offset);
    }
    return NULL;
}

bool MuObject::isNodeAnimFinished() {
    if (m_modelAnim == NULL) {
        return true;
    }
    nw4r::g3d::AnmObjChrRes* o = m_modelAnim->m_anmObjChrRes;
    if (o == NULL) {
        return true;
    }
    nw4r::g3d::ResAnmChrData* d = o->m_anmChrFile.ptr();
    if (d->m_loopMode != 1) {
        u16 length = d->m_animLength;
        if (o->GetFrame() >= (float)(length - 1)) {
            return true;
        }
    }
    return false;
}

bool MuObject::isVisAnimFinished() {
    if (m_modelAnim == NULL) {
        return true;
    }
    nw4r::g3d::AnmObjVisRes* o = m_modelAnim->m_anmObjVisRes;
    if (o == NULL) {
        return true;
    }
    nw4r::g3d::ResAnmVisData* d = o->m_anmVisFile.ptr();
    if (d->m_loopMode != 1) {
        u16 length = d->m_animLength;
        if (o->GetFrame() >= (float)(length - 1)) {
            return true;
        }
    }
    return false;
}

bool MuObject::isClrAnimFinished() {
    if (m_modelAnim == NULL) {
        return true;
    }
    nw4r::g3d::AnmObjMatClrRes* o = m_modelAnim->m_anmObjMatClrRes;
    if (o == NULL) {
        return true;
    }
    nw4r::g3d::ResAnmClrData* d = o->m_anmMatClrFile.ptr();
    if (d->m_loopMode != 1) {
        u16 length = d->m_animLength;
        if (o->GetFrame() >= (float)(length - 1)) {
            return true;
        }
    }
    return false;
}

inline bool MuObject::isTexPatAnimFinished() {
    if (m_modelAnim == NULL) {
        return true;
    }
    nw4r::g3d::AnmObjTexPatRes* o = m_modelAnim->m_anmObjTexPatRes;
    if (o == NULL) {
        return true;
    }
    nw4r::g3d::ResAnmTexPatData* d = o->m_anmTexPatFile.ptr();
    if (d->m_loopMode != 1) {
        u16 length = d->m_animLength;
        if (o->GetFrame() >= (float)(length - 1)) {
            return true;
        }
    }
    return false;
}

inline bool MuObject::isTexSrtAnimFinished() {
    if (m_modelAnim == NULL) {
        return true;
    }
    nw4r::g3d::AnmObjTexSrtRes* o = m_modelAnim->m_anmObjTexSrtRes;
    if (o == NULL) {
        return true;
    }
    nw4r::g3d::ResAnmTexSrtData* d = o->m_anmTexSrtFile.ptr();
    if (d->m_loopMode != 1) {
        u16 length = d->m_animLength;
        if (o->GetFrame() >= (float)(length - 1)) {
            return true;
        }
    }
    return false;
}

bool MuObject::isAnimFinished() {
    if (!isNodeAnimFinished()) {
        return false;
    }
    if (!isVisAnimFinished()) {
        return false;
    }
    if (!isTexPatAnimFinished()) {
        return false;
    }
    if (!isTexSrtAnimFinished()) {
        return false;
    }
    return isClrAnimFinished();
}

bool MuObject::isNodeAnimLoop() {
    nw4r::g3d::AnmObjChrRes* o = m_modelAnim->m_anmObjChrRes;
    if (o == NULL) {
        return false;
    }
    return o->m_anmChrFile->m_loopMode == 1;
}

void MuObject::getPos(Vec3f* pos) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(m_baseNode);
    pos->m_x = node->m_translation.m_x;
    pos->m_y = node->m_translation.m_y;
    pos->m_z = node->m_translation.m_z;
}

void MuObject::getPos(Vec3f* pos, const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    pos->m_x = node->m_translation.m_x;
    pos->m_y = node->m_translation.m_y;
    pos->m_z = node->m_translation.m_z;
}

void MuObject::setPos(Vec3f* pos) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(m_baseNode);
    node.SetTranslate(pos->m_x, pos->m_y, pos->m_z);
}

void MuObject::setPos(Vec3f* pos, const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    node.SetTranslate(pos->m_x, pos->m_y, pos->m_z);
}

void MuObject::setTrans(Vec3f* trans) {
    m_trans = *trans;
    Vec3f scale(1.0f, 1.0f, 1.0f);
    Matrix mtx;
    mtx.setSRT(scale, m_rot, m_trans);
    ScnMdl_SetNodeMtx(m_scnMdl, 0, &mtx);
    nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
}

void MuObject::getRect3D(Rect2D* rect, const char* nodeNameA, const char* nodeNameB) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode nodeA = mdl.GetResNode(nodeNameA);
    nw4r::g3d::ResNode nodeB = mdl.GetResNode(nodeNameB);
    u32 idB = nodeB.IsValid() ? nodeB->m_nodeIndex : 0;
    Vec3f a;
    a = nwSMGetGlobalPosition(m_sceneModel, nodeA.IsValid() ? nodeA->m_nodeIndex : 0);
    Vec3f b;
    b = nwSMGetGlobalPosition(m_sceneModel, idB);
    rect->m_left = nw4r::math::FSelect(a.m_x - b.m_x, b.m_x, a.m_x);
    rect->m_right = nw4r::math::FSelect(a.m_x - b.m_x, a.m_x, b.m_x);
    rect->m_up = nw4r::math::FSelect(b.m_y - a.m_y, b.m_y, a.m_y);
    rect->m_down = nw4r::math::FSelect(b.m_y - a.m_y, a.m_y, b.m_y);
}

void MuObject::getRect3D(Rect2D* rect, int nodeA, int nodeB) {
    Vec3f a;
    a = nwSMGetGlobalPosition(m_sceneModel, nodeA);
    Vec3f b;
    b = nwSMGetGlobalPosition(m_sceneModel, nodeB);
    rect->m_left = nw4r::math::FSelect(a.m_x - b.m_x, b.m_x, a.m_x);
    rect->m_right = nw4r::math::FSelect(a.m_x - b.m_x, a.m_x, b.m_x);
    rect->m_up = nw4r::math::FSelect(b.m_y - a.m_y, b.m_y, a.m_y);
    rect->m_down = nw4r::math::FSelect(b.m_y - a.m_y, a.m_y, b.m_y);
}

nw4r::g3d::ResNode MuObject::getNode(const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    return mdl.GetResNode(nodeName);
}

u32 MuObject::getNodeID(const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    return node.IsValid() ? node->m_nodeIndex : 0;
}

Matrix MuObject::getNodeMatrix(const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    Vec3f trans = node->m_translation;
    Vec3f rot = node->m_rotation;
    Vec3f scale = node->m_scale;
    Matrix mtx(true);
    mtx.setSRT(scale, rot, trans);
    return mtx;
}

Vec3f MuObject::getGlobalPosition(int resNode) {
    return nwSMGetGlobalPosition(m_sceneModel, resNode);
}

Vec3f MuObject::getGlobalPosition(const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    return nwSMGetGlobalPosition(m_sceneModel, node.IsValid() ? node->m_nodeIndex : 0);
}

Vec3f MuObject::getGlobalPosition() {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(m_baseNode);
    return nwSMGetGlobalPosition(m_sceneModel, node.IsValid() ? node->m_nodeIndex : 0);
}

void MuObject::getAnimScale(Vec3f* scl, const char* name) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(name);
    scl->m_x = node->m_scale.m_x;
    scl->m_y = node->m_scale.m_y;
    scl->m_z = node->m_scale.m_z;
}

Vec3f MuObject::getScale(const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    u32 id = node.IsValid() ? node->m_nodeIndex : 0;
    ChrAnmResult res;
    m_modelAnim->m_anmObjChrRes->GetResult((int*)&res, id);
    Vec3f scale;
    ChrAnmResult_GetScale(&res, &scale);
    Vec3f ret;
    ret = scale;
    return ret;
}

static inline void ScnObj_SetScale(nw4r::g3d::ScnMdl* mdl, const Vec3f& v) {
    Vec3f* dst = (Vec3f*)((u8*)mdl + 0xdc);
    float x = v.m_x;
    float y = v.m_y;
    dst->m_x = x;
    dst->m_y = y;
    dst->m_z = v.m_z;
}

void MuObject::setObjScale(Vec3f* scale) {
    ScnObj_SetScale(m_sceneModel, *scale);
    nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
}

static inline Vec3f ScnObj_GetScale(nw4r::g3d::ScnMdl* mdl) {
    Vec3f* src = (Vec3f*)((u8*)mdl + 0xdc);
    Vec3f r;
    r = *src;
    return r;
}

Vec3f MuObject::getObjScale() {
    return ScnObj_GetScale(m_sceneModel);
}

void MuObject::setRotate(Vec3f* rot) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(m_baseNode);
    ResNode_SetRotate(&node, rot->m_x, rot->m_y, rot->m_z);
}

void MuObject::setRotate(const char* nodeName, Vec3f* rot) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    ResNode_SetRotate(&node, rot->m_x, rot->m_y, rot->m_z);
}

void MuObject::setObjRotate(Vec3f* rot) {
    m_rot = *rot;
    Vec3f scale(1.0f, 1.0f, 1.0f);
    Matrix mtx;
    mtx.setSRT(scale, m_rot, m_trans);
    ScnMdl_SetNodeMtx(m_scnMdl, 0, &mtx);
    nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
    sceneModel->SetScnObjOption(2, 0);
    sceneModel->SetScnObjOption(5, 0);
}

void MuObject::getRotate(Vec3f* rot, const char* nodeName) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    rot->m_x = node->m_rotation.m_x;
    rot->m_y = node->m_rotation.m_y;
    rot->m_z = node->m_rotation.m_z;
}

void MuObject::setScale(const char* nodeName, Vec3f* scale) {
    nw4r::g3d::ResMdl mdl = m_sceneModel->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(nodeName);
    ResNode_SetScale(&node, scale->m_x, scale->m_y, scale->m_z);
}

void MuObject::setFrameNode(float frame) {
    if (frame != m_modelAnim->m_anmObjChrRes->GetFrame()) {
        nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
        sceneModel->SetScnObjOption(2, 0);
        sceneModel->SetScnObjOption(5, 0);
    }
    m_modelAnim->m_anmObjChrRes->SetFrame(frame);
}

void MuObject::setFrameVisible(float frame) {
    if (frame != m_modelAnim->m_anmObjVisRes->GetFrame()) {
        nw4r::g3d::ScnMdl* sceneModel = m_sceneModel;
        sceneModel->SetScnObjOption(2, 0);
        sceneModel->SetScnObjOption(5, 0);
    }
    m_modelAnim->m_anmObjVisRes->SetFrame(frame);
}

void MuObject::setFrameTex(float frame) {
    if (frame != m_modelAnim->m_anmObjTexPatRes->GetFrame()) {
        m_sceneModel->SetScnObjOption(3, 0);
    }
    m_modelAnim->m_anmObjTexPatRes->SetFrame(frame);
}

void MuObject::setFrameTexSrt(float frame) {
    if (frame != m_modelAnim->m_anmObjTexSrtRes->GetFrame()) {
        m_sceneModel->SetScnObjOption(3, 0);
    }
    m_modelAnim->m_anmObjTexSrtRes->SetFrame(frame);
}

void MuObject::setFrameMatCol(float frame) {
    if (frame != m_modelAnim->m_anmObjMatClrRes->GetFrame()) {
        m_sceneModel->SetScnObjOption(3, 0);
    }
    m_modelAnim->m_anmObjMatClrRes->SetFrame(frame);
}

void MuObject::changeMaterialTex(const char* matName, GXTexObj* srcObj) {
    u32 matId = m_resMdl.GetResMat(matName)->m_id;
    nw4r::g3d::ScnMdl::CopiedMatAccess access(m_scnMdl, matId);
    void* image;
    GXTexFmt fmt;
    GXTexWrapMode wrapS;
    GXTexWrapMode wrapT;
    nw4r::g3d::ResTexObj texObj(CopiedMatAccess_GetResTexObj(&access, false));
    u16 width;
    u16 height;
    GXBool mipmap;
    GXTexObj* obj = texObj.GetTexObj(GX_TEXMAP0);
    GXGetTexObjAll(srcObj, &image, &width, &height, &fmt, &wrapS, &wrapT, &mipmap);
    GXInitTexObj(obj, image, width, height, fmt, wrapS, wrapT, mipmap);
}

void MuObject::changeMaterialTex(const char* matName, void* image, u16 width, u16 height) {
    nw4r::g3d::ResTexObj texObj(NULL);
    u32 matId = m_resMdl.GetResMat(matName)->m_id;
    nw4r::g3d::ScnMdl::CopiedMatAccess access(m_scnMdl, matId);
    texObj = nw4r::g3d::ResTexObj(CopiedMatAccess_GetResTexObj(&access, false));
    GXTexObj* obj = texObj.GetTexObj(GX_TEXMAP0);
    GXInitTexObj(obj, image, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, false);
    GXLoadTexObj(obj, GX_TEXMAP0);
}

void MuObject::changeMaterialTex(const char* matName, int texIndex, nw4r::g3d::ResFile* texFile) {
    nw4r::g3d::ResFile* file = texFile == NULL ? &m_resFile : texFile;
    nw4r::g3d::ResTexData* tex = file->GetResTex(texIndex).ptr();
    u32 off = tex->m_nameOffset;
    const char* texName = off != 0 ? (const char*)((u8*)tex + off) : NULL;
    changeMaterialTex(m_resMdl.GetResMat(matName)->m_id, texName, texFile);
}

void MuObject::changeMaterialTex(u32 matId, const char* texName, nw4r::g3d::ResFile* texFile) {
    nw4r::g3d::ScnMdl::CopiedMatAccess access(m_scnMdl, matId);
    nw4r::g3d::ResTexObj texObj(CopiedMatAccess_GetResTexObj(&access, false));
    GXTexObj* obj = texObj.GetTexObj(GX_TEXMAP0);
    nw4r::g3d::ResTex tex(NULL);
    if (texFile == NULL) {
        texFile = &m_resFile;
    }
    tex = texFile->GetResTex(texName);
    if (tex->m_flags & 1) {
        u8* pltt = fn_8018D310(texFile, texName);
        void* tlutObjPtr = fn_801AFF94(&access, false);
        GXTlutObj* tlut = fn_80190DC8(&tlutObjPtr, 0);
        GXBool mipmap;
        f32 maxLod;
        f32 minLod;
        GXTexFmt fmt;
        u16 height;
        u16 width;
        void* image;
        fn_80192B00(&tex, &image, &width, &height, &fmt, &minLod, &maxLod, &mipmap);
        s32 plttOff = *(s32*)(pltt + 0x10);
        s32 mask = (-plttOff | plttOff) >> 31;
        void* plttData = (void*)((u32)(pltt + plttOff) & mask);
        GXInitTlutObj(tlut, plttData, (GXTlutFmt) * (u32*)(pltt + 0x18), *(u16*)(pltt + 0x1c));
        GXLoadTlut(tlut, GX_TLUT0);
        GXInitTexObjCI(obj, image, width, height, fmt, GX_CLAMP, GX_CLAMP, mipmap, 0);
        GXLoadTexObj(obj, GX_TEXMAP0);
        fn_801AFF94(&access, false);
        CopiedMatAccess_GetResTexObj(&access, false);
    } else {
        GXBool mipmap;
        f32 maxLod;
        f32 minLod;
        GXTexFmt fmt;
        u16 height;
        u16 width;
        void* image;
        fn_80192A44(&tex, &image, &width, &height, &fmt, &minLod, &maxLod, &mipmap);
        GXTexWrapMode wrapS = GXGetTexObjWrapS(obj);
        GXTexWrapMode wrapT = GXGetTexObjWrapT(obj);
        GXInitTexObj(obj, image, width, height, fmt, wrapS, wrapT, mipmap);
        CopiedMatAccess_GetResTexObj(&access, false);
    }
}

void MuObject::setFrame(float frame) {
    if (m_modelAnim->m_anmObjChrRes != NULL) {
        setFrameNode(frame);
    }
    if (m_modelAnim->m_anmObjVisRes != NULL) {
        setFrameVisible(frame);
    }
    if (m_modelAnim->m_anmObjTexPatRes != NULL) {
        setFrameTex(frame);
    }
    if (m_modelAnim->m_anmObjTexSrtRes != NULL) {
        setFrameTexSrt(frame);
    }
    if (m_modelAnim->m_anmObjMatClrRes != NULL) {
        setFrameMatCol(frame);
    }
}

static inline const char* ResMdl_GetName(nw4r::g3d::ResMdl& mdl) {
    u8* data = (u8*)mdl.ptr();
    u32 off = *(u32*)(data + 0x3c);
    return off != 0 ? (const char*)(data + off) : NULL;
}

void MuObject::changeAnimN(int animId, u32 flags, bool play, bool reset) {
    char name[0x40];
    sprintf(name, lbl_8059DF20, ResMdl_GetName(m_resMdl), animId);
    float rate = play ? 1.0f : 0.0f;
    if (flags & 1) {
        changeNodeAnimN(name);
        m_modelAnim->m_anmObjChrRes->SetUpdateRate(rate);
        if (reset) {
            setFrameNode(0.0f);
        }
    }
    if (flags & 2) {
        changeVisAnimN(name);
        m_modelAnim->m_anmObjVisRes->SetUpdateRate(rate);
        if (reset) {
            setFrameVisible(0.0f);
        }
    }
    if (flags & 4) {
        changeTexPatAnimN(name);
        m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(rate);
        if (reset) {
            setFrameTex(0.0f);
        }
    }
    if (flags & 8) {
        changeTexSrtAnimN(name);
        m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(rate);
        if (reset) {
            setFrameTexSrt(0.0f);
        }
    }
    if (flags & 0x10) {
        changeClrAnimN(name);
        m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(rate);
        if (reset) {
            setFrameMatCol(0.0f);
        }
    }
}

void MuObject::setAnimIdx(MuAnimIdxData* data, bool force) {
    bool changed = false;
    if (force) {
        if (m_animIdxData != NULL) {
            if (m_animIdxData->m_index != data->m_index) {
                changed = true;
            }
        } else {
            changed = true;
        }
    }
    m_animIdxData = data;
    if (m_animIdxData->m_flags & 1) {
        if (m_modelAnim->m_anmObjChrRes != NULL) {
            if (changed) {
                changeNodeAnimIdx(this, data->m_index);
            }
            setFrameNode(data->m_frame);
            m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        }
    }
    if (m_animIdxData->m_flags & 2) {
        if (m_modelAnim->m_anmObjVisRes != NULL) {
            if (changed) {
                changeVisAnimIdx(this, data->m_index);
            }
            setFrameVisible(data->m_frame);
            m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
        }
    }
    if (m_animIdxData->m_flags & 4) {
        if (m_modelAnim->m_anmObjTexPatRes != NULL) {
            if (changed) {
                changeTexPatAnim(data->m_index);
            }
            setFrameTex(data->m_frame);
            m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(1.0f);
        }
    }
    if (m_animIdxData->m_flags & 8) {
        if (m_modelAnim->m_anmObjTexSrtRes != NULL) {
            if (changed) {
                changeTexSrtAnimIdx(this, data->m_index);
            }
            setFrameTexSrt(data->m_frame);
            m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
        }
    }
    if (m_animIdxData->m_flags & 16) {
        if (m_modelAnim->m_anmObjMatClrRes != NULL) {
            if (changed) {
                changeClrAnimIdx(this, data->m_index);
            }
            setFrameMatCol(data->m_frame);
            m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
    }
}

void MuObject::setAnimName(MuAnimNameData* data, bool force) {
    const char* name;
    bool changed = false;
    if (force) {
        if (m_animNameData != NULL) {
            if (strcmp(m_animNameData->m_name, data->m_name) != 0) {
                changed = true;
            }
        } else {
            changed = true;
        }
    }
    m_animNameData = data;
    name = data->m_name;
    {
        bool valid = true;
        if (m_animNameData->m_flags & 1) {
            if (changed || m_modelAnim->m_anmObjChrRes == NULL) {
                nw4r::g3d::ResAnmChr anim = m_resFile.GetResAnmChr(name);
                if (!anim.IsValid()) {
                    valid = false;
                } else {
                    bindNodeAnimImpl(this, anim);
                    valid = true;
                }
            }
            if (valid) {
                setFrameNode(data->m_frame);
                m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            }
        }
    }
    {
        bool valid = true;
        if (m_animNameData->m_flags & 2) {
            if (changed || m_modelAnim->m_anmObjVisRes == NULL) {
                nw4r::g3d::ResAnmVis anim = m_resFile.GetResAnmVis(name);
                if (!anim.IsValid()) {
                    valid = false;
                } else {
                    bindVisAnimImpl(this, anim);
                    valid = true;
                }
            }
            if (valid) {
                setFrameVisible(data->m_frame);
                m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
            }
        }
    }
    {
        bool valid = true;
        if (m_animNameData->m_flags & 4) {
            if (changed || m_modelAnim->m_anmObjTexPatRes == NULL) {
                nw4r::g3d::ResAnmTexPat anim = m_resFile.GetResAnmTexPat(name);
                if (!anim.IsValid()) {
                    valid = false;
                } else {
                    bindTexPatAnimImpl(this, anim);
                    valid = true;
                }
            }
            if (valid) {
                setFrameTex(data->m_frame);
                m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(1.0f);
            }
        }
    }
    {
        bool valid = true;
        if (m_animNameData->m_flags & 8) {
            if (changed || m_modelAnim->m_anmObjTexSrtRes == NULL) {
                nw4r::g3d::ResAnmTexSrt anim = m_resFile.GetResAnmTexSrt(name);
                if (!anim.IsValid()) {
                    valid = false;
                } else {
                    bindTexSrtAnimImpl(this, anim);
                    valid = true;
                }
            }
            if (valid) {
                setFrameTexSrt(data->m_frame);
                m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
            }
        }
    }
    {
        bool valid = true;
        if (m_animNameData->m_flags & 16) {
            if (changed || m_modelAnim->m_anmObjMatClrRes == NULL) {
                nw4r::g3d::ResAnmClr anim = m_resFile.GetResAnmClr(name);
                if (!anim.IsValid()) {
                    valid = false;
                } else {
                    bindClrAnimImpl(this, anim);
                    valid = true;
                }
            }
            if (valid) {
                setFrameMatCol(data->m_frame);
                m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            }
        }
    }
}

void MuObject::calcMatrix() {
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModel;
    scnMdl->SetScnObjOption(2, 0);
    scnMdl->SetScnObjOption(5, 0);
}

// TODO: clean up, give variables real names
namespace jj_800B85AC {
extern "C" void fn_800B85AC(void) {}
}

namespace jj_800B092C {
struct Obj {
    u8 pad[0x2c];
    u8 flags;
    u8 pad2[0x40 - 0x2d];
    u32 unk40;
    u32 unk44;
};

extern "C" {
void fn_800B092C(Obj* p) {
    p->unk40 = 0;
    p->flags &= ~2;
    p->unk44 = 0;
}
}
}

namespace jj_800B803C {
extern "C" {

struct Obj {
    virtual s32 v0();
    virtual s32 v1();
    virtual s32 v2();
    virtual s32 v3();
    virtual s32 v4();
    virtual s32 v5();
};

struct Holder {
    s32 pad0;
    Obj* target;
};

// TODO: clean up, give variables real names
s32 fn_800B803C(Holder* h) {
    Obj* t = h->target;
    if (t)
        return t->v5();
    return (s32)t;
}

}
}

namespace jj_800B805C {
class A {
public:
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
};

// TODO: clean up, give variables real names
extern "C" void fn_800B805C(int* self) {
    A* obj = *(A**)(self + 1);
    if (!obj)
        return;
    obj->m4();
}
}

namespace jj_800B858C {
class Child {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
};

class Parent {
public:
    char pad[4];
    Child* m_child;
};

extern "C" {
// TODO: clean up, give variables real names
void fn_800B858C(Parent* p) {
    if (p->m_child) {
        p->m_child->v1();
    }
}
}
}

namespace jj_800B8608 {
// Release a heap allocation when requested, preserving its original pointer.
extern "C" void __dl__FPv(void*);

// TODO: clean up, give variables real names
extern "C" void* fn_800B8608(void* arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv(arg0);
    }
    return arg0;
}
}

namespace jj_800B85B0 {
extern "C" {
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
}

// TODO: clean up, give variables real names
extern "C" void* fn_800B85B0(void* p, int n) {
    if (p) {
        __dt__6gfTaskFv(p, 0);
        if (n > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

namespace jj_800B881C {
extern "C" int fn_8006CA64();
extern "C" int fn_8006C90C();
extern "C" int fn_8006C9EC();
extern "C" int fn_8006C790();
// TODO: clean up, give variables real names
extern "C" void fn_800B881C(unsigned char* p, int index) {
    int flags = 0;
    if ((unsigned int)((p[16] >> 7) & 1) != 1) {
        unsigned char old = p[index];
        p[index] = 0;
        for (int i = 0; i < 16; i++) flags |= p[i];
        if ((old & 2) && !(flags & 2U)) fn_8006CA64();
        if ((old & 1) && !(flags & 1)) fn_8006C90C();
        if ((old & 4) && !(flags & 4)) fn_8006C9EC();
        if ((old & 8) && !(flags & 8)) fn_8006C790();
    }
}
}

namespace jj_800B8648 {
extern "C" s32 fn_8006C7D4(s32);
extern "C" s32 fn_8006C870(s32);
extern "C" s32 fn_8006C950(s32);
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u8 fn_800B8648(u8* arg0, s32 arg1, s32 arg2, s32 arg3) {
    if ((u32)((*(u8*)(arg0 + 0x10) >> 7) & 1) == 1U) return 0;
    u32 orval;
    s32 flag;
    flag = arg1;
    if (arg1 & 4) flag = arg1 & ~1;
    orval = 0;
    for (s32 i = 0; i < 16; i++) orval |= arg0[i];
    if ((flag & 1) && !(orval & 1)) {
        s32 val = arg2;
        if (arg2 == 0xFF) val = 0x2F;
        if (arg3 == 0) fn_8006C7D4(val);
        else fn_8006C870(val);
    }
    if ((flag & 4) && !(orval & 4)) {
        s32 val2 = arg2;
        if (arg2 == 0xFF) val2 = 0x31;
        fn_8006C950(val2);
    }
    s32 idx;
    for (idx = 0; idx < 16; idx++) {
        if (arg0[idx] == 0) break;
    }
    *(u8*)(arg0 + idx) = (u8)arg1;
    return (u8)idx;
}
}

namespace jj_800B8350 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

extern f32 lbl_805A2200;

struct G {
    u8 p0[0x1C];
    u16 h1c;
    u8 p1[2];
    s32 w20;
};

class VT {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual f32 v6();
    virtual void v7();
    virtual void v8();
    virtual f32 v9();
    u8 pad[0x28];
    G* g;
};

class Iface {
public:
    virtual void i0();
    virtual void i1();
    virtual void i2();
    virtual void i3();
    virtual void i4();
    virtual void i5();
    virtual void i6(s32, s32);
};

struct C {
    u8 p0[8];
    VT* y;
    VT* x;
};

struct B {
    u8 p0[0x14];
    C* c;
};

struct Obj {
    u32 p0;
    VT* a;
    B* b;
};

extern "C" {
// TODO: clean up, give variables real names
void fn_800B8350(Obj* self, s32 arg1, Iface* arg2) {
    C* c;
    VT* z;
    G* g;
    s32 r0;

    if (self->a != 0) {
        self->a->v3();
    }
    if (arg1 == 4) {
        c = self->b->c;
        if (c == 0) {
            r0 = 1;
        } else {
            z = c->x;
            if (z == 0) {
                r0 = 1;
            } else {
                g = z->g;
                u16 h1;
                if (g->w20 != 1 && ((h1 = g->h1c), 1) && z->v6() >= (f32)(h1 - 1)) {
                    r0 = 1;
                } else {
                    r0 = 0;
                }
            }
        }
        s32 f1 = 1;
        if (r0 == 0) {
            if (lbl_805A2200 != self->b->c->x->v9()) {
                f1 = 0;
            }
        }
        c = self->b->c;
        if (c == 0) {
            r0 = 1;
        } else {
            z = c->y;
            if (z == 0) {
                r0 = 1;
            } else {
                g = z->g;
                u16 h2;
                if (g->w20 != 1 && ((h2 = g->h1c), 1) && z->v6() >= (f32)(h2 - 1)) {
                    r0 = 1;
                } else {
                    r0 = 0;
                }
            }
        }
        s32 f2 = 1;
        if (r0 == 0) {
            if (lbl_805A2200 != self->b->c->y->v9()) {
                f2 = 0;
            }
        }
        arg2->i6(2, f1 && f2);
        arg2->i6(5, f1);
    }
}
}
}

namespace jj_800B807C {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct G {
    u8 p0[0x1C];
    u16 h1c;
    u8 p1[2];
    s32 w20;
    s32 w24;
    u8 p2[4];
    u16 h2c;
    u8 p3[6];
    s32 w34;
};

class VT {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual f32 v6();
    virtual void v7();
    virtual void v8();
    virtual f32 v9();
    u8 pad[0x28];
    G* g;
};

class Iface {
public:
    virtual void i0();
    virtual void i1();
    virtual void i2();
    virtual void i3();
    virtual void i4();
    virtual void i5();
    virtual void i6(s32, s32);
};

struct C {
    u8 p0[0x10];
    VT* z;
    VT* d;
    VT* f;
};

struct B {
    u8 p0[0x14];
    C* c;
};

struct Obj {
    u32 p0;
    VT* a;
    B* b;
};

extern "C" {
extern f32 lbl_805A2200;

// TODO: clean up, give variables real names
void fn_800B807C(Obj* self, s32 arg1, Iface* arg2) {
    s32 r0;
    s32 flag;
    u16 h;
    C* c;
    VT* z;
    VT* d;
    VT* f;
    G* g;

    if (self->a != 0) {
        self->a->v2();
    }
    if (arg1 == 4) {
        c = self->b->c;
        if (c == 0) {
            r0 = 1;
        } else {
            z = c->z;
            if (z == 0) {
                r0 = 1;
            } else {
                g = z->g;
                if (g->w34 != 1 && ((h = g->h2c), 1) && z->v6() >= (f32)(h - 1)) {
                    r0 = 1;
                } else {
                    r0 = 0;
                }
            }
        }
        flag = 1;
        if (r0 == 0) {
            if (lbl_805A2200 != self->b->c->z->v9()) {
                flag = 0;
            }
        }
        if (flag != 0) {
            c = self->b->c;
            if (c == 0) {
                r0 = 1;
            } else {
                d = c->d;
                if (d == 0) {
                    r0 = 1;
                } else {
                    g = d->g;
                    if (g->w24 != 1 && ((h = g->h1c), 1) && d->v6() >= (f32)(h - 1)) {
                        r0 = 1;
                    } else {
                        r0 = 0;
                    }
                }
            }
            flag = 1;
            if (r0 == 0) {
                if (lbl_805A2200 != self->b->c->d->v9()) {
                    flag = 0;
                }
            }
        }
        if (flag != 0) {
            c = self->b->c;
            if (c == 0) {
                r0 = 1;
            } else {
                f = c->f;
                if (f == 0) {
                    r0 = 1;
                } else {
                    g = f->g;
                    if (g->w20 != 1 && ((h = g->h1c), 1) && f->v6() >= (f32)(h - 1)) {
                        r0 = 1;
                    } else {
                        r0 = 0;
                    }
                }
            }
            flag = 1;
            if (r0 == 0) {
                if (lbl_805A2200 != self->b->c->f->v9()) {
                    flag = 0;
                }
            }
        }
        arg2->i6(3, flag);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_800B08A8 {

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(void*, const char*, int, int, int, bool);
extern "C" s8 lbl_8059DF40;
extern "C" char lbl_80455D8C[];

struct TBase {
    char pad0[0x2C];
    u8 unk2C;
    char pad2D[0xF];
};

struct TTask : public TBase {
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
};

extern "C" TTask* create__16muObjProcessTaskFQ25Heaps8HeapType(int arg0) {
    TTask* task = (TTask*)__nw__FUlQ25Heaps8HeapType(0x48, arg0);
    if (task) {
        __ct__6gfTaskFPCcQ26gfTask8Categoryiib(task, (const char*)&lbl_8059DF40, 8, 0xF, 8, true);
        *(char**)((char*)task + 0x3C) = lbl_80455D8C;
        task->unk2C &= ~2;
    }
    task->v21();
    return task;
}
}

// TODO: clean up, give variables real names
namespace jj_800B20B0 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_EnableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_DisableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern int lbl_80455D18[8];
struct Owner;
struct Node { virtual ~Node(); Node* next; Owner* owner; };
struct Model {
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4(); virtual void v5();
    virtual void set(int, int);
    char pad[0xD0];
    Node* head;
};
struct Owner { char pad[0x10]; Model* model; char pad2[0x40]; int heap; Node* node; };
extern "C" void enableSkipMatrix__8MuObjectFb(Owner* self, int enabled) {
    Model* current;
    Model* model = self->model;
    if (enabled != 0) {
        if (self->node == 0) {
            Node* node = (Node*)__nw__FUlQ25Heaps8HeapType(12, self->heap);
            if (node != 0) {
                Node* next = model->head;
                *(void**)node = lbl_80455D18;
                node->next = next;
                node->owner = self;
            }
            model->head = node;
            ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(model, 4);
            if (self->node != node) {
                delete self->node;
                self->node = node;
            }
        }
        ScnObj_EnableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(model, 4);
    } else {
        ScnObj_DisableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(model, 4);
        current = self->model;
        current->set(2, 0);
        current->set(5, 0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800B21C4 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_EnableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(void*, int);
extern "C" void ScnObj_DisableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(void*, int);

extern int lbl_80455D18[8];

struct Owner;

struct Node {
    virtual ~Node();
    Node* next;
    Owner* owner;
};

struct Model {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void set(int, int);

    char pad[0xD0];
    Node* head;
};

struct Owner {
    char pad[0x10];
    Model* model;
    char pad2[0x40];
    int heap;
    Node* node;
};

extern "C" void enableSkipMaterial__8MuObjectFb(Owner* self, int enabled) {
    Model* model = self->model;
    if (enabled != 0) {
        if (self->node == 0) {
            Node* node = (Node*)__nw__FUlQ25Heaps8HeapType(12, self->heap);
            if (node != 0) {
                Node* next = model->head;
                *(void**)node = lbl_80455D18;
                node->next = next;
                node->owner = self;
            }
            model->head = node;
            ScnObj_EnableCallbackExecOp__FPQ34nw4r3g3d6ScnObjUl(model, 4);
            if (self->node != node) {
                delete self->node;
                self->node = node;
            }
        }
        ScnObj_EnableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(model, 2);
    } else {
        ScnObj_DisableCallbackTiming__FPQ34nw4r3g3d6ScnObjUl(model, 2);
        Model* current = self->model;
        current->set(3, 0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800B2318 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__8MuObjectFPCciiP8MuObjectPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(void*, int, int, int, int, int, int);
extern "C" void* copy__8MuObjectFiiQ25Heaps8HeapType(int a, int b, int c, int heap) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x60, heap);
    if (p != 0)
        p = __ct__8MuObjectFPCciiP8MuObjectPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(p, 0, b, c, a, 0, heap);
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_800B7AA4 {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5(float);
    virtual float slot6();
};
class Controller {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6(int, int);
};
struct Mid {
    char pad[8];
    Object_vptr_0* b;
    Object_vptr_0* a;
    Object_vptr_0* c;
    Object_vptr_0* d;
    Object_vptr_0* e;
};
struct Self {
    char pad[16];
    Controller* controller;
    Mid* mid;
};
extern "C" void setFrameFlg__8MuObjectFfUc(Self* self, unsigned int mask, float f) {
    if (mask == 31) {
        if (self->mid->a) {
            if (f != self->mid->a->slot6()) {
                Controller* c = self->controller;
                c->slot6(2, 0);
                c->slot6(5, 0);
            }
            self->mid->a->slot5(f);
        }
        if (self->mid->b) {
            if (f != self->mid->b->slot6()) {
                Controller* c = self->controller;
                c->slot6(2, 0);
                c->slot6(5, 0);
            }
            self->mid->b->slot5(f);
        }
        if (self->mid->c) {
            if (f != self->mid->c->slot6()) {
                self->controller->slot6(3, 0);
            }
            self->mid->c->slot5(f);
        }
        if (self->mid->d) {
            if (f != self->mid->d->slot6()) {
                self->controller->slot6(3, 0);
            }
            self->mid->d->slot5(f);
        }
        if (self->mid->e) {
            if (f != self->mid->e->slot6()) {
                self->controller->slot6(3, 0);
            }
            self->mid->e->slot5(f);
        }
    } else {
        if (mask & 1) {
            if (f != self->mid->a->slot6()) {
                Controller* c = self->controller;
                c->slot6(2, 0);
                c->slot6(5, 0);
            }
            self->mid->a->slot5(f);
        }
        if (mask & 2) {
            if (f != self->mid->b->slot6()) {
                Controller* c = self->controller;
                c->slot6(2, 0);
                c->slot6(5, 0);
            }
            self->mid->b->slot5(f);
        }
        if (mask & 4) {
            if (f != self->mid->c->slot6()) {
                self->controller->slot6(3, 0);
            }
            self->mid->c->slot5(f);
        }
        if (mask & 8) {
            if (f != self->mid->d->slot6()) {
                self->controller->slot6(3, 0);
            }
            self->mid->d->slot5(f);
        }
        if (mask & 16) {
            if (f != self->mid->e->slot6()) {
                self->controller->slot6(3, 0);
            }
            self->mid->e->slot5(f);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800B7F38 {
typedef unsigned int u32;

class AnimationObject {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void setUpdateRate(float rate);
};

struct gfModelAnimation {
    char pad[8];
    AnimationObject* visibility;
    AnimationObject* character;
    AnimationObject* texturePattern;
    AnimationObject* textureSrt;
    AnimationObject* materialColor;
};

struct ModelOwner {
    char pad[0x14];
    gfModelAnimation* animation;
};

extern "C" void setUpdateRate__16gfModelAnimationFf(gfModelAnimation*, float);

extern "C" void setUpdateRateFlg__8MuObjectFfUc(ModelOwner* self, u32 flags, float rate) {
    if (flags == 0x1F) {
        setUpdateRate__16gfModelAnimationFf(self->animation, rate);
        return;
    }

    if (flags & 1)
        self->animation->character->setUpdateRate(rate);
    if (flags & 2)
        self->animation->visibility->setUpdateRate(rate);
    if (flags & 4)
        self->animation->texturePattern->setUpdateRate(rate);
    if (flags & 8)
        self->animation->textureSrt->setUpdateRate(rate);
    if (flags & 0x10)
        self->animation->materialColor->setUpdateRate(rate);
}
}

