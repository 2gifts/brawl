#include <gf/gf_camera.h>
#include <snd/snd_3d_generator.h>
#include <st/stage.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the data record of a sound area trigger (the sound id, the rectangle the sound is heard in and the timings in
// frames); the fields are named by how the trigger callback uses them.
struct stTriggerSoundAreaData {
    SndID m_sndId;
    float m_posX;
    float m_posY;
    float m_halfWidth;
    float m_halfHeight;
    u16 m_beginDelay;
    u16 m_playDuration;
    u16 m_startOption;
    u16 m_stopOption;
    char _1c[4];
    u16 m_repeatDelay;
    u8 m_unk22;
};

// HYPOTHESIS: unk8 is initialized by a non-virtual base, before the derived vtable store.
class stActTriggerSoundKind {
public:
    u32 unk8;

    stActTriggerSoundKind() : unk8(0xb) { }
};

// A sound that plays while the listener (the camera) is inside the rectangle of the trigger area.
class stActTriggerAreaSound : public stTriggerCB, public stActTriggerSoundKind {
public:
    stTriggerSoundAreaData* m_data;
    snd3DGenerator m_snd;
    u8 m_state;
    float m_timer;
    int m_handle;
    bool m_inArea;

    stActTriggerAreaSound(stTriggerSoundAreaData* data);
    virtual ~stActTriggerAreaSound();
    virtual void userProc();
    virtual void updateBegin() { }
    virtual void updateFixPosition() { }
    virtual void updateEnd();
    void getSoundActorPosition(Vec3f* out);
    void playSound();
    void stopSound();
};

stActTriggerAreaSound::stActTriggerAreaSound(stTriggerSoundAreaData* data) {
    m_state = 0;
    m_timer = 0.0f;
    m_handle = -1;
    m_inArea = false;
    m_data = data;
}

stActTriggerAreaSound::~stActTriggerAreaSound() { }

void stActTriggerAreaSound::playSound() {
    u8 nextState = 1;
    m_handle = m_snd.prepareSE(m_data->m_sndId, -1);
    if (m_handle == -1) {
        OSReport("ＳＥに無効なＩＤが振られています。ID = %d\n", m_data->m_sndId);
        nextState = 5;
    } else {
        m_snd.startSE(m_handle, m_data->m_startOption);
    }
    m_state = nextState;
    m_timer = 0.0f;
}

void stActTriggerAreaSound::stopSound() {
    if (m_handle != -1) {
        m_snd.stopSE(m_handle, m_data->m_stopOption);
        m_handle = -1;
    }
}

void stActTriggerAreaSound::userProc() {
    Vec3f pos;
    getSoundActorPosition(&pos);
    m_snd.setPos(&pos);
    if (m_inArea == false && m_data->m_unk22 == 0) {
        return;
    }
    switch (m_state) {
    case 0:
        if (m_timer >= (float)m_data->m_beginDelay) {
            playSound();
        }
        break;
    case 3:
        if (m_data->m_repeatDelay != 0) {
            if (m_timer >= (float)m_data->m_repeatDelay) {
                playSound();
            }
        } else if (m_data->m_unk22 == 1) {
            m_state = 5;
            m_timer = 0.0f;
        } else {
            playSound();
        }
        break;
    case 5:
        m_handle = -1;
        break;
    }
}

void stActTriggerAreaSound::updateEnd() {
    int handle = m_handle;
    if (handle != -1 && m_state == 1) {
        if (m_inArea == false && m_data->m_unk22 == 0) {
            stopSound();
            m_state = 0;
            m_timer = 0.0f;
            return;
        }
        if (m_data->m_playDuration != 0 && m_timer >= (float)m_data->m_playDuration) {
            stopSound();
            m_state = 3;
            m_timer = 0.0f;
            return;
        }
        if (m_timer > 0.0f && !m_snd.isPlay(handle)) {
            stopSound();
            m_state = 3;
            m_timer = 0.0f;
            return;
        }
    }
    float delta = g_stGameFrame.m_frameDelta;
    m_timer = m_timer + delta;
}

// The sound follows the listener, kept inside the rectangle of the area; m_inArea tells whether the listener is inside.
void stActTriggerAreaSound::getSoundActorPosition(Vec3f* out) {
    Vec3f& target = gfCameraManager::getManager()->m_cameras[0].m_targetPos;
    Vec3f camera;
    camera.m_x = target.m_x;
    camera.m_y = target.m_y;
    camera.m_z = target.m_z;
    float dx = m_data->m_posX - camera.m_x;
    float halfWidth = m_data->m_halfWidth / 2.0f;
    if (dx > halfWidth) {
        out->m_x = m_data->m_posX - halfWidth;
    } else if (dx < -halfWidth) {
        out->m_x = m_data->m_posX + halfWidth;
    } else {
        out->m_x = camera.m_x;
    }
    float dy = m_data->m_posY - camera.m_y;
    float halfHeight = m_data->m_halfHeight / 2.0f;
    if (dy > halfHeight) {
        out->m_y = m_data->m_posY - halfHeight;
    } else if (dy < -halfHeight) {
        out->m_y = m_data->m_posY + halfHeight;
    } else {
        out->m_y = camera.m_y;
    }
    if (out->m_x == camera.m_x && out->m_y == camera.m_y) {
        m_inArea = true;
    } else {
        m_inArea = false;
    }
    out->m_z = 0.0f;
}
