#include <so/collision/so_collision_attack_module_impl.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <cl/cl_collision.h>
#include <so/model/so_model_module_simple.h>

soCollisionAttackModule::~soCollisionAttackModule() { }

void soCollisionAttackModuleImpl::begin() {
    int num = m_collisionGroupArray->size();
    for (int i = 0; i < num; i++) {
        m_collision.initAbsolute(i);
    }
    unk9c_2 = true;
    unk9c_4 = false;
}

void soCollisionAttackModuleImpl::clearAll() {
    for (int i = 0; i < m_collisionAttackPart->size(); i++) {
        clear(i);
    }
}

void soCollisionAttackModuleImpl::clear(u32 index) {
    if (m_collisionAttackPart->at(index).m_status != soCollisionAttackPart::Status_Inactive) {
        soCollisionAttackPart& part = m_collisionAttackPart->at(index);
        part.m_status = soCollisionAttackPart::Status_Inactive;
        u32 groupIndex = m_collisionAttackPart->at(index).m_groupIndex;
        if (m_collision.isRefLogGroup(groupIndex) == true) {
            m_collision.unUseLogGroup(groupIndex);
        }
    }
}

void soCollisionAttackModuleImpl::sleep(bool isSleep) {
    m_isSleep = isSleep;
    if (!m_isSleep) {
        for (int i = 0; i < m_collisionAttackPart->size(); i++) {
            if (m_collisionAttackPart->at(i).m_status != soCollisionAttackPart::Status_Inactive) {
                m_collisionAttackPart->at(i).resetStatus();
            }
        }
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
}

bool soCollisionAttackModuleImpl::isInvalidInvincible(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return true;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_isInvalidInvincible;
}

bool soCollisionAttackModuleImpl::isInvalidXlu(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return true;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_isInvalidXlu;
}

void soCollisionAttackModuleImpl::setPosX(u32 index, float posX) {
    m_collision.setPosX(posX, m_collisionAttackPart->at(index).m_groupIndex);
}

float soCollisionAttackModuleImpl::getPosX(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_pos.m_x;
    }
    return m_collision.getPosX(m_collisionAttackPart->at(index).m_groupIndex);
}

void soCollisionAttackModuleImpl::setPower(u32 index, int power, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_power = power;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_power = power;
    }
}

void soCollisionAttackModuleImpl::setVector(u32 index, int vector) {
    m_collisionAttackPart->at(index).m_attackData.m_vector = vector;
}

float soCollisionAttackModuleImpl::getSize(u32 index) {
    return m_collisionAttackPart->at(index).m_attackData.m_size;
}

void soCollisionAttackModuleImpl::setSize(u32 index, float size) {
    m_collisionAttackPart->at(index).m_attackData.m_size = size;
}

void soCollisionAttackModuleImpl::setTargetCategory(u32 index, soCollision::CategoryMask categoryMask) {
    m_collisionAttackPart->at(index).setTargetCategory(categoryMask);
}

float soCollisionAttackModuleImpl::getPower(u32 index, bool isAbsolute) {
    float power = getBasePower(index, isAbsolute);
    if (power < 0.0f) {
        power = 0.0f;
    }
    return power;
}

int soCollisionAttackModuleImpl::getReactionEffect(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionEffect;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_reactionEffect;
}

void soCollisionAttackModuleImpl::setReactionEffect(u32 index, int reactionEffect, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionEffect = reactionEffect;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_reactionEffect = reactionEffect;
    }
}

int soCollisionAttackModuleImpl::getReactionFix(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionFix;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_reactionFix;
}

void soCollisionAttackModuleImpl::setReactionFix(u32 index, int reactionFix, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionFix = reactionFix;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_reactionFix = reactionFix;
    }
}

int soCollisionAttackModuleImpl::getReactionAdd(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionAdd;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_reactionAdd;
}

void soCollisionAttackModuleImpl::setReactionAdd(u32 index, int reactionAdd, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionAdd = reactionAdd;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_reactionAdd = reactionAdd;
    }
}

soCollisionAttackData* soCollisionAttackModuleImpl::getData(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return &m_collisionAttackAbsoluteArray->at(index).m_attackData;
    }
    return &m_collisionAttackPart->at(index).m_attackData;
}

