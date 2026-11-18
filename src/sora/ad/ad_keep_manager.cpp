#include <ad/ad_keep_manager.h>
#include <cstring>
#include <gm/gm_global.h>
#include <memory.h>
#include <types.h>
#include <ad/ad_seal_info.h>

extern "C" adSealInfoResult* fn_8015B0E0(void* sealInfo);
extern "C" void fn_8015B230(adSealInfoResult* sealInfo);
extern "C" void fn_80052704(u16 id, u8 num);
extern "C" void fn_80052AC0(u16 id, u8 num);
extern "C" void fn_8004FB48(u32 id);
extern "C" void fn_80051440(int coins);
extern "C" void fn_8015A7DC(adKeepManager* keepManager, float rate);
extern "C" void fn_8015AAA4(adKeepManager* keepManager);

// HYPOTHESIS: the global instance keeps its placeholder name; no map name is known.
adKeepManager lbl_8049ED30;

static inline gmAdvData* getAdvData() {
    return g_GameGlobal->m_advData;
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
        delete m_sealInfoResult;
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
        delete m_sealInfoResult;
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
    gmAdvData* advData = getAdvData();
    advData->unk4910 = 0;
    advData->unk4916 = 0;
    advData->m_stageCoin = 0;
    advData->m_isNanaDead[0] = false;
    advData->m_isNanaDead[1] = false;
    advData->unk4918 = 0;
    advData->unk491C = 0;
}

void adKeepManager::useStepInTiming() {}

// HYPOTHESIS: inlined range sync of global flags to gmAdvData.
inline void adKeepManager::resetGlobalFlag(u32 start, u32 end) {
    for (; start < end; start++) {
        u32 word = start >> 5;
        u32 bit = start & 0x1F;
        if (getGlobalFlag(start)) {
            getAdvData()->m_globalFlags[word] |= 1 << bit;
        }
    }
}

void adKeepManager::useStepOutTiming() {
    resetGlobalFlag(0x300, 0x3A7);
}

void adKeepManager::useSealInfoPrepareStart() {
    createSealInfoResult();
    unk24_b31 = true;
    m_sealInfoResult->getSealInfoPrepare();
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
    calculateCoin(false);
}

void adKeepManager::calculateCoin(bool addStageCoin) {
    gmAdvData* advData = getAdvData();
    // HYPOTHESIS: coin multiplier and bonus tables indexed by difficulty level / 3.
    float multiplier[5] = {0.0003f, 0.0004f, 0.0005f, 0.00065f, 0.0008f};
    float bonus[5] = {2.0f, 3.0f, 5.0f, 8.0f, 12.0f};
    int tier = advData->m_difficultyLevel / 3;
    float rate = 1.0f - 0.05f * advData->unk4916;
    if (rate <= 0.5f) {
        rate = 0.5f;
    }
    float coins = 0.0f;
    if (addStageCoin) {
        coins += rate * (advData->unk4910 * multiplier[tier]) + bonus[tier];
    }
    float scale = 0.5f;
    if (advData->unk491C != 0) {
        scale = 0.4f;
    }
    float total = coins + 0.08f * (scale * (advData->unk491C + advData->unk4918 + 1));
    advData->m_stageCoin += (u32)total;
    advData->unk4918 = 0;
    advData->unk491C = 0;
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
    resetGlobalFlag(0, 0x400);
}

void adKeepManager::useStageClearInTiming() {
    getAdvData()->unk60C += getAdvData()->unk4910;
    if (getAdvData()->unk60C > 999999999) {
        getAdvData()->unk60C = 999999999;
    }
    fn_8015AAA4(this);
    calculateCoin(true);
}

void adKeepManager::useStageClearOutTiming() {
    setResultOfTotaling(true, true);
    resetGlobalFlag(0, 0x400);
}

void adKeepManager::useSavePointTiming() {
    fn_8015AAA4(this);
    calculateCoin(false);
    setResultOfTotaling(false, true);
    resetGlobalFlag(0, 0x400);
}

void adKeepManager::useQuitAdventureTiming() {
    fn_8015AAA4(this);
    calculateCoin(false);
    setResultOfTotaling(true, false);
    m_gotRareItemNum = 0;
    memset(m_gotRareItems, 0, sizeof(adKeepRareItem) * 0xFF);
    useGameOverOutTimingContinue();
}

