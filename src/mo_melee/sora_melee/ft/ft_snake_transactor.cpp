#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/snake/ft_snake_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/wn_weapon_builder.h>

struct wnSimpleData;

// The simple article (wn/wn_simple.h needs the builder chain): the members this unit uses.
class wnSimple : public Weapon {
public:
    void activate(s32 founderTaskId, s32 resourceId, const wnSimpleData* data, bool unk0, bool unk1,
                  const Vec3f& position, s32 unk2, s32 unk3, s32 unk4, bool unk5, bool unk6, s32 unk7, s32 unk8,
                  bool unk9, s32 unk10, s32 unk11, bool unk12);
};


// ftData::unk64 (declared only for the Kirby units) points at Kirby's table of simple-article data, 0x38-byte entries.
struct ftKirbySimpleDataEntry {
    u8 unk0[0x38];
};
static inline const ftKirbySimpleDataEntry* getKirbySimpleDataTable() {
    return **reinterpret_cast<ftKirbySimpleDataEntry** const*>(&g_ftCommonDataAccesser.getData(Fighter_Kirby)->unk58[0xC]);
}

// TODO: shootArticle (the grenade throw) and notifyArticleEventEject.

ftSnakeTransactor::ftSnakeTransactor() { }
ftSnakeTransactor::~ftSnakeTransactor() { }

bool ftSnakeTransactor::activeArticle1(wnSimple* article, soModuleAccesser* acc, u32 resId, const wnSimpleData* data) {
    article->activate(acc->getStageObject().m_taskId, resId, data, true, false, Vec3f(0.0f, 0.0f, 0.0f),
        0xffff, 0xffff, 0, true, false, -1, 0, false, 2, 0, false);
    return true;
}

// Kirby's copy hides in the box: simple article 0xd with entry 9 of Kirby's simple-article data.
bool ftSnakeTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnSimple* box = dynamic_cast<wnSimple*>(article);
        if (box == NULL) {
            return false;
        }
        const ftKirbySimpleDataEntry* table = getKirbySimpleDataTable();
        const wnSimpleData* data = reinterpret_cast<const wnSimpleData*>(&table[9]);
        u32 resId = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser()).getKirbyResId(Fighter_Snake);
        return activeArticle1(box, acc, resId, data);
    }
    return false;
}

