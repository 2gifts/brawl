#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_task.h>
#include <ut/ut_list.h>
#include <GX.h>
#include <gf/gf_application.h>
#include <if/if_utility.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

utLoaderList lbl_8049E550 __attribute__((aligned(8)));

bool utility::Loader::isLoadFinish() {
    utListNode* node = lbl_8049E550.m_list.getHead();
    utListNode* end = 0;
    for (; node != end; node = node->m_next) {
        if (!static_cast<Loader*>(node)->loading()) return false;
    }
    return true;
}

bool utility::AutoResFile::loading() {
    if (!m_file.m_handle.isReady()) return false;
    if (!m_isInit && m_file.m_handle.getReturnStatus() == 0) {
        static_cast<nw4r::g3d::ResFile&>(*this) = nw4r::g3d::ResFile(m_file.m_handle.getBuffer());
        nw4r::g3d::ResFile::Init(this);
        m_isInit = true;
    }
    return true;
}




void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* object, u32 flag);
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* object, u32 timing);

utility::DepthScaler::DepthScaler(MuObject& object, float first, float second)
    : unk4(first), unk8(second) {
    nw4r::g3d::ScnObj* model = object.m_sceneModel;
    model->m_callback = this;
    ScnObj_EnableCallbackExecOp(model, 1);
    ScnObj_EnableCallbackExecOp(model, 4);
    ScnObj_EnableCallbackTiming(model, 0x10);
    ScnObj_EnableCallbackTiming(model, 0x20);
}

void utility::DepthScaler::setZScaleOffset(nw4r::g3d::ScnObj::Timing timing) const {
    switch (timing) {
    case nw4r::g3d::ScnObj::CALLBACK_TIMING_A:
        GXSetZScaleOffset(unk4, 1.0f - unk4);
        GXSetZMode(true, GX_LEQUAL, true);
        break;
    case nw4r::g3d::ScnObj::CALLBACK_TIMING_C:
        GXSetZScaleOffset(1.0f, 0.0f);
        GXSetZMode(false, GX_LEQUAL, false);
        break;
    case 3:
        break;
    }
}

void utility::DepthScaler::ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj*, u32, void*) {
    setZScaleOffset(timing);
}

void utility::DepthScaler::ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj*, u32, void*) {
    setZScaleOffset(timing);
}


utility::DepthClearer::DepthClearer(gfSceneRoot::LayerType layer) : unk0(layer) {
    g_gfSceneRoot->setClearDepthBufferState(layer, true);
}

utility::DepthClearer::~DepthClearer() {
    g_gfSceneRoot->setClearDepthBufferState(unk0, false);
}

void utility::FadeOutTask::processDefault() {
    --unk40;
}

void utility::AutoFile::clear() {
    if (m_isRequested) {
        if (m_handle.isReady()) {
            gfHeapManager::free(m_handle.getBuffer());
        } else {
            m_handle.cancelRequest();
        }
        m_handle.release();
        m_isRequested = false;
    }
}

utility::StandardTask::~StandardTask() {
    if (m_unk42_5) {
        g_gfApplication->unkD0.startKeepScreen();
    }
    g_gfSceneRoot->removeInfoCamResAnm();
}

utility::DepthScaler::~DepthScaler() { }

// TODO: clean up, give variables real names
extern "C" s32 fn_800FB3A4(void) {
    return 0x1;
}

// TODO: clean up, give variables real names
extern "C" s32 fn_800FB3AC(void) {
    return 0x1;
}

// TODO: clean up, give variables real names
namespace jj_800FB828 {
extern "C" unsigned int fn_800FB828(const unsigned int *object)
{
    return object[0x10] >> 31;
}
}

