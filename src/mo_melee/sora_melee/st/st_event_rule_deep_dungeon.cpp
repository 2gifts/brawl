#include <st/st_event_rule.h>
#include <types.h>

// TODO: chechEventRule (Stage::isEventEnd plus a one-shot OSReport with a Shift-JIS message).

stEventRuleDeepDungeon::stEventRuleDeepDungeon() : stEventRule(1) { }

stEventRuleDeepDungeon::~stEventRuleDeepDungeon() { }

// TODO: clean up, give variables real names
namespace jj_25D3F8 {
struct Object {
    char padding[0x3c];

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
    virtual int callback(int, void*, void*);
};

struct GameGlobal {
    unsigned char padding[8];
    unsigned char* unk8;
};

extern "C" Object* g_Stage;
extern "C" GameGlobal* g_GameGlobal;
extern "C" char lbl_27_data_541F0[];
extern "C" void OSReport(const char* fmt, ...);

extern "C" int chechEventRule__22stEventRuleDeepDungeonFv(char* arg) {
    Object* obj = g_Stage;
    if (obj != 0) {
        if (obj->callback(0, arg + 8, arg + 12) != 0) {
            unsigned char* gg = g_GameGlobal->unk8;
            if (gg[0x1ad] != 1) {
                gg[0x1ad] = 1;
                OSReport(lbl_27_data_541F0);
            }
        }
    }
    return 0;
}
}

