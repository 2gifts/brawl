#include <ad/ad_keep_manager.h>
#include <cstring>
#include <gm/gm_global.h>
#include <memory.h>
#include <types.h>

extern "C" adSealInfoResult* fn_8015B0E0(void* sealInfo);
extern "C" void fn_8015B17C(adSealInfoResult* sealInfo, s32 flag);
extern "C" void fn_8015B1E0(adSealInfoResult* sealInfo);
extern "C" bool fn_8015B228(adSealInfoResult* sealInfo);
extern "C" void fn_8015B230(adSealInfoResult* sealInfo);
extern "C" u32 fn_8015B3CC(adSealInfoResult* sealInfo, u32 index);
extern "C" u32 fn_8015B4BC(adSealInfoResult* sealInfo, u32 index);
extern "C" void fn_80052704(u16 id, u8 num);
extern "C" void fn_80052AC0(u16 id, u8 num);
extern "C" void fn_8004FB48(u32 id);
extern "C" void fn_80051440(int coins);
extern "C" void fn_8015A7DC(adKeepManager* keepManager, float rate);
extern "C" void fn_8015AAA4(adKeepManager* keepManager);
extern "C" void fn_8015AE80(adKeepManager* keepManager, bool addStageCoin);

// HYPOTHESIS: the global instance keeps its placeholder name; no map name is known.
adKeepManager lbl_8049ED30;

static inline adKeepAdvData* getAdvData() {
    return reinterpret_cast<adKeepAdvData*>(g_GameGlobal->m_advData);
}

adKeepManager::adKeepManager() {
    m_stageFlags = NULL;
    m_globalFlags = NULL;
    m_sealInfoResult = NULL;
    m_stepFlags = NULL;
    m_stepFlagNum = 0;
    unk24_b31 = true;
}

adKeepManager* adKeepManager::getInstance() {
    return &lbl_8049ED30;
}

adKeepManager::~adKeepManager() {
    if (m_stageFlags) {
        delete[] m_stageFlags;
        m_stageFlags = NULL;
    }
    if (m_globalFlags) {
        delete[] m_globalFlags;
        m_globalFlags = NULL;
    }
    if (m_stepFlags) {
        delete[] m_stepFlags;
        m_stepFlags = NULL;
    }
    m_stepFlagNum = 0;
    if (m_gotRareItems) {
        delete[] m_gotRareItems;
        m_gotRareItems = NULL;
    }
    m_gotRareItemNum = 0;
    if (m_lostRareItems) {
        delete[] m_lostRareItems;
        m_lostRareItems = NULL;
    }
    m_lostRareItemNum = 0;
    if (m_sealInfoResult) {
        fn_8015B17C(m_sealInfoResult, 1);
    }
    m_sealInfoResult = NULL;
}

adKeepStepFlag* adKeepManager::getStepFlag(u32 stepId) {
    for (u32 i = 0; i < m_stepFlagNum; i++) {
        if (m_stepFlags[i].m_stepId == stepId) {
            return &m_stepFlags[i];
        }
    }
    if (m_stepFlagNum >= 0x60) {
        return m_stepFlags;
    }
    m_stepFlags[m_stepFlagNum].m_stepId = stepId;
    m_stepFlagNum++;
    return &m_stepFlags[m_stepFlagNum - 1];
}

void adKeepManager::resetGlobalFlag() {
    memcpy(m_globalFlags, getAdvData()->m_globalFlags, 0x80);
}

void adKeepManager::setStageFlag(u32 id, bool on) {
    u32 word = id >> 5;
    u32 bit = id & 0x1F;
    if (on == true) {
        m_stageFlags[word] |= 1 << bit;
    } else {
        m_stageFlags[word] &= ~(1 << bit);
    }
}

bool adKeepManager::getStageFlag(u32 id) {
    return m_stageFlags[id >> 5] & (1 << (id & 0x1F));
}

void adKeepManager::setGlobalFlag(u32 id, bool on) {
    u32 word = id >> 5;
    u32 bit = id & 0x1F;
    if (on == true) {
        m_globalFlags[word] |= 1 << bit;
    } else {
        m_globalFlags[word] &= ~(1 << bit);
    }
}

bool adKeepManager::getGlobalFlag(u32 id) {
    return m_globalFlags[id >> 5] & (1 << (id & 0x1F));
}

