#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_sp_melee.h>
#include <sr/sr_common.h>
#include <types.h>


static gfScene* s_selStage;

sqSpMelee* sqSpMelee::create() {
    return new (Heaps::GlobalMode) sqSpMelee("sqSpMelee");
}

void sqSpMelee::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xf;
    } else {
        unk8 = 0;
        unk11 = 0;
        s_selStage = gfSceneManager::getInstance()->searchScene("scSelStage");
        g_GameGlobal->initSelCharData();
        g_GameGlobal->m_modeMelee->gmInitVSMelee();
    }
}

void sqSpMelee::exit() { }

// TODO: clean up, give variables real names
namespace jj_23F08 {
typedef unsigned char u8;
typedef unsigned short u16;

struct Block {
    u8 top : 1;
    u8 middle : 3;
    u8 low : 4;
    u8 pad1[5];
    u16 field_a;
    u16 field_b;
    u8 pad2[0x16];
    u8 field_c;
    u8 pad3[3];
    float field_d;
    u8 pad4[4];
    float field_e;
    u8 pad5[0x2C];

    void reset();
};

struct ParentObj {
    u8 pad0[0xD];
    u8 flags;
    u8 pad1[0x42];
    float field;
    u8 pad2[0x60];
    Block blocks[4];
};

struct GameGlobal {
    u8 pad[8];
    ParentObj* obj;
};

extern "C" GameGlobal* g_GameGlobal;
extern "C" const float lbl_1_rodata_9E8;

inline void Block::reset()
{
    field_a = 0;
    top = 0;
    field_b = 0;
    field_d = lbl_1_rodata_9E8;
    field_c = 0;
    low = 0;
    field_e = lbl_1_rodata_9E8;
}

extern "C" void fn_1_23F08()
{
    ParentObj* obj = g_GameGlobal->obj;
    for (int i = 0; i < 4; ++i)
        obj->blocks[i].reset();
    obj->field = lbl_1_rodata_9E8;
    obj->flags &= ~0x18;
}
}

