#include <gf/gf_scene.h>
#include <sq/sq_edit.h>
#include <sr/sr_common.h>
#include <types.h>

sqEdit* sqEdit::create() {
    return new (Heaps::GlobalMode) sqEdit("sqEdit");
}

void sqEdit::start() {
    unk8 = 2;
}

void sqEdit::exit() { }

#include <gm/gm_global.h>
// TODO: clean up, give variables real names
namespace jj_36744 {

extern "C" {
void* getInstance__14gfSceneManagerFv();
void setNextScene__14gfSceneManagerFv(void*, const char*, int);
void setNextSequence__14gfSceneManagerFv(void*, const char*, int);
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFii(int, int);
extern const unsigned char lbl_1_data_8980[];
}

struct MenuData {
    unsigned char b0[8];
    unsigned char lo : 6;
    unsigned char hi : 2;
    unsigned char c : 3;
    unsigned char rest : 5;
    unsigned char pad0a[0x16 - 0xa];
    unsigned char b16;
    unsigned char pad17[3];
    unsigned short s1a;
    unsigned char b1c;
    unsigned char pad1d[3];
    int w20;
    unsigned char pad24[0xc];
    int w30;
    int w34;
    int w38;
    int w3c;
    unsigned char pad40[0x98 - 0x40];
    unsigned char kind;
    unsigned char b99;
    unsigned char b9a;
    unsigned char b9b;
    unsigned char b9c;
    unsigned char b9d;
    unsigned char b9e;
    unsigned char b9f;
    unsigned char pada0[3];
    unsigned char ba3;
    unsigned char pad[0x5c - 0xc];
};

struct StageEdit {
    int w0;
    unsigned char pad[0x10];
    unsigned char b14;
};

struct SceneObj {
    void* vtable;
    void* sceneName;
    int state;
};

extern "C" void fn_1_36744(SceneObj* self) {
    const char* base = (const char*)lbl_1_data_8980;
    void* mgr = getInstance__14gfSceneManagerFv();

    switch (self->state) {
    case 2:
        setNextScene__14gfSceneManagerFv(mgr, base + 8, 2);
        self->state = 0;
        break;

    case 0:
        setNextScene__14gfSceneManagerFv(mgr, base + 0x18, 0);
        self->state = 1;
        break;

    case 1: {
        int sceneState = *(int*)((char*)getInstance__14gfSceneManagerFv() + 0x284);
        void* gg = g_GameGlobal;
        switch (sceneState) {
        case 3:
            setNextSequence__14gfSceneManagerFv(mgr, base + 0x28, 0x16);
            self->state = 4;
            return;
        default:
            gmInitGlobalModeMelee__FP17gmGlobalModeMelee(*(void**)((char*)gg + 8));
            setNextScene__14gfSceneManagerFv(mgr, base + 8, 0x0b);
            self->state = 3;
            return;
        }
    }

    case 3: {
        StageEdit* stageEdit = (StageEdit*)g_GameGlobal->m_stageEditData;
        stageEdit->w0 = 0;

        MenuData* menuData = (MenuData*)g_GameGlobal->m_modeMelee;
        menuData->lo = 0;
        menuData->s1a = 0x35;
        menuData->b1c = stageEdit->b14;
        menuData->c = 1;
        menuData->w20 = 0;
        menuData->w34 = 0;
        menuData->w30 = 0;
        menuData->w38 = 0;
        menuData->w3c = 0;
        menuData->b16 = 0;

        MenuData* p = menuData;
        int* setRule = (int*)g_GameGlobal->m_setRule;
        int i = 0;
        do {
            p->kind = 0;
            p->b99 = 3;
            p->ba3 = 1;
            int character = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(0);
            int color = character == 0x28
                ? 0
                : getFighterColorFileNo__6muMenuFii(character, 0);
            p->b9d = (unsigned char)color;
            p->b9e = 0;
            p->b9a = (unsigned char)i;
            ++i;
            p->b9c = 1;
            p->b9f = (unsigned char)(setRule[9] + 1);
            p = (MenuData*)((char*)p + 0x5c);
        } while (i < 4);

        menuData->kind = 0;
        menuData->b99 = 0;
        setNextScene__14gfSceneManagerFv(mgr, base + 0x38, 0);
        self->state = 2;
        break;
    }

    case 4:
        break;
    }
}
}

