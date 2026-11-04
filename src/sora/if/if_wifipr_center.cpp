#include <if/if_wifipr_center.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
#include <types.h>

// HYPOTHESIS: one HUD model. start/end select the object slots (one model when start >= end).
struct IfWifiprCenterModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

__declspec(section ".sdata2") IfWifiprCenterModel lbl_805A2708 = {"WifiInfNews0000_TopN", 0, 0, 4};

IfWifiprCenter::IfWifiprCenter() {
    m_msg = NULL;
    m_msgData = NULL;
    m_msgId = -1;
    m_object = NULL;
}

IfWifiprCenter::~IfWifiprCenter() {
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
    for (int i = 0; i < 1; i++) {
        if (m_object != NULL) {
            delete m_object;
            m_object = NULL;
        }
    }
}

void IfWifiprCenter::createModel(nw4r::g3d::ResFile* res, void* msgData) {
    MuMsg* msg;
    int count = 1;
    int i;
    IfWifiprCenterModel* const info = &lbl_805A2708;
    u8 start;
    u8 end;
    char animName[0x40];
    end = info->m_end;
    start = info->m_start;
    if (start < end) {
        count = end - start;
    }
    sprintf(animName, "%s__0", info->m_name);
    for (i = 0; i < count; i++) {
        (&m_object)[info->m_start + i] = MuObject::create(res, info->m_name, info->m_priority, NULL, Heaps::InfoInstance);
        (&m_object)[info->m_start + i]->changeAnimN(animName);
    }
    m_msgData = msgData;
    msg = MuMsg::create(8, Heaps::InfoInstance, Heaps::MenuInstance);
    m_msg = msg;
    msg->allocMsgBuf(0x100, 2);
    msg->attachScnMdlSimple(0, m_object->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(0, 0xF0, 0xF0, 0xF0, 0xFF);
    msg->setFontType(0, 4);
    msg->getMessage()->setBufField34(0, 7);
    msg->attachScnMdlSimple(1, m_object->m_sceneModel, 1, 0.0625f);
    msg->setFontType(1, 4);
    msg->getMessage()->setBufField34(1, 7);
}

void IfWifiprCenter::startMelee(nw4r::g3d::ScnGroup* group) {
    group->Insert(group->sceneItemsCount, m_object->m_sceneModel);
}

void IfWifiprCenter::dispMsg(int msgId) {
    m_msg->printIndex(0, msgId, m_msgData);
    MuObject* object = m_object;
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    m_msgId = msgId;
}

void IfWifiprCenter::startBeginMeleeAnim() {
    m_object->changeAnimN(1, 0x10, true, false);
}
