#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>

// The stickers pasted on a trophy stand and the strongest bonus they give per bonus kind.
class muSealFigureStatus {
public:
    // HYPOTHESIS: the sticker parameter record; only the bonus kind and value are read here.
    struct Seal {
        char _0[0x2C];
        int m_effKind;      // 0x2C
        float m_effValue;   // 0x30
    };
    struct Entry {
        Vec2f m_pos;        // 0x0
        Seal* m_seal;       // 0x8
    };

    s32 m_num;              // 0x0
    Entry m_entries[64];    // 0x4
    float m_eff[96];        // 0x304

    void clearEff() {
        for (int i = 0; i < 96; i++) {
            m_eff[i] = 0.0f;
        }
    }
    Entry* getNth(int n) {
        if (m_num > 0) {
            int i = 0;
            Entry* entry = m_entries;
            for (; i < 64; i++, entry++) {
                if (entry->m_seal != NULL) {
                    if (n <= 0) {
                        return entry;
                    }
                    n--;
                }
            }
        }
        return NULL;
    }
    void applyEffInline(const Seal* seal) {
        if (m_eff[seal->m_effKind] < seal->m_effValue) {
            m_eff[seal->m_effKind] = seal->m_effValue;
        }
    }

    void init();
    int add(Seal* seal, const Vec2f& pos);
    Seal* del(int index);
    Entry* enumSeal(int n);
    virtual void calcEff();
    void applyEff(const Seal* seal);
};
static_assert(sizeof(muSealFigureStatus) == 0x488, "Class is wrong size!");