// TODO: clean up, give variables real names
namespace jj_3BB160 {
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" float getConstantFloatKirby__16soModuleAccesserFl(void*, unsigned long);
extern "C" void throwAttack__8BaseItemFfP5Vec3ff(void*, float, float*, float);
extern "C" void fall__8BaseItemFP5Vec3ff(void*, float*, float);
extern "C" int convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(void*, unsigned int, int, void*, unsigned int);
extern "C" char g_ftKirbyCopyAbilityIdConverter[];
extern "C" char __RTTI__8BaseItem[];
extern "C" char __RTTI__9soArticle[];
extern "C" float lbl_27_rodata_8030;
extern "C" float lbl_27_rodata_8034;

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual int slot4(unsigned int);
    virtual void slot5();
    virtual int slot6();
    virtual void slot7();
    virtual void slot8();
    virtual float slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual int slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual int slot21();
};
class Other_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual unsigned int slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
};
struct ResultBase { char pad[0x3c]; };
class Result : public ResultBase {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void slot60();
    virtual void slot61();
    virtual void slot62();
    virtual void slot63();
    virtual void slot64();
    virtual void slot65();
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void slot69();
    virtual void slot70();
    virtual void slot71();
    virtual void slot72();
    virtual void slot73();
    virtual void slot74();
    virtual void slot75();
    virtual void slot76();
    virtual void slot77();
    virtual void slot78();
    virtual void slot79();
    virtual void slot80();
    virtual void slot81();
    virtual void slot82();
    virtual void slot83();
    virtual void slot84();
    virtual void slot85();
    virtual void slot86();
    virtual void slot87();
    virtual void slot88();
    virtual void slot89();
    virtual void slot90();
    virtual void slot91();
    virtual void slot92();
    virtual void slot93();
    virtual void slot94();
    virtual void slot95();
    virtual void slot96();
    virtual void slot97();
    virtual void slot98();
    virtual void slot99();
    virtual void slot100();
    virtual void slot101();
    virtual void slot102();
    virtual void slot103();
    virtual void slot104();
    virtual void slot105();
    virtual void slot106();
    virtual void slot107();
    virtual void slot108();
    virtual void slot109();
    virtual void slot110();
    virtual void slot111();
    virtual void slot112();
    virtual void slot113();
    virtual void slot114();
    virtual void slot115();
    virtual void slot116();
    virtual void slot117();
    virtual void slot118();
    virtual void slot119();
    virtual void slot120();
    virtual void slot121();
    virtual void slot122();
    virtual void slot123();
    virtual void slot124();
    virtual void slot125();
    virtual void slot126();
    virtual void slot127();
    virtual void slot128();
    virtual void slot129();
    virtual void slot130();
    virtual void slot131();
    virtual void slot132();
    virtual void slot133();
    virtual void slot134();
    virtual void slot135();
    virtual void slot136();
    virtual void slot137();
    virtual void slot138();
    virtual void slot139();
    virtual void slot140();
    virtual void slot141();
    virtual void slot142();
    virtual void slot143();
    virtual void slot144();
    virtual void slot145();
    virtual void slot146();
    virtual void slot147();
    virtual void slot148();
    virtual void slot149();
    virtual void slot150();
    virtual void slot151(int);
    virtual char* slot152();
};
static inline void setValue(Result* item, int value) { item->slot151(value); }
struct Modules {
    char pad0[8];
    Object_vptr_0* motion;
    Object_vptr_0* posture;
    char pad1[0x54];
    Object_vptr_0* work;
    char pad2[8];
    Object_vptr_0* status;
    char pad3[0x54];
    Other_vptr_0* other;
};
struct Accesser {
    char pad[0xd8];
    Modules* modules;
};

extern "C" int fn_27_3BB160(void* unused, Object_vptr_0* o, Accesser* acc) {
    float v[6];
    if (o->slot6() != 0xB) {
        return 0;
    }
    int r31 = convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(g_ftKirbyCopyAbilityIdConverter, 0, acc->modules->status->slot16(), acc, 1);
    Result* item = (Result*)__dynamic_cast(o, 0, __RTTI__8BaseItem, __RTTI__9soArticle, 0);
    if (item == 0) {
        return 0;
    }
    if (r31 == 0x12D) {
        float f31 = getConstantFloatKirby__16soModuleAccesserFl(acc, 0xFA5);
        v[3] = getConstantFloatKirby__16soModuleAccesserFl(acc, 0xFA4);
        v[4] = f31;
        v[5] = lbl_27_rodata_8030;
        int r = convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(g_ftKirbyCopyAbilityIdConverter, 3, acc->modules->motion->slot21(), acc, 1);
        switch (r) {
        case 0x1E4:
        case 0x1DE:
            v[3] = getConstantFloatKirby__16soModuleAccesserFl(acc, 0xFA2);
            v[4] = getConstantFloatKirby__16soModuleAccesserFl(acc, 0xFA3);
            break;
        case 0x1E5:
        case 0x1DF:
            v[3] = getConstantFloatKirby__16soModuleAccesserFl(acc, 0xFA6);
            v[4] = getConstantFloatKirby__16soModuleAccesserFl(acc, 0xFA7);
            break;
        }
        if (acc->modules->posture->slot9() < lbl_27_rodata_8030) {
            v[3] = -v[3];
        }
        throwAttack__8BaseItemFfP5Vec3ff(item, acc->modules->posture->slot9(), v + 3, lbl_27_rodata_8034);
        if (acc->modules->other->slot6() == 1) {
            int a = acc->modules->work->slot4(0x10000040);
            setValue(item, (int)(item->slot152() + a));
        }
    } else {
        v[0] = lbl_27_rodata_8030;
        v[1] = lbl_27_rodata_8030;
        v[2] = lbl_27_rodata_8030;
        fall__8BaseItemFP5Vec3ff(item, v, acc->modules->posture->slot9());
    }
    return 1;
}
}

