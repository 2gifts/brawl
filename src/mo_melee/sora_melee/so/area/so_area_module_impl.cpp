#include <so/area/so_area_module_impl.h>
#include <so/area/so_area_instance.h>
#include <types.h>

void soAreaModuleImpl::deactivate() {
    setWhole(false);
    int num = m_areaInstanceArray->size();
    for (int i = 0; i < num; i++) {
        if (m_areaInstanceArray->at(i).m_isCreated) {
            m_areaInstanceArray->at(i).deactivate();
        }
    }
}

void soAreaModuleImpl::setWhole(bool isWhole) {
    if (m_isWhole != isWhole) {
        int num = m_areaInstanceArray->size();
        for (int i = 0; i < num; i++) {
            if (m_areaInstanceArray->at(i).m_isCreated) {
                m_areaInstanceArray->at(i).setWhole(isWhole);
            }
        }
        m_isWhole = isWhole;
        m_areaContactLogArray->clear();
        m_enviromentElementChecker->clear();
    }
}

void soAreaModuleImpl::clearLog() {
    m_areaContactLogArray->clear();
}

void soAreaModuleImpl::check() {
    if (m_enviromentElementChecker->isEnable() == true) {
        m_enviromentElementChecker->clear();
        m_enviromentElementChecker->check(m_moduleAccesser);
    }
}

bool soAreaModuleImpl::isEnableArea(int index) {
    if (index < 0 || index >= m_areaInstanceArray->size()) {
        return false;
    }
    return m_areaInstanceArray->at(index).m_isEnable;
}

void soAreaModuleImpl::setAreaTargetGroupMask(u32 index, u32 mask) {
    m_areaInstanceArray->at(index).setCheckTargetGroup(mask);
}

void soAreaModuleImpl::setAreaTargetLocalGroupMask(u32 index, u32 mask) {
    m_areaInstanceArray->at(index).setCheckTargetLocalGroup(mask);
}

void soAreaModuleImpl::setAreaShapeType(u32 index, soAreaInstance::ShapeType shapeType) {
    m_areaInstanceArray->at(index).setShapeType(shapeType);
}

void soAreaModuleImpl::setAreaDbgColor(u32 index, GXColor color) {}

bool soAreaModuleImpl::isExistAreaInstance(int index) {
    if (index < 0 || index >= m_areaInstanceArray->size()) {
        return false;
    }
    return m_areaInstanceArray->at(index).m_area != NULL;
}

int soAreaModuleImpl::getAreaIndexFromId(int id) {
    int num = m_areaInstanceArray->size();
    for (int i = 0; i < num; i++) {
        if (id == m_areaInstanceArray->at(i).getId()) {
            return i;
        }
    }
    return -1;
}

s32 soAreaModuleImpl::getContactLogCount() {
    return m_areaContactLogArray->size();
}

soAreaContactLog* soAreaModuleImpl::getContactLog(s32 index) {
    return &m_areaContactLogArray->at(index);
}

u32 soAreaModuleImpl::getAreaContactCount(int areaId) {
    if (areaId == -1) {
        return m_areaContactLogArray->size();
    }
    u32 count = 0;
    int num = m_areaContactLogArray->size();
    for (int i = 0; i < num; i++) {
        if (areaId == m_areaContactLogArray->at(i).m_area) {
            count++;
        }
    }
    return count;
}

soAreaContactLog* soAreaModuleImpl::getAreaContactLog(int areaId, int index) {
    if (areaId == -1) {
        return &m_areaContactLogArray->at(index);
    }
    int count = 0;
    int num = m_areaContactLogArray->size();
    for (int i = 0; i < num; i++) {
        if (areaId == m_areaContactLogArray->at(i).m_area) {
            if (count == index) {
                return &m_areaContactLogArray->at(i);
            }
            count++;
        }
    }
    return NULL;
}

bool soAreaModuleImpl::isObserv(char unk1) {
    return unk1 == 0x14;
}

void soAreaModuleImpl::setLogOverThrough(bool logOverThrough) {
    m_logOverThrough = logOverThrough;
}

soAreaEnviromentElementChecker* soAreaModuleImpl::getEnviromentElementChecker() {
    return m_enviromentElementChecker;
}

