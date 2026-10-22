#pragma once
#include <StaticAssert.h>
#include <types.h>

// HYPOTHESIS: local type/field names for the layout of GameGlobal::m_nteData
// (opaque nteGlobalData in BrawlHeaders) as the accessors read and write it.
// Dates are {year high, year low, month, day}; wider values are stored
// big-endian byte by byte.
struct nteGlobalDataLayout {
    u8 m_contribute;                // 0x00
    u8 m_isInfinityContribute;      // 0x01
    u8 m_collectionLifetime;        // 0x02
    u8 m_contributeStart[4];        // 0x03
    u8 m_contributeEnd[4];          // 0x07
    u8 m_watchStart[4];             // 0x0B
    u8 m_watchEnd[4];               // 0x0F
    u8 m_delivStart[4];             // 0x13
    u8 m_delivEnd[4];               // 0x17
    u8 m_lastContributed[4];        // 0x1B
    u8 m_lastDownload[4];           // 0x1F
    u8 m_timeStampFileSetting[4];   // 0x23
    u8 m_timeStampFileDeliv[8];     // 0x27
    u8 m_uploadSizeLimit;           // 0x2F
    u8 m_enableUploadCharacter[8];  // 0x30
    u8 m_enableUploadStage[8];      // 0x38
    u8 unk40[0x37];                 // 0x40
    u8 m_nwcSendDate[8];            // 0x77
    u8 m_nwcSendSize[4];            // 0x7F
    u8 m_permitFlags;               // 0x83
    u8 m_tryUpload[2];              // 0x84
    u8 m_uploaded[2];               // 0x86
};
static_assert(sizeof(nteGlobalDataLayout) == 0x88, "Class is wrong size!");

// HYPOTHESIS: local type/field names for the 0x20-byte block that GameGlobal::create
// allocates at GameGlobal+0x38.
struct nteGlobalUnk38 {
    u64 m_wiiID;                    // 0x00
    u8 m_gotDeliveryDataBits;       // 0x08
    u8 unk09;                       // 0x09
    u16 m_buttonCondition;          // 0x0A
    u8 unk0C[4];                    // 0x0C
    s64 m_serverTimeOffset;         // 0x10
    u8 m_enableNWCMail;             // 0x18
    u8 m_enableNWCDL;               // 0x19
    u8 m_ulCount;                   // 0x1A
    u8 m_permitWatchingGlobal;      // 0x1B
    u8 unk1C[4];                    // 0x1C
};
static_assert(sizeof(nteGlobalUnk38) == 0x20, "Class is wrong size!");

// HYPOTHESIS: the map gives no parameter types; these come from the callee register
// use and the callers' argument set-up.
class nteGlobal {
public:
    static void setContribute(u8 contribute);
    static bool isInfinityContribute();
    static void setIsInfinityContribute(bool isInfinity);
    static s64 getContribute_Start();
    static s64 getContribute_End();
    static void setContribute_Start(u16 year, u8 month, u8 day);
    static void setContribute_End(u16 year, u8 month, u8 day);
    static void setWatch_Start(u16 year, u8 month, u8 day);
    static void setWatch_End(u16 year, u8 month, u8 day);
    static void setDeliv_Start(u16 year, u8 month, u8 day);
    static void setDeliv_End(u16 year, u8 month, u8 day);
    static u8 getUploadSizeLimit();
    static void setUploadSizeLimit(u8 limit);
    static void setEnableUploadCharacter(u64 mask);
    static void setEnableUploadStage(u64 mask);
    static u8 getCollectionLifetime();
    static void setCollectionLifetime(u8 lifetime);
    static s64 getNWCSendDate();
    static u32 getNWCSendSize();
    static void setNWCSendDate(s64 date);
    static void setNWCSendSize(u32 size);
    static s64 getLastContributed();
    static void setLastContributed(u16 year, u8 month, u8 day);
    static s64 getLastDownload();
    static void setLastDownload(u16 year, u8 month, u8 day);
    static void setTimeStampFileSetting(u16 year, u8 month, u8 day);
    static s64 getTimeStampFileDeliv();
    static void setTimeStampFileDeliv(s64 time);
    static bool isPermitWatching();
    static void setPermitWatching(bool permit);
    static bool isWatchingEnableTime();
    static bool isPermitWatchingGlobal();
    static void setPermitWatchingGlobal(bool permit);
    static bool isPermitService();
    static void setPermitService(bool permit);
    static u8* getUploadSetting();
    static u16 getTryUpload();
    static void setTryUpload(u16 count);
    static u16 getUploaded();
    static void setUploaded(u16 count);
    static u64 getWiiID();
    static void setWiiID(u64 id);
    static bool isEnableButton(u16 mask);
    static void updateButtonCondition();
    static void resetGotDeliveryDataBits();
    static bool isGotDeliveryData(u8 mask);
    static void setEnableGotDeliveryData(u8 mask);
    static s64 getTime(bool* isValid);
    static void setServerTime(s64 serverTime);
    static bool isEnableNWCMail();
    static void setEnableNWCMail(bool enable);
    static bool isEnableNWCDL();
    static void setEnableNWCDL(bool enable);
    static u8 getULCount();
    static void addULCount();
};
