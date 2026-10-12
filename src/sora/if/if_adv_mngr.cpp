#include <types.h>
#include <gf/gf_3d_scene.h>

extern "C" s32 fn_80109C88(s32 state);
extern "C" void fn_801073AC();
extern "C" void fn_800DE360(s32 value);
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);

class IfAdvMngr {
public:
    s32 main();

    char unk0[0x8];
    nw4r::g3d::ScnMdl* unk8;
    char unkC[0x68];
    s32 unk74;
    char unk78[0x90];
    s32 unk108;
    char unk10C[0x5];
    u8 unk111;
    char unk112[0x6];
    u8 unk118;
};

s32 IfAdvMngr::main() {
    s32 result = 0;
    if (unk111 != 0) {
        result = 0;
        s32 state = fn_80109C88(unk74);
        switch (state) {
        case 1:
            result = 1;
            break;
        case 3:
            result = 2;
            break;
        case 2:
            result = 3;
            break;
        case 4:
            result = 4;
            break;
        }
    } else {
        fn_801073AC();
        fn_800DE360(unk108);
    }
    g_gfSceneRoot->remove(unk8);
    if (unk118 != 0) {
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, unk8);
    }
    if (unk118 == 1) {
        fn_8000DB40(g_gfSceneRoot, 8);
    }
    return result;
}