void soCollisionAttackModuleImpl::setNode(u32 index, int node) {
    m_collisionAttackPart->at(index).m_attackData.m_nodeIndex = node;
}

void soCollisionAttackModuleImpl::refLogGroup(u32 groupIndex, u32 logGroupIndex) {
    m_collision.refLogGroup(groupIndex, logGroupIndex);
}

void soCollisionAttackModuleImpl::unRefLogGroup(u32 groupIndex) {
    m_collision.unRefLogGroup(groupIndex);
}

int soCollisionAttackModuleImpl::getIndirectTeamNo() {
    return m_indirectTeam->getIndirectNo();
}

void soCollisionAttackModuleImpl::setIndirectInfo(int indirectTaskId, int teamNo) {
    m_indirectTaskId = indirectTaskId;
    m_indirectTeam->setIndirectNo(teamNo);
}

void soCollisionAttackModuleImpl::invalidLog(soCollisionLog* collisionLog, u32 groupIndex) {
    m_collision.invalidLog(collisionLog, groupIndex);
}

float soCollisionAttackModuleImpl::getPosX() {
    return m_pos.m_x;
}

bool soCollisionAttackModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

void soCollisionAttackModuleImpl::setPowerMul(float powerMul) {
    m_powerMul = powerMul;
}

void soCollisionAttackModuleImpl::renderDebug() { }

void soCollisionAttackModuleImpl::setRestrict(bool isRestrict) {
    m_isRestrict = isRestrict;
}

soCollision* soCollisionAttackModuleImpl::getCollision() {
    return &m_collision;
}

int soCollisionAttackModuleImpl::getIndirectTaskId() {
    return m_indirectTaskId;
}

int soCollisionAttackModuleImpl::isInflictStatus() {
    return m_isInflictStatus;
}

int soCollisionAttackModuleImpl::isInflict() {
    return m_isInflict;
}

u32 soCollisionAttackModuleImpl::getPartSize() {
    return m_collisionAttackPart->size();
}

float soCollisionAttackModuleImpl::getLr() {
    return m_lr;
}

void soCollisionAttackModuleImpl::setReactionMul(float reactionMul) {
    m_reactionMul = reactionMul;
}

float soCollisionAttackModuleImpl::getPowerMul2nd() {
    return m_powerMul2nd;
}

void soCollisionAttackModuleImpl::setPowerMul2nd(float powerMul2nd) {
    m_powerMul2nd = powerMul2nd;
}

float soCollisionAttackModuleImpl::getPowerMul() {
    return m_powerMul;
}

void soCollisionAttackModuleImpl::setPowerSpeedStatus(float powerSpeedStatus) {
    m_powerSpeedStatus = powerSpeedStatus;
}

float soCollisionAttackModuleImpl::getPowerSpeedStatus() {
    return m_powerSpeedStatus;
}

void soCollisionAttackModuleImpl::setPowerAddStatus(float powerAddStatus) {
    m_powerAdd = powerAddStatus;
}

float soCollisionAttackModuleImpl::getPowerMulStatus() {
    return m_powerMulStatus;
}

void soCollisionAttackModuleImpl::setPowerMulStatus(float powerMulStatus) {
    m_powerMulStatus = powerMulStatus;
}

Vec2f soCollisionAttackModuleImpl::getSpeed() {
    return m_speed;
}

float soCollisionAttackModuleImpl::getSpeedX() {
    return m_speed.m_x;
}

void soCollisionAttackModuleImpl::deactivate() {
    if (!unk9c_5) {
        m_collision.clean();
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
    m_collision.m_taskId = -1;
}

bool soCollisionAttackModuleImpl::isAttack(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_isAttack;
    } else {
        bool isAttack = false;
        if (m_collisionAttackPart->at(index).m_status != soCollisionAttackPart::Status_Inactive && !m_isSleep) {
            isAttack = true;
        }
        return isAttack;
    }
}

bool soCollisionAttackModuleImpl::setOffset(u32 index, Vec3f* offsetPos) {
    if (m_collisionAttackPart->at(index).m_status == soCollisionAttackPart::Status_Inactive) {
        return false;
    }
    m_collisionAttackPart->at(index).m_attackData.m_offsetPos = *offsetPos;
    return true;
}

void soCollisionAttackModuleImpl::setSpeed(Vec2f* speed) {
    float speedY;
    float speedX = speed->m_x;
    speedY = speed->m_y;
    m_speed.m_x = speedX;
    m_speed.m_y = speedY;
    unk9c_2 = false;
}


