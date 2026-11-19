#include <cm/cm_quake.h>
#include <gf/gf_camera.h>
#include <so/camera/so_camera_module_simple.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>

// main.dol cmGetQuakeAmplitude (cm_quake.o), not named in symbols.txt yet.
extern "C" cmQuake::Amplitude fn_8009E23C();

static const cmQuake::Amplitude g_quakeAmplitudes[] = {
    cmQuake::Amplitude_S, cmQuake::Amplitude_L, cmQuake::Amplitude_XL, cmQuake::Amplitude_M, cmQuake::Amplitude_S_Half,
};

static inline s16 getEventManageId(soModuleAccesser* accsr, soEventObserverRegistrationDesc* desc) {
    bool check = desc->checkAnimCmd();
    if (check == true) {
        soEventManager& m = static_cast<soEventManager&>(accsr->getEventManageModule());
        return m.getManageId();
    }
    return -1;
}

soCameraModuleSimple::soCameraModuleSimple(soModuleAccesser* moduleAccesser, soArray<soCameraSubject>* subjects,
                                           soSet<soCameraRange>* rangeSet, soSet<soCameraClipSphere>* clipSet,
                                           soEventObserverRegistrationDesc* regDesc) :
    soAnimCmdEventObserver(5, getEventManageId(moduleAccesser, regDesc)), m_moduleAccesser(moduleAccesser)
{
    m_isWhole = false;
    m_isQuakeMiddle = false;
    m_isQuake = true;
}

soCameraModuleSimple::~soCameraModuleSimple() { }

void soCameraModuleSimple::activate() {
    m_isWhole = true;
    m_isQuakeMiddle = false;
    m_isQuake = true;
}

void soCameraModuleSimple::deactivate() {
    setWhole(false);
}

void soCameraModuleSimple::update() {
    if (m_isWhole && m_isQuakeMiddle == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        cmReqQuake(cmQuake::Amplitude_M, &pos);
    }
}

void soCameraModuleSimple::setWhole(bool isWhole) {
    m_isWhole = isWhole;
    setEnableCamera(isWhole, -1);
}

void soCameraModuleSimple::setEnableCamera(bool, int index) { }

void soCameraModuleSimple::reqQuake(QuakeKind kind, soModuleAccesser* moduleAccesser) {
    if (m_isQuake) {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        reqQuake(kind, &pos);
    }
}

void soCameraModuleSimple::reqQuake(QuakeKind kind, Vec3f* pos) {
    if (!m_isQuake) {
        return;
    }
    if (kind == Quake_M) {
        if (m_isQuakeMiddle == true) {
            return;
        }
        m_isQuakeMiddle = true;
    }
    cmReqQuake(g_quakeAmplitudes[kind], pos);
}

void soCameraModuleSimple::stopQuake(QuakeKind kind) {
    if (kind == Quake_M) {
        if (!m_isQuakeMiddle) {
            return;
        }
        m_isQuakeMiddle = false;
    }
    cmRemoveQuake(g_quakeAmplitudes[kind]);
}

soCameraModule::QuakeKind soCameraModuleSimple::getQuakeKind() {
    cmQuake::Amplitude amplitude = fn_8009E23C();
    if (amplitude == g_quakeAmplitudes[Quake_S]) {
        return Quake_S;
    } else if (amplitude == g_quakeAmplitudes[Quake_L]) {
        return Quake_L;
    } else if (amplitude == g_quakeAmplitudes[Quake_XL]) {
        return Quake_XL;
    } else if (amplitude == g_quakeAmplitudes[Quake_M]) {
        return Quake_M;
    } else if (amplitude == g_quakeAmplitudes[Quake_S_Half]) {
        return Quake_S_Half;
    }
    return Quake_None;
}

Vec3f soCameraModuleSimple::getMainCameraTargetPos() {
    gfCameraManager* manager = gfCameraManager::getManager();
    if (manager == 0) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    Vec3f pos;
    pos = manager->getCamera(0)->m_targetPos;
    return pos;
}

#include <so/so_value_accesser.h>
// TODO: clean up, give variables real names
namespace jj_B49E4 {
extern "C" {
s32 fn_8009E23C(s32 value);
extern const s32 lbl_27_rodata_DE8[5];

s32 fn_27_B49E4(s32 value) {
    s32 v = fn_8009E23C(value);
    for (s32 i = 0; i < 5; i++) {
        if (v == lbl_27_rodata_DE8[i]) {
            return i;
        }
    }
    return -1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_B4AEC {
struct ArgData { int type; unsigned long data; };
struct Arg {
 const ArgData* p; unsigned char null;
 Arg():p(0),null(0) {}
 Arg(const ArgData* q):p(q),null(0) {}
 int type() const { return null==1?-1:p->type; }
 unsigned long value() const { return null==1?0:p->data; }
 bool boolean() const { return null==1?false:p->data!=0; }
};
class List { public:
 virtual void v0(); virtual void v1(); virtual const ArgData* at(int) const;
 char data[12];
 Arg get(int i) const { Arg a(at(i)); return a; }
};
class Cmd { public:
 virtual void v0(); virtual signed char group(); virtual signed char type();
 virtual void v3(); virtual void v4(); virtual List args(); virtual bool getArg(Arg*,int);
};
class Obj { public:
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
 virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
 virtual void v10(); virtual void setBool(bool); virtual void v12(); virtual void v13();
 virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18();
 virtual void v19(); virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
 virtual void v24(); virtual void setA(unsigned long,soModuleAccesser*); virtual void v26();
 virtual void setB(unsigned long); virtual void v28(); virtual void v29(); virtual void v30();
 virtual void v31(); virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
 virtual void v36(); virtual void v37(); virtual void v38(); virtual void v39(); virtual void v40();
 virtual void v41(); virtual void v42(); virtual void v43(); virtual void v44(); virtual void v45();
 virtual void v46(); virtual void v47(); virtual void v48(); virtual void v49(); virtual void v50();
 virtual void v51(); virtual void v52(); virtual void v53(); virtual void v54(); virtual void v55();
 virtual void v56(); virtual void v57(); virtual void v58(); virtual int valid(signed char);
};
extern "C" int fn_27_B4AEC(Obj* self, Cmd* cmd, soModuleAccesser* acc) {
 if (!self->valid(cmd->group())) return 0;
 if (cmd->type()<=-1 || cmd->type()>=0xE) return 0;
 switch(cmd->type()) {
 case 0: { Arg arg; if (!cmd->getArg(&arg,0) || arg.type()!=0) return 1; self->setA(arg.value(),acc); return 1; }
 case 10: { Arg arg; if (!cmd->getArg(&arg,0) || arg.type()!=0) return 1; self->setB(arg.value()); return 1; }
 case 5: {
  unsigned long value; List list=cmd->args(); Arg a=list.get(0);
  if(a.type()==5) { Arg b=list.get(0); value=soValueAccesser::getValueInt(acc,b.value(),0); }
  else { Arg c=list.get(0); if(c.type()==0) { Arg d=list.get(0); value=d.value(); } else return 1; }
  self->setA(value,acc); return 1;
 }
 case 7: { Arg arg; cmd->getArg(&arg,0); if(arg.type()!=3) return 1; self->setBool(arg.boolean()); return 1; }
 }
 return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_B5038 {
extern "C" void fn_27_B4AEC(void*);

extern "C" void fn_27_B5038(void* self) {
    fn_27_B4AEC(reinterpret_cast<u8*>(self) - 0x4);
}
}

