#include <st/st_event_rule.h>
#include <types.h>

// TODO: notifyEventDead.

stEventRuleEvent40::stEventRuleEvent40() : stEventRule(1) { }

stEventRuleEvent40::~stEventRuleEvent40() { }

bool stEventRuleEvent40::chechEventRule() {
    return false;
}

// TODO: clean up, give variables real names
namespace jj_25DC60 {
typedef unsigned char u8;

class Obj {
    char pad[0x3C];

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
    virtual void invoke(int, int*, int*);
};

struct Self {
    char pad0[4];
    u8 b4;
    char pad1[3];
    int w8;
    int wC;
};

extern "C" {
extern Obj* g_Stage;
extern void* g_ftManager;
int isValidEntryId__9ftManagerFi(void*, int);
void* getOwner__9ftManagerFi(void*, int);
void setStockCount__7ftOwnerFi(void*, void*);
}

extern "C" u8 notifyEventDead__18stEventRuleEvent40FiUl(Self* arg0, int arg1, int arg2) {
    if (arg2 == 0) {
        return arg0->b4;
    }

    if (g_Stage != 0) {
        g_Stage->invoke(0, &arg0->w8, &arg0->wC);
        if (arg0->wC == 0) {
            return arg0->b4;
        }

        if (isValidEntryId__9ftManagerFi(g_ftManager, arg1)) {
            void* owner = getOwner__9ftManagerFi(g_ftManager, arg1);
            setStockCount__7ftOwnerFi(owner, 0);
        }
        return 0;
    }

    return arg0->b4;
}
}

