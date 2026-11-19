#include <st/operator/st_operator_fighter_rebirth.h>
#include <types.h>

stOperatorFighterRebirth::stOperatorFighterRebirth() : stOperator("StOperatorFighterRebirth") {
    m_state = 0;
    m_rebirthFrames = 60.0f;
}

stOperatorFighterRebirth::~stOperatorFighterRebirth() { }

void stOperatorFighterRebirth::startOperator() {
    m_state = 1;
    m_rebirthCounter = m_rebirthFrames;
}

void stOperatorFighterRebirth::resetRebirth() {
    m_state = 0;
}

// TODO: clean up, give variables real names
namespace jj_245590 {
struct ObjectBase {
    unsigned char pad[0x3C];
};

class Object : public ObjectBase {
public:
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual int v39();
};

struct State {
    unsigned char pad00[0x40];
    unsigned char state;
    unsigned char pad41;
    unsigned char flag42;
    unsigned char flag43;
    unsigned char pad44[8];
    float timer;
    Object* object;
};

extern "C" {
    extern void* g_GameGlobal;
    int isJustGameFrame__10GameGlobalCFv(void*);
    extern const float lbl_27_rodata_3F34;
    extern const float lbl_27_rodata_3F38;
}

extern "C" void fn_27_245590(State* self) {
    if (isJustGameFrame__10GameGlobalCFv(g_GameGlobal) == 0) return;
    if (self->flag43 == 1) return;
    if (self->flag42 == 1) return;
    if (self->state == 0) return;

    switch (self->state) {
    case 1:
        self->timer -= lbl_27_rodata_3F34;
        if (self->timer < lbl_27_rodata_3F38)
            self->timer = lbl_27_rodata_3F38;

        if (lbl_27_rodata_3F38 == self->timer) {
            if (self->object->v39()) {
                self->state = 2;
                return;
            }
        }
        break;

    case 2:
        Object* object = self->object;
        self->state = 0;
        if (object)
            object->v34();
        break;
    }
}
}