// TODO: clean up, give variables real names
namespace jj_A38D8 {
typedef unsigned char u8;

struct Arr {
    virtual void isNull() const;
    virtual int* at(int i);
    virtual int* atC(int i) const;
    virtual int size() const;
    virtual void v18();
    virtual void v1C();
    virtual void v20();
    virtual void v24();
    virtual void clear();
};

struct Self {
    char pad[0x30];
    Arr* a;
    Arr* b;
    void* unk38;
    Arr* c;
    char pad40[5];
    u8 flag80 : 1;
    u8 flag40 : 1;
    u8 flags : 6;
};

extern "C" void destroy__14soAreaInstanceFv(int*);

extern "C" void clean__16soAreaModuleImplFv(Self* self) {
    int n = self->a->size();
    for (int i = 0; i < n; ++i)
        destroy__14soAreaInstanceFv(self->a->at(i));

    self->a->clear();
    self->b->clear();

    n = self->c->size();
    for (int j = 0; j < n; ++j)
        *self->c->at(j) = -1;

    self->flag40 = 1;
    self->flag80 = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_A3D5C {
struct FtOuter {
    u8 pad[0x1D];
    u8 flags;
};

struct Arr {
    virtual void isNull();
    virtual FtOuter* at(s32 i);
    virtual FtOuter* atC(s32 i) const;
    virtual s32 size() const;
};

struct Notify {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void update();
};

struct Other {
    u8 pad[8];
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void update();
};

class Self {
public:
    virtual void isNull();
    virtual void vA3704();
    virtual void vA3D5C();
    virtual void vA3E88();
    virtual void vA403C();
    virtual void vA40F4();
    virtual void vA42D0();
    virtual void vA38D8();
    virtual void vA39E4();
    virtual void vA3B6C();
    virtual void setWhole__16soAreaModuleImplFb(bool flag);
    virtual void vA4340();
    virtual void vA4374();
    virtual void vA44A4();
    virtual void vA4530();
    virtual void vA4600();
    virtual void vA4644();
    virtual void vA4688();
    virtual void vA46DC();
    virtual void vA4730();
    virtual void vA4774();
    virtual void vA47BC();
    virtual void vA47C0();
    virtual void vA47F4();
    virtual void vA487C();
    virtual void vA48B0();
    virtual void vA4978();
    virtual void vA4A28();
    virtual void vA6918();
    virtual void vA6908();
    virtual void eraseWind__16soAreaModuleImplFi(s32 v);
    u8 pad4[0x2C];
    Arr* m_arr;
    Notify* m_notify;
    Other* m_other;
    u8 pad3C[0x0C];
    s32 m_field48;
};

extern "C" void setOwnerId__14soAreaInstanceFi(void* a, s32 b);
extern "C" void fn_27_A6C80(void* a, s32 b);

extern "C" void activate__16soAreaModuleImplFi(Self* self, s32 arg) {
    s32 count = self->m_arr->size();
    for (s32 i = 0; i < count; i++) {
        if ((self->m_arr->at(i)->flags >> 6) & 1U) {
            setOwnerId__14soAreaInstanceFi(self->m_arr->at(i), arg);
            fn_27_A6C80(self->m_arr->at(i), self->m_field48);
        }
    }
    self->m_notify->update();
    self->m_other->update();
    self->setWhole__16soAreaModuleImplFb(true);
    self->eraseWind__16soAreaModuleImplFi(-1);
}
}

// TODO: clean up, give variables real names
namespace jj_A403C {
extern "C" void fn_27_A6DEC(int, int, int);

class Item {
public:
    u8 pad[0x1D];
    u8 unk1D;
};

class Obj {
public:
    virtual void v_c();
    virtual Item* v3(int index);
    virtual void v_d();
    virtual int v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
};

class Owner {
public:
    u8 pad[0x30];
    Obj* unk30;
    Obj* unk34;
    u8 pad2[0x45 - 0x38];
    u8 unk45;
    u8 pad3[2];
    int unk48;
};

extern "C" void update__16soAreaModuleImplFv(Owner* self) {
    int count = self->unk30->v5();
    for (int i = 0; i < count; i++) {
        Item* item = self->unk30->v3(i);
        if (((u8)item->unk1D >> 6) & 1) {
            int v = ((u8)self->unk45 >> 6) & 1;
            int n = -v | v;
            fn_27_A6DEC((int)item, self->unk48, (int)((unsigned)n >> 31));
        }
    }
    self->unk34->v10();
}
}

// TODO: clean up, give variables real names
namespace jj_A4340 {
class IBase {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

class Holder {
public:
    u8 m_pad[0x30];
    IBase* m_obj;
};

extern "C" void fn_27_A75A4();

extern "C" void getGroup__16soAreaModuleImplFUl(Holder* p) {
    p->m_obj->v1();
    fn_27_A75A4();
}
}

