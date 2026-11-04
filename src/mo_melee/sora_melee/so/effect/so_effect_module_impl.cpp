#include <ec/ec_mgr.h>
#include <so/effect/so_effect_continual.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/effect/so_effect_time.h>
#include <so/so_array.h>
#include <so/so_module_accesser.h>
#include <types.h>

// ecMgr::killEffectGroup (the map name); src/mo_melee/sora_melee/st/stage.cpp calls it by this stand-in name.
extern "C" void fn_80060E20(ecMgr* mgr, int group, int unk2, int unk3, int unk4, int unk5);

void soEffectModuleImpl::activate() {
    removeAllAfterImage(0, 0);
    removeAllContinual();
    removeAllTime();
    unk135_0 = false;
    unk54 = 0;
    unk135_2 = false;
    unk64 = 0;
    m_isSyncVisibility = true;
    unk74 = -1;
    unk134 = true;
}

void soEffectModuleImpl::pushGroup(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
}

void soEffectModuleImpl::reqTime(EfID effectID, int frame, Vec3f* pos, Vec3f* rot, float scale, u32 p6) {
    u32 handle = req(effectID, pos, rot, scale, p6, -1);
    if (handle != 0) {
        soEffectTime time;
        time.set(effectID, handle, frame);
        m_times->push(time);
    }
}

void soEffectModuleImpl::reqTimeFollow(EfID effectID, int nodeId, int frame, Vec3f* pos, Vec3f* rot, float scale, u32 p7) {
    u32 handle = reqFollow(effectID, nodeId, pos, rot, scale, false, p7, 0, -1);
    if (handle != 0) {
        soEffectTime time;
        time.set(effectID, handle, frame);
        m_times->push(time);
    }
}

void soEffectModuleImpl::removeContinual(u32 index) {
    soEffectContinual& continual = m_continuals->at(index);
    continual.m_isActive = false;
}

void soEffectModuleImpl::removeAllContinual() {
    int num = m_continuals->size();
    for (int i = 0; i < num; i++) {
        if (m_continuals->at(i).m_isActive == true) {
            m_continuals->at(i).m_isActive = false;
        }
    }
}

void soEffectModuleImpl::removeTime(int effectID) {
    for (int i = m_times->size() - 1; i >= 0; i--) {
        if (effectID == m_times->at(i).m_effectId) {
            m_times->at(i).end();
            m_times->erase(i);
        }
    }
}

void soEffectModuleImpl::removeAllTime() {
    int num = m_times->size();
    for (int i = 0; i < num; i++) {
        if (m_times->at(i).m_frame > 0) {
            m_times->at(i).end();
        }
    }
    m_times->clear();
}

void soEffectModuleImpl::remove(int handle) {
    g_ecMgr->endEffect(handle);
}

void soEffectModuleImpl::kill(int handle, bool p2, bool p3) {
    g_ecMgr->killEffect(handle, p2, p3);
}

void soEffectModuleImpl::setPos(int handle, Vec3f* pos) {
    g_ecMgr->setPos(handle, pos);
}

void soEffectModuleImpl::setRot(int handle, Vec3f* rot) {
    g_ecMgr->setRot(handle, rot);
}

void soEffectModuleImpl::setVisible(int handle, bool isVisible) {
    g_ecMgr->setVisible(handle, isVisible);
}

bool soEffectModuleImpl::isObserv(char unk1) {
    return unk1 == 0x11;
}

void soEffectModuleImpl::notifyEventDestructInstance(soModuleAccesser* moduleAccesser) {
    killAll(4, true, true);
}

void soEffectModuleImpl::setShieldEffect(int shieldEffect) {
    m_shieldEffect = shieldEffect;
}

bool soEffectModuleImpl::isSyncScale() {
    return m_isSyncScale;
}

void soEffectModuleImpl::setSyncScale(bool sync) {
    m_isSyncScale = sync;
}

bool soEffectModuleImpl::isSyncVisibility() {
    return m_isSyncVisibility;
}

void soEffectModuleImpl::setSyncVisibility(bool sync) {
    m_isSyncVisibility = sync;
}

u32 soEffectModuleImpl::req2D(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32 group) {
    g_ecMgr->preSet2dScene(true);
    u32 handle = req(effectID, pos, rot, scale, group, -1);
    g_ecMgr->preSet2dScene(false);
    return handle;
}

u32 soEffectModuleImpl::req(EfID effectID, int node, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool follow, u32 group) {
    int handle = 0xFFFF;
    return req(effectID, node, &handle, pos, rot, scale, posRange, rotRange, follow, group);
}

void soEffectModuleImpl::reqPause(bool pause) {
    if (pause) {
        if (!unk135_0) {
            g_ecMgr->pauseEffectGroup(m_groupId | 2, 0xFFFF02);
            ecMgr::getInst()->m_traceMgr.pauseGroup(true, m_groupId | 2, -1);
            unk135_0 = true;
        }
    } else if (!m_moduleAccesser->getStopModule().isStop() && unk135_0 == true) {
        g_ecMgr->playEffectGroup(m_groupId | 2, 0xFFFF02);
        ecMgr::getInst()->m_traceMgr.pauseGroup(false, m_groupId | 2, -1);
        unk135_0 = false;
    }
    unk135_2 = pause;
}

void soEffectModuleImpl::begin() {
    unk135_1 = false;
    if (m_moduleAccesser->getStopModule().isSpecialStop()) {
        if (!unk135_2 && unk135_0 == true) {
            g_ecMgr->playEffectGroup(m_groupId | 2, 0xFFFF02);
            ecMgr::getInst()->m_traceMgr.pauseGroup(false, m_groupId | 2, -1);
            unk135_0 = false;
        }
    } else if (m_moduleAccesser->getStopModule().isStop() || unk135_2 == true) {
        if (!unk135_0) {
            g_ecMgr->pauseEffectGroup(m_groupId | 2, 0xFFFF02);
            ecMgr::getInst()->m_traceMgr.pauseGroup(true, m_groupId | 2, -1);
            unk135_0 = true;
        }
    } else if (unk135_0 == true) {
        g_ecMgr->playEffectGroup(m_groupId | 2, 0xFFFF02);
        ecMgr::getInst()->m_traceMgr.pauseGroup(false, m_groupId | 2, -1);
        unk135_0 = false;
    }
}

void soEffectModuleImpl::removeAll(u32 flags) {
    g_ecMgr->endEffectGroup(m_groupId | flags, flags | 0xFFFF00);
}

void soEffectModuleImpl::killKind(int effectKind, bool p2, bool p3) {
    fn_80060E20(g_ecMgr, m_groupId, 0xFFFF00, p2, p3, effectKind);
}

void soEffectModuleImpl::killAll(u32 flags, bool p2, bool p3) {
    fn_80060E20(g_ecMgr, m_groupId | flags, flags | 0xFFFF00, p2, p3, 0);
}

bool soEffectModuleImpl::removeAfterImage(int id, int p2) {
    return g_ecMgr->m_traceMgr.end(id, p2, 0);
}

void soEffectModuleImpl::removeAllAfterImage(u32 flags, int p2) {
    g_ecMgr->m_traceMgr.endGroup(m_groupId | flags, flags | 0xFFFF00, p2);
}

void soEffectModuleImpl::pushGroupIgnoreScaleZ(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
    g_ecMgr->setFlatMode(false);
}

void soEffectModuleImpl::pushGroupScaleZ(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
    g_ecMgr->setFlatMode(true);
}
