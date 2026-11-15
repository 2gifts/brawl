#include <so/effect/so_effect_screen_manager.h>
#include <types.h>

// TODO: the constructor (screen handle and interpreter stack setup).

soEffectScreenManager* g_soEffectScreenManager;

soEffectScreenManager::~soEffectScreenManager() {
    g_soEffectScreenManager = NULL;
}

void soEffectScreenManager::initialize() { }

// The list ends by itself; then nobody holds the screen any more.
void soEffectScreenManager::processEffect() {
    m_interpreter.interpretCmd(1.0f);
    if (m_interpreter.m_currentCmd == NULL) {
        m_cmd = NULL;
    }
}

// A request replaces the running list unless that one has the higher priority.
void soEffectScreenManager::req(const acAnimCmdConv* cmd, int priority, u32 owner) {
    if (m_cmd == NULL || m_priority <= priority) {
        m_interpreter.resetInterpreter(0.0f, cmd);
        m_interpreter.interpretCmd(0.0f);
        m_cmd = cmd;
        m_priority = priority;
        m_owner = owner;
    }
}

void soEffectScreenManager::remove(const acAnimCmdConv* cmd, u32 owner) {
    if (cmd == m_cmd && owner == m_owner) {
        m_interpreter.resetInterpreter(0.0f, NULL);
        m_cmd = NULL;
    }
}
