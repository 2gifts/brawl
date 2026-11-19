#include <gf/gf_scene.h>
#include <sq/sq_button.h>
#include <sr/sr_common.h>
#include <types.h>

sqButton* sqButton::create() {
    return new (Heaps::GlobalMode) sqButton("sqButton");
}

void sqButton::start() {
    unk8 = 0;
}

void sqButton::exit() { }

#include <gm/gm_global.h>
// TODO: clean up, give variables real names
namespace jj_3CDAC {
struct Flags {
    unsigned char mode : 6;
    unsigned char unused : 2;
    unsigned char rule : 3;
    unsigned char players : 3;
    unsigned char bit1 : 1;
    unsigned char bit0 : 1;
    unsigned char top : 1;
    unsigned char rest : 7;
};
extern "C" {
void* getInstance__14gfSceneManagerFv();
void* searchScene__14gfSceneManagerFPCc(void*, const char*);
void setNextScene__14gfSceneManagerFv(void*, const char*, int);
void setNextSequence__14gfSceneManagerFv(void*, const char*, int);
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
void* __dynamic_cast(void*, long, const void*, const void*, long);
extern char lbl_1_data_9A80[];
extern char __RTTI__8scButton[];
extern char __RTTI__7gfScene[];
void fn_1_3CDAC(char* self) {
    const char* str = lbl_1_data_9A80;
    unsigned char flag;
    int value;
    gmGlobalModeMelee* p;
    void* mgr = getInstance__14gfSceneManagerFv();
    char* data = (char*)g_GameGlobal->m_setRule;
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(int*)(self + 8)) {
        case 0:
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 12) = 3;
            *(unsigned char*)(self + 16) = 14;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(mgr, str + 12, *(unsigned char*)(self + 16));
            *(int*)(self + 8) = 2;
            break;
        case 2:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 12);
            break;
        case 3: {
            p = g_GameGlobal->m_modeMelee;
            gmInitGlobalModeMelee__FP17gmGlobalModeMelee(p);
            Flags* f = (Flags*)((char*)p + 8);
            f->mode = 31;
            f->rule = 0;
            *(short*)((char*)p + 0x1a) = 0x26;
            f->bit1 = 0;
            p->m_meleeInitData.m_timeLimitFrames = 0;
            f->top = 0;
            f->players = 1;
            ((unsigned char*)p)[0x98] = 0;
            ((unsigned char*)p)[0x99] = 0;
            ((unsigned char*)p)[0x9f] = 1;
            value = *(int*)(data + 0x24);
            flag = *(unsigned char*)(data + 0x28);
            void* scene = searchScene__14gfSceneManagerFPCc(getInstance__14gfSceneManagerFv(), str + 28);
            char* result = (char*)__dynamic_cast(scene, 4, __RTTI__8scButton, __RTTI__7gfScene, 0);
            *(unsigned char*)(result + 0xfd) = flag;
            *(int*)(result + 0x100) = value;
            setNextScene__14gfSceneManagerFv(mgr, str + 28, 0);
            *(int*)(self + 8) = 4;
            break;
        }
        case 4:
            *(int*)(self + 8) = 5;
            cont = 1;
            break;
        case 5:
            setNextSequence__14gfSceneManagerFv(mgr, str + 40, 22);
            *(int*)(self + 8) = 6;
            break;
        }
    }
}
}
}

