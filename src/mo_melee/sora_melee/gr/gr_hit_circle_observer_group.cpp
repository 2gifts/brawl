#include <types.h>

#include <sr/sr_common.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_algorithm.h>
// TODO: clean up, give variables real names
namespace jj_271BF0 {

class Class_C80 { public: virtual void __dt__30soModelModuleCalcWorldCallbackFv(int); };

extern "C" {
extern const float lbl_27_rodata_4480;
extern const float lbl_27_rodata_4484;
}

struct Entry {
    Class_C80* owner;
    Vec2f pos;
    float radius;
    int param;
    char pad[8];
    unsigned char active;
    Vec2f position() const { return pos; }
};

struct Node {
    Node* prev;
    Node* next;
    Entry* entry;
    Node() : prev(0), next(0) {}
    ~Node() {}
};

struct State {
    char pad0[8];
    Node* head;
    Node* tail;
    Node* cursor;
    unsigned int count;
    int unk18;
    unsigned char active;
    char pad1[7];
    Node* head2;
    Node* tail2;
    Node* cursor2;
    unsigned int count2;
    int unk34;
    unsigned char active2;
    char pad2[3];
    Node* hits;
    Node* hitsTail;
    Node* hitsCursor;
    unsigned int hitsCount;
};

static inline bool intersects(Vec2f& d, float rb, const Vec2f& b, float ra, const Vec2f& a) {
    float x = a.m_x - b.m_x;
    float y = a.m_y - b.m_y;
    d.m_x = x;
    d.m_y = y;
    float xSq = d.m_x * d.m_x;
    float ySq = d.m_y * d.m_y;
    float sq = xSq + ySq;
    float length;
    if ((float)(double)nw4r::ut::Abs(sq) <= lbl_27_rodata_4484)
        length = lbl_27_rodata_4480;
    else
        length = sq * rsqrtf(sq);
    if (ra + rb > length) return true;
    return false;
}

extern "C" void update__24grHitCircleObserverGroupFv(State* self) {
    Entry* a;
    Entry* b;
    if (self->active == 0 || self->active2 == 0)
        return;
        self->cursor = self->head;
        unsigned int count = self->count;
        unsigned int count2 = self->count2;
        for (unsigned int i = 0; i < count; ++i) {
            a = self->cursor->entry;
            if (a->active) {
                self->cursor2 = self->head2;
                for (unsigned int j = 0; j < count2; ++j) {
                    b = self->cursor2->entry;
                    if (b->active) {
                        Vec2f d;
                        if (intersects(d, b->radius, b->position(), a->radius, a->position())) {
                            if (!self->hits) {
                                Node* n = new (Heaps::StageInstance) Node;
                                self->hitsCursor = n;
                                self->hitsTail = n;
                                self->hits = n;
                                n->entry = b;
                            } else {
                                self->hitsTail->next = new (Heaps::StageInstance) Node;
                                self->hitsTail->next->entry = b;
                                self->hitsTail->next->prev = self->hitsTail;
                                self->hitsTail = self->hitsTail->next;
                            }
                            ++self->hitsCount;
                        }
                    }
                    self->cursor2 = self->cursor2->next;
                }
            }
            self->cursor = self->cursor->next;
        }
        self->hitsCursor = self->hits;
        unsigned int n = self->hitsCount;
        for (unsigned int i = 0; i < n; ++i) {
            Entry* e = self->hitsCursor->entry;
            e->owner->__dt__30soModelModuleCalcWorldCallbackFv(e->param);
            self->hitsCursor = self->hitsCursor->next;
        }
        self->hitsCursor = self->hits;
        unsigned int j = 0;
        unsigned int m = self->hitsCount;
        for (; j < m; ++j) {
            Node* p = self->hitsCursor;
            self->hitsCursor = p->next;
            delete p;
        }
        self->hits = 0;
        self->hitsTail = 0;
        self->hitsCursor = 0;
        self->hitsCount = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_271EBC {
struct Input {
    int padding[6];
    int kind;
};
extern "C" void fn_27_2720E8(void*, const Input*);
extern "C" void addHitCircle__24grHitCircleObserverGroupFP15grHitCircleData(unsigned char* self, const Input* input)
{
    if (input->kind == 0)
        fn_27_2720E8(self + 4, input);
    if (input->kind == 1)
        fn_27_2720E8(self + 0x20, input);
}
}

// TODO: clean up, give variables real names
namespace jj_271F1C {
extern "C" void enableHitCircle__23grHitCircleObserverTeamFbUl(void* a, void* b, u32 c);

extern "C" void enableHitCircle__24grHitCircleObserverGroupFbiUl(u8* self, void* a, s32 n, u32 c) {
    if (n == 0) {
        enableHitCircle__23grHitCircleObserverTeamFbUl(self + 4, a, c);
    }
    if (n == 1) {
        enableHitCircle__23grHitCircleObserverTeamFbUl(self + 0x20, a, c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_271F94 {
extern "C" void enableHitCircle__23grHitCircleObserverTeamFb(void* a, void* b);

extern "C" {
void enableHitCircle__24grHitCircleObserverGroupFb(void* self, void* arg) {
    enableHitCircle__23grHitCircleObserverTeamFb((char*)self + 0x4, arg);
    enableHitCircle__23grHitCircleObserverTeamFb((char*)self + 0x20, arg);
}
}
}

// TODO: clean up, give variables real names
namespace jj_271FDC {
typedef unsigned char u8;
typedef unsigned int u32;

struct Vec2 {
    float x;
    float y;
};

struct Data {
    u32 a;
    float x;
    float y;
    u32 pad;
    u32 key;
};

struct Node {
    void* unk0;
    Node* next;
    Data* data;
};

struct List {
    void* unk0;
    Node* head;
    void* unk8;
    Node* cur;
    u32 count;
};

extern "C" void setPos__23grHitCircleObserverTeamFP5Vec2fUl(List* list, const Vec2* v, u32 key);

extern "C" void setPos__24grHitCircleObserverGroupFP5Vec2fiUl(u8* base, const Vec2* v, u32 flag, u32 key) {
    if (flag == 0)
        setPos__23grHitCircleObserverTeamFP5Vec2fUl((List*)(base + 4), v, key);
    else
        setPos__23grHitCircleObserverTeamFP5Vec2fUl((List*)(base + 0x20), v, key);
}
}

