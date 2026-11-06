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
