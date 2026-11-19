#include <types.h>
#include <it/it_gen_archive.h>
#include <so/so_archive_db.h>

bool itGenArchive::isComp() {
    u32 archiveId = m_archiveId;

    if (archiveId == 0xFFFF) {
        return false;
    }
    return soArchiveDb::getManager(2)->getLoadResult(archiveId, nullptr) != 0;
}

bool itGenArchive::isFailed()
{
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return true;
    }
    return soArchiveDb::getManager(2)->getLoadResult(archiveId, nullptr) == 2;
}

bool itGenArchive::isRemovable() {
    u32 archiveId = m_archiveId;

    if (archiveId == 0xFFFF) {
        return true;
    }
    return soArchiveDb::getManager(2)->getLoadResult(archiveId, nullptr) != 0;
}

// HYPOTHESIS: the "genParamSet" table of the archive: one list of parameter records for each of the five levels.
struct itGenParamSet {
    struct Level {
        itGenParam* m_params;
        u32 m_count;
    } m_levels[5];
};

itGenParam* itGenArchive::getGenParam(int id, int level) {
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return NULL;
    }
    itGenParamSet* set = static_cast<itGenParamSet*>(soArchiveDb::getManager(2)->getPublicAddress(archiveId, "genParamSet", 0, 0, -1));
    if (set == NULL) {
        return NULL;
    }
    if (level < 0 || level >= 5) {
        return NULL;
    }
    itGenParamSet::Level& entry = set->m_levels[level];
    for (u32 i = 0; i < entry.m_count; i++) {
        if ((u32)entry.m_params[i].m_id == (u32)id) {
            return &entry.m_params[i];
        }
    }
    return NULL;
}

itGenParam* itGenArchive::getGenParamFromIndex(u32 index, u32 level) {
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return NULL;
    }
    itGenParamSet* set = static_cast<itGenParamSet*>(soArchiveDb::getManager(2)->getPublicAddress(archiveId, "genParamSet", 0, 0, -1));
    if (set == NULL) {
        return NULL;
    }
    if ((int)level < 0 || (int)level >= 5) {
        return NULL;
    }
    itGenParamSet::Level& entry = set->m_levels[level];
    if (index >= entry.m_count) {
        return NULL;
    }
    return &entry.m_params[index];
}

u32 itGenArchive::getGenParamNum(u32 level) {
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return 0;
    }
    itGenParamSet* set = static_cast<itGenParamSet*>(soArchiveDb::getManager(2)->getPublicAddress(archiveId, "genParamSet", 0, 0, -1));
    if (set == NULL) {
        return 0;
    }
    if ((int)level < 0 || (int)level >= 5) {
        return 0;
    }
    itGenParamSet::Level& entry = set->m_levels[level];
    return entry.m_count;
}

#include <it/it_manager.h>
// TODO: clean up, give variables real names
namespace jj_2B2830 {
struct ItemKey { s32 kind; u32 variation; };
struct Object_vptr_3c_base { char pad[0x3c]; };
class Object_vptr_3c : public Object_vptr_3c_base {
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
virtual void processCamera();
virtual void slot12();
virtual void slot13();
virtual void slot14();
virtual void slot15();
virtual void slot16();
virtual void renderOpa();
virtual void slot18();
virtual void slot19();
virtual void slot20();
virtual void init();
virtual void slot22();
virtual void slot23();
virtual void slot24();
virtual void slot25();
virtual void slot26();
virtual void slot27();
virtual void slot28();
virtual void slot29();
virtual void slot30();
virtual u32 slot31(s32*, ItemKey*);
};
struct BssLoc { u32 unk0, unk4; };
struct BssArc { u32 unk0, unk4, unk8, unkC, unk10, unk14, unk18; };
struct CallOut { s32 a; u32 pad; ItemKey key; };
struct FooCtx { s32 unk0; ItemKey key; u32 unkC; u8 unk10, unk11, unk12; void* unk14; };
extern "C" {
void* fn_800455E0(void*, u32);
u32 fn_800453AC(void*, s32, s32, s32, s32, s32, s32, void*, s32);
}
extern BssLoc lbl_27_bss_7130, lbl_27_bss_7138;
extern BssArc lbl_27_bss_6A38;
extern Object_vptr_3c* g_Stage;
extern char lbl_27_data_790D0[], lbl_805A00C0[];
extern "C" FooCtx* __ct__12itGenArchiveF14itGenSheetKindPCci(FooCtx* self,ItemKey src,s32 arg5,s32 arg6) {
self->unk14=lbl_27_data_790D0;
self->unk0=arg6;
self->key=src;
self->unkC=0xFFFF;
self->unk10=0; self->unk11=0; self->unk12=0;
if (src.kind==0xFF) {
itManager::getInstance();
BssArc snap=lbl_27_bss_6A38;
if(lbl_27_bss_7130.unk4==src.variation) {
if(snap.unkC!=0) { void* mgr=soArchiveDb::getManager(2); self->unkC=(u32)fn_800455E0(mgr,snap.unkC); }
} else if(lbl_27_bss_7138.unk4==src.variation && snap.unk10!=0) {
void* mgr=soArchiveDb::getManager(2); self->unkC=(u32)fn_800455E0(mgr,snap.unk10);
}
} else {
CallOut co;
bool r=*((u8*)itManager::getInstance()+0x149D)==0;
self->unk0=4;
if(g_Stage!=0 && r) {
co.a=0; co.key=src;
g_Stage->slot31(&co.a,&co.key);
s32 found=co.a;
if(found!=0) { void* mgr=soArchiveDb::getManager(2); self->unkC=(u32)fn_800455E0(mgr,found); }
}
}
if(self->unkC==0xFFFF) {
void* mgr=soArchiveDb::getManager(2);
self->unkC=fn_800453AC(mgr,arg5,8,0,0,0,0xFF,lbl_805A00C0,1);
}
return self;
}
}

// TODO: clean up, give variables real names
namespace jj_2B2A30 {
extern "C" {
extern char lbl_27_data_790D0[];
void* getManager__11soArchiveDbFUl(u32 idx);
void fn_80045E64(void* mgr, u32 arg);
void __dl__FPv(void* p);
}

extern "C" void* fn_27_2B2A30(void* p, s32 flag) {
    if (p) {
        *(char**)((char*)p + 0x14) = lbl_27_data_790D0;
        void* mgr = getManager__11soArchiveDbFUl(2);
        fn_80045E64(mgr, *(u32*)((char*)p + 0xc));
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

