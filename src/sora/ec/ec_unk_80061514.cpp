#include <ec/ec_mgr.h>
#include <ef/ef_effect.h>
#include <gf/gf_model.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>
#include <ut/ut_nw.h>

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
