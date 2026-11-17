#define MT_VEC3F_ADD_NATIVE_ORDER // MATCH-ONLY: the vector adds reuse the right operand registers for the sum.
#include <gf/gf_camera.h>
#include <gr/ground.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_3d_generator.h>
#include <sr/sr_common.h>
#include <st/stage.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: one sound record of a sound trigger (0x50 bytes): the sound id, whether it follows the camera, its offset
// and the name of the node the sound is attached to.
struct stTriggerSoundData {
    SndID m_sndId;
    u8 m_unk4;
    u8 m_unk5;
    u8 m_followCamera;
    char _7[9];
    Vec3f m_offset;
    char m_nodeName[0x34];
};
static_assert(sizeof(stTriggerSoundData) == 0x50, "Class is wrong size!");

// The play state of one sound of the trigger.
struct stSoundInfo {
    u8 m_isPlaying;
    char _1[3];
    SndID m_sndId;
    int m_handle;
    int m_state;
    float m_timer;
    Vec3f m_pos;

    stSoundInfo() { }
};
static_assert(sizeof(stSoundInfo) == 0x20, "Class is wrong size!");

// HYPOTHESIS: unk8 is initialized by a non-virtual base, before the derived vtable store.
class stActTriggerSoundKind {
public:
    u32 unk8;

    stActTriggerSoundKind() : unk8(0xb) { }
};

// A trigger that starts its sounds one after another and keeps their positions up to date.
class stActTriggerSound : public stTriggerCB, public stActTriggerSoundKind {
public:
    u32 m_count;
    stSoundInfo* m_infos;
    stTriggerSoundData* m_data;
    int m_param;
    int m_playCount;
    Ground* m_ground;
    bool m_active;
    snd3DGenerator* m_snd;

    stActTriggerSound(stTriggerSoundData* data, int param);
    virtual ~stActTriggerSound();
    virtual void userProc();
    virtual void updateBegin();
    virtual void updateFixPosition() { }
    virtual void updateEnd();
    void updatePos(int index);
    int findFreeInfo() {
        for (u32 i = 0; i < m_count; i++) {
            if (m_infos[i].m_isPlaying == 0) {
                return i;
            }
        }
        return -1;
    }
};

// The report of the original lost its text in the release build.
static void srReportLessMastar(const char* format, ...) { }

stActTriggerSound::stActTriggerSound(stTriggerSoundData* data, int param)
    : m_count(1), m_infos(NULL), m_data(data), m_param(param), m_playCount(0), m_ground(NULL), m_active(true), m_snd(NULL) {
    m_infos = new (Heaps::StageInstance) stSoundInfo[m_count];
    memset(m_infos, 0, m_count * sizeof(stSoundInfo));
    m_snd = new (Heaps::StageInstance) snd3DGenerator;
}

stActTriggerSound::~stActTriggerSound() {
    if (m_snd != NULL) {
        for (u32 i = 0; i < m_count; i++) {
            if (m_infos[i].m_isPlaying != 0) {
                m_snd->stopSE(m_infos[i].m_handle, 0);
            }
        }
        delete m_snd;
    }
    m_snd = NULL;
    if (m_infos != NULL) {
        delete[] m_infos;
    }
    m_infos = NULL;
}

void stActTriggerSound::userProc() {
    for (u32 i = 0; i < m_count; i++) {
        if (m_active != false) {
            int index = findFreeInfo();
            if (index >= 0) {
                m_active = false;
                stSoundInfo* info = &m_infos[index];
                int handle = m_snd->prepareSE(m_data[0].m_sndId, -1);
                if (handle == -1) {
                    srReportLessMastar("ＳＥに無効なＩＤが振られています。ID = %d\n", m_data[0].m_sndId);
                } else {
                    info->m_isPlaying = 1;
                    info->m_sndId = m_data[0].m_sndId;
                    info->m_state = 1;
                    info->m_timer = 0.0f;
                    info->m_handle = handle;
                    m_playCount++;
                }
            }
        }
    }
}

void stActTriggerSound::updatePos(int index) {
    stSoundInfo* info = &m_infos[index];
    Vec3f* offset = &m_data[index].m_offset;
    info->m_pos.m_x = offset->m_x;
    info->m_pos.m_y = offset->m_y;
    info->m_pos.m_z = offset->m_z;
    if (m_data[index].m_followCamera != 0) {
        gfCameraManager* manager = gfCameraManager::getManager();
        if (manager != NULL) {
            Vec3f camera;
            camera.m_x = manager->m_cameras[0].m_centerPos.m_x;
            camera.m_y = manager->m_cameras[0].m_centerPos.m_y;
            camera.m_z = manager->m_cameras[0].m_centerPos.m_z;
            info->m_pos = camera + m_data[index].m_offset;
        }
    } else if (m_ground != NULL) {
        nw4r::g3d::ResMdl resMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(m_ground->m_sceneModels[0])->m_resMdl;
        nw4r::g3d::ResNode node = resMdl.GetResNode(m_data[index].m_nodeName);
        if (node.IsValid()) {
            Vec3f translation;
            translation.m_x = node.ptr()->m_translation.m_x;
            translation.m_y = node.ptr()->m_translation.m_y;
            translation.m_z = node.ptr()->m_translation.m_z;
            info->m_pos = translation + m_data[index].m_offset;
        }
    }
    if (m_snd != NULL) {
        m_snd->setPos(&info->m_pos);
    }
}

void stActTriggerSound::updateBegin() {
    stSoundInfo* info;
    u32 i;
    for (i = 0; i < m_count; i++) {
        info = &m_infos[i];
        if (info->m_isPlaying != 0) {
            switch (info->m_state) {
            case 1:
                info->m_timer = 0.0f;
                info->m_state = 2;
            case 2:
                if (m_snd->isPreparedSE(info->m_handle)) {
                    updatePos(i);
                    info->m_state = 3;
                }
                break;
            case 3:
                m_snd->startSE(info->m_handle, 0);
                info->m_state = 4;
                break;
            case 4:
            case 5:
                if (!m_snd->isPlay(info->m_handle)) {
                    info->m_state = 7;
                } else if (m_data[i].m_unk5 != 0) {
                    updatePos(i);
                }
                break;
            }
        }
    }
}

void stActTriggerSound::updateEnd() {
    for (u32 i = 0; i < m_count; i++) {
        stSoundInfo* info = &m_infos[i];
        if (info->m_isPlaying != 0) {
            switch (info->m_state) {
            case 7:
                info->m_state = 8;
            case 8:
                m_infos[i].m_isPlaying = 0;
                m_playCount--;
                break;
            }
        }
    }
}
