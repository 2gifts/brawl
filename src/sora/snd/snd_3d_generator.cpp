#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" nw4r::snd::Sound3DActor* __ct__Q34nw4r3snd12Sound3DActorFRQ34nw4r3snd18SoundArchivePlayerRQ34nw4r3snd14Sound3DManager(nw4r::snd::Sound3DActor* actor, nw4r::snd::SoundArchivePlayer* player, nw4r::snd::Sound3DManager* manager);
extern "C" void SetPosition__Q34nw4r3snd12Sound3DActorFRCQ34nw4r4math4VEC3(nw4r::snd::Sound3DActor* actor, Vec3f* pos);
extern f32 lbl_805A1B08;

static inline int isFinite(float f) {
    int res = 1;
    u32 i;
    __memcpy(&i, &f, 4);
    if ((i >> 23 & 0xFF) == 0xFF)
        res = 0;
    return res;
}

snd3DGenerator::~snd3DGenerator() {
    if (m_sndActor != NULL) {
        delete m_sndActor;
        m_sndActor = NULL;
    }
}

void snd3DGenerator::setPos(Vec3f* pos) {
    if (m_sndActor != NULL) {
        if (!(isFinite(pos->m_x) && isFinite(pos->m_y) && isFinite(pos->m_z))) {
            f32 zero = lbl_805A1B08;

            pos->m_x = zero;
            pos->m_y = zero;
            pos->m_z = zero;
        }
        SetPosition__Q34nw4r3snd12Sound3DActorFRCQ34nw4r4math4VEC3(m_sndActor, pos);
    }
}

snd3DGenerator::snd3DGenerator() {
    Vec3f pos;
    f32 zero;
    sndSystem* sys;
    nw4r::snd::Sound3DActor* actor;

    sys = g_sndSystem;
    actor = static_cast<nw4r::snd::Sound3DActor*>(operator new(0xC4, Heaps::Sound));
    if (actor != NULL) {
        actor = __ct__Q34nw4r3snd12Sound3DActorFRQ34nw4r3snd18SoundArchivePlayerRQ34nw4r3snd14Sound3DManager(actor, &sys->m_archivePlayer, &sys->m_3DManager);
    }
    m_sndActor = actor;
    zero = lbl_805A1B08;
    pos.m_x = zero;
    pos.m_y = zero;
    pos.m_z = zero;
    SetPosition__Q34nw4r3snd12Sound3DActorFRCQ34nw4r4math4VEC3(m_sndActor, &pos);
}

s32 snd3DGenerator::playSE(SndID sndID, int tag, int fadeIn, int group) {
    if (m_sndActor != NULL) {
        return g_sndSystem->play3DSE(sndID, m_sndActor, tag, fadeIn, 0, group);
    }
    return g_sndSystem->playSE(sndID, tag, fadeIn, 0, group);
}

s32 snd3DGenerator::prepareSE(SndID sndID, s32 tag) {
    sndSystem* sys = g_sndSystem;
    if (m_sndActor != NULL) {
        return sys->prepare3DSE(sndID, m_sndActor, tag, -1);
    }
    return sys->prepareSE(sndID, tag, -1);
}

void snd3DGenerator::stopGroupId(int tag, SndID sndID, int fade) {
    g_sndSystem->stopGroupId(tag, sndID, fade);
}

// TODO: clean up, give variables real names
namespace jj_80079780 {
extern "C" {

class sndSystem;
extern sndSystem* g_sndSystem;
void isPreparedSE__9sndSystemFl(sndSystem*);

void isPreparedSE__14snd3DGeneratorFl(void) {
    isPreparedSE__9sndSystemFl(g_sndSystem);
}

}
}

void snd3DGenerator::startSE(s32 handleId, s32 fadeIn) {
    g_sndSystem->startSE(handleId, fadeIn, 0);
}

void snd3DGenerator::stopSE(s32 handleId, s32 fade) {
    g_sndSystem->stopSE(handleId, fade);
}

// TODO: clean up, give variables real names
namespace jj_800797A4 {

extern "C" void isPlay__9sndSystemFl(sndSystem* p);

extern "C" void isPlay__14snd3DGeneratorFl(void) {
    isPlay__9sndSystemFl(g_sndSystem);
}
}

