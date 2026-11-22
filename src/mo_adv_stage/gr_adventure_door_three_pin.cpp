#include <revolution/GX/GXTypes.h>
#include <sora_adv_stage/gr_adventure_door_bomb_factory.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>
#include <st/st_trigger.h>
#include <types.h>

// MATCH-ONLY: the zero lookup statics of the material functions are in .data.
#pragma explicit_zero_data on

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<27grAdventureDoorThreePinData, ...>);
// the names of the triggers are guesses from how the door uses them.
struct grAdventureDoorThreePinData {
    u8 m_unk0[0x90];
    stTriggerData m_openTrigger;    // 0x90 the fourth status trigger: the door opens
    stTriggerData m_pinTriggers[3]; // 0x94 the blue, green and red pins
};

// The door of the bomb factory that is locked by three pins (blue, green, red): every pin trigger turns the colors of
// its switch and door parts on; when the last trigger is set the door can be entered.
class grAdventureDoorThreePin : public grAdventureDoorBombFactory, public grAdventureNotifyGimmickTriggerStatus {
public:
    u8 m_unk1B8[4];
    grAdventureDoorThreePinData* m_threePinData; // 0x1BC
    GXColor m_onColors[16];                      // 0x1C0 the colors of the materials of the model when they are on
    u8 m_isOpen;                                 // 0x200

    grAdventureDoorThreePin(const char* taskName) : grAdventureDoorBombFactory(taskName) {
        m_threePinData = NULL;
        m_isOpen = 0;
    }
    static grAdventureDoorThreePin* create(int mdlIndex, u32 jumpData, const char* taskName);
    virtual ~grAdventureDoorThreePin();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();
    virtual void update(float deltaFrame);
    virtual void setValidAttachedEffect(u32 valid);
    virtual void notifyEventOnTrigger(int index);
    virtual void notifyEventOffTrigger(int index);
    void offTurningMaterial();
    void getOnTurningMaterial();
    void onTurningMaterial(int index);
};
static_assert(sizeof(grAdventureDoorThreePin) == 0x204, "Class is wrong size!");

// HYPOTHESIS: a color made from the u32 the material getters return (r in the high byte).
struct grThreePinColor : public GXColor {
    grThreePinColor(u32 color) {
        r = color >> 24;
        g = color >> 16;
        b = color >> 8;
        a = color;
    }
};

grAdventureDoorThreePin::~grAdventureDoorThreePin() { }

grAdventureDoorThreePin* grAdventureDoorThreePin::create(int mdlIndex, u32 jumpData, const char* taskName) {
    grAdventureDoorThreePin* door = new (Heaps::StageInstance) grAdventureDoorThreePin(taskName);
    if (door != NULL) {
        door->setJumpData(jumpData);
        door->setMdlIndex(mdlIndex);
    }
    return door;
}

void grAdventureDoorThreePin::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureDoorBombFactory::startup(data, unk1, layerType);
    m_threePinData = static_cast<grAdventureDoorThreePinData*>(m_gimmickData);
    getOnTurningMaterial();
    offTurningMaterial();
    stTrigger* trigger = g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(0), &m_threePinData->m_pinTriggers[0]);
    createGimmickStatusTrigger(this, trigger, &m_threePinData->m_pinTriggers[0], 1, 0);
    trigger = g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(0), &m_threePinData->m_pinTriggers[1]);
    createGimmickStatusTrigger(this, trigger, &m_threePinData->m_pinTriggers[1], 1, 1);
    trigger = g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(0), &m_threePinData->m_pinTriggers[2]);
    createGimmickStatusTrigger(this, trigger, &m_threePinData->m_pinTriggers[2], 1, 2);
    trigger = g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(0), &m_threePinData->m_openTrigger);
    createGimmickStatusTrigger(this, trigger, &m_threePinData->m_openTrigger, 1, 3);
    disableArea();
}

