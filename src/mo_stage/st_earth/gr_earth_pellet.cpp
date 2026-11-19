#include <gr/gr_calc_world_callback.h>
#include <it/item.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_earth/gr_earth.h>

// Matrix functions of the main binary without names
// Matrix::setTrans (sets the translation of a matrix to the three numbers)
// Matrix::getScalingFactor
void getScalingFactor(Matrix* mtx, Vec3f* scale); // main binary, name unknown (as in cl_collision.cpp)
// the matrix of a translation
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z);
// the matrix of a rotation of the three angles
// the matrix scaled by the three factors

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct grEarthPelletMarker {
    int m_a;
    int m_b;
    grEarthPelletMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grEarthPelletMarker sMarkerA(0xFF, 0);
static grEarthPelletMarker sMarkerB(0xFF, 1);

grEarthPellet* grEarthPellet::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthPellet* ground = new (Heaps::StageInstance) grEarthPellet(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthPellet::~grEarthPellet() {
}

void grEarthPellet::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateState(deltaFrame);
        updatePos(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
        m_hasUpdatedG3dCalcWorld = false;
        updateG3dProcCalcWorld();
    }
}

// The place of the pellet follows the data of the stage (it is lowered by one) and keeps the size of it only while it is
// ripe (the state 0x13); then it is turned around.
void grEarthPellet::updatePos(float deltaFrame) {
    stEarthPelletData* pellet = m_pelletData;
    if (pellet != NULL) {
        Matrix mtx = pellet->m_mtx;
        Matrix offset;
        offset.setTrans(0.0f, -1.0f, 0.0f);
        mtx.mul(&offset, &mtx);
        Vec3f pos;
        pos.m_x = mtx.m[0][3];
        pos.m_y = mtx.m[1][3];
        pos.m_z = mtx.m[2][3];
        Vec3f rot;
        mtx.getRotate(&rot);
        Vec3f scale;
        getScalingFactor(&mtx, &scale);
        mtx.setIdentity();
        fn_8003F074(&mtx, pos.m_x, pos.m_y, pos.m_z);
        mtx.rot(&rot);
        if (m_state != 0x13) {
            scale.m_x = 1.0f;
            scale.m_y = 1.0f;
            scale.m_z = 1.0f;
        }
        mtx.scale(&scale, &mtx);
        mtx.rotY(3.1415927f);
        m_mtx = mtx;
    }
}

// The steps of the pellet: 0 is hidden, 1 waits for the stage to make it (the state 2), 0x13 shows it and grows it (the ripe
// state 0 - 3), 0x19 makes the item of the color and the ripeness.
void grEarthPellet::updateState(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (m_pelletData != NULL && data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setVisibility(0);
            m_state = 1;
            break;
        case 0x19: {
            Vec3f pos;
            pos.m_z = m_mtx.m[2][3];
            m_state = 0;
            u32 variation = 0;
            pos.m_y = m_mtx.m[1][3];
            pos.m_x = m_mtx.m[0][3];
            stEarthPelletData* pellet = m_pelletData;
            switch (pellet->m_color) {
            case 1:
                switch (pellet->unk0B) {
                case 2:
                    variation = 7;
                    break;
                case 0:
                    variation = 1;
                    break;
                case 1:
                    variation = 4;
                    break;
                case 3:
                    variation = 10;
                    break;
                }
                break;
            case 0:
                switch (pellet->unk0B) {
                case 2:
                    variation = 6;
                    break;
                case 0:
                    variation = 0;
                    break;
                case 1:
                    variation = 3;
                    break;
                case 3:
                    variation = 9;
                    break;
                }
                break;
            case 2:
                switch (pellet->unk0B) {
                case 2:
                    variation = 8;
                    break;
                case 0:
                    variation = 2;
                    break;
                case 1:
                    variation = 5;
                    break;
                case 3:
                    variation = 0xB;
                    break;
                }
                break;
            }
            itManager* items = itManager::getInstance();
            if (items != NULL) {
                BaseItem* item = items->createItem(Item_Stage_Pellet, variation, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
                if (item != NULL) {
                    item->warp(&pos);
                }
            }
            break;
        case 1:
            if (m_pelletData->m_state == 2) {
                changeColor();
                setVisibility(1);
                m_timer = data->unk24;
                m_state = 0x13;
            }
            break;
        case 0x13:
            if (m_timer == 0.0f) {
                stEarthPelletData* pellet = m_pelletData;
                if (pellet->unk0B == 1) {
                    m_timer = data->unk2C;
                    pellet->unk0B = 2;
                    m_pelletData->unk08 = 2;
                } else if (pellet->unk0B == 0) {
                    m_timer = data->unk28;
                    pellet->unk0B = 1;
                    m_pelletData->unk08 = 2;
                }
            }
            if (m_pelletData->unk08 == 4) {
                m_state = 0x19;
            }
            break;
        }
        }
    }
}

// The pellet shows the frame of the animation that is its ripeness (the animation does not run).
void grEarthPellet::updateMotion(float deltaFrame) {
    if (m_sceneModels[0] != NULL) {
        gfModelAnimation* modelAnim = *m_modelAnims;
        if (modelAnim != NULL && m_sceneModels[0]->m_resMdl.IsValid()) {
            float frame;
            switch (m_pelletData->unk0B) {
            case 0:
                frame = 0.0f;
                break;
            case 1:
                frame = 1.0f;
                break;
            case 2:
                frame = 2.0f;
                break;
            case 3:
                frame = 3.0f;
                break;
            default:
                frame = 0.0f;
                break;
            }
            modelAnim->setFrame(frame);
            modelAnim->setUpdateRate(0.0);
        }
    }
}

void grEarthPellet::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = m_mtx;
        }
    }
}

