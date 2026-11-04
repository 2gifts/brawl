#pragma once

#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <nw4r/g3d/g3d_resfile.h>

// HYPOTHESIS: a class local to a TexLoad() member in the original; it is declared under the linkage name the map gives it.
class muPrizeWndTask7TexLoadFv {
public:
    gfFileIOHandle unk0;
    nw4r::g3d::ResFile unk4;

    muPrizeWndTask7TexLoadFv();
    ~muPrizeWndTask7TexLoadFv();
};

// HYPOTHESIS: uninspected pointer members retain opaque types.
class muPrizeWndTask : public gfTask {
public:
    MuObject* unk40[5];
    nw4r::g3d::ScnMdlExpand* unk54[1];
    void* unk58;
    MuMsg* unk5C;
    muMenuController unk60;
    bool unk108;
    int unk10C;
    int unk110;
    int unk114;
    int unk118;
    u8 unk11C[0x100];
    int unk21C;
    int unk220;
    void* unk224;
    void* unk228;
    int unk22C;
    int unk230;
    muPrizeWndTask7TexLoadFv unk234[16];
    bool unk2B4;
    void* unk2B8;
    void* unk2BC;
    void* unk2C0;
    muPrizeWndTask7TexLoadFv unk2C4[16];
    void* unk344;
    void* unk348;
    muPrizeWndTask7TexLoadFv unk34C[16];
    int unk3CC;
    bool unk3D0;

    muPrizeWndTask();
    static muPrizeWndTask* create(nw4r::g3d::ResFile*, void*, void*, int, void*, int);
    void createObjResFile(const void*, int, nw4r::g3d::ResFile*, int);
    void initMsg(void*);
    virtual void processDefault();
    void setWndSealPrize(int);
};
static_assert(sizeof(muPrizeWndTask) == 0x3D4, "Class is wrong size!");
