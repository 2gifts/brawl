// Local BrawlHeaders shadow: declares create, createModel, destroyModel, startMelee and the setPlayerWiFiState methods.

#pragma once

class IfWifiprMngr {
public:
    void main();
    static IfWifiprMngr* create();
    void createModel();
    void destroyModel();
    void startMelee();
    void setPlayerWiFiStateNone(int playerNo);
    void setPlayerWiFiStateAccept(int playerNo);
    void setPlayerWiFiStateSelChar(int playerNo);
    void setPlayerWiFiStateSelStage(int playerNo);
};
// TODO size assertion

extern IfWifiprMngr* g_IfWifiprMngr;
