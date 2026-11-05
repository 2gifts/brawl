#include <so/shake/so_shake_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_value_accesser.h>

void soShakeModuleImpl::activate() {
    stop();
}

void soShakeModuleImpl::req(int kind, int frame, bool flag, const Vec2f* offset) {
    if (isShake() == true) {
        u8 priority = m_terms->at(kind).m_data->unk8;
        if (m_terms->at(m_kind).m_data->unk8 > priority) {
            return;
        }
        soShakeTerm& current = m_terms->at(m_kind);
        current.m_frame = 0;
        current.m_step = 0;
        current.m_isActive = false;
    }
    soShakeTerm& term = m_terms->at(kind);
    term.m_frame = frame;
    term.m_step = 0;
    term.m_flag = flag;
    term.m_offset = *offset;
    term.m_isActive = true;
    m_kind = kind;
}

bool soShakeModuleImpl::isShake() {
    return m_kind != -1;
}

void soShakeModuleImpl::extend(int kind, int frame) {
    if (m_kind == kind) {
        m_terms->at(kind).m_frame = frame;
    }
}

void soShakeModuleImpl::stop() {
    int num = m_terms->size();
    for (int i = 0; i < num; i++) {
        soShakeTerm& term = m_terms->at(i);
        term.m_frame = 0;
        term.m_step = 0;
        term.m_isActive = false;
    }
    m_kind = -1;
}

void soShakeModuleImpl::stopKind(int kind) {
    if (m_kind == kind) {
        soShakeTerm& term = m_terms->at(kind);
        term.m_frame = 0;
        term.m_step = 0;
        term.m_isActive = false;
    }
}

void soShakeModuleImpl::update(bool) {
    if (m_kind == -1) {
        return;
    }
    soShakeTerm& term = m_terms->at(m_kind);
    if (term.m_data->unk9 != 0 && term.m_frame > 0) {
        term.m_step++;
        if (term.m_step >= term.m_data->unk4) {
            term.m_step = 0;
        }
        if (--term.m_frame == 0) {
            term.m_isActive = false;
        }
    }
    if (!m_terms->at(m_kind).m_isActive) {
        m_kind = -1;
    }
}

bool soShakeModuleImpl::isObserv(char group) {
    return group == 0x1D;
}

bool soShakeModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 1) {
        return false;
    }
    switch (acmd->getType()) {
    case 0: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int kind;
        if (acGetCmdArg(args, 0).getArgType() == 0) {
            kind = acGetCmdArg(args, 0).getIntData();
        } else if (acGetCmdArg(args, 0).getArgType() == 5) {
            kind = soValueAccesser::getValueInt(moduleAccesser, acGetCmdArg(args, 0).getValueIndexData(), 0);
        } else {
            return true;
        }
        int frame;
        if (acGetCmdArg(args, 1).getArgType() == 0) {
            frame = acGetCmdArg(args, 1).getIntData();
        } else if (acGetCmdArg(args, 1).getArgType() == 5) {
            frame = soValueAccesser::getValueInt(moduleAccesser, acGetCmdArg(args, 1).getValueIndexData(), 0);
        } else {
            return true;
        }
        soGroundModule& ground = m_moduleAccesser->getGroundModule();
        int touchMask = acGetCmdArg(args, 2).getIntData();
        Vec2f offset(0.0f, 1.0f);
        bool touching = ground.isTouch(touchMask, 0);
        if (touching == true) {
            offset = ground.getTouchNormal(touchMask, 0);
        }
        req(kind, frame, touching, &offset);
        return true;
    }
    }
    return false;
}
