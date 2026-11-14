// Havok translation unit hkLinkedCollidable.o (main.dol 0x802FF394-0x802FF53C): the agent entry helpers of
// hkAgentNnMachine that hand an entry to its agent3 registration (filter update, time-of-impact reset, time warp).

#include <havok/hkAgentNnMachine.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkAgent1nTrack.h>

typedef void (*hkAgent3UpdateFilterFunc)(hkAgentNnEntry* entry, void* agentData, hkLinkedCollidable* collA,
                                         hkLinkedCollidable* collB, const hkCollisionInput& input);
typedef void (*hkAgent3InvalidateTimFunc)(hkAgentNnEntry* entry, void* agentData, const hkCollisionInput& input);
typedef void (*hkAgent3WarpTimeFunc)(hkAgentNnEntry* entry, void* agentData, hkReal oldTime, hkReal newTime,
                                     const hkCollisionInput& input);

// Offsets of the agent data behind an entry without and with time-of-impact information.
enum {
    HK_AGENT_DATA_OFFSET = 0x20,
    HK_AGENT_DATA_OFFSET_WITH_TIM = 0x30,
};

// Time of separating normal of an entry whose time-of-impact information is invalid.
static const hkReal HK_INVALID_TIM_TIME = -1.0f;

// Each helper switches on a copy of the stream command (a jump table over all seven commands); only the agent
// commands reach the registration.
void hkAgentNnMachine_UpdateShapeCollectionFilter(hkAgentNnEntry* entry, const hkCollisionInput& input) {
    void* agentData;
    u8 command = entry->m_streamCommand;
    switch (command) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
        break;
    case HK_AGENT_STREAM_NULL:
        return;
    case HK_AGENT_STREAM_END:
        return;
    case HK_AGENT_STREAM_CALL_FLIP:
        return;
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
        return;
    default:
        return;
    }
    hkAgent3UpdateFilterFunc func =
        (hkAgent3UpdateFilterFunc)input.m_dispatcher->m_agent3Func[entry->m_agentType].m_updateFilterFunc;
    if (func != NULL) {
        func(entry, agentData, entry->m_collidable[0], entry->m_collidable[1], input);
    }
}

void hkAgentNnMachine_InvalidateTimInAgent(hkAgentNnEntry* entry, const hkProcessCollisionInput& input) {
    void* agentData;
    u8 command = entry->m_streamCommand;
    switch (command) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM: {
        hkAgentNnTimEntry* timEntry = (hkAgentNnTimEntry*)entry;
        timEntry->m_timeOfSeparatingNormal = HK_INVALID_TIM_TIME;
        timEntry->m_separatingNormal.setZero4();
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
        break;
    }
    case HK_AGENT_STREAM_NULL:
        return;
    case HK_AGENT_STREAM_END:
        return;
    case HK_AGENT_STREAM_CALL_FLIP:
        return;
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
        return;
    default:
        return;
    }
    hkAgent3InvalidateTimFunc func =
        (hkAgent3InvalidateTimFunc)input.m_dispatcher->m_agent3Func[entry->m_agentType].m_invalidateTimFunc;
    if (func != NULL) {
        func(entry, agentData, input);
    }
}

// An entry whose separating plane was computed at the old time keeps it for the new time; any other is invalidated.
void hkAgentNnMachine_WarpTimeInAgent(hkAgentNnEntry* entry, float oldTime, float newTime,
                                      const hkProcessCollisionInput& input) {
    void* agentData;
    u8 command = entry->m_streamCommand;
    switch (command) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM: {
        hkAgentNnTimEntry* timEntry = (hkAgentNnTimEntry*)entry;
        if (timEntry->m_timeOfSeparatingNormal == oldTime) {
            timEntry->m_timeOfSeparatingNormal = newTime;
        } else {
            timEntry->m_timeOfSeparatingNormal = HK_INVALID_TIM_TIME;
        }
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
        break;
    }
    case HK_AGENT_STREAM_NULL:
        return;
    case HK_AGENT_STREAM_END:
        return;
    case HK_AGENT_STREAM_CALL_FLIP:
        return;
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
        return;
    default:
        return;
    }
    hkAgent3WarpTimeFunc func =
        (hkAgent3WarpTimeFunc)input.m_dispatcher->m_agent3Func[entry->m_agentType].m_warpTimeFunc;
    if (func != NULL) {
        func(entry, agentData, oldTime, newTime, input);
    }
}
