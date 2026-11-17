#define MT_VEC3F_ADD_OPERAND_ORDER_REGS // BYTES: the paired-single add keeps its operands in declaration order.
#include <ft/pikmin/ft_pikmin_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <wn/pikmin/wn_kirby_pikmin.h>
#include <types.h>

// TODO: getPikminVariation (needs its data carve).

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

// Kirby's copy of Olimar's Pikmin (article 0xd) is thrown a little ahead of Kirby, in the direction he faces.
bool ftPikminTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnKirbyPikmin* pikmin = dynamic_cast<wnKirbyPikmin*>(article);
        if (pikmin == NULL) {
            return false;
        }
        float offsetY = soValueAccesser::getConstantFloat(acc, 0x1095, 0);
        Vec3f offset;
        offset.m_x = soValueAccesser::getConstantFloat(acc, 0x1094, 0);
        offset.m_y = offsetY;
        offset.m_z = 0.0f;
        if (acc->getPostureModule().getLr() < 0.0f) {
            offset.m_x = -offset.m_x;
        }
        Vec3f pos = acc->getPostureModule().getPos() + offset;
        pikmin->shoot(&pos, acc->getPostureModule().getLr());
    }
    return false;
}

// Kirby's copy of Olimar's Pikmin (article 0xd) is started a little ahead of Kirby, in the direction he faces.
bool ftPikminTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        PullOutParam* param = getPullOutParam(acc);
        float motRate = param->m_motRate;
        acc->getWorkManageModule().setFloat(motRate, 0x21000004);
        int variation = getPikminVariation(acc);
        float offsetY = soValueAccesser::getConstantFloat(acc, 0x1093, 0);
        Vec3f offset;
        offset.m_x = soValueAccesser::getConstantFloat(acc, 0x1092, 0);
        offset.m_y = offsetY;
        offset.m_z = 0.0f;
        if (acc->getPostureModule().getLr() < 0.0f) {
            offset.m_x = -offset.m_x;
        }
        Vec3fScale(&offset, &offset, acc->getPostureModule().getScale()); // BYTES: the in-place paired-single scale, not Vec3f::operator*=
        wnKirbyPikmin* pikmin = dynamic_cast<wnKirbyPikmin*>(article);
        if (pikmin == NULL) {
            return false;
        }
        Vec3f pos = acc->getPostureModule().getPos() + offset;
        soTeamModule& team = acc->getTeamModule();
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        pikmin->activate(variation, &pos, acc->getStageObject().m_taskId, resources.getKirbyResId(Fighter_Pikmin), team.getTeam()->getNo(), acc->getPostureModule().getLr(), motRate);
        return true;
    }
    return false;
}
