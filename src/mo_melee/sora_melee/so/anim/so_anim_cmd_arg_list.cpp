#define SO_ANIM_CMD_ARG_LIST_FLOAT
#include <so/anim/so_anim_cmd_arg_list.h>
#include <so/so_value_accesser.h>
#include <types.h>

int soAnimCmdArgList::getInt(u32 index) {
    const acCmdArg& arg = acGetCmdArg(m_argList.argList, index);
    if (arg.getArgType() == 0) {
        return arg.getIntData();
    }
    if (arg.getArgType() == 5) {
        return soValueAccesser::getValueInt(m_moduleAccesser, arg.getValueIndexData(), 0);
    }
    m_errorOnValueFetch = 1;
    return 0;
}

float soAnimCmdArgList::getFloat(u32 index) {
    const acCmdArg& arg = acGetCmdArg(m_argList.argList, index);
    if (arg.getArgType() == 1) {
        return arg.getFloatData();
    }
    if (arg.getArgType() == 5) {
        return soValueAccesser::getValueFloat(m_moduleAccesser, arg.getValueIndexData(), 0);
    }
    m_errorOnValueFetch = 1;
    return 0.0f;
}

bool soAnimCmdArgList::getBool(u32 index) {
    const acCmdArg& arg = acGetCmdArg(m_argList.argList, index);
    if (arg.getArgType() == 3) {
        return arg.getBoolData();
    }
    m_errorOnValueFetch = 1;
    return false;
}

u32 soAnimCmdArgList::getValueIndex(u32 index) {
    const acCmdArg& arg = acGetCmdArg(m_argList.argList, index);
    if (arg.getArgType() == 5) {
        return arg.getValueIndexData();
    }
    m_errorOnValueFetch = 1;
    return 0;
}
