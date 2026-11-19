#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#define WN_MARIO_FIREBALL_CTOR_OUTLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/luigi/wn_luigi_fireball.h>
#include <wn/wn_class_info_impl.h>

// Registers the Luigi fireball article (kind 149) in the class info table during static initialization.
wnClassInfoImpl<149, wnLuigiFireball, wnMarioFireballData> g_wnLuigiFireballClassInfo;

// TODO: clean up, give variables real names
namespace jj_328BA8 {
extern "C" void __ct__15wnMarioFireballFlRC18wnArticleBuildInfoPC19wnMarioFireballData(void* self);
extern "C" unsigned char lbl_27_data_87A20[];

extern "C" {
void* __ct__15wnLuigiFireballFlRC18wnArticleBuildInfoPC19wnMarioFireballData(void* self) {
    __ct__15wnMarioFireballFlRC18wnArticleBuildInfoPC19wnMarioFireballData(self);
    unsigned char* s = (unsigned char*)self;
    unsigned char* base = lbl_27_data_87A20;
    *(unsigned char**)(s + 0x3C) = base;
    *(unsigned char**)(s + 0x40) = base + 0x64;
    *(unsigned char**)(s + 0x48) = base + 0x70;
    *(unsigned char**)(s + 0x54) = base + 0x84;
    *(unsigned char**)(s + 0x64) = base + 0xDC;
    *(unsigned char**)(s + 0x70) = base + 0xEC;
    *(unsigned char**)(s + 0x7C) = base + 0x100;
    *(unsigned char**)(s + 0x88) = base + 0x118;
    *(unsigned char**)(s + 0x90) = base + 0x124;
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_328C24 {
extern "C" {
void __dt__15wnMarioFireballFv(void*, short);
void __dl__FPv(void*);
}

extern "C" void* __dt__15wnLuigiFireballFv(void* p, int flag) {
    if (p) {
        __dt__15wnMarioFireballFv(p, 0);
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

