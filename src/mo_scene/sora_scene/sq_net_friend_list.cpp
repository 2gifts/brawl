#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <sq/sq_net_friend_list.h>
#include <sr/sr_common.h>
#include <types.h>

sqNetFriendList* sqNetFriendList::create() {
    return new (Heaps::GlobalMode) sqNetFriendList("sqNetFriendList");
}

void sqNetFriendList::start() {
    s32 kind = gfSceneManager::getInstance()->unk31C;
    unk14 = kind;
    unk8 = 0;
    unkC = g_gfApplication->unk110[0] != 0;
    switch (kind) {
    case 0x18:
    default:
        unk10 = 0;
        break;
    case 0x19:
        unk10 = 1;
        break;
    case 0x1a:
        unk10 = 2;
        break;
    case 0x1b:
        unk10 = 3;
        break;
    }
}

void sqNetFriendList::exit() { }

// TODO: clean up, give variables real names
namespace jj_378CC {

extern "C" {
extern char lbl_1_data_8C28[];
extern char* g_GameGlobal;
extern char* g_gfApplication;
void OSReport(const char*, ...);
void fn_1_3D450();
void* searchScene__14gfSceneManagerFPCc(gfSceneManager*, const char*);
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
}

struct SceneSeq {
    char pad[8];
    int state;          /* 0x08 */
    unsigned char arg;  /* 0x0C */
    char pad2[3];
    int a10;            /* 0x10 */
    int seq;            /* 0x14 */
};

extern "C" void fn_1_378CC(SceneSeq* self) {
    const char* base = lbl_1_data_8C28;
    int again;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    again = 1;

    while (again) {
        again = 0;
        switch (self->state) {
        default:
            setNextScene__14gfSceneManagerFv(mgr, base + 0x10, self->a10);
            self->state = 1;
            break;
        case 1: {
            int seq = *(int*)((char*)mgr + 0x284);
            switch (seq) {
            default:
            case 3:
            case 6:
                self->state = 2;
                break;
            case 7:
                self->state = 8;
                break;
            case 0:
            case 1:
            case 2:
            case 4:
            case 5: {
                char* s = (char*)searchScene__14gfSceneManagerFPCc(mgr, base + 0x10);
                *(int*)(*(char**)(g_GameGlobal + 0x1c) + 0x24) = *(int*)(s + 0x3A8);
                switch (seq) {
                case 0:
                    OSReport(base + 0x20);
                    self->state = 3;
                    break;
                case 1:
                    OSReport(base + 0x3C);
                    self->state = 4;
                    break;
                case 2:
                    OSReport(base + 0x58);
                    self->state = 5;
                    break;
                case 4:
                    OSReport(base + 0x74);
                    self->state = 6;
                    break;
                case 5:
                    OSReport(base + 0x90);
                    self->state = 7;
                    break;
                }
                break;
            }
            }
            again = 1;
            break;
        }
        case 2: {
            int r0 = self->seq;
            int a;
            switch (r0) {
            default:
            case 0x18: a = 0x17; break;
            case 0x19: a = 0x18; break;
            case 0x1A: a = 0x19; break;
            case 0x1B: a = 0x1A; break;
            }
            setNextSequence__14gfSceneManagerFv(mgr, base + 0xAC, a);
            break;
        }
        case 3:
        case 6: {
            int a = 0;
            switch (self->state) {
            case 3: a = 0; break;
            case 6: a = 1; break;
            }
            setNextSequence__14gfSceneManagerFv(mgr, base + 0xB8, a);
            break;
        }
        case 4:
            setNextSequence__14gfSceneManagerFv(mgr, base + 0xCC, 0);
            break;
        case 5:
        case 7: {
            int a = 0;
            switch (self->state) {
            case 5: a = 0; break;
            case 7: a = 1; break;
            }
            setNextSequence__14gfSceneManagerFv(mgr, base + 0xE0, a);
            break;
        }
        case 8:
            fn_1_3D450();
            setNextSequence__14gfSceneManagerFv(mgr, base + 0xAC, 0x23);
            *(unsigned char*)(g_gfApplication + 0x110) = self->arg;
            break;
        }
    }
}
}

