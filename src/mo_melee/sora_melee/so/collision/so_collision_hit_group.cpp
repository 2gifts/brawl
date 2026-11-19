#include <so/collision/so_collision_hit_group.h>
#include <so/collision/so_collision_hit_part.h>
#include <types.h>

soCollisionHitGroup::soCollisionHitGroup() {
    m_index = -1;
    m_partStart = 0;
    m_partSize = 0;
    m_multiSituation = 0;
    m_globalOffset = 0;
}

soCollisionHitGroup::~soCollisionHitGroup() { }

void soCollisionHitGroup::init() {
    m_partStart = -1;
    m_partSize = 0;
    m_index = 0;
}

void soCollisionHitGroup::resetStatusAll(soArray<soCollisionHitPart>* parts) {
    if (m_51 == 1) {
        for (int i = 0; i < m_partSize; i++) {
            soCollisionHitPart& part = parts->at(m_partStart + i);
            part.m_status = part.unk4;
        }
        m_51 = 0;
    }
}

void soCollisionHitGroup::setStatus(soArray<soCollisionHitPart>* parts, u32 index, int status) {
    parts->at(m_partStart + index).m_status = status;
    m_51 = 1;
}

void soCollisionHitGroup::setStatusNode(soArray<soCollisionHitPart>* parts, int nodeIndex, int status) {
    for (int i = 0; i < m_partSize; i++) {
        if (parts->at(m_partStart + i).m_hitData.m_nodeIndex == nodeIndex) {
            parts->at(m_partStart + i).m_status = status;
            m_51 = 1;
            return;
        }
    }
}

void soCollisionHitGroup::setStatusNodeDefault(soArray<soCollisionHitPart>* parts, int nodeIndex, int status) {
    for (int i = 0; i < m_partSize; i++) {
        if (parts->at(m_partStart + i).m_hitData.m_nodeIndex == nodeIndex) {
            parts->at(m_partStart + i).unk4 = status;
            m_51 = 1;
            return;
        }
    }
}

void soCollisionHitGroup::setStatusAll(soArray<soCollisionHitPart>* parts, int status) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).m_status = status;
    }
    m_51 = 1;
}

void soCollisionHitGroup::setWhole(int whole) {
    m_whole = whole;
}

int soCollisionHitGroup::getWhole() {
    return m_whole;
}

void soCollisionHitGroup::resetStatusGlobal() {
    if (m_49 == true || m_xluFrameGlobal != 0) {
        m_global = 2;
    } else if (m_50 == true || m_invincibleFrameGlobal != 0) {
        m_global = 1;
    } else {
        m_global = 0;
    }
}

void soCollisionHitGroup::setXluGlobal() {
    m_49 = true;
    resetStatusGlobal();
}

void soCollisionHitGroup::cancelXluGlobal() {
    m_49 = false;
    resetStatusGlobal();
}

void soCollisionHitGroup::setXluFrameGlobal(int xluFrameGlobal) {
    m_xluFrameGlobal = xluFrameGlobal;
    resetStatusGlobal();
}

void soCollisionHitGroup::setInvincibleGlobal() {
    m_50 = true;
    resetStatusGlobal();
}

void soCollisionHitGroup::cancelInvincibleGlobal() {
    m_50 = false;
    resetStatusGlobal();
}

void soCollisionHitGroup::setInvincibleFrameGlobal(int invincibleFrameGlobal, bool override) {
    if (override == true) {
        m_invincibleFrameGlobal = invincibleFrameGlobal;
    } else if (m_invincibleFrameGlobal < invincibleFrameGlobal) {
        m_invincibleFrameGlobal = invincibleFrameGlobal;
    }
    resetStatusGlobal();
}

void soCollisionHitGroup::updateStatusGlobal() {
    if (m_xluFrameGlobal > 0) {
        m_xluFrameGlobal--;
        if (m_xluFrameGlobal == 0) {
            resetStatusGlobal();
        }
    }
    if (m_invincibleFrameGlobal > 0) {
        m_invincibleFrameGlobal--;
        if (m_invincibleFrameGlobal == 0) {
            resetStatusGlobal();
        }
    }
}