// The color of the pellet (red, blue or yellow) is set on the material of the model.
void grEarthPellet::changeColor() {
    nw4r::g3d::ResMat mat;
    nw4r::g3d::ResMatTevColor tevColor;
    GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
        if (model.IsValid()) {
            mat = model.GetResMat("Pellet00_pellet");
            if (mat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                tevColor = access.GetResMatTevColor(false);
                if (tevColor.IsValid()) {
                    u8 kind = m_pelletData->m_color;
                    if (kind == 1) {
                        color.r = 0x00;
                        color.g = 0x00;
                        color.b = 0x80;
                        color.a = 0xFF;
                    } else if (kind == 0) {
                        color.r = 0x80;
                        color.g = 0x00;
                        color.b = 0x00;
                        color.a = 0xFF;
                    } else if (kind < 3) {
                        color.r = 0x80;
                        color.g = 0x64;
                        color.b = 0x00;
                        color.a = 0xFF;
                    }
                    tevColor.GXSetTevColor(GX_TEVREG1, color);
                    tevColor.GXSetTevColor(GX_TEVREG2, color);
                    tevColor.DCStore(false);
                    mat.DCStore(false);
                }
            }
        }
    }
}

#include <mt/mt_vector.h>
// TODO: clean up, give variables real names
namespace jj_F49C {


struct Ground_base { char pad[0x3c]; };

struct Data {
    char p[0x24];
    float a;   // 0x24
    float b;   // 0x28
    float c;   // 0x2c
};

struct Sub {
    unsigned char b0;   // 0x0
    char p1[7];
    unsigned char b8;   // 0x8
    unsigned char b9;   // 0x9
    unsigned char ba;   // 0xa
    unsigned char bb;   // 0xb
};

class Ground : public Ground_base {
public:
#define V(n) virtual void vf##n();
    V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9)
    V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19)
    V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28)
    virtual void setVisibility(unsigned long);   // 0x74
    V(30) V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39)
    V(40) V(41)
    virtual Data* getData();                     // 0xa8
    V(43) V(44) V(45) V(46) V(47) V(48) V(49) V(50) V(51) V(52)
    V(53) V(54) V(55) V(56) V(57) V(58) V(59) V(60) V(61) V(62)
    V(63) V(64) V(65) V(66) V(67) V(68) V(69) V(70) V(71) V(72)
    V(73) V(74) V(75) V(76) V(77) V(78) V(79) V(80) V(81) V(82)
    V(83) V(84) V(85) V(86) V(87) V(88) V(89) V(90) V(91) V(92)
    V(93) V(94) V(95) V(96) V(97) V(98) V(99) V(100) V(101) V(102)
    V(103) V(104) V(105) V(106) V(107) V(108) V(109) V(110) V(111) V(112)
    V(113) V(114) V(115) V(116) V(117)
    virtual void changeColor__13grEarthPelletFv();                   // 0x1d8