void grAdventureDoorThreePin::setInitializeFlag() {
    grAdventureDoor::setInitializeFlag();
    if (g_stTriggerMng->getTriggerFlag(&m_threePinData->m_pinTriggers[0], true) == true) {
        notifyEventOnTrigger(0);
    }
    if (g_stTriggerMng->getTriggerFlag(&m_threePinData->m_pinTriggers[1], true) == true) {
        notifyEventOnTrigger(1);
    }
    if (g_stTriggerMng->getTriggerFlag(&m_threePinData->m_pinTriggers[2], true) == true) {
        notifyEventOnTrigger(2);
    }
    if (g_stTriggerMng->getTriggerFlag(&m_threePinData->m_openTrigger, true) == true) {
        notifyEventOnTrigger(3);
    }
}

void grAdventureDoorThreePin::update(float deltaFrame) {
    grAdventureDoorBombFactory::update(deltaFrame);
}

void grAdventureDoorThreePin::offTurningMaterial() {
    GXColor off0 = {0x00, 0x06, 0x7F, 0xFF};
    GXColor off1 = {0x00, 0x00, 0x00, 0xFF};
    u32 off2 = 0;
    u32 off3 = 0;
    u32 off4 = 0;
    GXColor off5 = {0x04, 0x7F, 0x00, 0xFF};
    GXColor off6 = {0x00, 0x00, 0x00, 0xFF};
    u32 off7 = 0;
    u32 off8 = 0;
    u32 off9 = 0;
    GXColor off10 = {0x7F, 0x00, 0x00, 0xFF};
    GXColor off11 = {0x00, 0x00, 0x00, 0xFF};
    GXColor off12 = {0x00, 0x00, 0x00, 0xFF};
    u32 off13 = 0;
    GXColor off14 = {0x00, 0x00, 0x00, 0xFF};
    u32 off15 = 0;
    {
        static u32 s_off0Id = 0;
        static u32 s_off0Model = 0;
        setMaterialColor(&s_off0Id, "SwBlue", reinterpret_cast<u32*>(&off0), &s_off0Model);
    }
    {
        static u32 s_off1Id = 0;
        static u32 s_off1Model = 0;
        setMaterialColor(&s_off1Id, "SwBlueGlow", reinterpret_cast<u32*>(&off1), &s_off1Model);
    }
    {
        static u32 s_off2Id = 1;
        static u32 s_off2Model = 1;
        setMaterialTevColor(&s_off2Id, "FacDoorBlue", &off2, &s_off2Model);
    }
    {
        static u32 s_off3Id = 1;
        static u32 s_off3Model = 1;
        setMaterialTevColor(&s_off3Id, "FacDoorBlue1", &off3, &s_off3Model);
    }
    {
        static u32 s_off4Id = 1;
        static u32 s_off4Model = 1;
        setMaterialTevColor(&s_off4Id, "FacDoorBlue2", &off4, &s_off4Model);
    }
    {
        static u32 s_off5Id = 0;
        static u32 s_off5Model = 0;
        setMaterialColor(&s_off5Id, "SwGreen", reinterpret_cast<u32*>(&off5), &s_off5Model);
    }
    {
        static u32 s_off6Id = 0;
        static u32 s_off6Model = 0;
        setMaterialColor(&s_off6Id, "SwGreenGlow", reinterpret_cast<u32*>(&off6), &s_off6Model);
    }
    {
        static u32 s_off7Id = 1;
        static u32 s_off7Model = 1;
        setMaterialTevColor(&s_off7Id, "FacDoorGreen", &off7, &s_off7Model);
    }
    {
        static u32 s_off8Id = 1;
        static u32 s_off8Model = 1;
        setMaterialTevColor(&s_off8Id, "FacDoorGreen1", &off8, &s_off8Model);
    }
    {
        static u32 s_off9Id = 1;
        static u32 s_off9Model = 1;
        setMaterialTevColor(&s_off9Id, "FacDoorGreen2", &off9, &s_off9Model);
    }
    {
        static u32 s_off10Id = 0;
        static u32 s_off10Model = 0;
        setMaterialColor(&s_off10Id, "SwRed", reinterpret_cast<u32*>(&off10), &s_off10Model);
    }
    {
        static u32 s_off11Id = 0;
        static u32 s_off11Model = 0;
        setMaterialColor(&s_off11Id, "SwRedGlow", reinterpret_cast<u32*>(&off11), &s_off11Model);
    }
    {
        static u32 s_off12Id = 1;
        static u32 s_off12Model = 1;
        setMaterialTevColor(&s_off12Id, "FacDoorRed", reinterpret_cast<u32*>(&off12), &s_off12Model);
    }
    {
        static u32 s_off13Id = 1;
        static u32 s_off13Model = 1;
        setMaterialTevColor(&s_off13Id, "FacDoorRed1", &off13, &s_off13Model);
    }
    {
        static u32 s_off14Id = 0;
        static u32 s_off14Model = 0;
        setMaterialColor(&s_off14Id, "doorGlow01", reinterpret_cast<u32*>(&off14), &s_off14Model);
    }
    {
        static u32 s_off15Id = 1;
        static u32 s_off15Model = 0;
        setMaterialTevColor(&s_off15Id, "FacDoorWhite", &off15, &s_off15Model);
    }
}

