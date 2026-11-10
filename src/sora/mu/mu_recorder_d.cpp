#include <mu/mu_collection.h>
#include <mu/mu_recorder.h>
#include <string.h>

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

muCollection::FileHandle::FileHandle(const char* name) {
    m_name[0] = 0;
    strcat(m_name, lbl_8059E148);
    m_name[0] = 0;
    strcat(m_name, name);
}

muCollection::FileInfo::FileInfo() : unk0(0), unk1(0), m_count(0), m_free(0), m_names(0) {
}

void* muCollection::lock() {
    m_busy = 1;
    return m_io;
}

// HYPOTHESIS: processDefault takes the three words as (drive, data, size); see the note at muCollection::m_argFlag.
void muCollection::write(void* data, int size, int unk) {
    m_argFlag = 0;
    m_argData = data;
    m_writeName = 0;
    m_argSize = size;
    m_argUnk = unk;
    m_busy = 4;
}

void muCollection::overwriteIf1000(FileKey key, int size, int unk) {
    s32 id = key.m_drive;
    m_keyName[0] = 0;
    strcat(m_keyName, key.m_name);
    m_argFlag = 1;
    m_argId = id;
    m_writeName = m_keyName;
    m_argSize = size;
    m_argUnk = unk;
    m_busy = 4;
}

void muCollection::read(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 5;
    }
}

void muCollection::copy(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 7;
    }
}

void muCollection::erase(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        FileName fileName;
        fileName.m_name[0] = 0;
        strcat(fileName.m_name, lbl_8059E148);
        fileName.m_name[0] = 0;
        strcat(fileName.m_name, name.m_name);
        m_readName = fileName;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 6;
    }
}

void muCollection::erase(const FileKey& key) {
    s32 id = key.m_drive;
    FileName fileName;
    fileName.m_name[0] = 0;
    strcat(fileName.m_name, lbl_8059E148);
    fileName.m_name[0] = 0;
    strcat(fileName.m_name, key.m_name);
    m_readName = fileName;
    m_argDrive = id;
    m_argData = m_readName.m_name;
    m_busy = 6;
}

void muCollection::move(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 8;
    }
}

void muCollection::readSummary(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_summaryName = name;
        m_argDrive = drive;
        m_argData = m_summaryName.m_name;
        m_busy = 9;
    }
}

void muCollection::checkLimit(int drive) {
    m_argDrive = drive;
    m_busy = 3;
}

void muCollection::scanAll(int unk, int unk2) {
    m_scanDrive0 = 1;
    m_scanDrive1 = 1;
    m_scanFlagA = unk;
    m_scanFlagB = unk2;
    m_busy = 2;
}

void muCollection::scan(int drive, int unk, int unk2) {
    m_scanDrive0 = drive == 0;
    m_scanDrive1 = drive == 1;
    m_scanFlagA = unk;
    m_scanFlagB = unk2;
    m_busy = 2;
}
