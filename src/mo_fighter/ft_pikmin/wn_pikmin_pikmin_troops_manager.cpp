#define SO_ARRAY_LIST_CTOR_FLAG
#define SO_ARRAY_EXTERNAL_VECTOR_SIZE
#include <wn/pikmin/wn_pikmin_pikmin_troops_manager.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <types.h>

typedef soArrayListEnumerator<wnPikminPikmin*, s8> wnPikminPikminEnumerator;

wnPikminPikminTroopsManager::wnPikminPikminTroopsManager() : m_troops(0), m_hold(0), m_liberty(0), m_sortVariation(4) { }

void wnPikminPikminTroopsManager::addPikmin(wnPikminPikmin* pikmin) {
    m_troops.push(pikmin);
}

void wnPikminPikminTroopsManager::erasePikmin(int taskId) {
    wnPikminPikminEnumerator enm;
    if (m_troops.isEmpty() == false) {
        enm = m_troops.getEnumerator();
        do {
            if (taskId == static_cast<soEnumerator<wnPikminPikmin*>&>(enm).getCurrent()->m_taskId) {
                m_troops.erase(enm);
                return;
            }
        } while (enm.next());
    }
    if (m_hold.isEmpty() == false) {
        enm = m_hold.getEnumerator();
        do {
            if (taskId == static_cast<soEnumerator<wnPikminPikmin*>&>(enm).getCurrent()->m_taskId) {
                m_hold.erase(enm);
                return;
            }
        } while (enm.next());
    }
    if (m_liberty.isEmpty() == false) {
        enm = m_liberty.getEnumerator();
        do {
            if (taskId == static_cast<soEnumerator<wnPikminPikmin*>&>(enm).getCurrent()->m_taskId) {
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

void wnPikminPikminTroopsManager::reducePikminAll(const soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    const wnPikminPikminOwnerInfo* owner = &owners->at(0);
    while (m_hold.isEmpty() == false) {
        wnPikminPikmin* pikmin = m_hold.at(0);
        const wnPikminPikminOwnerInfo& follow = getAssignOwnerInfoSimple(pikmin, m_troops.size(), owners);
        pikmin->setOwnerInfo(const_cast<wnPikminPikminOwnerInfo*>(owner), const_cast<wnPikminPikminOwnerInfo*>(&follow));
        wnPikminPikmin* pushed = pikmin;
        m_troops.push(pushed);
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

void wnPikminPikminTroopsManager::update(const soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    if (owners->isEmpty() == true) {
        return;
    }
    if (m_liberty.isEmpty() == false) {
        {
            wnPikminPikminEnumerator enm = m_liberty.getEnumerator();
            while (enm.getNext() >= 0) {
                enm.getCurrent();
                if (enm.getCurrent()->isLiberty() == false) {
                    wnPikminPikmin* pikmin = enm.getCurrent();
                    m_troops.push(pikmin);
                    wnPikminPikminEnumerator copy = enm;
                    enm.next();
                    m_liberty.erase(copy);
                } else {
                    enm.next();
                }
            }
        }
        updateJostlePriority();
    }
    soArrayVector<wnPikminPikmin*, 10> autonomous(0);
    if (m_troops.isEmpty() == false) {
        wnPikminPikminEnumerator enm = m_troops.getEnumerator();
        while (enm.getNext() >= 0) {
            if (enm.getCurrent()->isAutonomy() == true && soExternalValueAccesser::getStatusKind(enm.getCurrent()) != 0x19 && soExternalValueAccesser::getStatusKind(enm.getCurrent()) != 0x1A) {
                autonomous.push(enm.getCurrent());
                wnPikminPikminEnumerator copy = enm;
                enm.next();
                m_troops.erase(copy);
            } else {
                enm.next();
            }
        }
    }
    for (int i = 0; i < autonomous.size(); i++) {
        wnPikminPikmin* pikmin = static_cast<soArray<wnPikminPikmin*>&>(autonomous).at(i);
        m_troops.push(pikmin);
    }
    updateJostlePriority();
    if (m_troops.isEmpty() == false) {
        {
            wnPikminPikminEnumerator enm = m_troops.getEnumerator();
            while (enm.getNext() >= 0) {
                u32 inactive = static_cast<soArticle&>(*enm.getCurrent()).isActiveArticle() == 0;
                if (inactive == 1) {
                    wnPikminPikminEnumerator copy = enm;
                    enm.next();
                    m_liberty.erase(copy);
                } else {
                    enm.next();
                }
            }
        }
        int index = 0;
        const wnPikminPikminOwnerInfo* owner = &owners->at(0);
        wnPikminPikminEnumerator enm2 = m_troops.getEnumerator();
        while (enm2.getNext() >= 0) {
            wnPikminPikmin*& target = static_cast<soEnumerator<wnPikminPikmin*>&>(enm2).getCurrent();
            const wnPikminPikminOwnerInfo& follow = getAssignOwnerInfoSimple(enm2.getCurrent(), index, owners);
            target->setOwnerInfo(const_cast<wnPikminPikminOwnerInfo*>(owner), const_cast<wnPikminPikminOwnerInfo*>(&follow));
            enm2.next();
            index++;
        }
    }
}

void wnPikminPikminTroopsManager::updateHoldPikmin(const soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    if (m_hold.isEmpty() == false) {
        int i = 0;
        wnPikminPikminEnumerator enm = m_hold.getEnumerator();
        for (;;) {
            if (owners->size() <= i) {
                break;
            }
            static_cast<soEnumerator<wnPikminPikmin*>&>(enm).getCurrent()->setTargetPos(reinterpret_cast<Vec2f*>(const_cast<wnPikminPikminOwnerInfo*>(&owners->at(i))));
            if (enm.next() == false) {
                break;
            }
            i++;
        }
    }
}

const wnPikminPikminOwnerInfo& wnPikminPikminTroopsManager::getAssignOwnerInfoSimple(wnPikminPikmin* pikmin, int index, const soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    float base = pikmin->getParamFloat(4000) - 1.0f;
    float spacing = pikmin->getParamFloat(0xFA1);
    int position = (int)(base + (float)index * spacing);
    return getAssignOwnerInfoSub(position, owners);
}

const wnPikminPikminOwnerInfo& wnPikminPikminTroopsManager::getAssignOwnerInfoSub(int index, const soArrayFixed<wnPikminPikminOwnerInfo>* owners) {
    if (index >= owners->size()) {
        int last = owners->size() - 1;
        return owners->at(last);
    }
    return owners->at(index);
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

#define VEC (static_cast<soArray<wnPikminPikmin*>&>(sortable))
void wnPikminPikminTroopsManager::sortPikmin() {
    if (m_troops.isEmpty() != true || m_liberty.isEmpty() != true) {
        soArrayVector<wnPikminPikmin*, 10> sortable(0);
        if (m_troops.isEmpty() == false) {
            wnPikminPikminEnumerator enm = m_troops.getEnumerator();
            while (enm.getNext() >= 0) {
                if (enm.getCurrent()->isEnableSort() == true) {
                    sortable.push(enm.getCurrent());
                    wnPikminPikminEnumerator copy = enm;
                    enm.next();
                    m_troops.erase(copy);
                } else {
                    enm.next();
                }
            }
        }
        if (m_liberty.isEmpty() == false) {
            wnPikminPikminEnumerator enm = m_liberty.getEnumerator();
            while (enm.getNext() >= 0) {
                if (enm.getCurrent()->isEnableSort() == true) {
                    sortable.push(enm.getCurrent());
                    wnPikminPikminEnumerator copy = enm;
                    enm.next();
                    m_liberty.erase(copy);
                } else {
                    enm.next();
                }
            }
        }
        for (int i = 0; i < 5 && VEC.isEmpty() == false; i++) {
            int variation = m_sortVariation - i;
            if (variation < 0) {
                variation += 5;
            }
            int index = 0;
            int count = sortable.size();
            for (int j = 0; j < count; j++) {
                if (variation == VEC.at(index)->getVariation()) {
                    m_troops.unshift(VEC.at(index));
                    if (VEC.at(index)->isNeedSortRepond() == true) {
                        VEC.at(index)->changeStatus(0x19);
                    }
                    VEC.erase(index);
                } else {
                    index++;
                }
            }
        }
        if (m_troops.isEmpty() == false) {
            m_sortVariation = m_troops.at(0)->getVariation();
        }
        updateJostlePriority();
    }
}
#undef VEC

void wnPikminPikminTroopsManager::syncLr(wnPikminPikmin* pikmin, float lr) {
    pikmin->syncLr(lr);
}

void wnPikminPikminTroopsManager::syncPos(wnPikminPikmin* pikmin, Vec2f* pos) {
    pikmin->syncPos(pos);
}

void wnPikminPikminTroopsManager::setTargetPos(wnPikminPikmin* pikmin, Vec2f* pos) {
    pikmin->setTargetPos(pos);
}

void wnPikminPikminTroopsManager::attachChain(wnPikminPikmin* pikmin, int unk1, int unk2, int logGroup) {
    pikmin->attachChain(unk1, unk2, logGroup);
}

void wnPikminPikminTroopsManager::detachChain(wnPikminPikmin* pikmin, int status) {
    pikmin->detachChain(status);
}

bool wnPikminPikminTroopsManager::isConstraint(wnPikminPikmin* pikmin, u32 taskId) {
    return pikmin->isConstraintTarget(taskId);
}

void wnPikminPikminTroopsManager::detachConstraint(wnPikminPikmin* pikmin) {
    pikmin->detachConstraint(false);
}

void wnPikminPikminTroopsManager::setPowerMulStatus(wnPikminPikmin* pikmin, float power) {
    pikmin->setPowerMulStatus(power);
}

void wnPikminPikminTroopsManager::onStatusFlag(wnPikminPikmin* pikmin, u32 flag) {
    pikmin->onStatusFlag(flag);
}

void wnPikminPikminTroopsManager::setStatusFloat(wnPikminPikmin* pikmin, float value, u32 index) {
    pikmin->setStatusFloat(value, index);
}

void wnPikminPikminTroopsManager::onInstanceFlag(wnPikminPikmin* pikmin, u32 flag) {
    pikmin->onInstanceFlag(flag);
}

int wnPikminPikminTroopsManager::getCatchKind(wnPikminPikmin* pikmin) {
    return pikmin->getCatchKind();
}

bool wnPikminPikminTroopsManager::attachGround(wnPikminPikmin* pikmin) {
    return pikmin->attachGround();
}

void wnPikminPikminTroopsManager::setOwnerCondition(wnPikminPikmin* pikmin, int condition) {
    pikmin->setOwnerConditionCurrent(condition);
}

void wnPikminPikminTroopsManager::updateNodeSRT(wnPikminPikmin* pikmin) {
    pikmin->updateNodeSRT();
}

bool wnPikminPikminTroopsManager::isMakeBloodAll() {
    wnPikminPikminEnumerator enm;
    if (m_troops.isEmpty() == false) {
        enm = m_troops.getEnumerator();
        do {
            if (soExternalValueAccesser::getWorkInt(enm.getCurrent(), 0x10000008) != 2) {
                return false;
            }
        } while (enm.next());
    }
    if (m_hold.isEmpty() == false) {
        enm = m_hold.getEnumerator();
        do {
            if (soExternalValueAccesser::getWorkInt(enm.getCurrent(), 0x10000008) != 2) {
                return false;
            }
        } while (enm.next());
    }
    if (m_liberty.isEmpty() == false) {
        enm = m_liberty.getEnumerator();
        do {
            if (soExternalValueAccesser::getWorkInt(enm.getCurrent(), 0x10000008) != 2) {
                return false;
            }
        } while (enm.next());
    }
    return true;
}

void wnPikminPikminTroopsManager::holdPikminForce(int taskId) {
    wnPikminPikminEnumerator enm;
    if (m_troops.isEmpty() == false) {
        enm = m_troops.getEnumerator();
        int index = 0;
        while (enm.getNext() >= 0) {
            wnPikminPikmin* pikmin = enm.getCurrent();
            enm.next();
            if (taskId == pikmin->m_taskId) {
                m_hold.push(pikmin);
                m_troops.erase(index);
                updateJostlePriority();
                return;
            }
            index++;
        }
    }
    if (m_liberty.isEmpty() == false) {
        enm = m_liberty.getEnumerator();
        int index = 0;
        while (enm.getNext() >= 0) {
            wnPikminPikmin* pikmin = enm.getCurrent();
            enm.next();
            if (taskId == pikmin->m_taskId) {
                m_hold.push(pikmin);
                m_liberty.erase(index);
                updateJostlePriority();
                return;
            }
            index++;
        }
    }
}

void wnPikminPikminTroopsManager::setVisibilityChain(u8 visible, int num, int rest) {
    if (m_hold.isEmpty() == false) {
        wnPikminPikminEnumerator enm = m_hold.getEnumerator();
        int index = 0;
        while (enm.getNext() >= 0) {
            wnPikminPikmin* pikmin = enm.getCurrent();
            if (num >= 0 && index >= num) {
                if (rest == 0) {
                    break;
                }
                pikmin->setVisible(!visible);
            } else {
                pikmin->setVisible(visible);
            }
            enm.next();
            index++;
        }
    }
}

void wnPikminPikminTroopsManager::setTeamAll(int team, int teamOwner, int skipFlagged) {
    wnPikminPikminEnumerator enm;
    if (m_troops.isEmpty() == false) {
        enm = m_troops.getEnumerator();
        do {
            wnPikminPikmin* pikmin = enm.getCurrent();
            if (skipFlagged == 0 || soExternalValueAccesser::getWorkFlag(pikmin, 0x12000006) == 0) {
                pikmin->setTeam(team, teamOwner);
            }
        } while (enm.next());
    }
    if (m_hold.isEmpty() == false) {
        enm = m_hold.getEnumerator();
        do {
            wnPikminPikmin* pikmin = enm.getCurrent();
            if (skipFlagged == 0 || soExternalValueAccesser::getWorkFlag(pikmin, 0x12000006) == 0) {
                pikmin->setTeam(team, teamOwner);
            }
        } while (enm.next());
    }
    if (m_liberty.isEmpty() == false) {
        enm = m_liberty.getEnumerator();
        do {
            wnPikminPikmin* pikmin = enm.getCurrent();
            if (skipFlagged == 0 || soExternalValueAccesser::getWorkFlag(pikmin, 0x12000006) == 0) {
                pikmin->setTeam(team, teamOwner);
            }
        } while (enm.next());
    }
}

// The pure virtual destructor needs a body; it is defined after every use so that the enumerator destructors call it
// out of line (weak copy emitted by this unit).
template <typename T>
soEnumerator<T>::~soEnumerator() { }
template soEnumerator<wnPikminPikmin*>::~soEnumerator();
