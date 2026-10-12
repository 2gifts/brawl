#include <types.h>
#include <gm/gm_global.h>
#include <gm/gm_syssave_task.h>

extern "C" void* lbl_8042E8D8[3];
extern "C" Heaps::HeapType lbl_8059CE48;
extern "C" gmSysSave* lbl_805A00FC;

// HYPOTHESIS: the 0x14-byte block is the gmSysSave instance; base link, vtable slot and status fields are set inline
struct gmSysSaveObj {
    gmSysSaveObj* unk0;
    void* unk4;
    s32 unk8;
    s32 unkC;
    u8 unk10;
};

gmSysSave* gmSysSave::create(Heaps::HeapType heap) {
    g_GameGlobal->m_appData->_0x0[0x28] |= 0x20;
    lbl_8059CE48 = heap;
    gmSysSaveObj* obj = new (heap) gmSysSaveObj;
    if (obj != NULL) {
        obj->unk0 = NULL;
        obj->unk4 = &lbl_8042E8D8;
        obj->unk8 = -1;
        obj->unkC = 0;
        obj->unk10 = 0;
    }
    lbl_805A00FC = (gmSysSave*) obj;
    obj->unk8 = 0;
    obj->unkC = 0;
    return lbl_805A00FC;
}