// TODO: clean up - 44%: the target copies the color through a stack temporary but keeps b and a in registers.
void grAdventureDoorThreePin::getOnTurningMaterial() {
    {
        static int s_get0Id = 0;
        static u32 s_get0Model = 0;
        u32 color0 = getMaterialColor(&s_get0Id, "SwBlue", &s_get0Model);
        GXColor c0 = {color0 >> 24, color0 >> 16, color0 >> 8, color0};
        m_onColors[0] = c0;
    }
    {
        static int s_get1Id = 0;
        static u32 s_get1Model = 0;
        u32 color1 = getMaterialColor(&s_get1Id, "SwBlueGlow", &s_get1Model);
        GXColor c1 = {color1 >> 24, color1 >> 16, color1 >> 8, color1};
        m_onColors[1] = c1;
    }
    {
        static int s_get2Id = 1;
        static u32 s_get2Model = 1;
        u32 color2 = getMaterialTevColor(&s_get2Id, "FacDoorBlue", &s_get2Model);
        GXColor c2 = {color2 >> 24, color2 >> 16, color2 >> 8, color2};
        m_onColors[2] = c2;
    }
    {
        static int s_get3Id = 1;
        static u32 s_get3Model = 1;
        u32 color3 = getMaterialTevColor(&s_get3Id, "FacDoorBlue1", &s_get3Model);
        GXColor c3 = {color3 >> 24, color3 >> 16, color3 >> 8, color3};
        m_onColors[3] = c3;
    }
    {
        static int s_get4Id = 1;
        static u32 s_get4Model = 1;
        u32 color4 = getMaterialTevColor(&s_get4Id, "FacDoorBlue2", &s_get4Model);
        GXColor c4 = {color4 >> 24, color4 >> 16, color4 >> 8, color4};
        m_onColors[4] = c4;
    }
    {
        static int s_get5Id = 0;
        static u32 s_get5Model = 0;
        u32 color5 = getMaterialColor(&s_get5Id, "SwGreen", &s_get5Model);
        GXColor c5 = {color5 >> 24, color5 >> 16, color5 >> 8, color5};
        m_onColors[5] = c5;
    }
    {
        static int s_get6Id = 0;
        static u32 s_get6Model = 0;
        u32 color6 = getMaterialColor(&s_get6Id, "SwGreenGlow", &s_get6Model);
        GXColor c6 = {color6 >> 24, color6 >> 16, color6 >> 8, color6};
        m_onColors[6] = c6;
    }
    {
        static int s_get7Id = 1;
        static u32 s_get7Model = 1;
        u32 color7 = getMaterialTevColor(&s_get7Id, "FacDoorGreen", &s_get7Model);
        GXColor c7 = {color7 >> 24, color7 >> 16, color7 >> 8, color7};
        m_onColors[7] = c7;
    }
    {
        static int s_get8Id = 1;
        static u32 s_get8Model = 1;
        u32 color8 = getMaterialTevColor(&s_get8Id, "FacDoorGreen1", &s_get8Model);
        GXColor c8 = {color8 >> 24, color8 >> 16, color8 >> 8, color8};
        m_onColors[8] = c8;
    }
    {
        static int s_get9Id = 1;
        static u32 s_get9Model = 1;
        u32 color9 = getMaterialTevColor(&s_get9Id, "FacDoorGreen2", &s_get9Model);
        GXColor c9 = {color9 >> 24, color9 >> 16, color9 >> 8, color9};
        m_onColors[9] = c9;
    }
    {
        static int s_get10Id = 0;
        static u32 s_get10Model = 0;
        u32 color10 = getMaterialColor(&s_get10Id, "SwRed", &s_get10Model);
        GXColor c10 = {color10 >> 24, color10 >> 16, color10 >> 8, color10};
        m_onColors[10] = c10;
    }
    {
        static int s_get11Id = 0;
        static u32 s_get11Model = 0;
        u32 color11 = getMaterialColor(&s_get11Id, "SwRedGlow", &s_get11Model);
        GXColor c11 = {color11 >> 24, color11 >> 16, color11 >> 8, color11};
        m_onColors[11] = c11;
    }
    {
        static int s_get12Id = 1;
        static u32 s_get12Model = 1;
        u32 color12 = getMaterialTevColor(&s_get12Id, "FacDoorRed", &s_get12Model);
        GXColor c12 = {color12 >> 24, color12 >> 16, color12 >> 8, color12};
        m_onColors[12] = c12;
    }
    {
        static int s_get13Id = 1;
        static u32 s_get13Model = 1;
        u32 color13 = getMaterialTevColor(&s_get13Id, "FacDoorRed1", &s_get13Model);
        GXColor c13 = {color13 >> 24, color13 >> 16, color13 >> 8, color13};
        m_onColors[13] = c13;
    }
    {
        static int s_get14Id = 0;
        static u32 s_get14Model = 0;
        u32 color14 = getMaterialColor(&s_get14Id, "doorGlow01", &s_get14Model);
        GXColor c14 = {color14 >> 24, color14 >> 16, color14 >> 8, color14};
        m_onColors[14] = c14;
    }
    {
        static int s_get15Id = 1;
        static u32 s_get15Model = 0;
        u32 color15 = getMaterialTevColor(&s_get15Id, "FacDoorWhite", &s_get15Model);
        GXColor c15 = {color15 >> 24, color15 >> 16, color15 >> 8, color15};
        m_onColors[15] = c15;
    }
}