void soCollisionAttackModuleImpl::setWhole(u32 index, bool isWhole) {
    m_collision.getCollision(index, m_collisionAttackPart->at(index).m_groupIndex)->m_isInfinite = isWhole;
}


void soCollisionAttackModuleImpl::addPastLog(void* module, u32 index, u32 groupIndex) {
    soCollision* collision = static_cast<soCollisionAttackModule*>(module)->getCollision();
    if (collision != 0) {
        m_collision.addPastLog(collision, index, groupIndex);
    }
}

void soCollisionAttackModuleImpl::hitAbsolute(u32 index, const char* nodeName, int targetTaskId, int targetGroup, int targetNo) {
    int nodeId = m_moduleAccesser->getModelModule().getNodeId(nodeName);
    hitAbsolute(index, nodeId, targetTaskId, targetGroup, targetNo);
}

void soCollisionAttackModuleImpl::hitAbsolute(u32 index, int nodeId, int targetTaskId, int targetGroup, int targetNo) {
    int correctNodeId = m_moduleAccesser->getModelModule().getCorrectNodeId(nodeId);
    Vec3f nodeGlobalPos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(correctNodeId, false);
    hitAbsolute(index, &nodeGlobalPos, targetTaskId, targetGroup, targetNo);
}

void soCollisionAttackModuleImpl::extractAnimCmd(acAnimCmd* command, soCollisionAttackData* attackData, u32* index, u32* groupIndex, soModuleAccesser* moduleAccesser) {
    soArrayContractibleTable<const acCmdArgConv> args = command->getArgList();
    extractAnimCmdArg(soGeneralFlag<bool>(false), moduleAccesser, &args, index, groupIndex, attackData);
    extractAnimCmdArg(soGeneralFlag<bool>(false), moduleAccesser, &args, attackData);
}

bool soCollisionAttackModuleImpl::setOffsetFromNode(u32 index, int node) {
    if (m_collisionAttackPart->at(index).m_status == soCollisionAttackPart::Status_Inactive) {
        return false;
    }
    soCollisionAttackData attackData = m_collisionAttackPart->at(index).m_attackData;
    Matrix nodeMatrix(true);
    m_moduleAccesser->getModelModule().getNodeLocalMtxFromNode(attackData.m_nodeIndex, node, &nodeMatrix, false);
    float offsetZ = nodeMatrix.m[2][3];
    float offsetY = nodeMatrix.m[1][3];
    float offsetX = nodeMatrix.m[0][3];
    Vec3f offset(offsetX, offsetY, offsetZ);
    return setOffset(index, &offset);
}

void soCollisionAttackModuleImpl::set(u32 index, u32 groupIndex) {
    if (m_collisionAttackPart->at(index).m_status != soCollisionAttackPart::Status_Inactive &&
        (int)groupIndex == m_collisionAttackPart->at(index).m_groupIndex) {
        return;
    }
    m_collisionAttackPart->at(index).resetStatus();
    m_collisionAttackPart->at(index).m_groupIndex = groupIndex;
    m_collision.initLog(groupIndex);
    int i;
    int currentGroup = m_collisionAttackPart->at(index).m_groupIndex;
    if (m_collision.isRefLogGroup(currentGroup) != true || m_collision.isUseLogGroup(currentGroup) != true) {
        for (i = 0; i < m_collisionAttackPart->size(); ++i) {
            if (m_collisionAttackPart->at(i).m_status != soCollisionAttackPart::Status_Inactive &&
                i != (int)index && currentGroup == m_collisionAttackPart->at(i).m_groupIndex) {
                goto groupReady;
            }
        }
        m_collision.initPastLog(currentGroup, true);
    }
groupReady:
    if (m_collision.isRefLogGroup(groupIndex) == true) {
        m_collision.useLogGroup(groupIndex);
    }
}

