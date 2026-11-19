#include <types.h>
#include <ad/ad_seal_info.h>
#include <ty/ty_seal_listmng.h>
#include <memory.h>

const u8 lbl_80409798[48] = {
    0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
    14, 14, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25,
    25, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 0, 0, 0, 0
};

adSealInfoResult::~adSealInfoResult() {
    if (unk0) {
        delete unk0;
    }
}

void adSealInfoResult::getSealInfoPrepare() {
    unk0 = new (Heaps::MenuInstance) tySealListDataManager(true);
}

bool adSealInfoResult::isLoadFinish() {
    return unk0->isLoadFinish();
}

u32 adSealInfoResult::getFighterSealInfoNum(s32 index) {
    u32 count = 0;
    u32 fighter;
    if (index < 0 || index > 43) {
        fighter = 35;
    } else {
        fighter = lbl_80409798[index];
    }
    for (u32 seal = 0; seal < 40; ++seal) {
        if (unk4[fighter][seal].unk0 == -1) {
            break;
        }
        ++count;
    }
    return count;
}

void* adSealInfoResult::getFighterSealInfo(s32 index) {
    u32 fighter;
    if (index < 0 || index > 43) {
        fighter = 35;
    } else {
        fighter = lbl_80409798[index];
    }
    return unk4[fighter];
}

// TODO: clean up, give variables real names
namespace jj_8015B230 {
struct Slot { float f0; int id; int data; float f1; };
struct Record { Slot slots[40]; };
struct Result { char pad[0x24]; int data; char pad28[4]; int id; float f30; float f34; };
struct GameGlobal { char pad[0x30]; void* data; };
extern "C" float lbl_805A2BA8;
extern "C" GameGlobal* g_GameGlobal;
extern "C" Result* fn_800AE578(void*, int);
extern "C" void fn_800AD87C(void*, int);
inline int find(int id, int i, Slot* p, int count) {
 for (; i<count; ++p, ++i) {
  if(p->id == -1) return i;
  if(id == p->id) return i;
 }
 return 0;
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8015B230(Record* self) {
 float sentinel=lbl_805A2BA8;
 int invalid=-1;
 unsigned int row=0;
 Record* current=self;
 do {
 Slot* p=current->slots;
 for(int batch=0;batch<5;++batch,p+=8) {
 p[0].id=invalid;p[0].f1=sentinel;p[1].f0=sentinel;
 p[1].id=invalid;p[1].f1=sentinel;p[2].f0=sentinel;
 p[2].id=invalid;p[2].f1=sentinel;p[3].f0=sentinel;
 p[3].id=invalid;p[3].f1=sentinel;p[4].f0=sentinel;
 p[4].id=invalid;p[4].f1=sentinel;p[5].f0=sentinel;
 p[5].id=invalid;p[5].f1=sentinel;p[6].f0=sentinel;
 p[6].id=invalid;p[6].f1=sentinel;p[7].f0=sentinel;
 p[7].id=invalid;p[7].f1=sentinel;p[8].f0=sentinel;
 }
 ++row;++current;
 }while(row<35U);
 unsigned char* page=(unsigned char*)g_GameGlobal->data+0x6F8;
 Record* rec=self;
 unsigned char* entry;
 int outer;
 for(outer=0;outer<35;++outer,page+=0x1E0,++rec) {
 entry=page;
 for(int i=0;i<40;++i,entry+=12) {
 int id=*(int*)entry;
 if(id<=0) break;
 Result* result=fn_800AE578(*(void**)self,id);
 int index=find(result->id,0,rec->slots,40);
 Slot* slot=&rec->slots[index];
 slot->id=*(unsigned int*)((char*)result+0x2C);
 if(slot->f1<result->f30) {
 slot->f1=result->f30;
 slot[1].f0=result->f34;
 slot->data=result->data;
 }
 }
 }
 fn_800AD87C(*(void**)self,1);
 *(void**)self=0;
}
}

