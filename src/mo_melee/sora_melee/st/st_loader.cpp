#include <gf/gf_task_scheduler.h>
#include <st/loader/st_loader.h>
#include <types.h>
#include <memory.h>

// TODO: clean up - the three bytes after m_state (_0x45) are named by their use: [0] is the entry flag, [1] the request flag
// that requestOnce sets, [2] the lock flag. Give them real names in the header once the loaders using them are understood.
// HYPOTHESIS: m_state runs 0 (idle) .. 3 (loaded) .. 5 .. 6 (finished).

stLoader::stLoader(const char* taskName) : gfTask(taskName, gfTask::Category_None, 0xd, 6, true), m_globalModeMelee(NULL), m_state(0) {
    _0x45[0] = 0;
    _0x45[1] = 0;
    _0x45[2] = 0;
    m_globalModeMelee = g_GameGlobal->m_modeMelee;
    unk2C_b1 = false;
}

stLoader::~stLoader() { }

void stLoader::processBegin() { }

bool stLoader::isLoad() {
    return m_state == 3;
}

bool stLoader::isEmpty() {
    return m_state == 0;
}

bool stLoader::isFinish() {
    if (m_state == 6) {
        return true;
    }
    return isEmpty() == true;
}

bool stLoader::isEntity() {
    return (u8)_0x45[0] == 1;
}

void stLoader::entryEntity() {
    _0x45[0] = 1;
}

void stLoader::removeEntity() {
    _0x45[0] = 0;
}

void stLoader::setLockRequested() {
    _0x45[2] = 1;
}

bool stLoader::requestOnce() {
    if ((u8)_0x45[1] == 1) {
        return false;
    }
    _0x45[1] = 1;
    return true;
}

void stLoader::setGlobalModeMelee(gmGlobalModeMelee* globalModeMelee) {
    m_globalModeMelee = globalModeMelee;
}