// TODO: clean up, give variables real names
namespace jj_39928 {
extern "C" void* fn_27_39928(void* p, s32 n) {
    if (p != nullptr && n > 0) {
        operator delete(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_39EF4 {
struct Data { int abcd[4]; float e; int fghi[4]; float j, k, l; unsigned int tail[4]; };
struct Elem { int active; Data data; };
class Object_vptr_0 { public: virtual void slot0(); virtual Elem* slot1(int); virtual void slot2(); virtual void slot3(); virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8(); };
class Object_extra { public: virtual void slot0(); virtual Elem* slot1(int, void*); };
struct Inner { char pad[4]; void* value; };
struct Context { char pad[0xd8]; Inner* inner; };
class soCollisionAttackModuleImpl {
public:
 virtual ~soCollisionAttackModuleImpl(); virtual void fn_27_39AA4(); virtual void deactivate__27soCollisionAttackModuleImplFv(); virtual void begin__27soCollisionAttackModuleImplFv(); virtual void clearAll__27soCollisionAttackModuleImplFv(); virtual void clear__27soCollisionAttackModuleImplFUl(); virtual void sleep__27soCollisionAttackModuleImplFb(); virtual void fn_27_39EF4(); virtual void set__27soCollisionAttackModuleImplFUlUl(int, int);
 char pad04[0x24]; Context* context; char pad2c[4]; Object_vptr_0* array; char pad34[0x2c]; float value; char pad64[4]; char work[0x13]; unsigned char flag; char pad7c[0x20]; unsigned int flags;
};
extern "C" unsigned int allocateCollision__11soCollisionFUlbUl(void*, int, unsigned int, int);
extern "C" void* getCollision__11soCollisionFUlUl(void*, int, int);
extern "C" void resetStatus__21soCollisionAttackPartFv(Elem*);
extern "C" void set__21soCollisionAttackPartFP12newCollision(Elem*, void*);
extern "C" void fn_27_38C08(Elem*, void*, void*, float);
extern "C" void fn_27_39EF4(soCollisionAttackModuleImpl* self, int index, int arg, Data* data) {
 void* result;
 if (allocateCollision__11soCollisionFUlbUl(self->work, index, data->tail[3] >> 31, arg) == 1) {
  if (self->array->slot1(index)->active != 0) resetStatus__21soCollisionAttackPartFv(self->array->slot1(index));
 }
 result = getCollision__11soCollisionFUlUl(self->work, index, arg);
 Elem* dst = self->array->slot1(index);
 int a; int b = (a = data->abcd[0], data->abcd[1]);
 dst->data.abcd[0] = a; a = data->abcd[2]; dst->data.abcd[1] = b;
 b = data->abcd[3]; dst->data.abcd[2] = a;
 float f = data->e; dst->data.abcd[3] = b;
 b = data->fghi[0]; dst->data.e = f;
 a = data->fghi[1]; dst->data.fghi[0] = b;
 b = data->fghi[2]; dst->data.fghi[1] = a;
 a = data->fghi[3]; dst->data.fghi[2] = b;
 f = data->j; dst->data.fghi[3] = a;
 float g = data->k; dst->data.j = f;
 f = data->l; dst->data.k = g;
 b = data->tail[0]; dst->data.l = f;
 a = data->tail[1]; dst->data.tail[0] = b;
 b = data->tail[2]; dst->data.tail[1] = a;
 a = data->tail[3]; dst->data.tail[2] = b; dst->data.tail[3] = a;
 self->set__27soCollisionAttackModuleImplFUlUl(index, arg);
 set__21soCollisionAttackPartFP12newCollision(self->array->slot1(index), result);
 self->flag = (data->tail[2] >> 8) & 1;
 if (((self->flags >> 27) & 1) == 1) {
  Inner* inner = self->context->inner; void* value = inner->value;
  Elem* e2 = ((Object_extra*)self->array)->slot1(index, inner);
  fn_27_38C08(e2, value, result, self->value);
 }
}
}

// TODO: clean up, give variables real names
namespace jj_3A960 {
struct Result {
    char padding[0x88];
    unsigned int value;
};

struct Interface {
    virtual ~Interface();
    virtual Result* get() const = 0;
};

struct Object {
    char padding[0x30];
    Interface* interface;
};

extern "C" unsigned int fn_27_3A960(const Object* object) {
    return object->interface->get()->value;
}
}

// TODO: clean up, give variables real names
namespace jj_3ABC8 {
extern const float lbl_27_rodata_498;

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual float slot13();
};

struct Modules {
    char padding[0xC4];
    Object_vptr_0* module;
};

struct Owner {
    char padding[0xD8];
    Modules* modules;
};

struct Object {
    char padding[0x28];
    Owner* owner;
    char padding2[0x30];
    float factor;
};

extern "C" float fn_27_3ABC8(const Object* self) {
    float scale = lbl_27_rodata_498;
    Object_vptr_0* module = self->owner->modules->module;
    scale *= module->slot13();
    return scale * self->factor;
}
}

// TODO: clean up, give variables real names
namespace jj_3AC30 {
struct Vec3 {
    float x, y, z;

    Vec3(const Vec3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }
};

struct ResultA {
    unsigned char pad[0x44];
    Vec3 value;
};

struct ResultB {
    unsigned char pad[0x88];
    unsigned int val;
};

struct IfaceA {
    virtual void vf0() = 0;
    virtual ResultA* vf1(unsigned int) = 0;
};

struct IfaceB {
    virtual void vf0() = 0;
    virtual ResultB* vf1(unsigned int) = 0;
};

struct ObjC {
    unsigned char pad[0x30];

    virtual void vf0() = 0;
    virtual void vf1() = 0;
    virtual void vf2() = 0;
    virtual void vf3() = 0;
    virtual Vec3 get() = 0;
};

struct Work {
    unsigned char pad[0x30];
    IfaceB* field30;
    IfaceA* field34;
    unsigned char pad2[0x30];
    unsigned int field68;
};

extern "C" ObjC* getCollision__11soCollisionFUlUl(void*, unsigned int, unsigned int);

extern "C" Vec3 fn_27_3AC30(
    Work* arg1, unsigned int arg2, unsigned int arg3)
{
    if (arg3 == 1U) {
        ResultA* r = arg1->field34->vf1(arg2);
        return r->value;
    } else {
        ResultB* rb = arg1->field30->vf1(arg2);
        ObjC* obj = getCollision__11soCollisionFUlUl(&arg1->field68, arg2, rb->val);
        return obj->get();
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3AF7C {
struct Vec3 {
    float x, y, z;
};

struct Data {
    char pad0[0x40];
    int value;
    Vec3 position;
    char pad50[8];
    int enabled;
    short kind;
    unsigned char flags;
    unsigned char index;
    void* handle;
    unsigned char state;
};

struct Provider {
    virtual void v0();
    virtual Data* get(int);
};

struct ProviderNoArg {
    virtual void v0();
    virtual Data* get();
};

struct Object {
    char pad0[0x34];
    Provider* provider;
    char pad38[0x30];
    char target[4];
};

extern "C" void soCollisionAddAbsolute__FP11soCollisionUlPv(void*, void*, void*);

extern "C" void fn_27_3AF7C(
    Object* self,
    int index,
    const Vec3* position,
    int value,
    int kind,
    int flags
) {
    if (((ProviderNoArg*)self->provider)->get()->state == 1) {
        Data* data = self->provider->get(index);
        data->value = value;
        data->enabled = 1;
        data->kind = kind;
        data->flags = flags;
        data->index = index;
        data->position = *position;
        soCollisionAddAbsolute__FP11soCollisionUlPv(
            self->target,
            self->provider->get(index)->handle,
            &self->provider->get(index)->value
        );
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3B1A8 {
extern "C" {
extern void* g_soCollisionManager;
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__18soCollisionManagerFv(void*);
s32 getSituation__11soCollisionFi(s32);
void initCheck__11soCollisionFUl(void*, s32);
void updateGroup__11soCollisionFffUl(void*, s32, float, float);
void updatePastLog__11soCollisionFbUl(void*, s32, s32);
void* getCollision__11soCollisionFUlUl(void*, s32, s32, void*);
u32 fn_27_389E0(void*, s32, s32, s32, s32, u32, u32, float);
void addCheckList__11soCollisionFv(void*);
}
class soCollisionAttackModuleImpl {
public:
    virtual ~soCollisionAttackModuleImpl();
    virtual void fn_27_39AA4();
    virtual void deactivate__27soCollisionAttackModuleImplFv();
    virtual void begin__27soCollisionAttackModuleImplFv();
    virtual void clearAll__27soCollisionAttackModuleImplFv();
    virtual void clear__27soCollisionAttackModuleImplFUl();
    virtual void sleep__27soCollisionAttackModuleImplFb();
    virtual void fn_27_39EF4();
    virtual void set__27soCollisionAttackModuleImplFUlUl();
    virtual s32 fn_27_3A270();
};
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void* slot1(s32);
    virtual void slot2();
    virtual s32 slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
};
extern "C" void fn_27_3B1A8(void* self, void* vec1, s32 index, void* vec2, float f0, float f1) {
    unsigned char* base = (unsigned char*)self;
    u32 flags = *(u32*)(base + 0x9c);
    if (((flags >> 29) & 1) == 1) {
        float b;
        float a;
        a = *(float*)vec2;
        b = *(float*)((unsigned char*)vec2 + 4);
        *(float*)(base + 0x40) = a;
        *(float*)(base + 0x44) = b;
    }
    float d;
    float c;
    c = *(float*)vec1;
    d = *(float*)((unsigned char*)vec1 + 4);
    *(float*)(base + 0x38) = c;
    *(float*)(base + 0x3c) = d;
    *(float*)(base + 0x60) = f0;
    *(s32*)(base + 0x88) = getSituation__11soCollisionFi(index);
    *(float*)(base + 0x8c) = f1;
    flags = *(u32*)(base + 0x9c);
    if (((flags >> 30) & 1) != 1) {
        s32 i;
        s32 val;
        u32 flag;
        flag = 0;
        val = (*(soCollisionAttackModuleImpl**)((unsigned char*)(*(void**)((unsigned char*)(*(void**)(base + 0x28)) + 0xd8)) + 0x44))->fn_27_3A270();
        s32 cnt = (*(Object_vptr_0**)(base + 0x64))->slot3();
        i = 0;
        while (i < cnt) {
            initCheck__11soCollisionFUl(base + 0x68, i);
            updateGroup__11soCollisionFffUl(base + 0x68, i, *(float*)vec1, f1);
            updatePastLog__11soCollisionFbUl(base + 0x68, val, i);
            i++;
        }
        cnt = (*(Object_vptr_0**)(base + 0x30))->slot3();
        s32 val2;
        unsigned char* item;
        s32 j = 0;
        while (j < cnt) {
            item = (unsigned char*)(*(Object_vptr_0**)(base + 0x30))->slot1(j);
            if (*(s32*)item) {
                void* p2 = *(void**)((unsigned char*)(*(void**)(base + 0x28)) + 0xd8);
                val2 = *(s32*)((unsigned char*)p2 + 4);
                void* res = getCollision__11soCollisionFUlUl(base + 0x68, j, *(s32*)(item + 0x88), p2);
                float amount = *(float*)(base + 0x60);
                u32 r = fn_27_389E0(item, val2, (s32)res, *(s32*)(base + 0x88), 0, 0, 0, amount);
                if (r == 1)
                    flag = 1;
            }
            j++;
        }
        if (flag == 1 || *(unsigned char*)(base + 0x7a) == 1) {
            if (!g_soCollisionManager) {
                void* mem = __nw__FUlQ25Heaps8HeapType(0x1508, 2);
                if (mem)
                    mem = __ct__18soCollisionManagerFv(mem);
                g_soCollisionManager = mem;
            }
            s32 bss_flag = *(s32*)((unsigned char*)g_soCollisionManager + 0xb4);
            if (!(bss_flag & 1) || !((*(s32*)(base + 0x9c) >> 28) & 1))
                addCheckList__11soCollisionFv(base + 0x68);
        }
    }
    *(u32*)(base + 0x9c) |= 0x08000000;
}
}

// TODO: clean up, give variables real names
namespace jj_3BA38 {
extern "C" s32 notifyEventCollisionAttackCheck__30soCollisionAttackEventObserverFUl(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_3BAFC {
typedef unsigned char u8;
typedef unsigned int u32;

class Mode {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual unsigned int slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual float slot26(void* data, Mode* mode, float value);
};

class soCollisionAttackModuleImpl {
public:
    virtual ~soCollisionAttackModuleImpl();
    virtual void fn_27_39AA4();
    virtual void deactivate__27soCollisionAttackModuleImplFv();
    virtual void begin__27soCollisionAttackModuleImplFv();
    virtual void clearAll__27soCollisionAttackModuleImplFv();
    virtual void clear__27soCollisionAttackModuleImplFUl();
    virtual void sleep__27soCollisionAttackModuleImplFb();
    virtual void fn_27_39EF4();
    virtual void set__27soCollisionAttackModuleImplFUlUl();
    virtual void fn_27_3A270();
    virtual void fn_27_3AF7C();
    virtual void hitAbsolute__27soCollisionAttackModuleImplFUliiii();
    virtual void hitAbsolute__27soCollisionAttackModuleImplFUlPCciii();
    virtual void setWhole__27soCollisionAttackModuleImplFUlb();
    virtual void isAttack__27soCollisionAttackModuleImplFUlb();
    virtual void isInvalidInvincible__27soCollisionAttackModuleImplFUlb();
    virtual void isInvalidXlu__27soCollisionAttackModuleImplFUlb();
    virtual void setPower__27soCollisionAttackModuleImplFUlib();
    virtual void setVector__27soCollisionAttackModuleImplFUli();
    virtual void getSize__27soCollisionAttackModuleImplFUl();
    virtual void setSize__27soCollisionAttackModuleImplFUlf();
    virtual void setTargetCategory__27soCollisionAttackModuleImplFUlUl();
    virtual void setOffset__27soCollisionAttackModuleImplFUlP5Vec3f();
    virtual void setOffsetFromNode__27soCollisionAttackModuleImplFUli();
    virtual void setNode__27soCollisionAttackModuleImplFUli();
    virtual void fn_27_3A75C();
    virtual float getPower(u8 index, u8 unk);
    virtual void fn_27_3A960();
    virtual void getReactionEffect__27soCollisionAttackModuleImplFUlb();
    virtual void setReactionEffect__27soCollisionAttackModuleImplFUlib();
    virtual void getReactionFix__27soCollisionAttackModuleImplFUlb();
    virtual void setReactionFix__27soCollisionAttackModuleImplFUlib();
    virtual void getReactionAdd__27soCollisionAttackModuleImplFUlb();
    virtual void setReactionAdd__27soCollisionAttackModuleImplFUlib();
    virtual void fn_27_3ABC8();
    virtual void setPosX__27soCollisionAttackModuleImplFUlf();
    virtual void getPosX__27soCollisionAttackModuleImplFUlb();
    virtual float getPosX();
    virtual void fn_27_3AC30();
    virtual void getSpeedX__27soCollisionAttackModuleImplFv();
    virtual void getSpeed__27soCollisionAttackModuleImplFv();
    virtual void setSpeed__27soCollisionAttackModuleImplFP5Vec2f();
    virtual void* getData(u8 index, u8 unk);
    virtual void setPowerMulStatus__27soCollisionAttackModuleImplFf();
    virtual void getPowerMulStatus__27soCollisionAttackModuleImplFv();
    virtual void setPowerAddStatus__27soCollisionAttackModuleImplFf();
    virtual void getPowerSpeedStatus__27soCollisionAttackModuleImplFv();
    virtual void setPowerSpeedStatus__27soCollisionAttackModuleImplFf();
    virtual void setPowerMul__27soCollisionAttackModuleImplFf();
    virtual void getPowerMul__27soCollisionAttackModuleImplFv();
    virtual void setPowerMul2nd__27soCollisionAttackModuleImplFf();
    virtual void getPowerMul2nd__27soCollisionAttackModuleImplFv();
    virtual void setReactionMul__27soCollisionAttackModuleImplFf();
    virtual void getLr__27soCollisionAttackModuleImplFv();
    virtual void getPartSize__27soCollisionAttackModuleImplFv();
    virtual void fn_27_3B1A8();
    virtual void fn_27_3B3DC();
    virtual void fn_27_3B7FC();
    virtual void refLogGroup__27soCollisionAttackModuleImplFUlUl();
    virtual void unRefLogGroup__27soCollisionAttackModuleImplFUl();
    virtual void isInflict__27soCollisionAttackModuleImplFv();
    virtual void isInflictStatus__27soCollisionAttackModuleImplFv();
    virtual void setIndirectInfo__27soCollisionAttackModuleImplFii();
    virtual void getIndirectTaskId__27soCollisionAttackModuleImplFv();
    virtual void getIndirectTeamNo__27soCollisionAttackModuleImplFv();
    virtual void getCollision__27soCollisionAttackModuleImplFv();
    virtual void addPastLog__27soCollisionAttackModuleImplFPvUlUl();
    virtual void invalidLog(void* log, u32 groupIndex);
};

struct Modules {
    char pad[0xC8];
    Mode* mode;
};

struct Access {
    char pad[0xD8];
    Modules* modules;
};

struct Entry {
    char pad[0x24];
    unsigned char arg24;
    char pad25[2];
    unsigned char arg27;
};

extern "C" {
extern float lbl_27_rodata_498;
extern float lbl_27_rodata_4B0;
void* getCollisionAttackModule__23soExternalValueAccesserFP11StageObject(void*);
void* getGlowModule__23soExternalValueAccesserFP11StageObject(void*);
float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(void* accesser, u32 id, u32 index);

int fn_27_3BAFC(soCollisionAttackModuleImpl* self, Access* accesser, void* task,
                Entry* entry, float* out1, float* out2, float value)
{
    soCollisionAttackModuleImpl* attack = (soCollisionAttackModuleImpl*)getCollisionAttackModule__23soExternalValueAccesserFP11StageObject(task);
    float power = attack->getPower(entry->arg24, entry->arg27);
    Mode* mode = (Mode*)getGlowModule__23soExternalValueAccesserFP11StageObject(task);
    if (mode->slot6() == 1) {
        if (((Mode*)accesser->modules->mode)->slot6() == 1) {
            power = ((Mode*)accesser->modules->mode)->slot26(
                attack->getData(entry->arg24, entry->arg27), mode, power);
        }
    }
    float limit = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(accesser, 0x7f5, 0);
    if (value - limit < power) {
        if (power > value) {
            value = power;
        }
        if (value > *out1) {
            *out1 = value;
            float attackPos = attack->getPosX();
            float selfPos = self->getPosX();
            *out2 = (selfPos < attackPos) ? lbl_27_rodata_498 : lbl_27_rodata_4B0;
        }
    } else {
        self->invalidLog(entry, 0);
        return 0;
    }
    return 1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_40784 {
extern "C" void initAbsolute__11soCollisionFUl(void* p, u32 index);

struct Elem {
    unsigned char pad[0x64];
    u8 m_flag;
};

class ArrB {
public:
    virtual void v0();
    virtual Elem* get(int index);
    virtual void v2();
    virtual int size();
};

class A {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(int value);
};

class Mod {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();

    unsigned char pad0[0x28];
    A* m2C;
    unsigned char pad1[0x34 - 0x30];
    ArrB* m34;
    unsigned char pad2[0x48 - 0x38];
    float f48;
    float f4C;
    float f50;
    unsigned char pad3[0x64 - 0x54];
    ArrB* m64;
    unsigned char m68[0x94 - 0x68];
    int m94;
    int m98;
    u32 : 1;
    u32 m9C_bit : 1;
    u32 m9C_rest : 30;
};

struct Params {
    unsigned char pad[0xC];
    u32 flags;
};

extern "C" void fn_27_40784(Mod* self, int a1, int a2, Params* p) {
    self->m2C->v5(-1);
    u32 flags = p->flags;
    self->m98 = -1;
    self->f48 = 1.0f;
    self->f4C = 0.0f;
    self->f50 = 0.0f;
    self->m94 = 0;
    self->m9C_bit = 0;
    if (!((flags >> 11) & 1)) {
        self->v4();
    }
    if ((p->flags >> 10) & 1) {
        return;
    }
    u8 zero = 0;
    int i;
    for (i = 0; i < self->m34->size(); i++) {
        self->m34->get(i)->m_flag = zero;
    }
    int n = self->m64->size();
    for (i = 0; i < n; i++) {
        initAbsolute__11soCollisionFUl(&self->m68, i);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_4146C {
extern "C" void fn_27_40784(void*);

extern "C" void fn_27_4146C(void* self) {
    fn_27_40784(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_41474 {
extern "C" void fn_27_3BCCC(void*);

extern "C" void fn_27_41474(void* self) {
    fn_27_3BCCC(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_4147C {
extern "C" void isObserv__27soCollisionAttackModuleImplFc(void*);

extern "C" void fn_27_4147C(void* self) {
    isObserv__27soCollisionAttackModuleImplFc(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_41484 {
extern "C" void __dt__27soCollisionAttackModuleImplFv(void*);

extern "C" void fn_27_41484(void* self) {
    __dt__27soCollisionAttackModuleImplFv(reinterpret_cast<u8*>(self) - 0x1c);
}
}

