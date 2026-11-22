#include <types.h>

class csElement;

// The coin shooter game object (sora_minigame coinshooter.o) as the elements see it.
// HYPOTHESIS: the parameter of entryElement is the element's group; the type of the default (3) is a guess.
class csCoinshooter {
public:
    void entryElement(csElement* element, const int& group);
    void removeElement(csElement* element);
};

// The coin shooter of the module (bss 0x8).
// TODO: clean up - name it once coinshooter.cpp is decompiled.
extern csCoinshooter* lbl_20_bss_8;

static const int s_defaultElementGroup = 3;

// The base of everything that takes part in the coin shooter (sora_minigame cs_element.o): elements register themselves
// with the game when they are made and leave it when they are destroyed.
// HYPOTHESIS: the vtable pointer is the first word, so csElement derives from a class without data members.
class csElementInterface {
public:
    virtual ~csElementInterface() { }
};

class csElement : public csElementInterface {
public:
    u8 m_isActive; // 0x4

    csElement();
    csElement(int group);
    virtual ~csElement();
};

csElement::csElement() {
    m_isActive = true;
    lbl_20_bss_8->entryElement(this, s_defaultElementGroup);
}

csElement::csElement(int group) {
    m_isActive = true;
    lbl_20_bss_8->entryElement(this, group);
}

csElement::~csElement() {
    lbl_20_bss_8->removeElement(this);
}