void adKeepManager::useReturnMapTiming() {
    fn_8015AAA4(this);
    calculateCoin(false);
    setResultOfTotaling(false, true);
    resetGlobalFlag(0, 0x400);
}

void adKeepManager::useGameEndingTiming() {}

bool adKeepManager::isLoadFinishSeal() {
    if (unk24_b31) {
        return m_sealInfoResult->isLoadFinish();
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
        *num = m_sealInfoResult->getFighterSealInfoNum(index);
        *info = reinterpret_cast<u32>(m_sealInfoResult->getFighterSealInfo(index));
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
    gmAdvData* advData = getAdvData();
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
        fn_80051440(advData->m_stageCoin * 10);
        advData->m_stageCoin = 0;
    }
    advData->m_isNanaDead[0] = false;
    advData->m_isNanaDead[1] = false;
}

void adKeepManager::setNanaDeadFlag(u32 index, bool dead) {
    if (g_GameGlobal->m_advData) {
        getAdvData()->m_isNanaDead[index] = dead;
    }
}

bool adKeepManager::isNanaDeadFlag(u32 index) {
    if (g_GameGlobal->m_advData) {
        return getAdvData()->m_isNanaDead[index];
    }
    return false;
}

// TODO: clean up, give variables real names
namespace jj_8015A138 {
struct Holder {
    char pad[0x24];
    unsigned int flags;
};

struct Arg0 {
    char pad[4];
    Holder* holder;
};

extern "C" void fn_8015A138(Arg0* arg0, int arg1, int arg2) {
    if (arg1) {
        arg0->holder->flags &= 0xFFFFEFFF;
        arg0->holder->flags &= 0xFFFFDFFF;
        arg0->holder->flags |= 0x4000;
        arg0->holder->flags |= 0x8000;
    } else {
        arg0->holder->flags |= 0x1000;
        arg0->holder->flags |= 0x2000;
        arg0->holder->flags &= 0xFFFFBFFF;
        arg0->holder->flags &= 0xFFFF7FFF;
    }
    arg0->holder->flags &= 0xFFFEFFFF;
    arg0->holder->flags &= 0xFFFDFFFF;
    arg0->holder->flags &= 0xFFFBFFFF;
    arg0->holder->flags &= 0xFFF7FFFF;
    arg0->holder->flags &= 0xFFEFFFFF;
    arg0->holder->flags &= 0xFFDFFFFF;
    arg0->holder->flags &= 0xFFBFFFFF;
    arg0->holder->flags &= 0xFF7FFFFF;
    switch (arg2) {
    case 0:
        arg0->holder->flags |= 0x10000;
        return;
    case 1:
        arg0->holder->flags |= 0x20000;
        return;
    case 2:
        if (arg1) {
            arg0->holder->flags |= 0x80000;
            return;
        }
        arg0->holder->flags |= 0x40000;
        return;
    case 3:
        if (arg1) {
            arg0->holder->flags |= 0x200000;
            return;
        }
        arg0->holder->flags |= 0x100000;
        return;
    case 4:
        if (arg1) {
            arg0->holder->flags |= 0x800000;
            return;
        }
        arg0->holder->flags |= 0x400000;
        return;
    default:
        arg0->holder->flags |= 0x20000;
        return;
    }
}
}

namespace jj_8015A434 {
struct Entry {
    int kind;
    int value;
    u8 count;
    bool keep;
};

struct Table {
    u8 unk00[0x14];
    Entry* entries;
    u32 unk18;
    u32 count;
};

extern "C" void fn_8015A434(void* arg0, Entry* arg1) {
    Table* self = static_cast<Table*>(arg0);
    u32 off;
    Entry* base;
    Entry* cur;
    u32 i;
    i = 0;
    off = 0;
    u32 n = self->count;

    for (; i < n; i++) {
        base = self->entries;
        cur = (Entry*)((u8*)base + off);
        if (arg1->kind == cur->kind && arg1->value == cur->value) {
            Entry* hit = base + i;
            hit->count++;
            return;
        }
        off += 12;
    }

    if (i < 0xFF) {
        self->entries[i] = *arg1;
        self->entries[i].count = 1;
        self->count++;
        return;
    }

    for (u32 k = 0; k < 254; k++) {
        self->entries[k] = self->entries[k + 1];
    }

    self->entries[254] = *arg1;
    self->entries[254].count = 1;
}
}