#undef V

    char pad1[0x110];        // 0x3c..0x14c (vptr occupies 4 bytes before base)
    unsigned char state;     // 0x150
    char pad2[3];
    float timer;             // 0x154
    char pad3[0xc];
    float f164;              // 0x164
    char pad4[0xc];
    float f174;              // 0x174
    char pad5[0xc];
    float f184;              // 0x184
    Sub* sub;                // 0x188
};

inline void initVec(Vec3f& v, float z, float y, float x) {
    v.m_x = x;
    v.m_y = y;
    v.m_z = z;
}

extern "C" {
extern const float lbl_66_rodata_1C8;
void* getInstance__9itManagerFv();
void* createItem__9itManagerF6itKindUliP16soResourceModuleUciii(void* self, int kind, unsigned long arg1, int arg2, void* mod, unsigned char arg4, int arg5, int arg6, int arg7);
void warp__8BaseItemFP5Vec3f(void* item, Vec3f* pos);

void updateState__13grEarthPelletFf(Ground* self, float delta) {
    if (self->sub != 0) {
        Data* d = self->getData();
        if (d != 0) {
            self->timer = self->timer - delta;
            if (self->timer < lbl_66_rodata_1C8) {
                self->timer = lbl_66_rodata_1C8;
            }
            switch (self->state) {
            case 0:
                self->setVisibility(0);
                self->state = 1;
                break;
            case 1:
                if (self->sub->b0 == 2) {
                    self->changeColor__13grEarthPelletFv();
                    self->setVisibility(1);
                    self->timer = d->a;
                    self->state = 0x13;
                }
                break;
            case 0x13:
                if (lbl_66_rodata_1C8 == self->timer) {
                    Sub* s = self->sub;
                    switch (s->bb) {
                    case 0:
                        self->timer = d->b;
                        s->bb = 1;
                        self->sub->b8 = 2;
                        break;
                    case 1:
                        self->timer = d->c;
                        s->bb = 2;
                        self->sub->b8 = 2;
                        break;
                    case 2:
                        break;
                    }
                }
                if (self->sub->b8 == 4) {
                    self->state = 0x19;
                }
                break;
            case 0x19: {
                self->state = 0;
                Vec3f v;
                float z = self->f184;
                float y = self->f174;
                float x = self->f164;
                initVec(v, z, y, x);
                int idx = 0;
                Sub* s = self->sub;
                switch (s->ba) {
                case 0:
                    switch (s->bb) {
                    case 0: idx = 0; break;
                    case 1: idx = 3; break;
                    case 2: idx = 6; break;
                    case 3: idx = 9; break;
                    }
                    break;
                case 1:
                    switch (s->bb) {
                    case 0: idx = 1; break;
                    case 1: idx = 4; break;
                    case 2: idx = 7; break;
                    case 3: idx = 10; break;
                    }
                    break;
                case 2:
                    switch (s->bb) {
                    case 0: idx = 2; break;
                    case 1: idx = 5; break;
                    case 2: idx = 8; break;
                    case 3: idx = 11; break;
                    }
                    break;
                }
                void* it = getInstance__9itManagerFv();
                if (it != 0) {
                    int sp8 = 0xFFFF;
                    void* item = createItem__9itManagerF6itKindUliP16soResourceModuleUciii(it, 0x4D, idx, -1, 0, 0, 0xFFFF, 0, sp8);
                    if (item != 0) {
                        warp__8BaseItemFP5Vec3f(item, &v);
                    }
                }
                break;
            }
            }
        }
    }
}
}
}

