#include <ft/pikmin/ft_pikmin_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// TODO: activeArticle, shootArticle, getPikminVariation.

ftPikminTransactor::ftPikminTransactor() { }
ftPikminTransactor::~ftPikminTransactor() { }

// Unless the owner wears the spycloak, the start glow follows the model, tinted by the costume.
void ftPikminTransactor::init(soModuleAccesser* acc) {
    if (g_ftManager->getOwner(acc->getWorkManageModule().getInt(0x10000000))->getSpycloak() == false) {
        acc->getEffectModule().reqFollow(static_cast<EfID>(0x11c0002), 0x11, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, true, 0x10,
                                         soValueAccesser::getValueInt(acc, 0x10000036, 0), -1);
    }
}

void ftPikminTransactor::exit(soModuleAccesser* acc) {
    acc->getEffectModule().killKind(0x11c0002 + soValueAccesser::getValueInt(acc, 0x10000036, 0), true, true);
}

float ftPikminTransactor::getPullOutMotRate(soModuleAccesser* acc) {
    return getPullOutParam(acc)->m_motRate;
}

// The row of the floor Olimar stands on; Kirby's copy has its own table.
ftPikminTransactor::PullOutParam* ftPikminTransactor::getPullOutParam(soModuleAccesser* acc) {
    PullOutParam* table = NULL;
    int count = 0;
    if (acc->getStageObject().soGetSubKind() == Fighter_Pikmin) {
        u8* data = reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Pikmin));
        table = *reinterpret_cast<PullOutParam**>(data + 0x7c);
        count = *reinterpret_cast<int*>(data + 0x80);
    } else if (acc->getStageObject().soGetSubKind() == Fighter_Kirby) {
        u8* data = reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Kirby));
        table = *reinterpret_cast<PullOutParam**>(data + 0x174);
        count = *reinterpret_cast<int*>(data + 0x178);
    }
    int materialType = 0;
    if (acc->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(8), 0) == true) {
        materialType = acc->getGroundModule().getTouchMaterialType(static_cast<grCollStatus::TouchMask>(8), 0);
    }
    return getPullOutParam(materialType, table, count);
}

ftPikminTransactor::PullOutParam* ftPikminTransactor::getPullOutParam(int materialType, PullOutParam* table, int count) {
    // BYTES: a second cursor for the default row.
    PullOutParam* param = table;
    PullOutParam* defaultParam = NULL;
    for (int i = 0; i < count; i++, param++) {
        if (table[i].m_materialType == materialType) {
            return &table[i];
        }
        if (table[i].m_materialType == 0) {
            defaultParam = param;
        }
    }
    if (defaultParam == NULL) {
        return table;
    }
    return defaultParam;
}
