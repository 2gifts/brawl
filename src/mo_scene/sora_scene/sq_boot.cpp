#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <sq/sq_boot.h>
#include <sr/sr_common.h>
#include <types.h>

sqBoot* sqBoot::create() {
    return new (Heaps::GlobalMode) sqBoot("sqBoot");
}

void sqBoot::start() {
    unk14 = gfSceneManager::getInstance()->unk31C;
    if ((g_gfApplication->m_e4 >> 24) == 1) {
        unk14 = 2;
    }
    unk8 = 0;
}

void sqBoot::exit() { }

#include <gm/gm_global.h>
// TODO: clean up, give variables real names
namespace jj_21F3C {

extern "C" {
extern unsigned char lbl_1_data_6CA0[];
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
unsigned int* getGlobalRecordBootCountDatap__10GameGlobalFv(GameGlobal*);
}

struct BootSequence {
    char pad[8];
    unsigned int state;
    unsigned int next;
    unsigned char param;
    char pad2[3];
    int result;
};

extern "C" void fn_1_21F3C(BootSequence* self) {
    char* base = (char*)lbl_1_data_6CA0;
    gfSceneManager* manager = gfSceneManager::getInstance();
    int again = 1;
    while (again) {
        again = 0;
        switch (self->state) {
        default:
        case 0:
            self->state = 5;
            again = 1;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(manager, base + 8, self->param);
            self->state = 2;
            break;
        case 2:
            again = 1;
            self->state = self->next;
            break;
        case 5:
            setNextScene__14gfSceneManagerFv(manager, base + 24, 0);
            self->state = 6;
            break;
        case 6:
            self->state = 1;
            again = 1;
            self->next = 3;
            self->param = 2;
            break;
        case 3:
            setNextScene__14gfSceneManagerFv(manager, base + 32, 0);
            self->state = 4;
            break;
        case 4:
            if (self->result == 2)
                self->state = 8;
            else
                self->state = 7;
            again = 1;
            break;
        case 8:
            ((unsigned char*)g_GameGlobal->m_appData)[0x28] |= 0x40;
            setNextSequence__14gfSceneManagerFv(manager, base + 40, 42);
            break;
        case 7:
            {
                unsigned int* count = getGlobalRecordBootCountDatap__10GameGlobalFv(g_GameGlobal);
                if (*count < 0x05F5E0FFU)
                    ++*count;
                ((unsigned char*)g_GameGlobal->m_appData)[0x28] |= 0x40;
                setNextSequence__14gfSceneManagerFv(manager, base + 52, 20);
            }
            break;
        }
    }
}
}

