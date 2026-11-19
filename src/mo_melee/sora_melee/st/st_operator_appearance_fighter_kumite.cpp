#include <st/operator/st_operator_appearance_fighter_kumite.h>
#include <types.h>

stOperatorAppearanceFighterKumite::stOperatorAppearanceFighterKumite(u8 arg) {
    unk1B6 = arg;
    unk1B8 = NULL;
    unk1B5 = false;
}

stOperatorAppearanceFighterKumite::~stOperatorAppearanceFighterKumite() { }

bool stOperatorAppearanceFighterKumite::isEnd() {
    return unk1B5;
}

// TODO: clean up, give variables real names
namespace jj_244974 {
extern "C" {
    extern void* g_GameGlobal;
    extern void* g_gfApplication;
    extern void* g_cmAIController;
    extern const float lbl_27_rodata_3EF8;
    int isJustGameFrame__10GameGlobalCFv(void*);
    int isEntryFighter__27stOperatorAppearanceFighterFi(void*, int);
    int fn_27_23AC10(void*);
    void fn_8009F8B4(void*);
    void render__17gfKeepFrameBufferFv(void*);
}

struct EntityBase {
    unsigned char pad[0x3C];
};

class Entity : public EntityBase {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual int v25();
    virtual unsigned int v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
};

struct State {
    unsigned char pad00[0x40];
    unsigned char state;
    unsigned char pad41;
    unsigned char pad42;
    unsigned char flag43;
    unsigned char pad44[0x30];
    Entity* entities[7];
    Entity* unk90;
    unsigned char pad94[0x121];
    unsigned char flag1B5;
    unsigned char flag1B6;
    unsigned char pad1B7;
    int unk1B8;
};

struct GameGlobal {
    unsigned char pad[0x44];
    void* unk44;
};

struct FloatHolder {
    unsigned char pad[4];
    float value;
};

extern "C" void fn_27_244974(State* self) {
    if (self->flag43 != 0) return;
    if (isJustGameFrame__10GameGlobalCFv(g_GameGlobal) == 0) return;
    if (self->state == 0) return;
    if (self->state == 7) return;
    if (self->flag1B5 == 1) return;

    if (self->unk1B8 > 0) {
        self->unk1B8 = self->unk1B8 - 1;
        return;
    }

    for (int i = 0; i < 7; i++) {
        if (isEntryFighter__27stOperatorAppearanceFighterFi(self, i) == 0) continue;
        if (self->entities[i]->v26() == 1) continue;
        if (self->entities[i]->v25() == 0) return;
    }

    for (int j = 6; j >= 0; j--) {
        if (isEntryFighter__27stOperatorAppearanceFighterFi(self, j) == 0) continue;
        if (self->entities[j]->v26() == 1) continue;
        if (fn_27_23AC10(self->entities[j]) != 0) continue;
        self->entities[j]->v23();
        if (self->flag1B6 != 0) {
            self->entities[j]->v34();
        }
    }

    render__17gfKeepFrameBufferFv((char*)g_gfApplication + 0xD0);
    fn_8009F8B4(g_cmAIController);
    ((FloatHolder*)((GameGlobal*)g_GameGlobal)->unk44)->value = lbl_27_rodata_3EF8;
    self->flag1B5 = 1;
    if (self->unk90 != 0) {
        self->unk90->v24();
    }
    self->state = 7;
}
}

