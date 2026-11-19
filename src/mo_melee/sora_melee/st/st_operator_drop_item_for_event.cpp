#include <st/operator/st_operator_drop_item_for_event.h>
#include <types.h>

stOperatorDropItemEvent::~stOperatorDropItemEvent() { }

stOperatorDropItemEvent::stOperatorDropItemEvent()
    : stOperatorDropItemMelee("StOperatorDropItemEvent") {
    unk74 = 0.0f;
    unk70 = 0.0f;
    unk78 = 0;
}

#include <it/it_manager.h>
#include <ft/ft_manager.h>
#include <st/stage.h>
#include <ec/ec_mgr.h>
// TODO: clean up, give variables real names
namespace jj_2479A0 {

struct Rodata3FF0 { float zero, one, half; };
extern "C" {
extern const Rodata3FF0 lbl_27_rodata_3FF0;
extern Stage* g_Stage;
void fn_27_247518(void*);
int isItemSwitch__9itManagerFi(itManager*, int);
int isExclusiveSpecialItem__9itManagerFib(itManager*, int, int);
BaseItem* createBaseItem__9itManagerFP5Vec3fP5Vec3ff6itKindUliiP16soResourceModuleUciiii(itManager*, Vec3f*, Vec3f*, float, itKind, u32, int, int, soResourceModule*, u8, int, int, int, int);
float randf__Fv();
int getItemNum__9itManagerF6itKind(itManager*, itKind);
}

extern "C" void fn_27_2479A0(unsigned char* self) {
    const Rodata3FF0* values = &lbl_27_rodata_3FF0;
    if (self[0x43] != 1 && self[0x42] != 1 && self[0x40] != 0) {
        itManager* mgr = itManager::getInstance();
        switch ((*(unsigned char**)(self + 0x44))[0x10]) {
        case 0xaf:
        if (self[0x78] == 0 && mgr->isCompItemKindArchive((itKind)0x86, 0, true)) {
            int owner = -1;
            ftManager* fm = g_ftManager;
            for (int i = 0; i < 7; ++i) {
                Fighter* fighter = fm->getFighter(fm->getEntryId(i), -1);
                if (fighter && fighter->getFtKind() == (ftKind)0x2c) {
                    owner = fighter->m_taskId;
                    break;
                }
            }
            Vec3f pos;
            g_Stage->getRandItemPos(&pos);
            BaseItem* item = createBaseItem__9itManagerFP5Vec3fP5Vec3ff6itKindUliiP16soResourceModuleUciiii(mgr, &pos, &pos, values->one, (itKind)0x86, 0, -1, owner, 0, 0, 0xffff, 0, 0xffff, 0x14);
            if (item) {
                self[0x78] = 1;
                item->setVanishMode(false);
            }
        }
        default: break;
        }
        if (*(float*)(self + 0x70) > values->zero && isItemSwitch__9itManagerFi(mgr, 0x37)) {
            *(float*)(self + 0x70) -= values->one;
            if (*(float*)(self + 0x70) <= values->zero) {
                *(float*)(self + 0x70) = *(float*)(self + 0x74) * (values->half + values->half * randf__Fv());
                if (!isExclusiveSpecialItem__9itManagerFib(mgr, 0x37, 1) && getItemNum__9itManagerF6itKind(mgr, (itKind)0x37) <= 0) {
                    Vec3f pos;
                    g_Stage->getRandItemPos(&pos);
                    g_ecMgr->setEffect((EfID)0x37, &pos);
                    mgr->createItem(&pos, values->one, (itKind)0x37, 0, -1, 0, 0, 0xffff, 0, 0xffff);
                }
            }
        }
        fn_27_247518(self);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_247C1C {
typedef unsigned char u8;

struct Rodata3FF0 {
    float f0;
    float f1;
    float half;
    float c1200;
    float c2400;
};

extern "C" {
extern const Rodata3FF0 lbl_27_rodata_3FF0;

void startOperator__23stOperatorDropItemMeleeFv(u8* self);
void initializeDragoonFlags__9itManagerFib(void* self, int a, int b);
void* getInstance__9itManagerFv();
int isCompItemKindArchive__9itManagerF6itKindUlb(void* self, int kind, unsigned long variation, int flag);
void preloadAssist__9itManagerF6itKind(void* self, int kind);
float randf__Fv();

void fn_27_247C1C(u8* self) {
    const Rodata3FF0* values = (const Rodata3FF0*)&lbl_27_rodata_3FF0;
    u8* mgr;
    u8* obj;

    self[0x78] = 0;
    mgr = (u8*)getInstance__9itManagerFv();
    *(float*)(self + 0x74) = values->f0;
    *(float*)(self + 0x70) = *(float*)(self + 0x74) * (values->half + values->half * randf__Fv());
    obj = *(u8**)(self + 0x44);
    switch (obj[0x10]) {
    case 0x1F:
    case 0x19:
        *(float*)(self + 0x74) = values->c1200;
        *(float*)(self + 0x70) = *(float*)(self + 0x74) * (values->half + values->half * randf__Fv());
        break;
    case 0x20:
        *(float*)(self + 0x74) = values->c2400;
        *(float*)(self + 0x70) = *(float*)(self + 0x74) * (values->half + values->half * randf__Fv());
        break;
    case 0x6:
        initializeDragoonFlags__9itManagerFib(mgr, 1, 0);
        break;
    case 0xAF:
        if (isCompItemKindArchive__9itManagerF6itKindUlb(mgr, 0x86, 0U, 1) == 0) {
            preloadAssist__9itManagerF6itKind(mgr, 0x86);
        }
        break;
    }
    startOperator__23stOperatorDropItemMeleeFv(self);
}
}
}

