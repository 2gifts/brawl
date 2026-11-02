#pragma once

#include <StaticAssert.h>
#include <mu/menu.h>  // MuCharKind
#include <types.h>

// The Chronicle diagrams of the Vault (sora_menu_main): records of each character and each name tag, shown as a table
// (one player), a comparison (two players) or rankings (three and four players).

// The records the diagrams show, in the order of the data table (map: MuDiagDataKind, muDiagGetDataChar ...).
// HYPOTHESIS: the names follow the getters of the map (muDiagGetTotalBeatChar ...); the last three kinds have no getter.
enum MuDiagDataKind {
    MuDiagData_TotalBeat = 0,
    MuDiagData_TotalFall = 1,
    MuDiagData_Suicide = 2,
    MuDiagData_HitRate = 3,
    MuDiagData_GiveDamage = 4,
    MuDiagData_TakeDamage = 5,
    MuDiagData_RecoverDamage = 6,
    MuDiagData_MaxDamage = 7,
    MuDiagData_RoundNum = 8,
    MuDiagData_ChampNum = 9,
    MuDiagData_KetsuNum = 10,
    MuDiagData_PlayTime = 11,
    MuDiagData_UseRate = 12,
    MuDiagData_JoinAverage = 13,
    MuDiagData_WalkDistance = 14,
    MuDiagData_JumpDistance = 15,
    MuDiagData_FallDistance = 16,
    MuDiagData_BlowDistance = 17,
    MuDiagData_GetCoin = 18,
    MuDiagData_MakeDropCoin = 19,
    MuDiagData_DropCoin = 20,
    MuDiagData_FallButtocks = 21,
    MuDiagData_SwimDistance = 22,
    MuDiagData_SwimFall = 23,
    MuDiagData_Unk24 = 24,
    MuDiagData_Unk25 = 25,
    MuDiagData_Unk26 = 26,
    MuDiagData_Num = 27,
};

// How muDiagTransformDataValue writes a value.
// HYPOTHESIS: the names come from the formats it uses ("%d", "%d:%02d", "%.2f" of the value / 100, centimetres as metres
// or feet).
enum MuDiagValueKind {
    MuDiagValue_Number = 0,
    MuDiagValue_MinSec = 1,
    MuDiagValue_Hundredths = 2,
    MuDiagValue_Distance = 3,
};

// One record of the diagrams: how to read it for a character and for a name tag, its unit (the message after the value)
// and its two messages (the title and the explanation).
// HYPOTHESIS: the struct name; the field names follow muDiagGetDataChar/Name/UnitKind/TitleMsgID/ExplMsgID of the map.
struct muDiagDataInfo {
    u32 (*m_getChar)(MuCharKind kind);
    u32 (*m_getName)(int nameNo);
    u8 m_unitKind;
    u8 m_valueKind;  // MuDiagValueKind
    u32 m_titleMsgId;
    u32 m_explMsgId;
};
static_assert(sizeof(muDiagDataInfo) == 0x14, "Class is wrong size!");

// The number of name tags of the records (GameGlobal::getGlobalRecordNameDatap)
enum { MuDiag_NameNum = 120 };

// The characters in the order the diagrams list them (map: muDiagGetMuCharKindRaw/RawNo).
MuCharKind muDiagGetMuCharKindRaw(int no);
int muDiagGetMuCharKindRawNo(MuCharKind kind);

u32 muDiagGetDataChar(MuDiagDataKind kind, MuCharKind chara);
u32 muDiagGetDataName(MuDiagDataKind kind, int nameNo);
u8 muDiagGetDataUnitKind(MuDiagDataKind kind);
u32 muDiagGetDataTitleMsgID(MuDiagDataKind kind);
u32 muDiagGetDataExplMsgID(MuDiagDataKind kind);
void muDiagTransformDataValue(char* buf, u32* unitMsgId, MuDiagDataKind kind, u32 value, u32 max);
