#include <mu/mu_prize_wnd.h>
#include <string.h>

extern struct {
    const char* unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
} lbl_804080C0[];

extern const struct {
    void (*unk0)(muPrizeWndTask*);
    int (*unk4)(muPrizeWndTask*);
} lbl_80407C28[];

extern const char lbl_80456DD8[];
extern const char lbl_8059E0F8[4];

muPrizeWndTask::muPrizeWndTask()
    : gfTask(lbl_80456DD8, Category_Menu, 15, 8, true),
      unk58(NULL), unk5C(NULL), unk108(true), unk10C(5), unk110(6),
      unk114(0), unk118(0), unk21C(0), unk220(0), unk224(NULL), unk228(NULL),
      unk22C(0), unk230(0), unk2B4(false), unk2B8(NULL), unk2BC(NULL),
      unk2C0(NULL), unk344(NULL), unk348(NULL), unk3CC(-1), unk3D0(false) {
    unk2C_b1 = false;
    for (int i = 0; i < 5; i++) {
        unk40[i] = NULL;
    }
    unk54[0] = NULL;
    memset(unk11C, 0, sizeof(unk11C));
}

muPrizeWndTask7TexLoadFv::muPrizeWndTask7TexLoadFv() {}

muPrizeWndTask7TexLoadFv::~muPrizeWndTask7TexLoadFv() {}

void muPrizeWndTask::createObjResFile(const void* models, int count,
                                     nw4r::g3d::ResFile* resFile, int priority) {
    int num;
    int i;
    int j;
    const __typeof__(lbl_804080C0[0])* model =
        static_cast<const __typeof__(lbl_804080C0[0])*>(models);
    for (i = 0; i < count; i++, model++) {
        num = 1;
        if (model->unk4 < model->unk5) {
            num = model->unk5 - model->unk4;
        }
        for (j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, model->unk0,
                model->unk6 + priority, NULL, Heaps::MenuInstance);
            unk40[model->unk4 + j] = object;
            char animName[0x40];
            strcpy(animName, model->unk0);
            strcat(animName, lbl_8059E0F8);
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muPrizeWndTask::processDefault() {
    fn_800B9984(&unk60);
    if (unk110 != unk10C) {
        if (lbl_80407C28[unk10C].unk0 != NULL) {
            lbl_80407C28[unk10C].unk0(this);
        }
        unk110 = unk10C;
    }
    if (lbl_80407C28[unk10C].unk4 != NULL) {
        unk10C = lbl_80407C28[unk10C].unk4(this);
    }
}
