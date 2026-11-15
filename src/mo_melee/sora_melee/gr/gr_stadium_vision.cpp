#include <gr/gr_stadium_vision.h>
#include <ms/ms_message.h>

// The big screen of the Pokemon Stadium stages: a stadium model with five text buffers drawn on its screen node.

grStadiumVision* grStadiumVision::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grStadiumVision* ground = new (Heaps::StageInstance) grStadiumVision(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    ground->setupMelee();
    return ground;
}

grStadiumVision::grStadiumVision(const char* taskName) : grStadium(taskName) {
    m_unk19E = 0;
    m_isDisplay = true;
    m_flag6 = false;
    m_flag5 = false;
    m_flag4 = false;
    m_unk16C = 60.0f;
    m_screenKind = 0;
    m_unk19E = 0;
    m_message = NULL;
    m_miscData = NULL;
    m_unk174 = 0.9f;
    m_unk178 = 0.9f;
    m_unk17C = 38.0f;
    m_unk180 = 32.0f * m_unk174;
    m_unk184 = 0.6f;
    m_unk190 = -190;
    m_unk194 = -120;
    m_unk192 = 190;
    m_unk196 = 90;
    m_unk198 = -11;
    m_unk19A = -22;
    m_unk19C = -1;
    m_screenMode = 0;
    m_unk188 = 4.5f * m_unk180;
    m_unk18C = 1.6f * m_unk174;
}

void grStadiumVision::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_message = new (Heaps::StageInstance) Message(10, Heaps::StageInstance);
    m_message->allocMsgBuf(0x100, 5, Heaps::StageInstance);
    for (int i = 0; i < 5; i++) {
        m_message->attachMsgBuf(i, m_sceneModels[0], "AuroraVisionFont", 0, 3, 0.0625f);
    }
}

grStadiumVision::~grStadiumVision() {
    if (m_message != NULL) {
        delete m_message;
        m_message = NULL;
    }
}

void grStadiumVision::setDisplay(bool display) {
    m_isDisplay = display;
    if (!m_isDisplay) {
        for (int i = 0; i < 5; i++) {
            m_message->changeMsgBuf(i);
            m_message->clearMsgBuf();
        }
    }
}
