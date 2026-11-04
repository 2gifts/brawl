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

void muClearGetTask::setPosRot(u32 id) {
    u32 row = id / 4;
    u32 count = 0;
    u32 position = unk10A0;
    if (row < unk1090) {
        for (u32 current = unk1090 - 1; current >= row && current != 0; --current) {
            if (position > 3 && position > current + 1) {
                --position;
                ++count;
            }
        }
        unk12AC -= 9.1f * count;
        unk10A0 -= count;
    } else {
        for (u32 current = unk1090 + 1; current <= row; ++current) {
            if (position < 28 && current > position + 1) {
                ++position;
                ++count;
            }
        }
        unk12AC += 9.1f * count;
        unk10A0 += count;
    }
    if (count != 0) {
        unk10A4 = true;
    }
}

void muClearGetTask::setPrizeModel(u32 id, int state) {
    char name[8];
    sprintf(name, "pos%02d", id);
    if (state != 0) {
        switch (unkE84.unk0[id].unk4Words[0]) {
        case 0:
            unkB64[1]->PushBack(unkB64[4 + id], name);
            break;
        case 1:
            unkB64[1]->PushBack(unkB64[4 + id], name);
            if (state == 1) {
                unk100[0x219 + id]->setFrameFlg(1.0f, 0x10);
            } else {
                unk100[0x219 + id]->setFrameFlg(0.0f, 0x10);
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            unkB64[1]->PushBack(unk100[0x219 + id]->m_scnMdl, name);
            break;
        }
        for (u32 index = 0; index < 5; ++index) {
            if (id == unk12B8Words[index]) {
                if (state == 1) {
                    unkB64[1]->PushBack(unk100[12 + index]->m_scnMdl, name);
                } else {
                    unkB64[1]->Remove(unk100[12 + index]->m_scnMdl);
                }
                break;
            }
        }
    }
}

bool muClearGetTask::calcHammerClose() {
    char name[8];
    u32 index = unk12B0 + 1;
    sprintf(name, "pos%02d", index + 94);
    Vec3f current;
    Vec3f target;
    target = unk100[3]->getGlobalPosition(name);
    current = unk100[7 + index]->getGlobalPosition();
    float distance = (target.m_x - current.m_x) / 6.0f;
    if (static_cast<float>(__fabs(target.m_x - (current.m_x + distance))) < 0.1f) {
        for (; index < unk12B4; ++index) {
            unkB64[2]->Remove(unk100[7 + index]->m_scnMdl);
        }
        unk12B0 = 0;
        --unk12B4;
        for (index = 0; index < unk12B4; ++index) {
            unk100[7 + index]->setTrans(&unk12CC);
            sprintf(name, "pos%02d", index + 95);
            unkB64[2]->PushBack(unk100[7 + index]->m_scnMdl, name);
        }
        return false;
    }
    for (; index < unk12B4; ++index) {
        Vec3f translation;
        translation = unk100[7 + index]->m_trans;
        translation.m_x += distance;
        unk100[7 + index]->setTrans(&translation);
    }
    return true;
}
