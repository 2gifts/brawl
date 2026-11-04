#define FT_MODULE_BUILDER
#include <em/term/em_general_term_decide_module_impl.h>
#include <mt/mt_prng.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/transition/so_transition_module_impl.h>
#include <types.h>

void soAnimCmdInterpreter::interpretCmd(soModuleAccesser* moduleAccesser, float timeStep) {
    m_moduleAccesser = moduleAccesser;
    unk4C = 0;
    acCmdInterpreter::interpretCmd(timeStep);
}

void soAnimCmdInterpreter::resetInterpreter(const soAnimCmdAddressPackConv& pack, soModuleAccesser* moduleAccesser, float frame) {
    m_moduleAccesser = moduleAccesser;
    if (unk48 != 0) {
        acCmdInterpreter::resetInterpreter(0.0f, (const acAnimCmdConv*)unk48);
        m_moduleAccesser = moduleAccesser;
        unk4C = 0;
        acCmdInterpreter::interpretCmd(0.0f);
    }
    unk48 = (u32)pack.addr3;
    if (pack.addr1 != nullptr) {
        acCmdInterpreter::resetInterpreter(0.0f, pack.addr1);
        m_moduleAccesser = moduleAccesser;
        unk4C = 0;
        acCmdInterpreter::interpretCmd(0.0f);
    }
    acCmdInterpreter::resetInterpreter(frame, pack.addr2);
}

void soAnimCmdInterpreter::resetInterpreter(const acAnimCmdConv* initCmd, soModuleAccesser* moduleAccesser, u32 flag, float frame) {
    if (flag == 1) {
        if (unk48 != 0) {
            acCmdInterpreter::resetInterpreter(0.0f, (const acAnimCmdConv*)unk48);
            m_moduleAccesser = moduleAccesser;
            unk4C = 0;
            acCmdInterpreter::interpretCmd(0.0f);
        }
        unk48 = 0;
    }
    acCmdInterpreter::resetInterpreter(frame, initCmd);
}

bool soAnimCmdInterpreter::dispatchCmdIf(const acAnimCmd* cmd) {
    if (m_moduleAccesser == nullptr) {
        return false;
    }
    u32 resultWord = 0;
    soGeneralTermDecideModule* decider = static_cast<soGeneralTermDecideModule*>(m_moduleAccesser->m_enumerationStart->m_generalTermDecideModule);
    // HYPOTHESIS: the term is the argument list, as transition commands cast the same table.
    soGeneralTerm term(*(soGeneralTerm*)&const_cast<acAnimCmd*>(cmd)->getArgList());
    return decider->checkEstablish(&term, m_moduleAccesser, &resultWord, 0);
}

bool soAnimCmdInterpreter::isBreakInterpret() {
    if (acCmdInterpreter::isBreakInterpret() == true) {
        return true;
    }
    acCmdInterpreterStackData* top = getLastSystemStackData();
    if (top == nullptr) {
        return false;
    }
    switch (top->kind) {
    case 8:
        if (m_moduleAccesser != nullptr) {
            if (m_moduleAccesser->getMotionModule().isLooped() == 0) {
                return true;
            }
            if (unk4C != 0) {
                return true;
            }
        }
        unk4C = (u32)top->addressData;
        popSystemStack();
        break;
    case 9:
        return true;
    default:
        break;
    }
    return false;
}

bool soAnimCmdInterpreter::isSleepStackData(s32 kind) const {
    if (acCmdInterpreter::isSleepStackData(kind) == true) {
        return true;
    }
    return kind == 8 || kind == 9;
}

void soAnimCmdInterpreter::skipNextMotionGroup(soModuleAccesser* moduleAccesser) {
    unk30Field2 = 1;
    m_moduleAccesser = moduleAccesser;
    acCmdInterpreterStackData* top = getLastSystemStackData();
    if (top != nullptr && top->kind == 9) {
        popSystemStack();
    } else {
        skipCmd();
    }
}

bool soAnimCmdInterpreter::isSkipEnd() const {
    if (acCmdInterpreter::isSkipEnd() == true) {
        return true;
    }
    soAnimCmdInterpreter* self = const_cast<soAnimCmdInterpreter*>(this);
    const acAnimCmd* cmd = self->getCurrentCmd();
    if (cmd->isNull() == true) {
        return true;
    }
    s32 mode = unk30Field2;
    if (mode == 1) {
        if ((cmd->getGroup() == 4 && cmd->getType() == 0) || (cmd->getGroup() == 1 && cmd->getType() == 1)) {
            return true;
        }
    } else if (mode == 2) {
        acCmdInterpreterStackData* top = self->getLastSystemStackData();
        if (top != nullptr) {
            if (top->kind == 9) {
                return true;
            }
            if (top->kind == 8) {
                return true;
            }
        }
    }
    return false;
}

void soAnimCmdInterpreter::systemCmdFuncWaitLoop() {
    if (m_moduleAccesser == nullptr) {
        return;
    }
    const acAnimCmd* cmd = getCurrentCmd();
    if ((u32)cmd->getCmdAddress() != unk4C) {
        if (m_moduleAccesser->getMotionModule().isLooped() == true) {
            return;
        }
        float frame = m_currentFrame;
        if (frame >= m_moduleAccesser->getMotionModule().getEndFrame()) {
            return;
        }
    }
    const acAnimCmdConv* addr = cmd->getCmdAddress();
    float curFrame = m_moduleAccesser->getMotionModule().getFrame();
    acCmdInterpreterStackData data(static_cast<acCmdInterpreterStackData::Kind>(8), 0, true, addr, curFrame);
    pushSystemStack(data);
}

void soAnimCmdInterpreter::systemCmdFuncWaitMotionTransition() {
    acCmdInterpreterStackData data(static_cast<acCmdInterpreterStackData::Kind>(9));
    pushSystemStack(data);
}

void soAnimCmdInterpreter::systemCmdFuncSyncMotionFrame() {
    if (m_moduleAccesser != nullptr) {
        m_currentFrame = m_moduleAccesser->getMotionModule().getFrame();
    }
}

void soAnimCmdInterpreter::systemCmdFuncWaitRnadi() {
    if (m_moduleAccesser == nullptr) {
        return;
    }
    if (m_currentCmd == nullptr) {
        return;
    }
    const acAnimCmd* cmd = getCurrentCmd();
    s32 maximum;
    s32 minimum = 0;
    acCmdArg argument;
    cmd->getArg(&argument, 0);
    if (argument.getArgType() == 0) {
        maximum = argument.getIntData();
    } else {
        maximum = soValueAccesser::getValueInt(m_moduleAccesser, argument.getValueIndexData(), 0);
    }
    if (cmd->getArgNum() >= 2) {
        minimum = maximum;
        cmd->getArg(&argument, 1);
        if (argument.getArgType() == 0) {
            maximum = argument.getIntData();
        } else {
            maximum = soValueAccesser::getValueInt(m_moduleAccesser, argument.getValueIndexData(), 0);
        }
    }
    if (maximum - minimum > 0) {
        minimum += randi(maximum - minimum);
    }
    systemCmdFuncWaitSub(minimum);
}
