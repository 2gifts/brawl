#include <types.h>

#include <mu/mu_collection_viewer.h>

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
