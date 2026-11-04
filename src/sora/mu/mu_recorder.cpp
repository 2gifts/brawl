#include <if/if_snap_save.h>
#include <mu/mu_recorder.h>
#include <string.h>

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

void muRecorder::saveAdv(u32 slot) {
    unk10C = slot;
    unk100 = 2;
}

void muRecorder::loadAdv(u32 slot) {
    unk10C = slot;
    unk100 = 3;
}

muCollection::FileHandle::FileHandle(const char* name) {
    unk0[0] = 0;
    strcat(unk0, lbl_8059E148);
    unk0[0] = 0;
    strcat(unk0, name);
}

muCollection::FileInfo::FileInfo() : unk0(0), unk1(0), unk8(0), unkC(0), unk10(0) {
}

void* muCollection::lock() {
    m_busy = 1;
    return unk110;
}

void muCollection::write(void* data, int size, int unk) {
    unk380_flag = 0;
    unk384_ptr = data;
    unk388 = 0;
    unk38C = size;
    unk390 = unk;
    m_busy = 4;
}

void muCollection::overwriteIf1000(FileKey key, int size, int unk) {
    s32 id = key.unk0;
    unk204[0] = 0;
    strcat(unk204, key.unk4);
    unk380_flag = 1;
    unk384_word = id;
    unk388 = unk204;
    unk38C = size;
    unk390 = unk;
    m_busy = 4;
}

void muCollection::read(int drive, FileName name) {
    bool differs = strcmp(name.unk0, lbl_8059E14C) != 0;
    if (differs) {
        unk15D = name;
        unk380_word = drive;
        unk384_ptr = unk15D.unk0;
        m_busy = 5;
    }
}

void muCollection::copy(int drive, FileName name) {
    bool differs = strcmp(name.unk0, lbl_8059E14C) != 0;
    if (differs) {
        unk15D = name;
        unk380_word = drive;
        unk384_ptr = unk15D.unk0;
        m_busy = 7;
    }
}

void muCollection::erase(int drive, FileName name) {
    bool differs = strcmp(name.unk0, lbl_8059E14C) != 0;
    if (differs) {
        FileName fileName;
        fileName.unk0[0] = 0;
        strcat(fileName.unk0, lbl_8059E148);
        fileName.unk0[0] = 0;
        strcat(fileName.unk0, name.unk0);
        unk15D = fileName;
        unk380_word = drive;
        unk384_ptr = unk15D.unk0;
        m_busy = 6;
    }
}

void muCollection::erase(const FileKey& key) {
    s32 id = key.unk0;
    FileName fileName;
    fileName.unk0[0] = 0;
    strcat(fileName.unk0, lbl_8059E148);
    fileName.unk0[0] = 0;
    strcat(fileName.unk0, key.unk4);
    unk15D = fileName;
    unk380_word = id;
    unk384_ptr = unk15D.unk0;
    m_busy = 6;
}
