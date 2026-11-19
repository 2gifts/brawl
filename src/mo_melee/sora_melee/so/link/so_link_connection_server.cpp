#define SO_LINK_MODULE_IMPL_FULL
#include <so/link/so_link_connection_server.h>
#include <types.h>

soLinkModuleImpl* soLinkNode::getLinkModule() {
    return m_linkModule;
}

void soLinkConnection::reset() {
    m_node.m_taskId = 0;
    m_node.m_linkModule = NULL;
    m_node.m_connection = this;
    m_parent.m_taskId = 0;
    m_parent.m_linkModule = NULL;
    m_parent.m_connection = this;
    m_attribute = 0;
    m_isLinked = false;
}

soLinkConnectionServer::~soLinkConnectionServer() { }

void soLinkConnectionServer::entry(utListNode* node) {
    if (isExistNode(node) == -1) {
        addTail(node);
    }
}

void soLinkConnectionServer::exit(utListNode* node) {
    if (isExistNode(node) != -1) {
        remove(node);
    }
}

soLinkConnectionServer lbl_27_bss_110;

#include <ut/ut_list.h>
// TODO: clean up, give variables real names
namespace jj_6F418 {
typedef unsigned int u32;

struct Node {
    Node* next;
    Node* prev;
    void* data;
};

struct utListNode {
    utListNode* next;
    utListNode* prev;
};

struct utList {
    Node* getNode(unsigned long);
    void addTail(utListNode*);
};

struct Info {
    char pad[0x28];
    u32 id;
};

struct Event {
    u32 type;
    unsigned char flag;
};

struct Object {
    char pad[0x20];
    utList list;
};

struct Link {
    char pad[8];
    utListNode node;
    u32 source;
    u32 unknown14;
    Object* owner;
    utListNode node2;
    u32 target;
    u32 unknown28;
    void* found;
    u32 flags;
};

extern "C" Info* getOwner__16soLinkModuleImplFv(void*);
extern "C" void fn_27_6AA44(void*, void*, Event*, u32);

extern "C" bool link__22soLinkConnectionServerFP16soLinkModuleImpliP16soLinkConnection(utList* list, Object* obj, u32 id, Link* link) {
    Node* n = list->getNode(0);
    void* found;

    while (n) {
        if (id == getOwner__16soLinkModuleImplFv(n->data)->id) {
            found = n->data;
            goto done;
        }
        n = n->next;
    }
    found = 0;

done:
    if (found) {
        link->source = getOwner__16soLinkModuleImplFv(obj)->id;
        link->owner = obj;
        link->target = id;
        link->found = found;
        link->flags |= 1;
        obj->list.addTail(&link->node2);
        ((utList*)((char*)found + 0x14))->addTail(&link->node);

        u32 flags = (link->flags >> 1) & 0x7f;
        Event e;
        e.type = 0x3b;
        e.flag = 0;
        fn_27_6AA44(found, obj, &e, flags);
        return true;
    }
    return false;
}
}

// TODO: clean up, give variables real names
namespace jj_6F5F8 {

struct Object {
    u8 pad[0x14];
    utList incoming;
    utList outgoing;
};
struct Link {
    u8 pad[8];
    utListNode node;
    u32 source;
    Link* self1;
    Object* owner;
    utListNode node2;
    u32 target;
    Link* self2;
    Object* found;
    union {
        u32 flags;
        struct { u32 unused : 31; u32 active : 1; } bits;
    };
};
struct ListEntry : utListNode {
    u32 field8;
    Link* data;
};
struct Event {
    u32 type;
    u8 flag;
};
extern "C" void onUnlink__16soLinkModuleImplFi(Object*, u32);
extern "C" void fn_27_6AA44(Object*, Object*, Event*, u32);

extern "C" void disconnectAll__22soLinkConnectionServerFP16soLinkModuleImpl(void*, Object* obj) {
    Event e2;
    Event e1;
    while (*(s32*)((u8*)obj + 0x28) != 0) {
        Link* link = ((ListEntry*)obj->outgoing.getNode(0))->data;
        Object* owner = link->owner;
        Object* found = link->found;
        onUnlink__16soLinkModuleImplFi(owner, (link->flags >> 1) & 0x7f);
        owner->outgoing.remove(&link->node2);
        found->incoming.remove(&link->node);
        link->source = 0;
        link->owner = 0;
        link->self1 = link;
        link->target = 0;
        link->found = 0;
        link->self2 = link;
        link->bits.active = 0;
        u32 flags = link->flags;
        link->flags = (u8)flags;
        e1.type = 0x3d;
        e1.flag = 0;
        fn_27_6AA44(owner, found, &e1, (flags >> 1) & 0x7f);
        flags = (link->flags >> 1) & 0x7f;
        e2.type = 0x3c;
        e2.flag = 0;
        fn_27_6AA44(found, owner, &e2, flags);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6F6E8 {
struct Object { u8 pad[0x14]; utList incoming; utList outgoing; };
struct Link {
    u8 pad[8]; utListNode node; u32 source; Link* self1; Object* owner;
    utListNode node2; u32 target; Link* self2; Object* found;
    union { u32 flags; struct { u32 unused : 31; u32 active : 1; } bits; };
};
struct ListEntry : utListNode { u32 field8; Link* data; };
struct HeadEntry : utListNode { u32 field8; u32 fieldc; Object* data; };
struct Info { u8 pad[0x28]; u32 id; };
struct Event { u32 type; u8 flag; };
extern "C" Info* getOwner__16soLinkModuleImplFv(void*);
extern "C" void onUnlink__16soLinkModuleImplFi(Object*, u32);
extern "C" void fn_27_6AA44(Object*, Object*, Event*, u32);
extern "C" void fn_27_6F6E8(void*, Object* obj) {
    ListEntry* node;
    u32 id;
    Object* owner;
    Object* found;
    Link* link;
    Event e1;
    Event e2;
    while (*(s32*)((u8*)obj + 0x1c) != 0) {
        HeadEntry* head = (HeadEntry*)obj->incoming.getNode(0);
        id = getOwner__16soLinkModuleImplFv(obj)->id;
        node = (ListEntry*)head->data->outgoing.getNode(0);
        while (node) {
            link = node->data;
            if (link->target == id) {
                owner = link->owner;
                found = link->found;
                onUnlink__16soLinkModuleImplFi(owner, (link->flags >> 1) & 0x7f);
                owner->outgoing.remove(&link->node2);
                found->incoming.remove(&link->node);
                link->source = 0;
                link->owner = 0;
                link->self1 = link;
                link->target = 0;
                link->found = 0;
                link->self2 = link;
                link->bits.active = 0;
                u32 flags = link->flags;
                link->flags = (u8)flags;
                e1.type = 0x3d;
                e1.flag = 0;
                fn_27_6AA44(owner, found, &e1, (flags >> 1) & 0x7f);
                flags = (link->flags >> 1) & 0x7f;
                e2.type = 0x3c;
                e2.flag = 0;
                fn_27_6AA44(found, owner, &e2, flags);
            }
            node = (ListEntry*)node->m_next;
        }
    }
}
}

