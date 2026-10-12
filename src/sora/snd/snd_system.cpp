#include <types.h>
#include <snd/snd_system.h>

extern "C" void fn_80076A6C(sndSystem* p1, s32 p2);

// HYPOTHESIS: object at sndSystem+0x6E0 is a BGM player; slot 0x1C is a two-argument setter
class sndBgmTarget {
public:
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C(s32 p1, s32 p2);
};

void sndSystem::specialResume() {
    sndBgmTarget* target;

    if (*(s32*)&_2D4[0x450] != 1) {
        target = *(sndBgmTarget**)&_2D4[0x40C];
        if (target != NULL) {
            target->unk1C(0, 0);
        }
    }
    if ((u8)_2D4[0x44D] != 0) {
        fn_80076A6C(this, 0);
        _2D4[0x44D] = 0;
    }
}
