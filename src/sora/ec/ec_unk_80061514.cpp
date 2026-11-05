#include <ec/ec_mgr.h>
#include <ef/ef_effect.h>
#include <gf/gf_model.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>
#include <ut/ut_nw.h>

void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* mdl, u32 nodeId, const Matrix* mtx);

void ecEffectWork::render() {
    if (unk24 == 2) {
        unk24 = 3;
    }
}

void ecEffectWork::release(bool p1) {
    if (unk44 != NULL) {
        if (unk40 != NULL) {
            unk40->Remove(unk44);
            unk40 = NULL;
        }
        if (unk4C != NULL) {
            if (unk4C != NULL) {
                delete unk4C;
                unk4C = NULL;
            }
        }
        if (unk48 != NULL) {
            gfModelAnimation::unbind(unk44);
            gfModelAnimation* anim = unk48;
            if (anim->m_anmObjChrRes != NULL) {
                anim->m_anmObjChrRes->Destroy();
            }
            if (anim->m_anmObjVisRes != NULL) {
                anim->m_anmObjVisRes->Destroy();
            }
            if (anim->m_anmObjTexPatRes != NULL) {
                anim->m_anmObjTexPatRes->Destroy();
            }
            if (anim->m_anmObjTexSrtRes != NULL) {
                anim->m_anmObjTexSrtRes->Destroy();
            }
            if (anim->m_anmObjMatClrRes != NULL) {
                anim->m_anmObjMatClrRes->Destroy();
            }
            if (anim->m_anmObjShpRes != NULL) {
                anim->m_anmObjShpRes->Destroy();
            }
            if (anim->m_resFile.ptr() != NULL) {
                if ((u8)anim->_spacer[0] != 0) {
                    gfHeapManager::free(anim->m_resFile.ptr());
                    anim->m_resFile = nw4r::g3d::ResFile((void*)NULL);
                }
            }
            anim->m_anmObjChrRes = NULL;
            anim->m_anmObjVisRes = NULL;
            anim->m_anmObjTexPatRes = NULL;
            anim->m_anmObjTexSrtRes = NULL;
            anim->m_anmObjMatClrRes = NULL;
            anim->m_anmObjShpRes = NULL;
            gfHeapManager::free(unk48);
            unk48 = NULL;
        }
        unk44->Destroy();
        unk44 = NULL;
    }
    if (p1 == true && unk34_ != NULL) {
        for (u32 i = 0; i < unk38; i++) {
            if (unk34_[i] != NULL) {
                unk34_[i]->endForce();
                unk34_[i] = NULL;
            }
        }
    }
    if (unk34_ != NULL) {
        delete[] unk34_;
        unk34_ = NULL;
    }
}

void ecEffectWork::pause() {
    if (!(unk58 & 8)) {
        unk58 = (unk58 & ~2) | 4;
    }
}

void ecEffectWork::play() {
    if (!(unk58 & 8)) {
        unk58 = (unk58 & ~4) | 2;
    }
}

void ecEffectWork::end(bool p1, bool p2) {
    if (!(unk58 & 0x10)) {
        if (p1) {
            if (unk34_ != NULL) {
                for (u32 i = 0; i < unk38; i++) {
                    if (unk34_[i] != NULL) {
                        unk34_[i]->endForce();
                        unk34_[i] = NULL;
                    }
                }
            }
        }
        if (p2 || unk3C < 0) {
            unk3C = 0;
        }
        unk58 = 8;
        if (p1 == true && p2 == true) {
            if (unk44 != NULL) {
                nwSMSetVisibility(unk44, false);
                if (unk4C != NULL) {
                    unk4C->SetCallbackCondition(false);
                }
                if (unk40 != NULL) {
                    unk40->Remove(unk44);
                    unk40 = NULL;
                }
            }
            unk58 = (unk58 | 0x10) & ~0x20;
        } else if (!p1) {
            unk58 |= 0x20;
        }
    }
}

void ecEffectWork::set_parent(nw4r::g3d::ScnMdl* p1, const char* p2, bool p3) {
    nw4r::g3d::ResMdl mdl = p1->m_resMdl;
    nw4r::g3d::ResNode node = mdl.GetResNode(p2);
    u32 idx = node.IsValid() ? (u16)node.GetID() : 0;
    set_parent(p1, idx, p3);
}

void ecEffectWork::set_parent(nw4r::g3d::ScnMdl* p1, u32 p2, bool p3) {
    unk50 = p1;
    if (p1 != NULL) {
        unk54 = p2;
        unk56 &= ~1;
        if (p3) {
            unk56 |= 1;
        }
        init_attach();
    }
}

void ecEffectWork::set_delay(int frames) {
    if (unk24 != 0) {
        return;
    }
    unk28 = -frames;
}

void ecEffectWork::set_visible(bool visible) {
    if (visible) {
        unk56 &= ~0x8000;
        if (unk28 < 0) {
            return;
        }
        if (unk44 != NULL && unk3C != 0) {
            nwSMSetVisibility(unk44, true);
        }
        if (unk34_ != NULL) {
            for (u32 i = 0; i < unk38; i++) {
                if (unk34_[i] != NULL) {
                    unk34_[i]->unkD_b6 = true;
                }
            }
        }
    } else {
        unk56 |= 0x8000;
        if (unk44 != NULL) {
            nwSMSetVisibility(unk44, false);
        }
        if (unk34_ != NULL) {
            for (u32 i = 0; i < unk38; i++) {
                if (unk34_[i] != NULL) {
                    unk34_[i]->unkD_b6 = false;
                }
            }
        }
    }
}

