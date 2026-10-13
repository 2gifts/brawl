#include <types.h>
#include <gf/gf_scene.h>

extern "C" char lbl_80422EB8[];
extern "C" char lbl_8059C660;
extern "C" void OSReport(const char* msg, ...);

void gfSceneManager::changeNextScene() {
    m_prevScene = m_currentScene;
    m_currentScene = NULL;
    while (true) {
        m_currentSequence->setNext();
        if (unk28C != 3) {
            break;
        }
        // MATCH-ONLY: the next sequence name must be loaded before the null check to match the schedule.
        const char* nextName = m_nextSequence->m_sequenceName;
        OSReport(lbl_80422EB8, m_currentSequence != NULL ? m_currentSequence->m_sequenceName : &lbl_8059C660, nextName);
        m_currentSequence->exit();
        gfSequence* nextSequence = m_nextSequence;
        m_nextSequence = NULL;
        m_prevSequence = m_currentSequence;
        m_currentSequence = nextSequence;
        unk28C = 0;
        nextSequence->start();
        unk28C = 1;
    }
    // MATCH-ONLY: the next scene name must be loaded before the null check to match the schedule.
    const char* nextName = m_nextScene->m_sceneName;
    OSReport(lbl_80422EB8, m_prevScene != NULL ? m_prevScene->m_sceneName : &lbl_8059C660, nextName);
    processStep = 0;
    m_currentScene = m_nextScene;
    m_nextScene = NULL;
    m_currentScene->setModule();
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0060[0x8] __attribute__((aligned(4))) = { 0 };
