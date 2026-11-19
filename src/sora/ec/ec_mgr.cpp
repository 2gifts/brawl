#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <ec/ec_trace_mgr.h>
#include <ec/ec_mgr.h>
#include <gf/gf_heap_manager.h>
#include <revolution/MTX/mtx.h>
#include <sr/sr_common.h>
#include <types.h>

// ecMgr::loadResource (the map name); st_loader_stage.cpp calls it by this stand-in name.
extern "C" void fn_8005EA4C(ecMgr* mgr, int resourceId, void* data, u32 size, int unk1, int unk2);

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
Mtx lbl_80497810 __attribute__((aligned(8))) = { 0 };
const void** lbl_80497840[0x138] __attribute__((aligned(8))) = { 0 };
extern const u8 lbl_80407060[];
extern const void* lbl_80449F98[];

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
ecMgr* g_ecMgr = 0;
unsigned char lbl_805A014C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0150[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0154[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0158[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A015C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0160[0x4] __attribute__((aligned(4))) = { 0 };
ecEffectWork* lbl_805A0164 = 0;
unsigned char lbl_805A0168[0x8] __attribute__((aligned(4))) = { 0 };

ecResource::ecResource() {
    unk0 = 0;
    unk4 = 0;
    unk8 = 0;
    unkC = 0;
}

ecResource::~ecResource() {
    release(false);
}

ecEffectWork::ecEffectWork() : unk60(0xFFFF) {
    unk5C = 0;
}

void ecMgr::create(u32 count, u32 resources, u32 limit) {
    const void** data;
    const u8* sizes;
    const void*** entries;
    u32 size;
    sizes = lbl_80407060;
    entries = lbl_80497840;
    data = lbl_80449F98;
    u32 offset = 1;
    for (u32 i = 0; i < 0x137; i++, entries++, sizes++) {
        size = *sizes;
        if (!size) {
            *entries = 0;
        } else {
            *entries = &data[offset];
            offset += size;
        }
    }
    PSMTXIdentity(lbl_80497810);
    if (!g_ecMgr) {
        g_ecMgr = new (Heaps::Effect) ecMgr(count, resources, limit);
    }
}

void ecMgr::registResource(char* name, void* data, int size, int flag) {
    int id = searchResourceID(name);
    if (id < 0x137) {
        if (unk42) {
            fn_8005EA4C(this, id, 0, 0, true, 0xFF);
        }
        fn_8005EA4C(this, id, data, size, flag != 0, 0xFF);
    }
}

void ecMgr::render() {
    m_traceMgr.renderXlu();
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        for (; index <= unk74; ++index, ++work) {
            if (work->unk7C) {
                work->render();
            }
        }
    }
    if (unk40_) {
        gfHeapManager::getHeap(Heaps::Effect);
    }
}

void ecMgr::setHeapInstance(int heapId) {
    if (heapId == 0xFF) {
        unk48 = 3;
    } else {
        unk48 = heapId;
    }
}

void ecMgr::preSet2dScene(bool enabled) {
    if (enabled) {
        unk54 = unk50 ? unk50 : unk4C;
    } else {
        unk54 = unk4C;
    }
}

void ecMgr::setMdlPriority(u32 handleId, int priority) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work && work->unk44) {
        nw4r::g3d::ScnMdl* model = work->unk44;
        model->SetPriorityDrawOpa(priority);
        model->SetPriorityDrawXlu(priority);
    }
}

bool ecMgr::isExistEffectResource(u32 resourceId) {
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        for (; index <= unk74; ++index, ++work) {
            if (work->unk7C && work->unk30->unk10 == resourceId) {
                return true;
            }
        }
    }
    return false;
}

void ecMgr::setMdlCalcMtx(u32 handleId, void (*callback)(nw4r::g3d::ScnMdl*, void*), void* userData) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_mdl_calc_mtx(callback, userData);
    }
}

