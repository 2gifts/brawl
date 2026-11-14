#include <mu/adv/mu_seal_figure_status.h>
#include <types.h>

void muSealFigureStatus::init() {
    m_num = 0;
    Entry* entry = m_entries;
    for (int i = 0; i < 64; i++, entry++) {
        entry->m_seal = NULL;
    }
    clearEff();
}

int muSealFigureStatus::add(Seal* seal, const Vec2f& pos) {
    if (seal != NULL && m_num < 64) {
        int i;
        Entry* entry = m_entries;
        for (i = 0; i < 64; i++, entry++) {
            if (entry->m_seal == NULL) {
                entry->m_seal = seal;
                entry->m_pos = pos;
                m_num++;
                break;
            }
        }
        if (i < 64) {
            calcEff();
            return i;
        }
    }
    return -1;
}

muSealFigureStatus::Seal* muSealFigureStatus::del(int index) {
    Seal* seal = NULL;
    if (index >= 0 && index < 64 && m_num > 0 && m_entries[index].m_seal != NULL) {
        seal = m_entries[index].m_seal;
        m_entries[index].m_seal = NULL;
        m_num--;
    }
    calcEff();
    return seal;
}

muSealFigureStatus::Entry* muSealFigureStatus::enumSeal(int n) {
    return getNth(n);
}

void muSealFigureStatus::calcEff() {
    clearEff();
    for (int i = 0;; i++) {
        Entry* entry = getNth(i);
        if (entry == NULL) {
            break;
        }
        applyEffInline(entry->m_seal);
    }
}

void muSealFigureStatus::applyEff(const Seal* seal) {
    applyEffInline(seal);
}
