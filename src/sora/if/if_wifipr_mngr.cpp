#include <types.h>

class gfSceneRoot;

class IfWifiprMngrUnk38 {
public:
    char unk0[0x1C];
    u8 unk1C;
};

class IfWifiprMngr {
public:
    void main();

    char unk0[0x18];
    s32 unk18;
    s32 unk1C[7];
    IfWifiprMngrUnk38* unk38;
    char unk3C[0x14];
    u8 unk50;
};

extern "C" void fn_800FD448(s32 unk);
extern "C" void fn_800FD8E0(s32 unk);
extern "C" s32 fn_800FBFDC();
extern "C" void fn_8000DB40(gfSceneRoot* root, s32 layer);

extern gfSceneRoot* g_gfSceneRoot;

void IfWifiprMngr::main() {
    fn_800FD448(unk18);
    for (s32 i = 0; i < 7; i++) {
        fn_800FD8E0(unk1C[i]);
    }
    if (unk38->unk1C != 0 && fn_800FBFDC() == 4) {
        unk50 = 1;
    }
    fn_8000DB40(g_gfSceneRoot, 8);
}