void grAdventureDoorThreePin::onTurningMaterial(int index) {
    if (index == 0) {
        {
            static u32 s_on0Id = 0;
            static u32 s_on0Model = 0;
            setMaterialColor(&s_on0Id, "SwBlue", reinterpret_cast<u32*>(&m_onColors[0]), &s_on0Model);
        }
        {
            static u32 s_on1Id = 0;
            static u32 s_on1Model = 0;
            setMaterialColor(&s_on1Id, "SwBlueGlow", reinterpret_cast<u32*>(&m_onColors[1]), &s_on1Model);
        }
        {
            static u32 s_on2Id = 1;
            static u32 s_on2Model = 1;
            setMaterialTevColor(&s_on2Id, "FacDoorBlue", reinterpret_cast<u32*>(&m_onColors[2]), &s_on2Model);
        }
        {
            static u32 s_on3Id = 1;
            static u32 s_on3Model = 1;
            setMaterialTevColor(&s_on3Id, "FacDoorBlue1", reinterpret_cast<u32*>(&m_onColors[3]), &s_on3Model);
        }
        {
            static u32 s_on4Id = 1;
            static u32 s_on4Model = 1;
            setMaterialTevColor(&s_on4Id, "FacDoorBlue2", reinterpret_cast<u32*>(&m_onColors[4]), &s_on4Model);
        }
    }
    if (index == 1) {
        {
            static u32 s_on5Id = 0;
            static u32 s_on5Model = 0;
            setMaterialColor(&s_on5Id, "SwGreen", reinterpret_cast<u32*>(&m_onColors[5]), &s_on5Model);
        }
        {
            static u32 s_on6Id = 0;
            static u32 s_on6Model = 0;
            setMaterialColor(&s_on6Id, "SwGreenGlow", reinterpret_cast<u32*>(&m_onColors[6]), &s_on6Model);
        }
        {
            static u32 s_on7Id = 1;
            static u32 s_on7Model = 1;
            setMaterialTevColor(&s_on7Id, "FacDoorGreen", reinterpret_cast<u32*>(&m_onColors[7]), &s_on7Model);
        }
        {
            static u32 s_on8Id = 1;
            static u32 s_on8Model = 1;
            setMaterialTevColor(&s_on8Id, "FacDoorGreen1", reinterpret_cast<u32*>(&m_onColors[8]), &s_on8Model);
        }
        {
            static u32 s_on9Id = 1;
            static u32 s_on9Model = 1;
            setMaterialTevColor(&s_on9Id, "FacDoorGreen2", reinterpret_cast<u32*>(&m_onColors[9]), &s_on9Model);
        }
    }
    if (index == 2) {
        {
            static u32 s_on10Id = 0;
            static u32 s_on10Model = 0;
            setMaterialColor(&s_on10Id, "SwRed", reinterpret_cast<u32*>(&m_onColors[10]), &s_on10Model);
        }
        {
            static u32 s_on11Id = 0;
            static u32 s_on11Model = 0;
            setMaterialColor(&s_on11Id, "SwRedGlow", reinterpret_cast<u32*>(&m_onColors[11]), &s_on11Model);
        }
        {
            static u32 s_on12Id = 1;
            static u32 s_on12Model = 1;
            setMaterialTevColor(&s_on12Id, "FacDoorRed", reinterpret_cast<u32*>(&m_onColors[12]), &s_on12Model);
        }
        {
            static u32 s_on13Id = 1;
            static u32 s_on13Model = 1;
            setMaterialTevColor(&s_on13Id, "FacDoorRed1", reinterpret_cast<u32*>(&m_onColors[13]), &s_on13Model);
        }
    }
    if (index == 3) {
        {
            static u32 s_on14Id = 0;
            static u32 s_on14Model = 0;
            setMaterialColor(&s_on14Id, "doorGlow01", reinterpret_cast<u32*>(&m_onColors[14]), &s_on14Model);
        }
        {
            static u32 s_on15Id = 1;
            static u32 s_on15Model = 0;
            setMaterialTevColor(&s_on15Id, "FacDoorWhite", reinterpret_cast<u32*>(&m_onColors[15]), &s_on15Model);
        }
    }
}

void grAdventureDoorThreePin::notifyEventOnTrigger(int index) {
    onTurningMaterial(index);
    if (index == 3) {
        m_isOpen = 1;
        if (m_isValid) {
            enableArea();
        }
    }
}

void grAdventureDoorThreePin::notifyEventOffTrigger(int index) { }

void grAdventureDoorThreePin::setValidAttachedEffect(u32 valid) {
    Ground::setValidAttachedEffect(valid);
    if (valid == 1) {
        enableHit(0, 0);
        if (m_isOpen != 0) {
            enableArea();
        }
    }
    if (valid == 0) {
        disableHit(0, 0);
        disableAttack(0);
        disableArea();
    }
}