int soCollisionHitGroup::getGlobal() {
    return m_global;
}

int soCollisionHitGroup::getPartStatus(soArray<soCollisionHitPart>* parts, u32 index) {
    return parts->at(m_partStart + index).m_status;
}

void soCollisionHitGroup::initMultiSituation() {
    m_multiSituation = 0;
}

void soCollisionHitGroup::setMultiSituation(SituationKind situation) {
    m_multiSituation = 1 << soCollision::getSituation(situation);
}

void soCollisionHitGroup::setOpponentCategory(soArray<soCollisionHitPart>* parts, u32 categoryMask, bool enable) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setOpponentCategory(categoryMask, enable);
    }
}

void soCollisionHitGroup::setSelfCategory(soArray<soCollisionHitPart>* parts, soCollision::Category category) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setSelfCategory(category);
    }
}

void soCollisionHitGroup::reset(soCollision* collision, soArray<soCollisionHitPart>* parts, float posX, float scale, float lr, SituationKind situation) {
    m_posX = posX;
    m_scale = scale;
    m_situationKind = situation;
    m_lr = lr;
    m_multiSituation = 0;
    if (m_index != -1) {
        unk24 = 0.0f;
        m_whole = 0;
        m_global = 0;
        m_49 = false;
        m_xluFrameGlobal = 0;
        m_50 = false;
        m_invincibleFrameGlobal = 0;
        m_51 = 1;
        unk48 = 0;
        soCollision::getSituation(situation);
        soCollisionHitPart* part;
        for (int i = 0; i < m_partSize; i++) {
            part = &parts->at(m_partStart + i);
            part->reset(collision->getCollision(i, m_index));
        }
    }
}

void soCollisionHitGroup::set(soArray<soCollisionHitPart>* parts, u32 index, soCollisionHitData* data) {
    parts->at(m_partStart + index).setData(data);
}

void soCollisionHitGroup::set(soArray<soCollisionHitPart>* parts, soSet<soCollisionHitData>* data) {
    for (int i = 0; i < (int)data->m_size; i++) {
        parts->at(m_partStart + i).setData(&data->m_elements[i]);
    }
}

void soCollisionHitGroup::setCheckCatch(soArray<soCollisionHitPart>* parts, bool checkCatch) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setCheckCatch(checkCatch);
    }
}

// TODO: clean up, give variables real names
namespace jj_41C40 {
struct VObj {
    virtual void f0();
    virtual void* f1(s32 idx);
};

struct Ent {
    u8 pad[0x1c];
    u32 flags;
};

struct Hdr {
    u8* base;
    s32 count;
};

struct Self {
    u32 unk0;
    s16 unk4;
    s16 unk6;
};

extern "C" void allocateCollision__11soCollisionFUlbUl(s32 a, s32 b, s32 c, s32 d);
extern "C" void setData__18soCollisionHitPartFP18soCollisionHitData(void* p, u8* addr);

extern "C" s32 fn_27_41C40(Self* self, s32 a1, VObj* obj, Hdr* hdr, s32 a4, s32 a5) {
    s32 off;
    s32 i;
    i = 0;
    off = 0;
    for (; i < hdr->count; off += 0x20, i++) {
        allocateCollision__11soCollisionFUlbUl(a1, i, (((Ent*)(hdr->base + off))->flags >> 16) & 1, a4);
        void* p = obj->f1(a5 + i);
        setData__18soCollisionHitPartFP18soCollisionHitData(p, hdr->base + off);
    }
    self->unk4 = a5;
    self->unk6 = i;
    self->unk0 = a4;
    return i;
}
}

