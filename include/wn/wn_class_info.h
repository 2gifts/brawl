#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>

// Class info for weapon articles. One wnClassInfo per article kind is registered in a kind-indexed table;
// it builds the article array for its kind and disposes it again. HYPOTHESIS: parameter names; the module
// map gives the class names, the assembly the field roles.
//
// Note: neither class declares a virtual destructor -- the vtables carry no __dt slots (six words: two RTTI
// headers, isNull, createArray) and the base vtable's createArray slot is null (pure). The base does declare
// an empty plain destructor: it adds no vtable slot, but the original static initializer's register
// allocation only reproduces when it is there, and the linker dedups its weak __dt body against the module's
// deleting-form destructor. The Null object's dtor is only declared: the static initializer registers
// __dt__15wnClassInfoNullFv, whose deleting-form body lives outside this unit.
struct wnArticleBuildInfo;
class soArticle;

class wnArticleList {
public:
    // HYPOTHESIS: the container class was not reconstructed; only the slots the class-info code calls
    // through get named, the rest place them in their vtable slots.
    virtual void pad8() = 0;
    virtual soArticle** at(s32 index) = 0; // +0x0C
    virtual void pad10() = 0;
    virtual void pad14() = 0;
    virtual bool isEnd() = 0; // +0x18
    virtual void pad1c() = 0;
    virtual void removeCurrent() = 0; // +0x20
    virtual void pad24() = 0;
    virtual void pad28() = 0;
    virtual void pad2c() = 0;
    virtual void insert(soArticle* const& entry) = 0; // +0x30
};

class wnDisposalArticle {
public:
    // HYPOTHESIS: the element interface the disposal loop drives; the destructor call and the two status
    // slots are visible in the assembly, the remaining slots are padding.
    virtual void pad8() = 0;
    virtual void padc() = 0;
    virtual void pad10() = 0;
    virtual void pad14() = 0;
    virtual ~wnDisposalArticle() = 0; // +0x18
    virtual void pad1c() = 0;
    virtual void pad20() = 0;
    virtual void pad24() = 0;
    virtual void pad28() = 0;
    virtual void pad2c() = 0;
    virtual void pad30() = 0;
    virtual void pad34() = 0;
    virtual void pad38() = 0;
    virtual void pad3c() = 0;
    virtual void pad40() = 0;
    virtual void pad44() = 0;
    virtual void pad48() = 0;
    virtual void pad4c() = 0;
    virtual void pad50() = 0;
    virtual void pad54() = 0;
    virtual u32 isActiveArticle() = 0; // +0x58 (the disposal loop compares the result unsigned)
    virtual void deactivateArticle() = 0; // +0x5C
};

class wnClassInfo : private soNull, public soNullable {
public:
    wnClassInfo(bool isNull = false);
    // Declared only, defined elsewhere in the module: a weak copy emitted here wins the
    // linker's weak dedup against the named copy in the module, appends its 0x40 to this
    // unit's .text and shifts every address behind it. The reference-only form reproduces
    // the original object; it also parks this class's vtable/RTTI in the key function's unit.
    ~wnClassInfo();

    virtual void createArray(s32 count, s32 heapType, s32 articleId, const wnArticleBuildInfo& info,
                             wnArticleList* list) const = 0;

    void setClassInfo(s32 kind, wnClassInfo* info);
    static wnClassInfo* getClassInfo(s32 kind);
    static void disposeArray(wnArticleList* list);
};
static_assert(sizeof(wnClassInfo) == 0xC, "Class is the wrong size!");

class wnClassInfoNull : public wnClassInfo {
public:
    wnClassInfoNull() : wnClassInfo(true) { }

    ~wnClassInfoNull();

    virtual void createArray(s32 count, s32 heapType, s32 articleId, const wnArticleBuildInfo& info,
                             wnArticleList* list) const;
};
static_assert(sizeof(wnClassInfoNull) == 0xC, "Class is the wrong size!");

extern wnClassInfoNull g_wnClassInfoNull;