u32 ecMgr::setEffect(EfID effectID, const Vec3f* pos) {
    return setEffect(effectID, static_cast<int>(effectID) >> 16, pos, 0, 0);
}

void ecMgr::setDelay(u32 handleId, int frames) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_delay(frames);
    }
}

void ecMgr::killEffectGroup(u32 group, u32 mask, bool immediate, bool release, int effectKind) {
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        if (group & 0xFF000000) {
            mask |= 0xFF000000;
        }
        group &= mask;
        if (!effectKind) {
            for (; index <= unk74; ++index, ++work) {
                if (work->unk7C && group == (work->unk78 & mask)) {
                    work->end(immediate, release);
                }
            }
        } else {
            for (; index <= unk74; ++index, ++work) {
                if (work->unk7C && group == (work->unk78 & mask)) {
                    int currentEffect = (work->unk30->unk10 << 16) | (work->unk2C & 0xFFFF);
                    if (currentEffect == effectKind) {
                        work->end(immediate, release);
                    }
                }
            }
        }
    }
}

bool ecMgr::isExistEffect(u32 handleId) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    return work != 0;
}

extern "C" void fn_8005F660(ecMgr* mgr) {}

extern "C" void fn_8005F664(ecMgr* mgr) {}

extern "C" void fn_8005F704(ecMgr* mgr) {}

void ecMgr::setFlatMode(bool flat) {
    unk43 = flat;
}

extern "C" u8 fn_8005F79C(ecMgr* mgr) {
    return mgr->unk43;
}

void ecMgr::setDrawPrio(int prio) {
    if (prio <= 0) {
        unk80 = 0;
    } else {
        unk80 = 1;
    }
}

void ecMgr::pushsetCurrentGroup(u32 group) {
    u8 depth = unk81;
    if (depth < 2) {
        depth++;
        unk81 = depth;
        (&unk84)[depth] = group;
    }
}

void ecMgr::popCurrentGroup() {
    u32 depth = unk81;
    if (depth != 0) {
        depth--;
        unk81 = depth;
        if ((depth & 0xFF) == 0) {
            unk80 = 0;
            unk43 = 0;
        }
    }
}

u32 ecMgr::setEffect(EfID effectID) {
    return setEffect(effectID, static_cast<int>(effectID) >> 16, 0, 0, 0);
}

u32 ecMgr::setEffect(EfID effectID, Vec3f* pos) {
    return setEffect(effectID, static_cast<int>(effectID) >> 16, pos, 0, 0);
}

u32 ecMgr::setEffect(EfID effectID, const Vec3f* pos, const Vec3f* rot, const Vec3f* scale) {
    return setEffect(effectID, static_cast<int>(effectID) >> 16, pos, rot, scale);
}

void ecMgr::setVisible(u32 handleId, bool isVisible) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_visible(isVisible);
    }
}

void ecMgr::setRot(u32 handleId, Vec3f* rot) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_rot(rot);
    }
}

void ecMgr::setScl(u32 handleId, Vec3f* scale) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_scl(scale);
    }
}

void ecMgr::setMdlTexpat(u32 handleId, u32 texpat) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_mdl_texpat(texpat);
    }
}

void ecMgr::setSlowRate(u32 handleId, u32 rate) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_slow_rate(rate);
    }
}

extern "C" void fn_80062D08(ecEffectWork* work, grCollisionLineWork* lineWork);

void ecMgr::endEffect(u32 handleId) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->end(false, false);
    }
}

void ecMgr::setGroundAttach(u32 handleId, grCollisionLineWork* lineWork) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        fn_80062D08(work, lineWork);
    }
}

void ecMgr::endEffectGroup(u32 group, u32 mask) {
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        if (group & 0xFF000000) {
            mask |= 0xFF000000;
        }
        group &= mask;
        for (; index <= unk74; ++index, ++work) {
            if (work->unk7C && group == (work->unk78 & mask)) {
                work->end(false, false);
            }
        }
    }
}

