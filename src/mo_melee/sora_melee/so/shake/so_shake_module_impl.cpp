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

// TODO: clean up, give variables real names
namespace jj_689B0 {
class soAnimCmdEventObserver;

template <class T>
struct soEventObserver {
    static void __vt(int, int);
};

extern "C" {
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dt__19soShakeModuleSimpleFv(void*, short);
void __dl__FPv(void*);
}

struct IEvSys {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual u32 f6(s32 id);
};

struct IEvMgr {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3(s16 a, s16 b);
};

struct fn27Obj {
    char pad[8];
    void* vtbl;
    s16 m_c;
    s16 m_e;
    s16 m_10;
};

extern "C" fn27Obj* __dt__17soShakeModuleImplFv(fn27Obj* p, s32 flag) {
    if (p != nullptr) {
        if (((char*)p + 8 != nullptr) && ((char*)p + 8 != nullptr)) {
            s16 id = p->m_c;
            p->vtbl = (void*)&soEventObserver<soAnimCmdEventObserver>::__vt;
            IEvSys* inst = (IEvSys*)getInstance__13soEventSystemFv();
            u32 r = inst->f6(id);
            if (r == 1U) {
                IEvMgr* mgr = (IEvMgr*)getManager__13soEventSystemFl(
                    getInstance__13soEventSystemFv(), p->m_c);
                mgr->f3(p->m_e, p->m_10);
            }
            p->m_10 = -1;
            p->m_c = -1;
        }
        __dt__19soShakeModuleSimpleFv(p, 0);
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_693E4 {
extern "C" void notifyEventAnimCmd__17soShakeModuleImplFP9acAnimCmdP16soModuleAccesserl(void*);

extern "C" void fn_27_693E4(void* self) {
    notifyEventAnimCmd__17soShakeModuleImplFP9acAnimCmdP16soModuleAccesserl(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_693EC {
extern "C" void isObserv__17soShakeModuleImplFc(void*);

extern "C" void fn_27_693EC(void* self) {
    isObserv__17soShakeModuleImplFc(reinterpret_cast<u8*>(self) - 0x8);
}
}

