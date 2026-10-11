#include <gf/gf_3d_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <revolution/GX.h>
#include <types.h>

#include <mu/mu_adv_telop.h>

// The channel (color / alpha) settings of a material: main.dol has them, the shared g3d header does not.
namespace nw4r {
namespace g3d {
struct ResChanData;
class ResMatChan : public ResCommon<ResChanData> {
public:
    inline ResMatChan() : ResCommon() { }
    bool GXGetChanMatColor(GXChannelID id, GXColor* color) const;
    void GXSetChanMatColor(GXChannelID id, GXColor color);
};
}  // namespace g3d
}  // namespace nw4r
// ScnMdl::CopiedMatAccess::GetResMatChanEx()
extern "C" nw4r::g3d::ResMatChan GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(nw4r::g3d::ScnMdl::CopiedMatAccess* access);

// Writes the alpha of the telop into the color channel of the material of the model.
inline void muAdvTelopTask::setAlpha() {
    nw4r::g3d::ScnMdl* scnMdl = m_object->m_scnMdl;
    nw4r::g3d::ResMdl resMdl = scnMdl->m_resMdl;
    nw4r::g3d::ResMat mat = resMdl.GetResMat("lambert42");
    nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
    nw4r::g3d::ResMatChan chan = GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(&access);
    GXColor color;
    chan.GXGetChanMatColor(GX_ALPHA0, &color);
    color.a = m_alphaByte;
    chan.GXSetChanMatColor(GX_ALPHA0, color);
}

// nw4r::g3d::ResTex: the parameters of the texture object of a texture, and the number of textures of a file
extern "C" void fn_80192A44(const nw4r::g3d::ResTex* tex, void** data, u16* width, u16* height, int* format, float* minLod,
                            float* maxLod, char* mipmap);
extern "C" u32 GetResTexNumEntries__Q34nw4r3g3d7ResFileCFv(const nw4r::g3d::ResFile* file);
// MuObject::changeMaterialTex(material name, texture object)
extern "C" void changeMaterialTex__8MuObjectFPCcP9_GXTexObj(MuObject* object, const char* material, GXTexObj* texObj);

muAdvTelopTask* muAdvTelopTask::create(const muAdvTelopParam* param) {
    muAdvTelopTask* task = new (Heaps::MenuInstance) muAdvTelopTask();
    task->m_hold = param->m_hold;
    task->m_fadeIn = param->m_fadeIn;
    task->m_fadeOut = param->m_fadeOut;
    task->m_texIndex = param->m_texIndex;
    task->m_archive = param->m_archive;
    if (task->m_fadeIn == 0) {
        task->m_fadeIn = 1;
    }
    if (task->m_fadeOut == 0) {
        task->m_fadeOut = 1;
    }
    return task;
}

muAdvTelopTask::~muAdvTelopTask() {
    if (m_object != NULL) {
        delete m_object;
        m_object = NULL;
    }
}

// The steps: create the model, fade in, hold, fade out, remove the model from the scene, delete it.
void muAdvTelopTask::processDefault() {
    switch (m_step) {
    case Step_Create:
        createData(m_archive);
        m_unk71 = 0xFF;
        m_alpha = 0.0f;
        m_step = Step_FadeIn;
        m_alphaStep = 255.0f / static_cast<float>(m_fadeIn);
        // fall through
    case Step_FadeIn:
        m_alpha = m_alpha + m_alphaStep;
        if (m_alpha >= 255.0f) {
            m_alpha = 255.0f;
            m_step = Step_Hold;
        }
        m_alphaByte = static_cast<int>(m_alpha);
        setAlpha();
        break;
    case Step_Hold:
        if (static_cast<u32>(m_frame) + 1 < m_hold) {
            return;
        }
        m_step = Step_FadeOut;
        m_alphaStep = -255.0f / static_cast<float>(m_fadeOut);
        return;
    case Step_FadeOut:
        m_alpha = m_alpha + m_alphaStep;
        if (m_alpha <= 0.0f) {
            m_alpha = 0.0f;
            m_step = Step_Remove;
        }
        m_alphaByte = static_cast<int>(m_alpha);
        setAlpha();
        break;
    case Step_Remove:
        m_alphaByte = 0;
        setAlpha();
        if (m_object != NULL) {
            g_gfSceneRoot->remove(m_object->m_scnMdl);
        }
        m_step = Step_Delete;
        break;
    case Step_Delete:
        if (m_object != NULL) {
            delete m_object;
            m_object = NULL;
        }
        m_step = Step_Done;
        break;
    }
}

bool muAdvTelopTask::isAlive() const {
    return m_step < Step_Done;
}

// Starts the fade out (if the telop did not start it yet) and lets the task run even when the game is paused.
void muAdvTelopTask::kill() {
    if (m_step < Step_Remove) {
        m_step = Step_Remove;
    }
    setPaused(false);
}

// Loads the model and the picture of the caption, puts the picture into the material of the model and adds the model to
// the scene.
void muAdvTelopTask::createData(gfArchive* archive) {
    nw4r::g3d::ResFile file;
    file = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 1, 0xFFFE));
    nw4r::g3d::ResFile::Init(&file);
    if (GetResTexNumEntries__Q34nw4r3g3d7ResFileCFv(&file) <= static_cast<u32>(m_texIndex)) {
        m_texIndex = 0;
    }
    m_tex = file.GetResTex(m_texIndex);
    file = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&file);
    m_object = MuObject::create(&file, "Telop", 1, NULL, Heaps::MenuInstance);
    // the model is stretched to the shape of the screen (4:3 or 16:9)
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        Vec3f scale;
        scale.m_x = 0.677116f;
        scale.m_y = 0.6753247f;
        scale.m_z = 1.0f;
        m_object->setObjScale(&scale);
    } else {
        Vec3f scale;
        scale.m_x = 0.9028213f;
        scale.m_y = 0.9004329f;
        scale.m_z = 1.0f;
        m_object->setObjScale(&scale);
    }
    float maxLod;
    float minLod;
    int format;
    void* data;
    u16 height;
    u16 width;
    char mipmap;
    fn_80192A44(&m_tex, &data, &width, &height, &format, &minLod, &maxLod, &mipmap);
    GXTexObj texObj;
    GXInitTexObj(&texObj, data, width, height, static_cast<GXTexFmt>(format), GX_CLAMP, GX_CLAMP, mipmap);
    changeMaterialTex__8MuObjectFPCcP9_GXTexObj(m_object, "lambert42", &texObj);
    m_alphaByte = 0;
    setAlpha();
    g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(8), m_object->m_scnMdl);
}
