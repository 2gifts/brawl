#pragma once

#include <StaticAssert.h>
#include <sr/sr_common.h>
#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

class IfWifiprPlayer {
public:
    // HYPOTHESIS: init runs when the state is entered; main runs every frame and returns the next state.
    struct DispStateFunc {
        void (*m_init)(IfWifiprPlayer* player);
        int (*m_main)(IfWifiprPlayer* player);
    };

    int m_mode; // 0x00 HYPOTHESIS: 0 is the in-match model (info heap); any other value is the menu model
    u8 m_playerNo; // 0x04
    int m_state; // 0x08
    MuObject* m_object; // 0x0C
    int unk10; // 0x10
    // HYPOTHESIS: group the select-character model is attached to (startSelChar stores the pointer here).
    nw4r::g3d::ScnGroup* m_scnGroup; // 0x14
    Heaps::HeapType m_heap; // 0x18

    IfWifiprPlayer(int playerNo, int mode);
    virtual ~IfWifiprPlayer();

    void createModel(nw4r::g3d::ResFile* res, int priority);
    void setWifiStatePlayerColor(int color);
    void setWifiDispState(int state);
    void main();

    static void createPlayerLocator(nw4r::g3d::ResFile* res);
    static void destroyPlayerLocator();
    static void startMelee(nw4r::g3d::ScnGroup* group);

    static void wifiDispStateNoneInit(IfWifiprPlayer* player);
    static void wifiDispStateAcceptInit(IfWifiprPlayer* player);
    static int wifiDispStateAcceptMain(IfWifiprPlayer* player);
    static void wifiDispStateSelCharInit(IfWifiprPlayer* player);
    static int wifiDispStateSelCharMain(IfWifiprPlayer* player);
    static void wifiDispStateSelStageInit(IfWifiprPlayer* player);
    static int wifiDispStateSelStageMain(IfWifiprPlayer* player);
    static void wifiDispStatePracticeInit(IfWifiprPlayer* player);
    static int wifiDispStatePracticeMain(IfWifiprPlayer* player);
    static void wifiDispStateEscapeInit(IfWifiprPlayer* player);
    static int wifiDispStateEscapeMain(IfWifiprPlayer* player);
    static void wifiDispStateMeleeInit(IfWifiprPlayer* player);
};
static_assert(sizeof(IfWifiprPlayer) == 0x20, "Class is the wrong size!");
