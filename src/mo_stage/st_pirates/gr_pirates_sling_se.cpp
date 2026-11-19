#include <gr/gr_pirates_sling.h>
#include <snd/snd_id.h>

// MATCH-ONLY: keep the stage's original constant pool.
extern const float g_piratesSlingMotionConstants[]; // Begins with 0.0f, 60.0f, 5.0f

// The sounds of the sling: they follow the matrix of the sling and start when the motion reaches a frame.
void grPiratesSling::updateSE(float deltaFrame) {
    const float* constants = g_piratesSlingMotionConstants;
    switch (m_state) {
    case State_Reset:
    case State_Wait:
        break;
    default: {
        if (m_mtxWork == NULL) {
            return;
        }
        // MATCH-ONLY: coordinate temporaries preserve the order of the loads.
        float z = m_mtxWork->m[2][3];
        float y = m_mtxWork->m[1][3];
        float x = m_mtxWork->m[0][3];
        Vec3f pos(x, y, z);
        m_sndGenerator.setPos(&pos);
        switch (m_motionId) {
        case 0:
            switch (unk188) {
            case 0:
                m_sndGenerator.playSE(snd_se_stage_Pirates_16, 0, 0, -1);
                unk188++;
                break;
            case 1:
                if (getMotionFrame(0) < constants[3]) {
                    break;
                }
                m_soundHandle = m_sndGenerator.playSE(snd_se_stage_Pirates_17, 0, 0, -1);
                unk188++;
                break;
            case 2:
                if (getMotionFrame(0) < constants[4]) {
                    break;
                }
                if (m_soundHandle != -1) {
                    m_sndGenerator.stopSE(m_soundHandle, 0);
                }
                m_soundHandle = -1;
                unk188++;
                break;
            case 3:
                if (getMotionFrame(0) < constants[5]) {
                    break;
                }
                m_soundHandle = m_sndGenerator.playSE(snd_se_stage_Pirates_18, 0, 0, -1);
                unk188++;
                break;
            }
            break;
        case 1:
            switch (unk188) {
            case 0:
                m_sndGenerator.playSE(snd_se_stage_Pirates_20, 0, 0, -1);
                unk188++;
                break;
            case 1:
                if (getMotionFrame(0) < constants[6]) {
                    break;
                }
                m_soundHandle = m_sndGenerator.playSE(snd_se_stage_Pirates_17, 0, 0, -1);
                unk188++;
                break;
            case 2:
                if (getMotionFrame(0) < constants[7]) {
                    break;
                }
                if (m_soundHandle != -1) {
                    m_sndGenerator.stopSE(m_soundHandle, 0);
                }
                m_soundHandle = -1;
                m_sndGenerator.playSE(snd_se_stage_Pirates_19, 0, 0, -1);
                unk188++;
                break;
            case 3:
                if (getMotionFrame(0) < constants[8]) {
                    break;
                }
                m_soundHandle = m_sndGenerator.playSE(snd_se_stage_Pirates_16, 0, 0, -1);
                unk188++;
                break;
            }
            break;
        }
    }
    }
}