bool adKeepManager::getKeepFlag(u32 id, u32 kind) {
    if (kind == 0) {
        return false;
    }
    if (kind == 1) {
        return getStageFlag(id);
    }
    if (kind == 2) {
        return getGlobalFlag(id);
    }
    return false;
}

void adKeepManager::useInitilizeTiming() {
    if (!m_stageFlags) {
        m_stageFlags = new (Heaps::GlobalMode) u32[0x20];
        memset(m_stageFlags, 0, 0x80);
    }
    if (!m_globalFlags) {
        m_globalFlags = new (Heaps::GlobalMode) u32[0x20];
        resetGlobalFlag();
    }
    if (!m_stepFlags) {
        m_stepFlags = new (Heaps::GlobalMode) adKeepStepFlag[0x60];
        m_stepFlagNum = 0;
        memset(m_stepFlags, 0, sizeof(adKeepStepFlag) * 0x60);
    }
    if (!m_gotRareItems) {
        m_gotRareItems = new (Heaps::GlobalMode) adKeepRareItem[0xFF];
        m_gotRareItemNum = 0;
        memset(m_gotRareItems, 0, sizeof(adKeepRareItem) * 0xFF);
        if (m_lostRareItems) {
            delete[] m_lostRareItems;
            m_lostRareItems = NULL;
        }
        m_lostRareItemNum = 0;
    }
}

void adKeepManager::createSealInfoResult() {
    if (!m_sealInfoResult) {
        void* sealInfo = operator new(0x7D04, Heaps::StageCommonResource);
        if (sealInfo) {
            sealInfo = fn_8015B0E0(sealInfo);
        }
        m_sealInfoResult = static_cast<adSealInfoResult*>(sealInfo);
    }
}

void adKeepManager::releaseSealInfoResult() {
    if (m_sealInfoResult) {
        fn_8015B17C(m_sealInfoResult, 1);
    }
    m_sealInfoResult = NULL;
}

void adKeepManager::useStageSelectTiming() {
    memset(m_stageFlags, 0, 0x80);
    resetGlobalFlag();
    m_stepFlagNum = 0;
    memset(m_stepFlags, 0, sizeof(adKeepStepFlag) * 0x60);
}

void adKeepManager::useWorldMapTiming() {
    adKeepAdvData* advData = getAdvData();
    advData->unk4910 = 0;
    advData->unk4916 = 0;
    advData->unk4920 = 0;
    advData->unk4914[0] = false;
    advData->unk4914[1] = false;
    advData->unk4918 = 0;
    advData->unk491C = 0;
}

void adKeepManager::useStepInTiming() {}

void adKeepManager::useStepOutTiming() {
    for (u32 i = 0x300; i < 0x3A7; i++) {
        if (getGlobalFlag(i)) {
            getAdvData()->m_globalFlags[i >> 5] |= 1 << (i & 0x1F);
        }
    }
}

void adKeepManager::useSealInfoPrepareStart() {
    createSealInfoResult();
    unk24_b31 = true;
    fn_8015B1E0(m_sealInfoResult);
}

void adKeepManager::useGameOverInTiming() {
    getAdvData()->unk4910++;
    if (getAdvData()->unk4910 > 999999999) {
        getAdvData()->unk4910 = 999999999;
    }
    if (getAdvData()->unk4916 < 0xFF) {
        getAdvData()->unk4916++;
    }
    fn_8015A7DC(this, 0.5f);
    fn_8015AAA4(this);
    fn_8015AE80(this, false);
}

void adKeepManager::useGameOverOutTimingContinue() {
    if (m_lostRareItems) {
        delete[] m_lostRareItems;
        m_lostRareItems = NULL;
    }
    m_lostRareItemNum = 0;
}

void adKeepManager::useGameOverOutTiming() {
    setResultOfTotaling(true, true);
    for (u32 i = 0; i < 0x400; i++) {
        if (getGlobalFlag(i)) {
            getAdvData()->m_globalFlags[i >> 5] |= 1 << (i & 0x1F);
        }
    }
}

void adKeepManager::useStageClearInTiming() {
    getAdvData()->unk60C += getAdvData()->unk4910;
    if (getAdvData()->unk60C > 999999999) {
        getAdvData()->unk60C = 999999999;
    }
    fn_8015AAA4(this);
    fn_8015AE80(this, true);
}

