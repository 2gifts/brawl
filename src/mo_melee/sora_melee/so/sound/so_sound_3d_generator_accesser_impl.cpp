#define SO_SOUND_3D_GENERATOR_ACCESSER_FULL
#include <mt/mt_vector.h>
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
#include <so/sound/so_sound_manager.h>
#include <types.h>

void soSound3dGeneratorAccesser::GeneratorInstance::initialize() {
    m_id = -1;
    m_soundGenerator = NULL;
}

void soSound3dGeneratorAccesser::allocateInstance(GeneratorInstance* inst, Vec3f* pos) {
    if (inst->m_id == -1) {
        int id = lbl_27_bss_D0.allocateGenerator();
        if (id != -1) {
            inst->m_soundGenerator = lbl_27_bss_D0.getGenerator(id);
            inst->m_id = id;
            Vec3f p = *pos;
            inst->m_soundGenerator->setPos(&p);
        }
    }
}

void soSound3dGeneratorAccesser::freeInstance(GeneratorInstance* inst) {
    if (inst->m_id != -1) {
        lbl_27_bss_D0.freeGenerator(inst->m_id);
        inst->initialize();
    }
}

soSound3dGeneratorAccesserImpl::soSound3dGeneratorAccesserImpl() {
    m_generatorInstance.initialize();
}

void soSound3dGeneratorAccesserImpl::activate(Vec3f* pos) {
    allocateInstance(&m_generatorInstance, pos);
}

void soSound3dGeneratorAccesserImpl::deactivate() {
    freeInstance(&m_generatorInstance);
}

soSound3dGeneratorAccesser::GeneratorInstance* soSound3dGeneratorAccesserImpl::getInstance(int) {
    return &m_generatorInstance;
}

soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

// TODO: clean up, give variables real names
namespace jj_57FE8 {
struct Vec3f {
    float x, y, z;
};

struct snd3DGenerator;

struct SoundHandle {
    unsigned int unknown;
    int index;
    snd3DGenerator* generator;
};

extern "C" {
extern unsigned char lbl_27_bss_D0[0x18];

int allocateGenerator__14soSoundManagerFv(void*);
snd3DGenerator* getGenerator__14soSoundManagerFi(void*, int);
void setPos__14snd3DGeneratorFP5Vec3f(snd3DGenerator*, Vec3f*);

void fn_27_57FE8(SoundHandle* self, const Vec3f* position) {
    if (self->index == -1) {
        int index = allocateGenerator__14soSoundManagerFv(lbl_27_bss_D0);
        if (index != -1) {
            self->generator = getGenerator__14soSoundManagerFi(lbl_27_bss_D0, index);
            self->index = index;
            Vec3f local = *position;
            setPos__14snd3DGeneratorFP5Vec3f(self->generator, &local);
        }
    }
}
}
}

