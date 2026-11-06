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
