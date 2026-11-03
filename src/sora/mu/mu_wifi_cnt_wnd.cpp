#include <cstring>
#include <mu/mu_object.h>
#include <mu/mu_wifi_cnt_wnd.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// HYPOTHESIS: one entry of the node table createObjResFile walks; the type's real name is unknown.
struct UnkModelNodeInfo {
    const char* m_name;
    u8 unk4;
    u8 unk5;
    u8 unk6;
};

muWifiCntWndTask::muWifiCntWndTask()
    : gfTask("WifiCntWnd", Category_Menu, 15, 8, true), unk58(0), unk5C(0), unk60(1), unk64(0) {
    unk2C_b1 = false;
    for (int i = 0; i < 5; i++) {
        unk40[i] = 0;
    }
    unk54 = 0;
}

void muWifiCntWndTask::createObjResFile(const void* nodeInfo, int count, nw4r::g3d::ResFile* resFile,
                                       int priority, HeapType heap) {
    int i;
    int j;
    const UnkModelNodeInfo* p = static_cast<const UnkModelNodeInfo*>(nodeInfo);
    char name[0x40];
    for (i = 0; i < count; i++, p++) {
        int copies = p->unk4 < p->unk5 ? p->unk5 - p->unk4 : 1;
        for (j = 0; j < copies; j++) {
            MuObject* obj = MuObject::create(resFile, p->m_name, p->unk6 + priority, 0, heap);
            unk40[p->unk4 + j] = obj;
            strcpy(name, p->m_name);
            strcat(name, "__0");
            obj->changeAnimN(name);
            if (obj->m_modelAnim) {
                obj->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muWifiCntWndTask::setWndStyle(int style, int digits) {
    int maxDigits = 2;
    if (style == 0) {
        maxDigits = 3;
    }
    int count = digits > 1 ? digits : 1;
    unk60 = count < maxDigits ? count : maxDigits;
    unk64 = style;
    nw4r::g3d::ScnMdlExpand* group = unk54;
    while (!group->Empty()) {
        group->PopBack();
    }
    float frame;
    if (unk64 == 0) {
        dispNum(0, unk60, 1, 0, 1);
        frame = unk60 - 1;
    } else {
        dispNum(0, 2, 0, 0, 1);
        dispNum(0, 2, 1, 2, 3);
        frame = unk60 + 2;
    }
    unk40[0]->setFrameNode(frame);
}
