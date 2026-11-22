#include <memory.h>
#include <stddef.h>
#include <nw4r/ut/ut_list.h>
#include <so/jostle/so_jostle_manager.h>
#include <types.h>

soJostleManager* soJostleManager::m_instance;

// TODO: clean up - near-miss: the original gives the loop counters the higher registers in both list loops.
soJostleManager::soJostleManager() : gfTask("Jostle", Category_None, 0, Process_Effect, true) {
    for (int i = 0; i < 4; i++) {
        nw4r::ut::List_Init(&m_checkList[i], offsetof(soJostle, m_link));
    }
    for (int i = 0; i < 4; i++) {
        nw4r::ut::List_Init(&m_referList[i], offsetof(soJostle, m_link));
    }
    m_count = 0;
}

soJostleManager::~soJostleManager() {
    m_instance = NULL;
}

void soJostleManager::addCheckList(soJostle* jostle, s32 layer) {
    nw4r::ut::List_Append(&m_checkList[layer], jostle);
    if (!(m_count & 1)) {
        jostle->m_0x19 = 3;
    } else {
        jostle->m_0x19 = 1;
    }
    m_count++;
}

void soJostleManager::addReferList(soJostle* jostle, s32 layer) {
    nw4r::ut::List_Append(&m_referList[layer], jostle);
    if (!(m_count & 1)) {
        jostle->m_0x19 = 3;
    } else {
        jostle->m_0x19 = 1;
    }
    m_count++;
}

void soJostleManager::processBegin() {
    {
        int i = 0;
        nw4r::ut::List* list = m_checkList;
        for (; i < 4; i++, list++) {
            nw4r::ut::List_Init(list, offsetof(soJostle, m_link));
        }
    }
    {
        nw4r::ut::List* list = m_referList;
        int i = 0;
        for (; i < 4; i++, list++) {
            nw4r::ut::List_Init(list, offsetof(soJostle, m_link));
        }
    }
    m_count = 0;
}

// TODO: clean up - near-miss: the original keeps the log offset separate from the stack base when clearing the entries.
void soJostleManager::processEffect() {
    soJostleLog* log = (soJostleLog*)__alloca(m_count * sizeof(soJostleLog));
    s32 logNo = 0;
    for (s32 i = 0; i < 4; i++) {
        soJostle* jostle = NULL;
        while ((jostle = (soJostle*)nw4r::ut::List_GetNext(&m_checkList[i], jostle)) != NULL) {
            if (jostle->setLogNo(logNo) == true) {
                log[logNo].m_front = 0;
                log[logNo].m_back = 0;
                logNo++;
            }
        }
        jostle = NULL;
        while ((jostle = (soJostle*)nw4r::ut::List_GetNext(&m_referList[i], jostle)) != NULL) {
            if (jostle->setLogNo(logNo) == true) {
                log[logNo].m_front = 0;
                log[logNo].m_back = 0;
                logNo++;
            }
        }
    }
    for (s32 i = 0; i < 4; i++) {
        soJostle* jostle = NULL;
        while ((jostle = (soJostle*)nw4r::ut::List_GetNext(&m_checkList[i], jostle)) != NULL) {
            soJostle* other = jostle;
            while ((other = (soJostle*)nw4r::ut::List_GetNext(&m_checkList[i], other)) != NULL) {
                jostle->check(other, log, true);
            }
        }
    }
    for (s32 i = 0; i < 4; i++) {
        soJostle* jostle = NULL;
        while ((jostle = (soJostle*)nw4r::ut::List_GetNext(&m_checkList[i], jostle)) != NULL) {
            jostle->updatePosZ(log);
        }
    }
    for (s32 i = 0; i < 4; i++) {
        soJostle* jostle = NULL;
        while ((jostle = (soJostle*)nw4r::ut::List_GetNext(&m_referList[i], jostle)) != NULL) {
            soJostle* other = NULL;
            while ((other = (soJostle*)nw4r::ut::List_GetNext(&m_checkList[i], other)) != NULL) {
                jostle->check(other, log, false);
            }
        }
    }
    for (s32 i = 0; i < 4; i++) {
        soJostle* jostle = NULL;
        while ((jostle = (soJostle*)nw4r::ut::List_GetNext(&m_referList[i], jostle)) != NULL) {
            jostle->updatePosZ(log);
        }
    }
}
