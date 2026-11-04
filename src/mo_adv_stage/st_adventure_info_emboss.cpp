#include <em/enemy.h>
#include <gm/gm_global.h>
#include <sora_adv_stage/st_adventure_info_emboss.h>

extern "C" void fn_40_15B48(stAdventureInfoContact* contact);

void stAdventureInfoEmBoss::setInfoParam(int createId) {
    Enemy* enemy = getEnemy(createId);
    if (enemy != NULL && enemy->getEnemyKind() == 0x2E) {
        stAdventureInfoIconParam param0 = getIconParamForEnemy(createId, m_packnWnLeft, 0, &m_params[0]);
        m_params[0].m_disp.m_index = param0.m_disp.m_index;
        m_params[0].m_disp.m_hp = param0.m_disp.m_hp;
        m_params[0].m_disp.m_maxHp = param0.m_disp.m_maxHp;
        m_params[0].m_disp.m_nameMsgId = param0.m_disp.m_nameMsgId;
        m_params[0].m_disp.m_faceTex = param0.m_disp.m_faceTex;
        m_params[0].m_isValid = param0.m_isValid;
        m_params[0].unk15 = param0.unk15;
        m_params[0].m_faceTexData = param0.m_faceTexData;
        stAdventureInfoIconParam param1 = getIconParamForEnemy(createId, m_packnWnRight, 1, &m_params[1]);
        m_params[1].m_disp.m_index = param1.m_disp.m_index;
        m_params[1].m_disp.m_hp = param1.m_disp.m_hp;
        m_params[1].m_disp.m_maxHp = param1.m_disp.m_maxHp;
        m_params[1].m_disp.m_nameMsgId = param1.m_disp.m_nameMsgId;
        m_params[1].m_disp.m_faceTex = param1.m_disp.m_faceTex;
        m_params[1].m_isValid = param1.m_isValid;
        m_params[1].unk15 = param1.unk15;
        m_params[1].m_faceTexData = param1.m_faceTexData;
    } else if (m_isPair) {
        if (enemy != NULL && enemy->getEnemyKind() == 0x3B) {
            stAdventureInfoIconParam param0 = getIconParamForEnemy(createId, enemy->getTaskId(), 0, &m_params[0]);
            m_params[0].m_disp.m_index = param0.m_disp.m_index;
            m_params[0].m_disp.m_hp = param0.m_disp.m_hp;
            m_params[0].m_disp.m_maxHp = param0.m_disp.m_maxHp;
            m_params[0].m_disp.m_nameMsgId = param0.m_disp.m_nameMsgId;
            m_params[0].m_disp.m_faceTex = param0.m_disp.m_faceTex;
            m_params[0].m_isValid = param0.m_isValid;
            m_params[0].unk15 = param0.unk15;
            m_params[0].m_faceTexData = param0.m_faceTexData;
        }
        Enemy* subEnemy = getEnemy(m_subCreateId);
        if (subEnemy != NULL && subEnemy->getEnemyKind() == 0x3C) {
            stAdventureInfoIconParam param1 = getIconParamForEnemy(m_subCreateId, subEnemy->getTaskId(), 1, &m_params[1]);
            m_params[1].m_disp.m_index = param1.m_disp.m_index;
            m_params[1].m_disp.m_hp = param1.m_disp.m_hp;
            m_params[1].m_disp.m_maxHp = param1.m_disp.m_maxHp;
            m_params[1].m_disp.m_nameMsgId = param1.m_disp.m_nameMsgId;
            m_params[1].m_disp.m_faceTex = param1.m_disp.m_faceTex;
            m_params[1].m_isValid = param1.m_isValid;
            m_params[1].unk15 = param1.unk15;
            m_params[1].m_faceTexData = param1.m_faceTexData;
        }
    } else if (enemy != NULL) {
        stAdventureInfoIconParam param0 = getIconParamForEnemy(createId, enemy->getTaskId(), 0, &m_params[0]);
        m_params[0].m_disp.m_index = param0.m_disp.m_index;
        m_params[0].m_disp.m_hp = param0.m_disp.m_hp;
        m_params[0].m_disp.m_maxHp = param0.m_disp.m_maxHp;
        m_params[0].m_disp.m_nameMsgId = param0.m_disp.m_nameMsgId;
        m_params[0].m_disp.m_faceTex = param0.m_disp.m_faceTex;
        m_params[0].m_isValid = param0.m_isValid;
        m_params[0].unk15 = param0.unk15;
        m_params[0].m_faceTexData = param0.m_faceTexData;
    }
}

bool stAdventureInfoEmBoss::isBoss(int createId) {
    int kinds[10] = {0x2E, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x3A, 0x3B, 0x3C};
    Enemy* enemy = getEnemy(createId);
    for (int i = 0; i < 10; i++) {
        if (enemy != NULL && kinds[i] == enemy->getEnemyKind()) {
            return true;
        }
    }
    return false;
}

void stAdventureInfoEmBoss::setBossPacknWnLeft(u32 createId) {
    m_packnWnLeft = createId;
}

void stAdventureInfoEmBoss::setBossPacknWnRight(u32 createId) {
    m_packnWnRight = createId;
}

void stAdventureInfoEmBoss::remove() {
    m_isActive = false;
    fn_40_15B48(this);
}

void stAdventureInfoEmBoss::init(int createId) {
    if (isBoss(createId)) {
        m_isPair = g_GameGlobal->m_advData->m_flags.m_unk1;
        if (m_isPair) {
            Enemy* enemy = getEnemy(createId);
            bool isSub;
            if (enemy != NULL && enemy->getEnemyKind() == 0x3C) {
                isSub = true;
            } else {
                isSub = false;
            }
            if (isSub) {
                m_subCreateId = createId;
                return;
            }
        }
        m_createId = createId;
        m_isActive = true;
    }
}

void stAdventureInfoEmBoss::notifyDead(u32 createId) {
    Enemy* enemy = getEnemy(createId);
    if (enemy != NULL && enemy->getEnemyKind() == 0x2E) {
        m_isDead[0] = true;
        m_isDead[1] = true;
    } else if (m_isPair && enemy != NULL && enemy->getEnemyKind() == 0x3B) {
        m_isDead[0] = true;
    } else if (m_isPair && enemy != NULL && enemy->getEnemyKind() == 0x3C) {
        m_isDead[1] = true;
    } else if (isBoss(createId)) {
        m_isDead[0] = true;
    }
}