void adKeepManager::useStageClearOutTiming() {
    setResultOfTotaling(true, true);
    for (u32 i = 0; i < 0x400; i++) {
        if (getGlobalFlag(i)) {
            getAdvData()->m_globalFlags[i >> 5] |= 1 << (i & 0x1F);
        }
    }
}

void adKeepManager::useSavePointTiming() {
    fn_8015AAA4(this);
    fn_8015AE80(this, false);
    setResultOfTotaling(false, true);
    for (u32 i = 0; i < 0x400; i++) {
        if (getGlobalFlag(i)) {
            getAdvData()->m_globalFlags[i >> 5] |= 1 << (i & 0x1F);
        }
    }
}

void adKeepManager::useQuitAdventureTiming() {
    fn_8015AAA4(this);
    fn_8015AE80(this, false);
    setResultOfTotaling(true, false);
    m_gotRareItemNum = 0;
    memset(m_gotRareItems, 0, sizeof(adKeepRareItem) * 0xFF);
    useGameOverOutTimingContinue();
}

void adKeepManager::useReturnMapTiming() {
    fn_8015AAA4(this);
    fn_8015AE80(this, false);
    setResultOfTotaling(false, true);
    for (u32 i = 0; i < 0x400; i++) {
        if (getGlobalFlag(i)) {
            getAdvData()->m_globalFlags[i >> 5] |= 1 << (i & 0x1F);
        }
    }
}

void adKeepManager::useGameEndingTiming() {}

bool adKeepManager::isLoadFinishSeal() {
    if (unk24_b31) {
        return fn_8015B228(m_sealInfoResult);
    }
    return true;
}

void adKeepManager::getSealInfoFromGameGlobal() {
    if (unk24_b31) {
        fn_8015B230(m_sealInfoResult);
        unk24_b31 = false;
    }
}

void adKeepManager::getFighterSealInfo(u32 index, u32* num, u32* info) {
    *num = 0;
    *info = 0;
    if (m_sealInfoResult) {
        *num = fn_8015B3CC(m_sealInfoResult, index);
        *info = fn_8015B4BC(m_sealInfoResult, index);
    }
}

u32 adKeepManager::getAdGotRareItemNum() {
    return m_gotRareItemNum;
}

adKeepRareItem* adKeepManager::getAdGotRareItem(u32 index) {
    return &m_gotRareItems[index];
}

u32 adKeepManager::getAdLostRareItemNum() {
    return m_lostRareItemNum;
}

adKeepRareItem* adKeepManager::getAdLostRareItem(u32 index) {
    return &m_lostRareItems[index];
}

void adKeepManager::setResultOfTotaling(bool addCoin, bool applyItems) {
    adKeepAdvData* advData = getAdvData();
    if (applyItems) {
        for (u32 i = 0; i < m_gotRareItemNum; i++) {
            if (m_gotRareItems[i].unk8 != 0) {
                if (m_gotRareItems[i].unk0 == 0x17) {
                    fn_80052704(m_gotRareItems[i].unk4, m_gotRareItems[i].unk8);
                }
                if (m_gotRareItems[i].unk0 == 0x33) {
                    fn_80052AC0(m_gotRareItems[i].unk4, m_gotRareItems[i].unk8);
                }
                if (m_gotRareItems[i].unk0 == 0xB) {
                    fn_8004FB48(m_gotRareItems[i].unk4);
                }
            }
        }
        m_gotRareItemNum = 0;
        memset(m_gotRareItems, 0, sizeof(adKeepRareItem) * 0xFF);
        useGameOverOutTimingContinue();
    }
    if (addCoin) {
        fn_80051440(advData->unk4920 * 10);
        advData->unk4920 = 0;
    }
    advData->unk4914[0] = false;
    advData->unk4914[1] = false;
}

void adKeepManager::setNanaDeadFlag(u32 index, bool dead) {
    if (g_GameGlobal->m_advData) {
        getAdvData()->unk4914[index] = dead;
    }
}

bool adKeepManager::isNanaDeadFlag(u32 index) {
    if (g_GameGlobal->m_advData) {
        return getAdvData()->unk4914[index];
    }
    return false;
}