// TODO: clean up, give variables real names
namespace jj_41CF8 {
struct VObj { virtual void f0(); virtual void* f1(s32 idx); };
struct Hdr { u8* base; s32 count; };
struct Ent { f32 f[7]; u8 byte1C; u8 pad[3]; u32 w20; u32 w24; };
union Packed {
    u32 raw;
    struct {
        u32 hi:9;
        u32 gap0:2;
        u32 mid:2;
        u32 gap1:2;
        u32 low:1;
        u32 tail:16;
    } bits;
};
struct Rec { f32 f[7]; Packed h; };
struct Self { u32 unk0; s16 unk4; s16 unk6; };
extern "C" void allocateCollision__11soCollisionFUlbUl(s32, s32, s32, s32, u32, void*);
extern "C" void setData__18soCollisionHitPartFP18soCollisionHitData(void*, void*);
extern "C" s32 fn_27_41CF8(Self* self, s32 a1, VObj* obj, Hdr* hdr, s32 a4, s32 a5) {
    Rec rec;
    s32 off;
    s32 i;
    i = 0;
    off = 0;
    for (; i < hdr->count; off += 0x28, i++) {
        Ent* e = (Ent*)(hdr->base + off);
        rec.f[0] = e->f[0];
        rec.f[1] = e->f[1];
        rec.f[2] = e->f[2];
        rec.f[3] = e->f[3];
        rec.f[4] = e->f[4];
        rec.f[5] = e->f[5];
        rec.f[6] = e->f[6];
        rec.h.bits.hi = e->w24;
        rec.h.bits.gap0 = 0;
        rec.h.bits.mid = e->w20;
        rec.h.bits.gap1 = 0;
        s32 b = (s32)((e->byte1C << 24) & 0xC0000000) >> 31;
        rec.h.bits.low = (u32)(-b | b) >> 31;
        allocateCollision__11soCollisionFUlbUl(a1, i, rec.h.raw >> 16 & 1, a4, rec.h.raw, e);
        void* p = obj->f1(a5 + i);
        setData__18soCollisionHitPartFP18soCollisionHitData(p, &rec);
    }
    self->unk4 = a5;
    self->unk6 = i;
    self->unk0 = a4;
    return i;
}
}

// TODO: clean up, give variables real names
namespace jj_41F7C {
struct Obj {
    virtual void f0();
    virtual void* f1(s32);
};

struct Src {
    u8 pad[4];
    s16 off;
};

struct Entry {
    float x0, x4, x8, xC, x10, x14, x18;
    s8 flag : 1;
    u32 kind;
    u32 id;
};

struct Desc {
    Entry* base;
    s32 count;
};

struct Temp {
    float x0, x4, x8, xC, x10, x14, x18;
    u32 id : 9;
    u32 zero0 : 2;
    u32 kind : 2;
    u32 zero1 : 2;
    u32 flag : 1;
    u32 rest : 16;
};

extern "C" void setData__18soCollisionHitPartFP18soCollisionHitData(void*, const Temp*);

extern "C" void fn_27_41F7C(Src* src, Obj* obj, Desc* desc) {
    s32 off;
    s32 i;
    i = 0;
    off = 0;

    for (; i < desc->count; off += 0x28, i++) {
        Entry* in = (Entry*)((u8*)desc->base + off);
        Temp temp;
        temp.x0 = in->x0;
        temp.x4 = in->x4;
        temp.x8 = in->x8;
        temp.xC = in->xC;
        temp.x10 = in->x10;
        temp.x14 = in->x14;
        temp.x18 = in->x18;
        temp.id = in->id;
        temp.zero0 = 0;
        temp.kind = in->kind;
        temp.zero1 = 0;
        temp.flag = in->flag != 0;
        setData__18soCollisionHitPartFP18soCollisionHitData(obj->f1(src->off + i), &temp);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_42348 {
struct R { char pad[0x24]; unsigned int a:11; unsigned int b:2; unsigned int c:19; };
struct B { virtual void f0(); virtual R& get(int) const; };
struct A { char pad[4]; short off; };
extern "C" unsigned int fn_27_42348(const A* a, const B* b, int i) {
    return b->get(a->off + i).b;
}
}

// TODO: clean up, give variables real names
namespace jj_42A54 {
typedef unsigned int u32;

struct Result {
    u32 words[4];
};

class Object {
    unsigned char padding[0x30];

public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual Result get();
};

extern "C" Object* getCollision__11soCollisionFUlUl(void*, u32, u32);

extern "C" Result getCenterPos__19soCollisionHitGroupFP11soCollisionUs(const u32* index, void* work, u32 value) {
    return getCollision__11soCollisionFUlUl(work, value, *index)->get();
}
}

