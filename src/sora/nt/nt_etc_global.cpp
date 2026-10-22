#include <types.h>
#include <string.h>
#include <gm/gm_global.h>
#include <nt/nt_etc_global.h>
#include <revolution/OS/OSTime.h>

// HYPOTHESIS: local accessor names; they only cast the GameGlobal storage.
static inline nteGlobalDataLayout* getData() {
    return reinterpret_cast<nteGlobalDataLayout*>(g_GameGlobal->m_nteData);
}

static inline nteGlobalUnk38* getUnk38() {
    return *reinterpret_cast<nteGlobalUnk38**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x38);
}

// HYPOTHESIS: local names modelled on the map's setWatch_*/setDeliv_*; only their inlined
// copies in updateButtonCondition exist in the target.
static inline s64 getWatch_Start() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_watchStart[1] + (data->m_watchStart[0] << 8));
    time.month = data->m_watchStart[2] - 1;
    time.mday = data->m_watchStart[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

static inline s64 getWatch_End() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_watchEnd[1] + (data->m_watchEnd[0] << 8));
    time.month = data->m_watchEnd[2] - 1;
    time.mday = data->m_watchEnd[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

static inline s64 getDeliv_Start() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_delivStart[1] + (data->m_delivStart[0] << 8));
    time.month = data->m_delivStart[2] - 1;
    time.mday = data->m_delivStart[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

static inline s64 getDeliv_End() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_delivEnd[1] + (data->m_delivEnd[0] << 8));
    time.month = data->m_delivEnd[2] - 1;
    time.mday = data->m_delivEnd[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

static inline void setDate(u8* date, u16 year, u8 month, u8 day) {
    date[0] = year >> 8;
    date[1] = year;
    date[2] = month;
    date[3] = day;
}

static inline void setU64(u8* dst, u64 value) {
    dst[0] = value >> 56;
    dst[1] = value >> 48;
    dst[2] = value >> 40;
    dst[3] = value >> 32;
    dst[4] = value >> 24;
    dst[5] = value >> 16;
    dst[6] = value >> 8;
    dst[7] = value;
}

static inline u64 getU64(const u8* src) {
    return ((u64)src[0] << 56) + ((u64)src[1] << 48) + ((u64)src[2] << 40) + ((u64)src[3] << 32) +
           ((u64)src[4] << 24) + ((u64)src[5] << 16) + ((u64)src[6] << 8) + (u64)src[7];
}

// HYPOTHESIS: local name for the inlined contribute-type query of updateButtonCondition.
static inline bool isContribute(u8 mask) {
    bool result = false;
    if (getData()->m_contribute & mask) {
        result = true;
    }
    return result;
}

void nteGlobal::setContribute(u8 contribute) {
    getData()->m_contribute = contribute;
}

bool nteGlobal::isInfinityContribute() {
    return getData()->m_isInfinityContribute != 0;
}

void nteGlobal::setIsInfinityContribute(bool isInfinity) {
    getData()->m_isInfinityContribute = isInfinity;
}

s64 nteGlobal::getContribute_Start() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_contributeStart[1] + (data->m_contributeStart[0] << 8));
    time.month = data->m_contributeStart[2] - 1;
    time.mday = data->m_contributeStart[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

s64 nteGlobal::getContribute_End() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_contributeEnd[1] + (data->m_contributeEnd[0] << 8));
    time.month = data->m_contributeEnd[2] - 1;
    time.mday = data->m_contributeEnd[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

void nteGlobal::setContribute_Start(u16 year, u8 month, u8 day) {
    setDate(getData()->m_contributeStart, year, month, day);
}

void nteGlobal::setContribute_End(u16 year, u8 month, u8 day) {
    setDate(getData()->m_contributeEnd, year, month, day);
}

void nteGlobal::setWatch_Start(u16 year, u8 month, u8 day) {
    setDate(getData()->m_watchStart, year, month, day);
}

void nteGlobal::setWatch_End(u16 year, u8 month, u8 day) {
    setDate(getData()->m_watchEnd, year, month, day);
}

void nteGlobal::setDeliv_Start(u16 year, u8 month, u8 day) {
    setDate(getData()->m_delivStart, year, month, day);
}

void nteGlobal::setDeliv_End(u16 year, u8 month, u8 day) {
    setDate(getData()->m_delivEnd, year, month, day);
}

u8 nteGlobal::getUploadSizeLimit() {
    return getData()->m_uploadSizeLimit;
}

void nteGlobal::setUploadSizeLimit(u8 limit) {
    getData()->m_uploadSizeLimit = limit;
}

void nteGlobal::setEnableUploadCharacter(u64 mask) {
    setU64(getData()->m_enableUploadCharacter, mask);
}

void nteGlobal::setEnableUploadStage(u64 mask) {
    setU64(getData()->m_enableUploadStage, mask);
}

u8 nteGlobal::getCollectionLifetime() {
    return getData()->m_collectionLifetime;
}

void nteGlobal::setCollectionLifetime(u8 lifetime) {
    getData()->m_collectionLifetime = lifetime;
}

s64 nteGlobal::getNWCSendDate() {
    return getU64(getData()->m_nwcSendDate);
}

u32 nteGlobal::getNWCSendSize() {
    const u8* size = getData()->m_nwcSendSize;
    return (size[0] << 24) + (size[1] << 16) + (size[2] << 8) + size[3];
}

void nteGlobal::setNWCSendDate(s64 date) {
    setU64(getData()->m_nwcSendDate, date);
}

void nteGlobal::setNWCSendSize(u32 size) {
    u8* dst = getData()->m_nwcSendSize;
    dst[0] = size >> 24;
    dst[1] = size >> 16;
    dst[2] = size >> 8;
    dst[3] = size;
}

s64 nteGlobal::getLastContributed() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_lastContributed[1] + (data->m_lastContributed[0] << 8));
    time.month = data->m_lastContributed[2] - 1;
    time.mday = data->m_lastContributed[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

void nteGlobal::setLastContributed(u16 year, u8 month, u8 day) {
    setDate(getData()->m_lastContributed, year, month, day);
}

s64 nteGlobal::getLastDownload() {
    nteGlobalDataLayout* data = getData();
    OSCalendarTime time;
    memset(&time, 0, sizeof(time));
    time.year = (u16)(data->m_lastDownload[1] + (data->m_lastDownload[0] << 8));
    time.month = data->m_lastDownload[2] - 1;
    time.mday = data->m_lastDownload[3];
    if (time.year < 1 || time.mday < 1) {
        return 0;
    }
    return OSCalendarTimeToTicks(&time);
}

void nteGlobal::setLastDownload(u16 year, u8 month, u8 day) {
    setDate(getData()->m_lastDownload, year, month, day);
}

void nteGlobal::setTimeStampFileSetting(u16 year, u8 month, u8 day) {
    setDate(getData()->m_timeStampFileSetting, year, month, day);
}

s64 nteGlobal::getTimeStampFileDeliv() {
    return getU64(getData()->m_timeStampFileDeliv);
}

void nteGlobal::setTimeStampFileDeliv(s64 time) {
    setU64(getData()->m_timeStampFileDeliv, time);
}

bool nteGlobal::isPermitWatching() {
    return getData()->m_permitFlags & 1;
}

void nteGlobal::setPermitWatching(bool permit) {
    nteGlobalDataLayout* data = getData();
    if (permit) {
        data->m_permitFlags |= 1;
    } else {
        data->m_permitFlags &= ~1;
    }
}

bool nteGlobal::isWatchingEnableTime() {
    return (getUnk38()->m_buttonCondition >> 5) & 1;
}

bool nteGlobal::isPermitWatchingGlobal() {
    return getUnk38()->m_permitWatchingGlobal != 0;
}

void nteGlobal::setPermitWatchingGlobal(bool permit) {
    getUnk38()->m_permitWatchingGlobal = permit;
}

bool nteGlobal::isPermitService() {
    return (getData()->m_permitFlags & 2) != 0;
}

void nteGlobal::setPermitService(bool permit) {
    nteGlobalDataLayout* data = getData();
    if (permit) {
        data->m_permitFlags |= 2;
    } else {
        data->m_permitFlags &= ~2;
    }
}

u8* nteGlobal::getUploadSetting() {
    return &getData()->m_uploadSizeLimit;
}

u16 nteGlobal::getTryUpload() {
    const u8* count = getData()->m_tryUpload;
    return count[1] + (count[0] << 8);
}

void nteGlobal::setTryUpload(u16 count) {
    u8* dst = getData()->m_tryUpload;
    dst[0] = count >> 8;
    dst[1] = count;
}

u16 nteGlobal::getUploaded() {
    const u8* count = getData()->m_uploaded;
    return count[1] + (count[0] << 8);
}

void nteGlobal::setUploaded(u16 count) {
    u8* dst = getData()->m_uploaded;
    dst[0] = count >> 8;
    dst[1] = count;
}

u64 nteGlobal::getWiiID() {
    return getUnk38()->m_wiiID;
}

void nteGlobal::setWiiID(u64 id) {
    getUnk38()->m_wiiID = id;
}

bool nteGlobal::isEnableButton(u16 mask) {
    u16 cond = getUnk38()->m_buttonCondition;
    return (cond & mask) != 0;
}

void nteGlobal::updateButtonCondition() {
    getUnk38()->m_buttonCondition = 0;
    bool isValid;
    s64 now = getTime(&isValid);
    if (!isValid) {
        return;
    }
    if (isPermitService()) {
        s64 start = getContribute_Start();
        s64 end = getContribute_End();
        getLastContributed();
        if (start != 0 && end != 0 && start <= now && now <= end) {
            if (isContribute(1)) {
                getUnk38()->m_buttonCondition |= 2;
            }
            if (isContribute(2)) {
                getUnk38()->m_buttonCondition |= 1;
            }
            if (isContribute(4)) {
                getUnk38()->m_buttonCondition |= 4;
            }
        }
    }
    {
        s64 start = getWatch_Start();
        s64 end = getWatch_End();
        if (start != 0 && end != 0 && start <= now && now <= end) {
            getUnk38()->m_buttonCondition |= 0x20;
            if (isContribute(8)) {
                getUnk38()->m_buttonCondition |= 0x10;
            }
        }
    }
    {
        s64 start = getDeliv_Start();
        s64 end = getDeliv_End();
        if (start != 0 && end != 0 && start <= now && now <= end) {
            getUnk38()->m_buttonCondition |= 0x40;
        }
    }
    if ((getUnk38()->m_buttonCondition & 7) || (getUnk38()->m_gotDeliveryDataBits & 7)) {
        getUnk38()->m_buttonCondition |= 8;
    }
}

void nteGlobal::resetGotDeliveryDataBits() {
    getUnk38()->m_gotDeliveryDataBits = 0;
}

bool nteGlobal::isGotDeliveryData(u8 mask) {
    u8 bits = getUnk38()->m_gotDeliveryDataBits;
    return (bits & mask) != 0;
}

void nteGlobal::setEnableGotDeliveryData(u8 mask) {
    getUnk38()->m_gotDeliveryDataBits |= mask;
}

s64 nteGlobal::getTime(bool* isValid) {
    nteGlobalUnk38* unk38 = getUnk38();
    s64 time = OSGetTime();
    time += unk38->m_serverTimeOffset;
    if (isValid != NULL) {
        if (unk38->m_serverTimeOffset == 0) {
            *isValid = false;
        } else {
            *isValid = true;
        }
    }
    return time;
}

void nteGlobal::setServerTime(s64 serverTime) {
    if (serverTime != 0) {
        nteGlobalUnk38* unk38 = getUnk38();
        unk38->m_serverTimeOffset = serverTime - OSGetTime();
        if (unk38->m_serverTimeOffset == 0) {
            unk38->m_serverTimeOffset = 1;
        }
    }
}

bool nteGlobal::isEnableNWCMail() {
    return getUnk38()->m_enableNWCMail != 0;
}

void nteGlobal::setEnableNWCMail(bool enable) {
    getUnk38()->m_enableNWCMail = enable;
}

bool nteGlobal::isEnableNWCDL() {
    return getUnk38()->m_enableNWCDL != 0;
}

void nteGlobal::setEnableNWCDL(bool enable) {
    getUnk38()->m_enableNWCDL = enable;
}

u8 nteGlobal::getULCount() {
    return getUnk38()->m_ulCount;
}

void nteGlobal::addULCount() {
    getUnk38()->m_ulCount++;
}
