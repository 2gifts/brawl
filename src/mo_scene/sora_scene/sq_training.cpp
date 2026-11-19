#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_training.h>
#include <sr/sr_common.h>
#include <types.h>


sqTraining* sqTraining::create() {
    return new (Heaps::GlobalMode) sqTraining("sqTraining");
}

void sqTraining::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xa;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
    }
}

void sqTraining::exit() { }

// TODO: clean up, give variables real names
namespace jj_35FBC {

struct PInit {
    u8 m_characterKind;       // 0x00
    u8 m_state;               // 0x01
    u8 m_playerId;            // 0x02
    u8 m_playerNo;            // 0x03
    s8 m_stockCount;          // 0x04
    s8 m_colorFileNo;         // 0x05
    s8 m_colorNo;             // 0x06
    s8 m_controllerNo;        // 0x07
    s8 m_startPointIdx;       // 0x08
    char _0x9[0x02];          // 0x09
    s8 m_teamNo;              // 0x0B
    wchar_t m_name[5];        // 0x0c
    u16 m_0x16;               // 0x16
    u8 m_nameIndex;           // 0x18
    char _0x17;               // 0x19
    bool : 1;
    bool : 1;
    bool : 1;
    bool : 1;
    bool : 1;
    bool : 1;
    bool : 1;
    bool m_isNoVoice : 1;     // 0x1a
    bool : 1;
    bool : 1;
    bool : 1;
    bool m_isMetal : 1;
    bool : 1;
    bool m_isSpycloak : 1;
    bool : 1;
    bool : 1;                 // 0x1b
    bool m_isStamina : 1;
    bool : 1;
    bool : 1;
    bool m_isRebirthXlu : 1;
    bool m_isRabbitCap : 1;
    bool m_isFlower : 1;
    bool m_isCurry : 1;
    bool m_isReflector : 1;   // 0x1c
    char _0x1D[1];            // 0x1d
    u8 m_cpuType;             // 0x1e
    u8 m_cpuRank;             // 0x1f
    u16 m_0x20;               // 0x20
    u16 m_startDamage;        // 0x22
    u16 m_hitPointMax;        // 0x24
    u16 m_0x26;               // 0x26
    u16 m_glowAttack;         // 0x28
    u16 m_glowDefense;        // 0x2a
    float m_attackRatio;      // 0x2c
    float m_damageRatio;      // 0x30
    float m_attackReactionMul;// 0x34
    float m_damageReactionMul;// 0x38
    char _0x3C[0x04];         // 0x3c
    float m_scale;            // 0x40
    float m_visibilityScale;  // 0x44
    float m_gravity;          // 0x48
    u8 m_0x4C;                // 0x4c
    s32 m_0x50[3];            // 0x50
};

extern "C" {
extern const char lbl_1_data_8890[];
void setNextScene__14gfSceneManagerFv(gfSceneManager* mgr, const char* name, int arg);
void setNextSequence__14gfSceneManagerFv(gfSceneManager* mgr, const char* name, int arg);
void fn_1_3D2D8();
void fn_1_363F4(char* self, int arg);
s32 gmGetMenuDecisionPad__Fv(GameGlobal* gg);
void gmAddSingleTimeAll__FUlUl(u32 arg0, s32 arg1);
}

extern "C" void fn_1_35FBC(char* self) {
    const char* str = lbl_1_data_8890;
    char* p;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(u32*)(self + 8)) {
        default:
            *(int*)(self + 8) = 0;
            cont = 1;
            break;
        case 0:
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 3;
            *(u8*)(self + 0x10) = 0xd;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(mgr, str + 0xc, *(u8*)(self + 0x10));
            *(int*)(self + 8) = 2;
            break;
        case 2:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 0xc);
            break;
        case 3: {
            fn_1_3D2D8();
            setNextScene__14gfSceneManagerFv(mgr, str + 0x1c, 0);
            *(u8*)((char*)g_GameGlobal->m_selCharData + 8) = 0;
            *(u8*)((char*)g_GameGlobal->m_selCharData + 4) = 0xa;
            p = (char*)g_GameGlobal->m_selCharData;
            *p = gmGetMenuDecisionPad__Fv(g_GameGlobal) + 1;
            *(int*)(self + 8) = 4;
            break;
        }
        case 4:
            switch (mgr->unk1) {
            case 0:
                *(int*)(self + 8) = 0xb;
                break;
            case 1:
                *(int*)(self + 8) = 5;
                break;
            }
            cont = 1;
            break;
        case 5:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x30, 0);
            *(u8*)((char*)g_GameGlobal->m_selCharData + 4) = 0xa;
            *(int*)(self + 8) = 6;
            break;
        case 6: {
            int v = mgr->unk1;
            switch (v) {
            case 0:
                *(int*)(self + 8) = 3;
                break;
            case 1:
            case 3: {
                int a = 0;
                if (v == 3) {
                    a = 0x35;
                }
                *(int*)(self + 8) = 1;
                *(int*)(self + 0xc) = 7;
                *(u8*)(self + 0x10) = 0;
                fn_1_363F4(self, a);
                break;
            }
            }
            cont = 1;
            break;
        }
        case 7:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x40, 0);
            *(int*)(self + 8) = 8;
            break;
        case 8: {
            gmAddSingleTimeAll__FUlUl(*(u32*)((char*)g_GameGlobal->m_resultInfo + 0x1384) / 60,
                        (*(u8*)((char*)g_GameGlobal->m_modeMelee + 0xf5) == 1) ? 1 : 2);
            cont = 1;
            *(int*)(self + 8) = 9;
            break;
        }
        case 9: {
            PInit* dst = *(PInit**)((char*)g_GameGlobal + 0xc);
            *dst = *(PInit*)((char*)g_GameGlobal->m_modeMelee + 0x98);
            *(s32*)((char*)g_GameGlobal->m_setRule + 0x24) = (s32)(*(u8*)((char*)g_GameGlobal->m_selCharData + 0xbf) - 1);
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x48, 0x15);
            *(int*)(self + 8) = 0xa;
            break;
        }
        case 10:
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 3;
            *(u8*)(self + 0x10) = 0xd;
            break;
        case 11:
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x58, 0xc);
            *(int*)(self + 8) = 0xc;
            break;
        }
    }
}
}