void ecMgr::pauseEffectGroup(u32 group, u32 mask) {
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        if (group & 0xFF000000) {
            mask |= 0xFF000000;
        }
        group &= mask;
        for (; index <= unk74; ++index, ++work) {
            if (work->unk7C && group == (work->unk78 & mask)) {
                work->pause();
            }
        }
    }
}

void ecMgr::playEffectGroup(u32 group, u32 mask) {
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        if (group & 0xFF000000) {
            mask |= 0xFF000000;
        }
        group &= mask;
        for (; index <= unk74; ++index, ++work) {
            if (work->unk7C && group == (work->unk78 & mask)) {
                work->play();
            }
        }
    }
}

extern "C" void fn_800610BC(ecMgr* mgr, u32 group, u32 mask, u32 rate) {
    if (mgr->unk6C) {
        u32 index = mgr->unk70;
        ecEffectWork* work = &mgr->unk60[index];
        if (group & 0xFF000000) {
            mask |= 0xFF000000;
        }
        group &= mask;
        for (; index <= mgr->unk74; ++index, ++work) {
            if (work->unk7C && group == (work->unk78 & mask)) {
                work->set_slow_rate(rate);
            }
        }
    }
}

// HYPOTHESIS: ecMgr::exit under its stand-in name, forwarding to the gfTask exit.
extern "C" void fn_8005E6C0() {
    g_ecMgr->exit();
}

void ecMgr::preSetAnmIdx(int chrIndex, int visIndex, int patIndex, int srtIndex, int clrIndex, int shpIndex) {
    *(int*)lbl_805A014C = chrIndex;
    *(int*)lbl_805A0150 = visIndex;
    *(int*)lbl_805A0154 = patIndex;
    *(int*)lbl_805A0158 = srtIndex;
    *(int*)lbl_805A015C = clrIndex;
    *(int*)lbl_805A0160 = shpIndex;
}

void ecMgr::setParent(u32 handleId, nw4r::g3d::ScnMdl* model, const char* nodeName, bool flag) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_parent(model, nodeName, flag);
    }
}

void ecMgr::setParent(u32 handleId, nw4r::g3d::ScnMdl* model, u32 nodeIndex, bool flag) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->set_parent(model, nodeIndex, flag);
    }
}

// HYPOTHESIS: ecMgr::changeMdlTex under its stand-in name, forwarding to the effect work.
extern "C" void fn_8006097C(ecMgr* mgr, u32 handleId, const char* a, const char* b, nw4r::g3d::ResFile* file, int flag) {
    ecEffectWork* work;
    if (handleId && mgr->unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &mgr->unk60[mgr->unk70];
            for (u32 index = mgr->unk70; index <= mgr->unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        work->change_mdl_tex(a, b, file, flag);
    }
}

extern "C" void fn_80063084(ecEffectWork* work, Vec3f* pos);

void ecMgr::setPos(u32 handleId, Vec3f* pos) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        fn_80063084(work, pos);
    }
}

// HYPOTHESIS: grCollStatus::getTouchLine (main.dol 0x80134524), as declared in gr_collision_body.cpp.
extern "C" grCollisionLineWork* fn_80134524(grCollStatus* status, int touchId);

