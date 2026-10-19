#define MT_VEC2F_PLAIN_BASE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/collision/so_collision_manager.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/pikmin/wn_pikmin_chain.h>
#include <wn/pikmin/wn_pikmin_dolfin.h>
#include <wn/pikmin/wn_pikmin_pikmin.h>

class ftPikminTransactor {
public:
    static float getPullOutMotRate(soModuleAccesser* moduleAccesser);
    static int getPikminVariation(soModuleAccesser* moduleAccesser);
};

// How Olimar moves, as the Pikmin of the line have to know it: the status decides, the rest is told by the situation and the
// speed. 9 means "hidden around Olimar", 4 "in the air", 1 "moving", 0 "standing".
// HYPOTHESIS: all numbers and what they mean.
int ftPikminUtility::getPikminOwnerCondition(soModuleAccesser* moduleAccesser, float speed) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    switch (statusKind) {
    case 0x1:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0xa:
    case 0x1f:
    case 0x20:
    case 0x26:
    case 0x52:
    case 0x8c:
    case 0x90:
    case 0x91:
    case 0xc5:
        return 1;
    case 0x27:
    case 0x29:
    case 0xa1:
        return 2;
    case 0x7:
    case 0x8:
    case 0x2c:
    case 0x2f:
    case 0x32:
        return 3;
    case 0xb:
    case 0xc:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x33:
    case 0x79:
    case 0x7a:
    case 0xac:
    case 0xc3:
    case 0xc4:
        return 4;
    case 0xb6:
        return 5;
    case 0xb3:
    case 0xb4:
    case 0xbb:
    case 0xbc:
        return 6;
    case 0xb5:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
        return 7;
    case 0x45:
    case 0x49:
        return 8;
    case 0xdc:
    case 0xdd:
    case 0xde:
    case 0xed:
    case 0xef:
    case 0xf7:
    case 0xf8:
    case 0xf9:
    case 0x100:
    case 0x101:
    case 0x116:
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11a:
        return 10;
    }
    if (isHideOrderOwnerCondition(statusKind) == true) {
        return 9;
    }
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        return 4;
    }
    if (fabs(speed) > 0.01f) {
        return 1;
    }
    return 0;
}

// The statuses that keep the Pikmin hidden around Olimar.
// HYPOTHESIS: the status numbers.
bool ftPikminUtility::isHideOrderOwnerCondition(int statusKind) {
    switch (statusKind) {
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x80:
    case 0x81:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0xc0:
    case 0xc2:
        return true;
    }
    return false;
}
