#include <memory.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_shutter.h>

grAdventureShutter* grAdventureShutter::create(int mdlIndex, const char* taskName) {
    grAdventureShutter* ground = new (Heaps::StageInstance) grAdventureShutter(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureShutter::~grAdventureShutter() { }

#include <gr/gr_gimmick.h>
// TODO: clean up, give variables real names
namespace jj_317A8 {

extern "C" const unsigned int lbl_40_rodata_5D0[];
extern "C" const char lbl_40_data_E3A8[];

extern "C" void fn_40_317A8(grGimmick* self, void* archive) {
    unsigned int pathInfo[9];
    const unsigned int* defaults = lbl_40_rodata_5D0;

    pathInfo[0] = *defaults++;
    pathInfo[1] = *defaults++;
    pathInfo[2] = *defaults++;
    pathInfo[3] = *defaults++;
    pathInfo[4] = *defaults++;
    pathInfo[5] = *defaults++;
    pathInfo[6] = *defaults++;
    pathInfo[7] = *defaults++;
    pathInfo[8] = *defaults++;

    pathInfo[0] = (unsigned int)archive;
    pathInfo[1] = *(unsigned int*)((char*)self + 0x154);

    self->createAttachMotionPath(
        (grGimmickMotionPathInfo*)pathInfo,
        (stTriggerData*)((char*)*(void**)((char*)self + 0x154) + 0x1E4),
        lbl_40_data_E3A8
    );
}
}

// TODO: clean up, give variables real names
namespace jj_3183C {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef int s32;
typedef float f32;

struct MP { char pad0[0x14C]; u8 unk14C; char pad1[0xB]; f32 unk158; };
struct GG { char pad0[0xCC]; MP* unkCC; char padD0[0x84]; char* unk154; f32 unk158; u8 unk15C; };

extern "C" {
f32 getFrame__19grGimmickMotionPathFv(MP* self);
void stopGimmickSE__9grGimmickFUl(GG* self, u32 n);
void startGimmickSE__9grGimmickFUl(GG* self, u32 n);
void update__9grGimmickFf(GG* self, f32 f);
void updateCallback__9grGimmickFUl(GG* self, u32 n);
}
extern const f32 lbl_40_rodata_5C8;

extern "C" void fn_40_3183C(GG* arg0, f32 delta) {
    f32 temp_f31 = arg0->unkCC->unk158;
    MP* temp_r5 = arg0->unkCC;
    u8 temp_r29 = temp_r5->unk14C;
    u32 i;
    s32 j;
    s32 off;
    if (temp_f31 > lbl_40_rodata_5C8) {
        if (getFrame__19grGimmickMotionPathFv(temp_r5) <= temp_f31) {
            i = 0;
            do {
                stopGimmickSE__9grGimmickFUl(arg0, i);
                i++;
            } while (i < 5);
            if (*(u16*)(arg0->unk154 + 0x1A0) != 0) {
                stopGimmickSE__9grGimmickFUl(arg0, 5);
            }
            if (*(u16*)(arg0->unk154 + 0x1AC) != 0) {
                stopGimmickSE__9grGimmickFUl(arg0, 6);
            }
            j = 0;
            do {
                startGimmickSE__9grGimmickFUl(arg0, (u32)j);
                j++;
            } while (j < 5);
        }
    }
    if (arg0->unkCC->unk158 < lbl_40_rodata_5C8) {
        if (arg0->unk158 >= lbl_40_rodata_5C8) {
            stopGimmickSE__9grGimmickFUl(arg0, 5);
            stopGimmickSE__9grGimmickFUl(arg0, 6);
            i = 0;
            off = 0;
            do {
                if (*(u16*)(arg0->unk154 + off + 0x128) != 0) {
                    stopGimmickSE__9grGimmickFUl(arg0, i);
                }
                i++;
                off += 0xC;
            } while (i < 5);
            startGimmickSE__9grGimmickFUl(arg0, 5);
            startGimmickSE__9grGimmickFUl(arg0, 6);
        }
    }
    if (arg0->unk15C == 1 && temp_r29 == 0) {
        if (*(u16*)(arg0->unk154 + 0x1A0) != 0) {
            stopGimmickSE__9grGimmickFUl(arg0, 5);
        }
        if (*(u16*)(arg0->unk154 + 0x1AC) != 0) {
            stopGimmickSE__9grGimmickFUl(arg0, 6);
        }
        startGimmickSE__9grGimmickFUl(arg0, 7);
    }
    MP* temp_r4 = arg0->unkCC;
    arg0->unk15C = temp_r4->unk14C;
    arg0->unk158 = temp_r4->unk158;
    update__9grGimmickFf(arg0, delta);
    updateCallback__9grGimmickFUl(arg0, 0);
}
}

