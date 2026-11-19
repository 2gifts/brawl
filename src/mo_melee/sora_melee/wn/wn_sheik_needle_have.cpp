#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#include <wn/sheik/wn_sheik_needle_have.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <memory.h>

// Registers the Sheik held needle article class (kind 69) in the class info table during static initialization.
wnClassInfoImpl<69, wnSheikNeedleHave, wnSheikNeedleHaveData> g_wnSheikNeedleHaveClassInfo;

void wnSheikNeedleHave::activate(int founderTaskId, int resourceId, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = pos;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}

// TODO: clean up, give variables real names
namespace jj_342ED4 {
extern "C" {
extern void __dl__FPv(void*);

void* __dt__30wnSheikNeedleHaveParamAccesserFv(void* p, int n) {
    if (p && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_342F14 {
extern "C" void* fn_27_342F14(void* self) {
    return reinterpret_cast<u8*>(self) + 0x4f8;
}
}

// TODO: clean up, give variables real names
namespace jj_343CC0 {
extern "C" void* fn_27_343CC0(void* self) {
    return reinterpret_cast<u8*>(self) + 0xdc;
}
}

// TODO: clean up, give variables real names
namespace jj_343CC8 {
extern "C" void* fn_27_343CC8(void* self) {
    return reinterpret_cast<u8*>(self) + 0xb8;
}
}