void ecMgr::setGroundAttach(u32 handleId, soGroundModule* groundModule) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work) {
        fn_80062D08(work, fn_80134524(groundModule->getCollStatus(0), 3));
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_8005F814(void);

extern "C" void fn_8005F7FC(void) {
    fn_8005F814();
}

extern "C" void fn_80061510(void) {}

__declspec(section ".ctors") extern void* const fn_80061510_ctor = (void*)fn_80061510;

// TODO: clean up, give variables real names
namespace jj_8005F194 {
extern "C" int fn_8005F194(u8* self, s32 arg1) {
    u32 i = 0;
    u32 low = ((u32)arg1 << 16) >> 16;
    s32 off = 0;
    u8* found;

    for (; i < *(u32*)(self + 0x5C); i++) {
        u8* tbl = *(u8**)(self + 0x58);
        u8* e = tbl + off;
        if (*(u32*)e != 0 && (u32)(arg1 >> 16) == *(u32*)(e + 0x10)) {
            found = tbl + i * 0x18;
            goto done;
        }
        off += 0x18;
    }
    found = 0;
done:
    if (found != 0) {
        u8* p = *(u8**)(found + 0xC);
        p += low << 4;
        if (p[0x1C] & 1) return 1;
    }
    return 0;
}
}

namespace jj_8005F220 {
extern "C" bool fn_8005F220(void* self, s32 arg1) {
    u32 count = *(u32*)((char*)self + 0x5C);
    u32 i;
    u32 low = (u32)arg1 & 0xFFFF;
    s32 off;
    void* found;

    i = 0;
    off = 0;

    for (; i < count; i++) {
        u32* base = *(u32**)((char*)self + 0x58);
        char* entry = (char*)base + off;
        if (*(s32*)entry != 0 &&
            (u32)(arg1 >> 16) == (u32)*(s32*)(entry + 0x10)) {
            found = (char*)base + i * 0x18;
            goto done;
        }
        off += 0x18;
    }

    found = NULL;

done:
    if (found != NULL) {
        u8* inner = *(u8**)((char*)found + 0xC);
        u8 val = *(u8*)(inner + (low << 4) + 0x1C);
        if (val & 2) {
            return true;
        }
    }
    return false;
}
}

namespace jj_8005F2AC {
extern "C" bool fn_8005F2AC(void* self, s32 arg1) {
    u32 count = *(u32*)((char*)self + 0x5C);
    u32 i;
    u32 low = (u32)arg1 & 0xFFFF;
    s32 off;
    void* found;
    i = 0;
    off = 0;
    for (; i < count; i++) {
        u32* base = *(u32**)((char*)self + 0x58);
        char* entry = (char*)base + off;
        if (*(s32*)entry != 0 &&
            (u32)(arg1 >> 16) == (u32)*(s32*)(entry + 0x10)) {
            found = (char*)base + i * 0x18;
            goto done;
        }
        off += 0x18;
    }
    found = NULL;
done:
    if (found != NULL) {
        u8* inner = *(u8**)((char*)found + 0xC);
        u8 val = *(u8*)(inner + (low << 4) + 0x1C);
        if (val & 4) return true;
    }
    return false;
}
}

namespace jj_800607C8 {
typedef unsigned long u32;

struct Entry {
    char pad[0x44];
    int model;
    char pad48[0x34];
    u32 key;
    u32 tail;
};

extern Entry* lbl_805A0164;

class EcMgr {
public:
    char pad[0x60];
    Entry* entries;
    char pad64[8];
    int active;
    u32 first;
    u32 last;

    Entry* find(u32 key) {
        if (key && active) {
            Entry* entry = lbl_805A0164;
            if (entry && entry->key == key)
                return entry;

            entry = &entries[first];
            for (u32 i = first; i <= last; ++i, ++entry) {
                if (entry->key == key) {
                    lbl_805A0164 = entry;
                    return entry;
                }
            }
        }
        return 0;
    }
};

extern "C" int fn_800607C8(EcMgr* self, u32 key) {
    Entry* entry = self->find(key);
    return !entry ? 0 : entry->model;
}
}

namespace jj_80060858 {
typedef unsigned long u32;

struct Entry {
    char pad[0x48];
    int model;
    char pad4C[0x30];
    u32 key;
    u32 tail;
};

extern Entry* lbl_805A0164;

class EcMgr {
public:
    char pad[0x60];
    Entry* entries;
    char pad64[8];
    int active;
    u32 first;
    u32 last;

    Entry* find(u32 key) {
        if (key && active) {
            Entry* entry = lbl_805A0164;
            if (entry && entry->key == key)
                return entry;

            entry = &entries[first];
            for (u32 i = first; i <= last; ++i, ++entry) {
                if (entry->key == key) {
                    lbl_805A0164 = entry;
                    return entry;
                }
            }
        }
        return 0;
    }
};

extern "C" int fn_80060858(EcMgr* self, u32 key) {
    Entry* entry = self->find(key);
    return !entry ? 0 : entry->model;
}
}

// TODO: clean up, give variables real names
namespace jj_80061160 {

struct Tbl1 { u32 v[134]; };
struct Tbl2 { const char* v[134]; };

struct Entry {
    u32 active;
    u8 pad04[4];
    void* p8;
    u8 pad0C[4];
    u32 id;
    u8 pad14[4];
};

struct Owner {
    u8 pad00[0x58];
    Entry* entries;
    u32 count;
    u8 pad60[0x21];
    u8 kind;
    u8 pad82[2];
    u32 slot[4];
};

static inline Entry* findEntry(Owner* self, u32 id) {
    for (u32 i = 0; i < self->count; i++) {
        if (self->entries[i].active && self->entries[i].id == id)
            return &self->entries[i];
    }
    return 0;
}

extern "C" {
extern const Tbl1 lbl_80407198;
extern const Tbl2 lbl_804073B0;
extern void* g_GameGlobal;
u32 fn_8004E958(void* p);
void* fn_8018D3F0(void* a, const char* b);

void fn_80061160(Owner* self, u32 a1, s32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u8 a7, f32 f) {
    Tbl1 ids = lbl_80407198;
    Tbl2 names = lbl_804073B0;

    if (a2 >= 1) {
        u32 idx = a2 - 1;
        if (idx < 0x86) {
            Entry* found = findEntry(self, ids.v[idx]);
            if (found) {
                u32 sp10 = (u32)fn_8018D3F0(found->p8, names.v[idx]);
                u32* pSp10 = &sp10;
                if (sp10 != 0) {
                    u32 flag = 0;
                    if (a2 >= 0x55 && a2 <= 0x59) {
                        flag = 1;
                    } else if (a2 >= 0x5F && a2 <= 0x63) {
                        flag = 1;
                    } else if (a2 >= 0x69 && a2 <= 0x6D) {
                        flag = 1;
                    } else if (a2 >= 0x73 && a2 <= 0x77) {
                        flag = 1;
                    } else {
                        switch (a2) {
                        case 0xA:
                        case 0xB:
                        case 0x10:
                        case 0x1E:
                        case 0x1F:
                        case 0x20:
                        case 0x22:
                        case 0x23:
                        case 0x25:
                        case 0x28:
                        case 0x2B:
                        case 0x2E:
                        case 0x31:
                        case 0x34:
                        case 0x37:
                        case 0x3A:
                        case 0x3D:
                        case 0x40:
                        case 0x43:
                        case 0x46:
                        case 0x49:
                        case 0x4C:
                        case 0x4F:
                        case 0x7B:
                        case 0x82:
                            flag = 1;
                            break;
                        }
                    }
                    ((ecTraceMgr*)((u8*)self + 0x98))->start(a1, (u32)pSp10, a3, a4, a5, self->slot[self->kind], a6, a7, flag, f);
                    return;
                }
            }
        }
    }
    fn_8004E958(g_GameGlobal);
    ((ecTraceMgr*)((u8*)self + 0x98))->start(a1, a3, a4, a5, self->slot[self->kind], a6, a7, 0, f);
}
}
}

// TODO: clean up, give variables real names
namespace jj_8005F56C {
struct Entry {
    u8 pad[0x7C];
    s32 active;
    u8 group;
    u8 pad81[3];
};

struct Manager {
    u8 pad[0x60];
    Entry* entries;
    u8 pad64[8];
    s32 count;
    s32 first;
    s32 last;
    s32 freeIndex;
};

extern "C" u32 fn_80061944(Entry*, f32);

extern "C" u8 fn_8005F56C(Manager* self, int group, u32* low, u32* high, f32 value) {
    u32 i;
    Entry* entry = self->entries + self->first;
    u8 next = 0;

    for (i = self->first; i <= self->last;) {
        if (entry->active) {
            if (!(!(group != (int)entry->group))) {
                if (group < (int)entry->group && !next)
                    next = entry->group;
            } else {
                if (fn_80061944(entry, value) == 1) {
                    self->count--;
                    if (self->freeIndex > i)
                        self->freeIndex = i;
                }
                if (*low > i)
                    *low = i;
                if (*high < i)
                    *high = i;
            }
        }
        ++i;
        ++entry;
    }
    return next;
}
}

namespace jj_8005EB68 {
struct Entry {
    int active;
    unsigned char pad04[0x0C];
    unsigned int id;
    unsigned int pad14;
};

struct Record {
    unsigned char pad00[0x30];
    Entry* entry;
    unsigned char pad34[0x48];
    int active;
    unsigned int pad80;
};

struct Owner {
    unsigned char pad00[0x58];
    Entry* entries;
    unsigned int count;
    Record* records;
    unsigned char pad64[8];
    int enabled;
    unsigned int first;
    unsigned int last;
};

extern "C" void end__12ecEffectWorkFbb(Record*, int, int);
extern "C" void release__10ecResourceFb(Entry*, int);

static inline Entry* findEntry(Owner* self, unsigned int id)
{
    for (unsigned int i = 0; i < self->count; ++i) {
        if (self->entries[i].active && self->entries[i].id == id)
            return &self->entries[i];
    }
    return 0;
}

extern "C" void fn_8005EB68(Owner* self, unsigned int id)
{
    Entry* entry = findEntry(self, id);
    if (entry) {
        if (self->enabled) {
            Record* record;
            unsigned int i = self->first;
            record = &self->records[i];
            for (; i <= self->last; ++i, ++record) {
                if (record->active && record->entry->id == id)
                    end__12ecEffectWorkFbb(record, 1, 1);
            }
        }
        release__10ecResourceFb(entry, 0);
    }
}
}

namespace jj_80060A18 {

struct Entry {
    char pad[0x7C];
    u32 key;
    u32 tail;
};

extern Entry* lbl_805A0164;

struct Manager {
    char pad[0x60];
    Entry* entries;
    char pad64[8];
    int active;
    u32 first;
    u32 last;

    Entry* find(u32 key) {
        if (key && active) {
            Entry* entry = lbl_805A0164;
            if (entry && entry->key == key)
                return entry;

            entry = &entries[first];
            for (u32 i = first; i <= last; ++i, ++entry) {
                if (entry->key == key) {
                    lbl_805A0164 = entry;
                    return entry;
                }
            }
        }
        return 0;
    }
};

struct Resource {
    char pad[0x14];
    int offset;

    void* data() {
        return offset ? (char*)this + offset : 0;
    }
};

extern "C" Resource* GetResTex__Q34nw4r3g3d7ResFileCFi(void*, void*);
extern "C" void change_mdl_tex__12ecEffectWorkFPCcPCcPQ34nw4r3g3d7ResFilei(Entry*, int, void*, void*, int);

extern "C" void fn_80060A18(
    Manager* self, u32 key, int arg2, void* arg3, void* arg4, int arg5
) {
    Entry* entry = self->find(key);
    if (entry) {
        Resource* resource = GetResTex__Q34nw4r3g3d7ResFileCFi(arg4, arg3);
        change_mdl_tex__12ecEffectWorkFPCcPCcPQ34nw4r3g3d7ResFilei(entry, arg2, resource->data(), arg4, arg5);
    }
}
}

namespace jj_8005F08C {
struct Entry {
    int active;
    unsigned char pad04[0x0C];
    unsigned int id;
    unsigned int pad14;
};

struct Record {
    unsigned char pad00[0x30];
    Entry* entry;
    unsigned char pad34[0x48];
    int active;
    unsigned int pad80;
};

struct Owner {
    unsigned char pad00[0x58];
    Entry* entries;
    unsigned int count;
    Record* records;
    unsigned char pad64[8];
    int enabled;
    unsigned int first;
    unsigned int last;
};

extern "C" int searchResourceID__5ecMgrFPc(Owner*);
extern "C" void end__12ecEffectWorkFbb(Record*, int, int);
extern "C" void release__10ecResourceFb(Entry*, int);

static inline void findEntry(Owner* self, unsigned int id, Entry*& result)
{
    for (unsigned int i = 0; i < self->count; ++i) {
        if (self->entries[i].active && self->entries[i].id == id) {
            result = &self->entries[i];
            return;
        }
    }
    result = 0;
}

extern "C" void fn_8005F08C(Owner* self)
{
    int id = searchResourceID__5ecMgrFPc(self);
    if (id < 0x137) {
        Record* record;
        Entry* entry;
        findEntry(self, id, entry);
        if (entry) {
            if (self->enabled) {
                unsigned int i = self->first;
                record = &self->records[i];
                for (; i <= self->last; ++i, ++record) {
                    if (record->active && record->entry->id == (unsigned int)id)
                        end__12ecEffectWorkFbb(record, 1, 1);
                }
            }
            release__10ecResourceFb(entry, 0);
        }
    }
}
}

namespace jj_800613B8 {

extern unsigned char lbl_8040ABC0[0x18];
extern unsigned char lbl_8040AC08[0x18];
extern "C" void GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(nw4r::g3d::ScnMdlSimple* self, u32 arg1, Matrix* arg2);
extern "C" void GetScnMtxPos__Q34nw4r3g3d12ScnMdlSimpleCFPQ34nw4r4math5MTX34Q44nw4r3g3d6ScnObj13ScnObjMtxTypeUl(const nw4r::g3d::ScnMdlSimple* self, Matrix* out, u32 type, u32 arg3);

class VtblProbe {
public:
    virtual bool f0(void* a, void* b);
};

extern "C" void fn_800613B8(Matrix* arg0, nw4r::g3d::ScnMdlSimple* arg1, u32 arg2, Vec3f* arg3, Vec3f* arg4, Vec3f* arg5) {
    Matrix sp10(true);
    sp10.setSRT(*arg3, *arg4, *arg5);

    bool ok = false;
    if (arg1 != NULL) {
        void* spC = lbl_8040ABC0;
        if (((VtblProbe*)arg1)->f0(&spC, lbl_8040ABC0)) {
            ok = true;
        }
    }
    nw4r::g3d::ScnMdlSimple* sel = ok ? arg1 : NULL;
    if (sel != NULL) {
        GetScnMtxPos__Q34nw4r3g3d12ScnMdlSimpleCFPQ34nw4r4math5MTX34Q44nw4r3g3d6ScnObj13ScnObjMtxTypeUl(sel, arg0, 1, arg2);
    } else {
        bool ok2 = false;
        if (arg1 != NULL) {
            void* sp8 = lbl_8040AC08;
            if (((VtblProbe*)arg1)->f0(&sp8, lbl_8040AC08)) {
                ok2 = true;
            }
        }
        nw4r::g3d::ScnMdlSimple* sel2 = ok2 ? arg1 : NULL;
        if (sel2 != NULL) {
            GetScnMtxPos__Q34nw4r3g3d12ScnMdlSimpleCFPQ34nw4r4math5MTX34Q44nw4r3g3d6ScnObj13ScnObjMtxTypeUl(*(nw4r::g3d::ScnMdlSimple**)((char*)sel2 + 0xE8), arg0, 1, arg2);
        } else {
            GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(arg1, 1, arg0);
        }
    }
    arg0->mul(&sp10, arg0);
}
}

