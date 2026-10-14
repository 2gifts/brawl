#include <wn/pikmin/wn_pikmin_pikmin_troops_manager.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <types.h>

typedef soArrayListEnumerator<wnPikminPikmin*, s8> wnPikminPikminEnumerator;

wnPikminPikminTroopsManager::wnPikminPikminTroopsManager() : m_sortVariation(4) { }

void wnPikminPikminTroopsManager::addPikmin(wnPikminPikmin* pikmin) {
    m_troops.push(pikmin);
}

void wnPikminPikminTroopsManager::erasePikmin(int taskId) {
    wnPikminPikminEnumerator enm;
    if (m_troops.isEmpty() == false) {
        enm = m_troops.getEnumerator();
        do {
            if (taskId == enm.getCurrent()->m_taskId) {
                m_troops.erase(enm);
                return;
            }
        } while (enm.next());
    }
    if (m_hold.isEmpty() == false) {
        enm = m_hold.getEnumerator();
        do {
            if (taskId == enm.getCurrent()->m_taskId) {
                m_hold.erase(enm);
                return;
            }
        } while (enm.next());
    }
    if (m_liberty.isEmpty() == false) {
        enm = m_liberty.getEnumerator();
        do {
            if (taskId == enm.getCurrent()->m_taskId) {
                m_liberty.erase(enm);
                return;
            }
        } while (enm.next());
    }
}

void wnPikminPikminTroopsManager::reset() {
    m_troops.clear();
    m_hold.clear();
    m_liberty.clear();
}

int wnPikminPikminTroopsManager::getPikminNum() {
    return m_troops.size();
}

int wnPikminPikminTroopsManager::getHoldPikminNum() {
    return m_hold.size();
}

void wnPikminPikminTroopsManager::holdPikmin(int num) {
    if (m_troops.isEmpty() != true) {
        int index = 0;
        for (int i = 0; i < num; i++) {
            if (index >= m_troops.size() || m_troops.isEmpty() == true) {
                return;
            }
            if (m_troops.at(index)->isEnableHold() == true) {
                m_hold.push(m_troops.at(index));
                m_troops.erase(index);
            } else {
                index++;
            }
        }
        updateJostlePriority();
    }
}

wnPikminPikmin* wnPikminPikminTroopsManager::getHoldPikminTop() {
    m_hold.isEmpty();
    return m_hold.at(0);
}

soArrayList<wnPikminPikmin*, 10>* wnPikminPikminTroopsManager::getHoldPikminList() {
    return &m_hold;
}

void wnPikminPikminTroopsManager::reducePikminAll(soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    wnPikminPikminOwnerInfo* owner = &owners->at(0);
    while (m_hold.isEmpty() == false) {
        wnPikminPikmin* pikmin = m_hold.at(0);
        wnPikminPikminOwnerInfo& follow = getAssignOwnerInfoSimple(pikmin, m_troops.size(), owners);
        pikmin->setOwnerInfo(owner, &follow);
        m_troops.push(pikmin);
        m_hold.shift();
    }
    updateJostlePriority();
}

void wnPikminPikminTroopsManager::libertyPikminAll() {
    while (m_hold.isEmpty() == false) {
        m_liberty.push(m_hold.at(0));
        m_hold.shift();
    }
}

void wnPikminPikminTroopsManager::updateHoldPikmin(soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    if (m_hold.isEmpty() == false) {
        wnPikminPikminEnumerator enm = m_hold.getEnumerator();
        for (int i = 0; i < owners->size(); i++) {
            Vec2f* pos = reinterpret_cast<Vec2f*>(&owners->at(i));
            enm.getCurrent()->setTargetPos(pos);
            if (enm.next() == false) {
                break;
            }
        }
    }
}

wnPikminPikminOwnerInfo& wnPikminPikminTroopsManager::getAssignOwnerInfoSimple(wnPikminPikmin* pikmin, int index, soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    float spacing = pikmin->getParamFloat(0xFA1);
    float base = pikmin->getParamFloat(4000) - 1.0f;
    return getAssignOwnerInfoSub((int)(base + (float)index * spacing), owners);
}

wnPikminPikminOwnerInfo& wnPikminPikminTroopsManager::getAssignOwnerInfoSub(int index, soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    if (index < owners->size()) {
        return owners->at(index);
    }
    return owners->at(owners->size() - 1);
}

void wnPikminPikminTroopsManager::updateJostlePriority() {
    if (m_troops.isEmpty() != true) {
        wnPikminPikminEnumerator enm = m_troops.getEnumerator();
        int priority = 0;
        while (enm.getNext() >= 0) {
            wnPikminPikmin* pikmin = enm.getCurrent();
            enm.next();
            pikmin->setJostlePriority(6 - priority);
            priority++;
        }
    }
}
