#include <types.h>

#include <mu/mu_collection_viewer.h>
#include <mu/mu_notice_wnd.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>

// The base of the menus that browse through the collection (module sora_menu_collect_viewer). Only a part of its functions
// is decompiled.
/// Function names come from the sora_menu_collect_viewer map.

// 0x10 bytes of the game data: two words that tell how the files of each medium are counted
extern int lbl_80423690[4];

u32 muCollectionViewer::getCurFileID() {
    Work* work = m_work;
    int numFiles;
    if (work->m_extraPanel) {
        numFiles = work->m_numFiles - 1;
    } else {
        numFiles = work->m_numFiles;
    }
    u32 numPic = getNumPic(work->m_curMedia);
    u32 id = 0x80000000;
    if ((u32)numFiles < numPic) {
        id = numFiles;
    }
    return id;
}

muCollectionFileName muCollectionViewer::getFileHandle(int index) {
    return muCollectionFileName(m_work->m_media[m_work->m_curMedia].m_names, index);
}

int muCollectionViewer::getNumPic(int media) {
    return m_work->m_media[media].m_numPic;
}

int muCollectionViewer::getNumPanel(int media) {
    Work* work = m_work;
    u8 extraPanel = work->m_extraPanel;
    return extraPanel + getNumPic(media);
}

void muCollectionViewer::getNewLabel() { }

int muCollectionViewer::dispThumbnail(MuObject* object, void* summary) {
    return 0;
}

void muCollectionViewer::dispPreviewCore(void* info) { }

void muCollectionViewer::erasePreviewCore() { }

int muCollectionViewer::unk_2FA0() {
    Work* work = m_work;
    int* table = lbl_80423690;
    if (work->m_curMedia == 0) {
        return table[3];
    }
    return table[2];
}

void muCollectionViewer::unk_3134() {
    addObjCore(m_work->m_objs[23]->m_sceneModel);
    addObjCore(m_work->m_objs[32]);
}

void muCollectionViewer::unk_3194(int cur, int count) {
    int left = cur > 0;
    int right;
    if (count != 0) {
        right = (u32)(count - 1) > (u32)cur;
    } else {
        right = 0;
    }
    unk_31D8(left, right);
}

void muCollectionViewer::unk_31D8(int left, int right) {
    int flags = 0;
    if (left) {
        flags |= 1;
    }
    if (right) {
        flags |= 2;
    }
    m_work->m_objs[23]->setFrameTex((float)flags);
}

void muCollectionViewer::unk_38F8(int flag) {
    float frame;
    if (flag == 0) {
        frame = 1.0f;
    } else {
        frame = 0.0f;
    }
    if (m_work->m_flag1C74 == 0) {
        frame += 2.0f;
    }
    m_work->m_objs[1]->setFrameVisible(frame);
}

void muCollectionViewer::onLoadFailed() { }

void muCollectionViewer::controlBrowsing() { }

void muCollectionViewer::controlBrowsingLoading() { }

void muCollectionViewer::controlPopUpStart() { }

void muCollectionViewer::controlPopUp() { }

void muCollectionViewer::controlPopUpCheckStart() { }

void muCollectionViewer::controlPopUpChecking() { }

void muCollectionViewer::controlNewDataStart() { }

void muCollectionViewer::controlNewDataChecking() { }

void muCollectionViewer::controlNewData() { }

void muCollectionViewer::controlNewDataExit() { }

void muCollectionViewer::controlScanStart(int drive, int unk, int unk2) {
    Work* work = m_work;
    muCollection* collection = (muCollection*)work->m_collection;
    if (collection->m_busy == 0) {
        if (unk2 == 0 && collection->m_result != 0) {
            work->m_exitRequest = 0;
        } else {
            collection->scan(drive, unk, unk2);
            m_work->m_exitRequest = 6;
        }
    }
}

void muCollectionViewer::controlScanAllStart(int unk) {
    muCollection* collection = (muCollection*)m_work->m_collection;
    if (collection->m_busy == 0) {
        collection->scanAll(1, unk);
        m_work->m_exitRequest = 6;
    }
}

void muCollectionViewer::controlScan() {
    Work* work = m_work;
    muCollection* collection = (muCollection*)work->m_collection;
    if (collection->m_busy == 0) {
        work->m_exitRequest = 0;
    }
}

void muCollectionViewer::controlWaitForDialog(int step) {
    Work* work = m_work;
    muNoticeWndTask* wnd = work->m_wnd;
    if (wnd == NULL || wnd->m_step == 3) {
        if (wnd != NULL) {
            wnd->exit();
        }
        work->m_wnd = NULL;
        m_work->m_state = step;
    }
}

void muCollectionViewer::closeWaitWnd() {
    m_work->m_objs[22]->m_scnMdl->DetachFromParent();
}

void muCollectionViewer::endErase() {
    closeEraseReportWnd();
    m_work->m_state = 7;
    addObjCore(m_work->m_objs[23]->m_sceneModel);
}

void muCollectionViewer::endCopy() {
    closeEraseReportWnd();
    m_work->m_state = 10;
}

void muCollectionViewer::openMediaMenu(int attach, int second) {
    int index = 2;
    if (second == 0) {
        index = 1;
    }
    if (attach != 0) {
        addObjCore(*(MuObject**)((char*)m_work + index * 4 + 0x184));
    } else {
        removeObjCore(*(MuObject**)((char*)m_work + index * 4 + 0x184));
    }
}

void muCollectionViewer::unk_64C8(int index) {
    m_work->m_msg->printIndex(0xF, index, m_work->m_msgFile[1].m_handle.getBuffer());
}

void muCollectionViewer::closeEraseReportWnd() {
    m_work->m_msg->printf(0xF, "");
    removeObjCore(m_work->m_objs[35]);
    g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
}

void muCollectionViewer::openEraseReportWnd() {
    m_work->m_objs[2]->setFrameNode(2.0f);
    addObjCore(m_work->m_objs[35]);
    g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
}

void muCollectionViewer::dispMediaMenuSelect(int selected) {
    float frame = (float)selected;
    m_work->m_objs[3]->setFrameNode(frame);
    m_work->m_objs[4]->setFrameNode(frame);
}

void muCollectionViewer::closeEraseConfirmWnd() {
    m_work->m_msg->printf(0xC, "");
    m_work->m_msg->printf(0xD, "");
    m_work->m_msg->printf(0xE, "");
    removeObjCore(m_work->m_objs[35]);
}

void muCollectionViewer::openWaitWnd() {
    MuObject* obj = m_work->m_objs[22];
    ((nw4r::g3d::ScnMdlExpand*)m_work->m_objs[35])->PushBack(obj->m_sceneModel, (u32)0);
    obj->setFrameNode(0.0f);
    obj->setFrameTexSrt(0.0f);
    Vec3f trans(0.0f, 7.0f, 0.0f);
    obj->setTrans(&trans);
}

void muCollectionViewer::beginCopy(int flag) {
    openWaitWnd();
    openEraseReportWnd();
    int index = 2;
    if (flag != 0) {
        index = 0;
    }
    unk_64C8(index);
    int state = 0xE;
    if (flag != 0) {
        state = 0xB;
    }
    m_work->m_state = state;
}