void ecEffectWork::set_rot(const Vec3f* rot) {
    m_rot = *rot;
    if (unk64 == 0 && unk50 == NULL && unk44 != NULL) {
        Matrix mtx(true);
        mtx.setSRT(m_scale, m_rot, m_pos);
        ScnMdl_SetNodeMtx(unk44, 0, &mtx);
    }
    if (unk30->unkC->unk10[unk2C].unk4 != 0 && unk34_ != NULL) {
        efEffect* effect = unk34_[unk38 - 1];
        if (effect != NULL) {
            effect->m_rot = m_rot;
        }
    }
}

void ecEffectWork::set_scl(const Vec3f* scale) {
    m_scale = *scale;
    if (unk64 == 0 && unk50 == NULL && unk44 != NULL) {
        Matrix mtx(true);
        mtx.setSRT(m_scale, m_rot, m_pos);
        ScnMdl_SetNodeMtx(unk44, 0, &mtx);
    }
    if (unk30->unkC->unk10[unk2C].unk4 != 0 && unk34_ != NULL) {
        efEffect* effect = unk34_[unk38 - 1];
        if (effect != NULL) {
            effect->m_scale = m_scale;
        }
    }
}

void ecEffectWork::set_mdl_texpat(u32 patIndex) {
    if (unk48 == NULL) {
        return;
    }
    if (unk48->m_anmObjTexPatRes == NULL) {
        return;
    }
    unk56 |= 8;
    unk48->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    unk48->m_anmObjTexPatRes->SetFrame((float)patIndex);
}

void ecEffectWork::set_mdl_frame(float frame, s8 flags) {
    if (unk48 == NULL) {
        return;
    }
    if ((u8)flags == 0xFF) {
        unk48->setFrame(frame);
    } else {
        if (flags & 1) {
            if (unk48->m_anmObjChrRes != NULL) {
                unk48->m_anmObjChrRes->SetFrame(frame);
            }
        }
        if (flags & 2) {
            if (unk48->m_anmObjVisRes != NULL) {
                unk48->m_anmObjVisRes->SetFrame(frame);
            }
        }
        if (flags & 4) {
            if (unk48->m_anmObjTexPatRes != NULL) {
                unk48->m_anmObjTexPatRes->SetFrame(frame);
            }
        }
        if (flags & 8) {
            if (unk48->m_anmObjTexSrtRes != NULL) {
                unk48->m_anmObjTexSrtRes->SetFrame(frame);
            }
        }
        if (flags & 0x10) {
            if (unk48->m_anmObjMatClrRes != NULL) {
                unk48->m_anmObjMatClrRes->SetFrame(frame);
            }
        }
        if (flags & 0x20) {
            if (unk48->m_anmObjShpRes != NULL) {
                unk48->m_anmObjShpRes->SetFrame(frame);
            }
        }
    }
    unk3C = -1;
}

void ecEffectWork::set_mdl_calc_mtx(void (*callback)(nw4r::g3d::ScnMdl*, void*), void* arg) {
    unk56 |= 0x80;
    if (callback == NULL) {
        return;
    }
    unk70 = callback;
    unk74 = arg;
}

void ecEffectWork::change_mdl_tex(const char* matName, const char* texName, nw4r::g3d::ResFile* resFile, int filter) {
    if (unk44 == NULL) {
        return;
    }
    unk30->change_tex(unk44, unk2C, matName, texName, resFile, filter);
}

void ecEffectWork::update_odd(float frames) {
    if (unk5A > 1) {
        frames *= 1.0f / unk5A;
    }
    switch (unk24) {
    case 1:
        if (!(unk58 & 0x10)) {
            renew_attach();
            if (unk44 != NULL && unk3C != 0) {
                if (unk58 & 1) {
                    if (unk48 != NULL && !(unk56 & 0x7E)) {
                        unk48->setUpdateRate(0.0f);
                    }
                } else if (unk48 != NULL) {
                    if (!(unk56 & 0x7E)) {
                        unk48->setUpdateRate(frames);
                    } else {
                        if (!(unk56 & 2) && unk48->m_anmObjChrRes != NULL) {
                            unk48->m_anmObjChrRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 4) && unk48->m_anmObjVisRes != NULL) {
                            unk48->m_anmObjVisRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 8) && unk48->m_anmObjTexPatRes != NULL) {
                            unk48->m_anmObjTexPatRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 0x10) && unk48->m_anmObjTexSrtRes != NULL) {
                            unk48->m_anmObjTexSrtRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 0x20) && unk48->m_anmObjMatClrRes != NULL) {
                            unk48->m_anmObjMatClrRes->SetUpdateRate(frames);
                        }
                        if (!(unk56 & 0x40) && unk48->m_anmObjShpRes != NULL) {
                            unk48->m_anmObjShpRes->SetUpdateRate(frames);
                        }
                    }
                }
                unk44->UpdateFrame();
                unk44->SetScnObjOption(2, 0);
                unk44->G3dProc(1, 0, 0);
                unk44->SetScnObjOption(2, 1);
                if ((unk56 & 0x80) && unk70 != NULL) {
                    unk70(unk44, unk74);
                }
            }
        }
        break;
    }
}
