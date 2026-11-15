#include <st/st_enemy_id_manager.h>
#include <em/em_manager.h>
#include <em/enemy.h>
#include <sr/sr_common.h>
#include <types.h>

stEnemyIdManager* g_stEnemyIdManager;

void stEnemyIdManager::create() {
    if (g_stEnemyIdManager == NULL) {
        g_stEnemyIdManager = new (Heaps::StageInstance) stEnemyIdManager;
    }
}

void stEnemyIdManager::remove() {
    if (g_stEnemyIdManager != NULL) {
        delete g_stEnemyIdManager;
        g_stEnemyIdManager = NULL;
    }
}

stEnemyIdManager::~stEnemyIdManager() { }

// The index of the closest enemy (within the range when it is positive), skipping kind 0x39; -1 when there is none.
int stEnemyIdManager::searchNearEnemy(Vec3f* pos, float, float range) {
    int nearest = -1;
    if ((int)getNumEnemies() > 0) {
        // BYTES: declared in this order for the original registers.
        int i;
        float nearestDistSq = 3.4028235e38f;
        emManager* manager = emManager::getInstance();
        float rangeSq = range * range;
        for (i = 0; i < (int)getNumEnemies(); i++) {
            Enemy* enemy = manager->getEnemyRefFromId(getEnemyId(i));
            Vec3f center = enemy->getCenterPos();
            Vec3f diff = *pos - center;
            float distSq = diff.m_x * diff.m_x + diff.m_y * diff.m_y + diff.m_z * diff.m_z;
            if (enemy->soGetSubKind() == 0x39) {
                continue;
            }
            if (range > 0.0f && distSq > rangeSq) {
                continue;
            }
            if (nearestDistSq > distSq) {
                nearestDistSq = distSq;
                nearest = i;
            }
        }
    }
    return nearest;
}
