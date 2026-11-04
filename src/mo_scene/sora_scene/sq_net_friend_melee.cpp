#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <sq/sq_net_friend_melee.h>
#include <sr/sr_common.h>
#include <types.h>

sqNetFriendMelee* sqNetFriendMelee::create() {
    return new (Heaps::GlobalMode) sqNetFriendMelee("sqNetFriendMelee");
}

void sqNetFriendMelee::start() {
    unk8 = 0;
    m_isNetOnline = g_gfApplication->unk110[0] != 0;
    g_GameGlobal->initSelCharData();
    switch (gfSceneManager::getInstance()->unk31C) {
    case 1:
        g_GameGlobal->m_selCharData->unk6 = 1;
        break;
    }
}

void sqNetFriendMelee::exit() { }
