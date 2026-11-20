// The out-of-line soDamage constructor that the soArrayVector<soDamage> units call (so_vtable_ct_vec_8soDamage_1_D2FF0.cpp).
#define SO_ARRAY_ELEMENT_CTOR
#include <so/templates/so_damage.h>

soDamage::soDamage() {
    unk60 = 1.0f;
    unk64 = 1.0f;
    unk68 = 1.0f;
    unk74 = unk74 & ~0x1F; // HYPOTHESIS: five flag bits of the word at 0x74 are cleared
}
