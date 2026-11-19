#include <so/anim/so_anim_obj.h>

void soAnimObj::reset() {
    destroy();
    m_anmVisRes = 0;
    m_anmMatClrRes = 0;
    m_anmTexPatRes = 0;
    m_anmTexSrtRes = 0;
    m_anmShpRes = 0;
    m_animFrame = 0.0f;
    m_startFrame = 0.0f;
    m_saveFrame = 0.0f;
    m_rate = 1.0f;
    m_loopFlag = 0;
    m_reverseFlag = 0;
    m_emptyFlag = 1;
}

void soAnimObj::setFrame(float frame) {
    if (m_emptyFlag == 0) {
        if (m_anmVisRes) {
            m_anmVisRes->SetFrame(frame);
        }
        if (m_anmMatClrRes) {
            m_anmMatClrRes->SetFrame(frame);
        }
        if (m_anmTexPatRes) {
            m_anmTexPatRes->SetFrame(frame);
        }
        if (m_anmTexSrtRes) {
            m_anmTexSrtRes->SetFrame(frame);
        }
        if (m_anmShpRes) {
            m_anmShpRes->SetFrame(frame);
        }
        m_animFrame = frame;
    }
}

float soAnimObj::getEndFrame() const {
    if (m_emptyFlag == 0) {
        if (m_anmVisRes) {
            return (s32)m_anmVisRes->m_anmVisFile->m_animLength - 1.0f;
        }
        if (m_anmMatClrRes) {
            return (s32)m_anmMatClrRes->m_anmMatClrFile->m_animLength - 1.0f;
        }
        if (m_anmTexPatRes) {
            return (s32)m_anmTexPatRes->m_anmTexPatFile->m_animLength - 1.0f;
        }
        if (m_anmTexSrtRes) {
            return (s32)m_anmTexSrtRes->m_anmTexSrtFile->m_animLength - 1.0f;
        }
        if (m_anmShpRes) {
            return (s32)m_anmShpRes->m_anmShpFile->m_animLength - 1.0f;
        }
    }
    return 0.0f;
}

bool soAnimObj::isEnd() const {
    if (m_emptyFlag == 0 && m_loopFlag == 0 && m_animFrame >= getEndFrame()) {
        return true;
    }
    return false;
}

bool soAnimObj::isLooped() const {
    if (m_emptyFlag == 0 && m_loopFlag == 1 && m_animFrame >= 0.0f && m_animFrame < m_rate && m_animFrame < m_startFrame) {
        return true;
    }
    return false;
}

void soAnimObj::rewind() {
    m_saveFrame = m_animFrame;
    setFrame(m_startFrame);
}

void soAnimObj::turnBack() {
    setFrame(m_saveFrame);
}

void soAnimObj::setAnimObj(float startFrame, float rate, nw4r::g3d::ResFile resFile, nw4r::g3d::ResMdl resMdl, Heaps::HeapType heapType, u8 loopFlag, u8 reverseFlag, const char* animationName) {
    create(nw4r::g3d::ResFile(resFile), nw4r::g3d::ResMdl(resMdl), heapType, animationName);
    m_rate = rate;
    m_loopFlag = loopFlag;
    setFrame(startFrame);
    m_startFrame = startFrame;
    m_reverseFlag = reverseFlag;
}

// TODO: clean up, give variables real names
namespace jj_1F980 {
struct fn_27_1F980_obj {
    u32 m_0;
    u32 m_4;
    u32 m_8;
    u32 m_c;
    u32 m_10;
    float m_14;
    float m_18;
    float m_1c;
    float m_20;
    u8 m_24;
};

extern const float lbl_27_rodata_370;
extern const float lbl_27_rodata_374;

extern "C" {
void __ct__9soAnimObjFv(fn_27_1F980_obj* p) {
    p->m_0 = 0;
    p->m_4 = 0;
    p->m_8 = 0;
    p->m_c = 0;
    p->m_10 = 0;
    p->m_14 = lbl_27_rodata_370;
    p->m_18 = lbl_27_rodata_370;
    p->m_1c = lbl_27_rodata_370;
    p->m_20 = lbl_27_rodata_374;
    p->m_24 = (p->m_24 & ~0xC0) | 0x20;
}
}
}

// TODO: clean up, give variables real names
namespace jj_1F9CC {
extern "C" void destroy__9soAnimObjFv(const void*);

extern "C" void* __dt__9soAnimObjFv(void* p, int flag) {
    if (p) {
        destroy__9soAnimObjFv(p);
        if (flag > 0) {
            operator delete(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_1FE18 {
class Target {
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
    virtual void invoke(void*, int);
};

struct Object {
    void* values[5];
    char padding[0x10];
    unsigned char high : 2;
    unsigned char disabled : 1;
    unsigned char low : 5;
};

extern "C" void set__9soAnimObjFPQ34nw4r3g3d6ScnMdl(Object* self, Target* target) {
    if (!self->disabled) {
        if (self->values[0]) target->invoke(self->values[0], 6);
        if (self->values[1]) target->invoke(self->values[1], 6);
        if (self->values[2]) target->invoke(self->values[2], 6);
        if (self->values[3]) target->invoke(self->values[3], 6);
        if (self->values[4]) target->invoke(self->values[4], 6);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1FF0C {
class Object {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void update();
};

struct Context {
    Object* objects[5];
    unsigned char padding[16];
    unsigned char unused : 2;
    unsigned char disabled : 1;
    unsigned char remaining : 5;
};

extern "C" void remove__9soAnimObjFv(Context* self) {
    if (!self->disabled) {
        if (self->objects[0]) self->objects[0]->update();
        if (self->objects[1]) self->objects[1]->update();
        if (self->objects[2]) self->objects[2]->update();
        if (self->objects[3]) self->objects[3]->update();
        if (self->objects[4]) self->objects[4]->update();
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1FFCC {
typedef unsigned char u8;

namespace nw4r {
namespace g3d {
class G3dObj {
public:
    void Destroy();
};
} // namespace g3d
} // namespace nw4r

extern "C" {
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void destroy__9soAnimObjFv(u8* self) {
    void* entry;
    if (!((self[0x24] >> 5) & 1)) {
        entry = *reinterpret_cast<void**>(self + 0x0);
        if (entry) {
            Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            *reinterpret_cast<void**>(self + 0x0) = 0;
        }
        entry = *reinterpret_cast<void**>(self + 0x4);
        if (entry) {
            Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            *reinterpret_cast<void**>(self + 0x4) = 0;
        }
        entry = *reinterpret_cast<void**>(self + 0x8);
        if (entry) {
            Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            *reinterpret_cast<void**>(self + 0x8) = 0;
        }
        entry = *reinterpret_cast<void**>(self + 0xC);
        if (entry) {
            Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            *reinterpret_cast<void**>(self + 0xC) = 0;
        }
        entry = *reinterpret_cast<void**>(self + 0x10);
        if (entry) {
            Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            *reinterpret_cast<void**>(self + 0x10) = 0;
        }
        self[0x24] = (u8)(self[0x24] | 0x20);
    }
}
}
}

