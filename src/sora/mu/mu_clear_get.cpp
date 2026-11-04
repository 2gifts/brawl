#include <mu/mu_clear_get.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>
#include <types.h>
#include <ut/ut_relocate.h>

extern MuAnimNameData lbl_804569E0[];

bool muClearGetTask::isEnd() {
    return unk44 >= 10;
}

void muClearGetTask::setCursor() {
    char name[8];
    if (unk100[4]->m_modelAnim->m_anmObjVisRes->GetFrame() != 2.0f) {
        if (unk1098) {
            u32 position = unk1094 + unk1090 * 4;
            unk100[4]->setFrameVisible(0.0f);
            sprintf(name, "pos%02d", position);
            unkB64[1]->Remove(unk100[4]->m_scnMdl);
            unkB64[2]->Remove(unk100[4]->m_scnMdl);
            unkB64[1]->PushBack(unk100[4]->m_scnMdl, name);
        } else {
            u32 position = unk12B0 + 95;
            unk100[4]->setFrameVisible(1.0f);
            sprintf(name, "pos%02d", position);
            unkB64[1]->Remove(unk100[4]->m_scnMdl);
            unkB64[2]->Remove(unk100[4]->m_scnMdl);
            unkB64[2]->PushBack(unk100[4]->m_scnMdl, name);
        }
        if (unk12D8) {
            unkB64[1]->Remove(unk100[7 + unk12B0]->m_scnMdl);
            unkB64[1]->PushBack(unk100[7 + unk12B0]->m_scnMdl, name);
        }
    }
}

void muClearGetTask::releaseHammer(int mode) {
    u32 index;
    char name[8];
    unk12E0 = mode;
    unk12E5 = true;
    unk48 = 0;
    unk12E8 = unk1094 + unk1090 * 4;
    switch (mode) {
    case 0:
        unk1098 = false;
        index = unk12B0;
        break;
    case 2:
        index = unk12B0;
        unk100[7 + unk12B0]->setAnimName(lbl_804569E0, true);
        unk100[7 + unk12B0]->setUpdateRateFlg(0.0f, 0x11);
        unk100[7 + unk12B0]->setFrameFlg(0.0f, 0x11);
        break;
    case 1:
        index = unk12B4 - 1;
        unk100[6 + unk12B4]->setAnimName(lbl_804569E0 + 2, true);
        break;
    }
    sprintf(name, "pos%02d", unk12E8);
    unkB64[1]->PushBack(unk100[7 + index]->m_scnMdl, name);
    unk100[7 + index]->setTrans(&unk12CC);
}

u32 muClearGetTask::setBreakBoxId(u32 id) {
    if (unkE84.unk8[id] != 2) {
        unk10A8[unk12A8] = id;
        if (unk12A8 == 0 && unk44 >= 2) {
            unk44 = 6;
            setPosRot(id);
            unk1094 = id % 4;
            unk1090 = id / 4;
        }
        ++unk12A8;
    }
    return unk12A8;
}

void muClearGetDataManager::setData(u8* buffer, u32 size) {
    utRelocate relocate(buffer, size);
    unk0 = static_cast<muClearGetData*>(relocate.getPublicAddress("muClearGetDataList"));
    for (unk4 = 0; unk0[unk4].unk0 >= 0; unk4++) {
    }
    if (unk4 > 128) {
        unk4 = 128;
    }
    u8* flags = g_GameGlobal->m_record->unk1528;
    for (int i = 0; i < 128; i++) {
        unk8[i] = flags[i];
    }
}

void muClearGetDataManager::setOpenState(u32 index) {
    unk8[index] = 2;
    if (index >= 4 && unk8[index - 4] == 0) {
        unk8[index - 4] = 1;
    }
    if (index + 4 < 128 && unk8[index + 4] == 0) {
        unk8[index + 4] = 1;
    }
    saveFlags();
}

muClearGetDataManager::~muClearGetDataManager() {
    saveFlags();
}