namespace jj_800FB554 {
extern "C" void fn_800B9984(void*);
extern "C" int fn_800FAD8C(void*);
extern "C" void fn_800FB098(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800FB554(unsigned char* self)
{
    fn_800B9984(self + 0x100);
    *(int*)(self + 0x1a8) = fn_800FAD8C(self + 0x100);
    fn_800FB098(self);
}
}

namespace jj_800FB3B4 {
extern "C" {
void* GetResAnmScn__Q34nw4r3g3d7ResFileFi(void*, int);
void fn_801971D0(void*, void*);
void fn_8000D6DC(void*, void*);
extern void* g_gfSceneRoot;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800FB3B4(void* self) {
    char* p = (char*)self;
    if (*(void**)(p + 0x48)) {
        void* spC;
        void* sp8;
        spC = GetResAnmScn__Q34nw4r3g3d7ResFileFi(p + 0x48, 0);
        sp8 = spC;
        fn_801971D0(&spC, &sp8);
        fn_8000D6DC(g_gfSceneRoot, &spC);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_800FB834 {
extern "C" void __dt__6gfTaskFv(void*, int);
extern "C" void __dl__FPv(void*);
extern "C" void fn_8005E3C8(void*, const unsigned char*, float);
extern "C" void* g_efScreen;
extern "C" char lbl_8045C278[];
extern "C" const float lbl_805A26D0;

struct Task {
    char pad[0x3C];
    void* vtable;
    char pad40[4];
    unsigned char effect;
};

// TODO: clean up, give variables real names
extern "C" void* fn_800FB834(Task* self, int flags) {
    unsigned char effect;

    if (self != 0) {
        unsigned char b = self->effect;
        self->vtable = lbl_8045C278;
        float f = lbl_805A26D0;
        effect = b;
        fn_8005E3C8(g_efScreen, &effect, f);

        if (self != 0) {
            __dt__6gfTaskFv(self, 0);
        }

        if (flags > 0) {
            __dl__FPv(self);
        }
    }

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_800FB8BC {

extern "C" {

void startKeepScreen__17gfKeepFrameBufferFv(void*);
void fn_8000D7B8(void*);
int isReady__14gfFileIOHandleCFv(const void*);
void* getBuffer__14gfFileIOHandleCFv(const void*);
void free__13gfHeapManagerFPv(void*);
void cancelRequest__14gfFileIOHandleFv(void*);
void release__14gfFileIOHandleFv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
void removeExist__6utListFP10utListNode(void*, void*);

extern char lbl_8045C41C[];
extern char lbl_8045C4BC[];
extern char lbl_8045B4B8[];
extern char lbl_8049E550[];
extern char* g_gfApplication;
extern char* g_gfSceneRoot;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_800FB8BC(void* self, int arg1) {
    char* s = (char*)self;

    if (self != 0) {
        if (self != 0) {
            *(void**)(s + 0x3C) = lbl_8045C41C;
            if (((*(unsigned char*)(s + 0x42) >> 5) & 1) != 0) {
                startKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
            }

            fn_8000D7B8(g_gfSceneRoot);

            if ((void*)(s + 0x48) != 0) {
                *(void**)(s + 0x78) = lbl_8045C4BC;
                *(char**)(s + 0x58) = lbl_8045C4BC + 8;

                if (*(unsigned char*)(s + 0x70) != 0) {
                    if (isReady__14gfFileIOHandleCFv(s + 0x6C)) {
                        void* buf = getBuffer__14gfFileIOHandleCFv(s + 0x6C);
                        free__13gfHeapManagerFPv(buf);
                    } else {
                        cancelRequest__14gfFileIOHandleFv(s + 0x6C);
                    }
                    release__14gfFileIOHandleFv(s + 0x6C);
                    *(unsigned char*)(s + 0x70) = 0;
                }

                *(int*)(s + 0x48) = 0;
                *(signed char*)(s + 0x74) = 0;

                if ((void*)(s + 0x5C) != 0) {
                    *(void**)(s + 0x68) = lbl_8045B4B8;

                    if (*(unsigned char*)(s + 0x70) != 0) {
                        if (isReady__14gfFileIOHandleCFv(s + 0x6C)) {
                            void* buf = getBuffer__14gfFileIOHandleCFv(s + 0x6C);
                            free__13gfHeapManagerFPv(buf);
                        } else {
                            cancelRequest__14gfFileIOHandleFv(s + 0x6C);
                        }
                        release__14gfFileIOHandleFv(s + 0x6C);
                        *(unsigned char*)(s + 0x70) = 0;
                    }

                    release__14gfFileIOHandleFv(s + 0x6C);
                    __dt__14gfFileIOHandleFv(s + 0x6C, -1);

                    if ((void*)(s + 0x5C) != 0) {
                        if ((void*)(s + 0x5C) != 0) {
                            removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x5C);
                            *(void**)(s + 0x64) = 0;

                            if ((void*)(s + 0x5C) != 0) {
                                if (*(void**)(s + 0x64) != 0) {
                                    removeExist__6utListFP10utListNode(0, s + 0x5C);
                                    *(void**)(s + 0x64) = 0;
                                }
                            }
                        }
                    }
                }

                if ((void*)(s + 0x4C) != 0) {
                    if ((void*)(s + 0x4C) != 0) {
                        removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x4C);
                        *(void**)(s + 0x54) = 0;

                        if ((void*)(s + 0x4C) != 0) {
                            if (*(void**)(s + 0x54) != 0) {
                                removeExist__6utListFP10utListNode(0, s + 0x4C);
                                *(void**)(s + 0x54) = 0;
                            }
                        }
                    }
                }
            }

            __dt__6gfTaskFv(s, 0);
        }

        if (arg1 > 0) {
            __dl__FPv(s);
        }
    }

    return self;
}

}
}

