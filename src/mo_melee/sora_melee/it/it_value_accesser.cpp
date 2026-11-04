#include <it/it_extend_param_accesser.h>
#include <it/it_resource_module_impl.h>
#include <it/it_value_accesser.h>
#include <it/item.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

itValueAccesser::itValueAccesser() : soValueAccesser(4) { }

itValueAccesser::~itValueAccesser() { }

extern itExtendParamAccesser lbl_27_bss_71DC;
extern const float lbl_27_rodata_5FD8[6];
extern const float lbl_27_rodata_5FF0[2];

float itValueAccesser::getConstantFloatCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if ((int)id > 3999) {
        return static_cast<soParamAccesser*>(&lbl_27_bss_71DC)->getParamFloat(moduleAccesser, id, index);
    }
    if ((int)id <= 2999) {
        return soValueAccesser::getConstantFloatCore(moduleAccesser, id, index);
    }
    if ((int)id > 3000 && (int)id < 3017) {
        return static_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule()).unk30->unk0[(int)id - 3001];
    }
    if ((int)id > 3400 && (int)id < 3443) {
        return static_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule()).unk34->unk0[(int)id - 3401];
    }
    if ((int)id > 3500 && (int)id < 3502) {
        BaseItem* item = static_cast<BaseItem*>(moduleAccesser->m_stageObject);
        switch (id) {
        case 3501:
            int parameter = -1;
            if (item->getTrait() & itParam::TRAIT_MASK_RECOVER) {
                switch (item->m_kind) {
                case 0x20:
                    if ((int)item->m_variation == 1) {
                        return lbl_27_rodata_5FF0[0];
                    }
                    parameter = 0x15E0;
                    break;
                case 0x24:
                    parameter = 0x16A8;
                    break;
                case 0x40:
                    parameter = item->m_variation + 0x1C20;
                    break;
                case 0x58:
                    parameter = 0x20D0;
                    break;
                case 0x4A:
                    parameter = 0x1E14;
                    break;
                case 0x4E:
                    parameter = item->m_variation + 0x1EDC;
                    break;
                }
            }
            if (parameter >= 0) {
                return soValueAccesser::getConstantFloat(moduleAccesser, parameter, 0);
            }
            return lbl_27_rodata_5FD8[0];
        }
    }
    return lbl_27_rodata_5FD8[0];
}

void* itValueAccesser::getConstantIndefiniteCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if ((int)id > 43999) {
        return static_cast<soParamAccesser*>(&lbl_27_bss_71DC)->getParamIndefinite(moduleAccesser, id, index);
    }
    if ((int)id <= 42999) {
        return soValueAccesser::getConstantIndefiniteCore(moduleAccesser, id, index);
    }
    itResourceModuleImpl& resource = dynamic_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule());
    __typeof__(resource.unk34) specialized = resource.unk34;
    __typeof__(resource.unk38) animation = resource.unk38;
    switch (id) {
    case Common_Param_Indefinite_Effect_Anim_Cmd:
        return resource.unk30->unk60 ? *resource.unk30->unk60 : 0;
    case Anim_Param_Indefinite_Motion_Data:
        return animation->unk0;
    case Anim_Param_Indefinite_Status_Data:
        return animation->unk30;
    case Anim_Param_Indefinite_Collision_Hit_Data:
        return animation->unk34;
    case Anim_Param_Indefinite_Collision_Shield_Data:
        return animation->unk38;
    case Anim_Param_Indefinite_Ground_Shape_Data:
        return animation->unk8;
    case Anim_Param_Indefinite_Visibility_Data:
        return animation->unk10;
    case Anim_Param_Indefinite_Physics_IK_Data:
        return animation->unk14;
    case Anim_Param_Indefinite_Effect_Node_Data:
        return animation->unk18;
    case Anim_Param_Indefinite_Motion_Anim_Cmd_Game:
        return animation->unk1C;
    case Anim_Param_Indefinite_Motion_Anim_Cmd_Effect:
        return animation->unk20;
    case Anim_Param_Indefinite_Motion_Anim_Cmd_Sound:
        return animation->unk24;
    case Anim_Param_Indefinite_Status_Anim_Cmd:
        return animation->unk28;
    case Anim_Param_Indefinite_Event_Anim_Cmd:
        return animation->unk2C;
    case Sub_Param_Indefinite:
        return specialized->unkEC;
    }
    return 0;
}
