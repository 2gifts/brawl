#define SO_SOUND_3D_GENERATOR_ACCESSER_FULL
#include <mt/mt_vector.h>
#include <so/sound/so_sound_util.h>
#include <types.h>

int soSoundUtil::playSE(soSound3dGeneratorAccesser* accesser, int index, SndID sndID, int groupId, int soundGroup) {
    snd3DGenerator* generator = accesser->getInstance(index)->m_soundGenerator;
    if (generator != NULL) {
        return generator->playSE(sndID, groupId, 0, soundGroup);
    }
    return -1;
}

void soSoundUtil::stopSE(soSound3dGeneratorAccesser* accesser, int index, int unk, int groupId) {
    snd3DGenerator* generator = accesser->getInstance(index)->m_soundGenerator;
    if (generator != NULL) {
        generator->stopGroupId(groupId, static_cast<SndID>(unk), 0);
    }
}

void soSoundUtil::setPos(soSound3dGeneratorAccesser* accesser, Vec3f* pos, int index) {
    soSound3dGeneratorAccesser::GeneratorInstance* instance = accesser->getInstance(index);
    if (instance->m_soundGenerator != NULL) {
        Vec3f p = *pos;
        instance->m_soundGenerator->setPos(&p);
    }
}

// TODO: clean up, give variables real names
namespace jj_581F8 {
struct Vec3f {
    float x, y, z;
};

struct snd3DGenerator;

struct SoundHandle {
    int id;
    snd3DGenerator* generator;
};

struct SoundArray {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual SoundHandle* at(int);
};

extern "C" void setPos__14snd3DGeneratorFP5Vec3f(snd3DGenerator*, Vec3f*);

extern "C" void fn_27_581F8(SoundArray* self, const Vec3f* position, int index) {
    SoundHandle* handle = self->at(index);
    if (handle->generator) {
        Vec3f copy = *position;
        setPos__14snd3DGeneratorFP5Vec3f(handle->generator, &copy);
    }
}
}

