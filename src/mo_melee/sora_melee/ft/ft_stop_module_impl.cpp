#include <so/stop/so_stop_module_impl.h>

// The fighter stop module: hit-stop frames come from the fighter's damage constants.
// TODO: getHitStopFrame(float power, float mul) and setHitStop(float, int) override the base slot that our soStopModule
// view declares without arguments (getHitStopFrame()), so they stay in the module blob until that view is fixed.
class ftStopModuleImpl : public soStopModuleImpl {
public:
    ftStopModuleImpl(soModuleAccesser* moduleAccesser);
    virtual ~ftStopModuleImpl();
};

ftStopModuleImpl::ftStopModuleImpl(soModuleAccesser* moduleAccesser) : soStopModuleImpl(moduleAccesser) { }

ftStopModuleImpl::~ftStopModuleImpl() { }

// TODO: clean up, give variables real names
namespace jj_154124 {
extern "C" void getHitStopFrame__17soDamageUtilActorFP16soModuleAccesserff(void*);

extern "C" void getHitStopFrame__16ftStopModuleImplFff(void* self) {
    getHitStopFrame__17soDamageUtilActorFP16soModuleAccesserff(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x20));
}
}

// TODO: clean up, give variables real names
namespace jj_15412C {
extern "C" const float lbl_27_rodata_1F08[2];

class Object {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual int calculate(float, float);
    virtual void v15();
    virtual void apply(int, int);
};

extern "C" void setHitStop__16ftStopModuleImplFfi(Object* self, int index, float value) {
    self->apply(self->calculate(value, lbl_27_rodata_1F08[0]), index);
}
}

