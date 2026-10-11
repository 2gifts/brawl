// dvdfs: the disc file system. the fst is a flat table of 12 byte nodes in memory (set up at boot), paths
// are walked component by component; files are read through dvd.c command blocks.
#include "revolution/dvd.h"
#include "revolution/os.h"
#include "revolution/os/OSBootInfo.h"
#include <cctype>

#define DVD_RESULT_CANCELED (-3)
#define DVD_RESULT_FATAL (-1)

typedef struct DVDNode {
    union {
        struct {
            u32 is_folder : 8;
            u32 name : 24;
        };
        u32 packed_type_name;
    }; // 0x0

    union {
        struct {
            u32 offset;
            u32 size;
        };

        struct {
            u32 parent;
            u32 sibling_next;
        } folder;
    }; // 0x4
} DVDNode;

#define DVDNodeIsFolder(node) (((node).packed_type_name & 0xFF000000))
#define DVDNodeGetName(node) ((node).packed_type_name & 0x00FFFFFF)

extern volatile u32 __DVDLayoutFormat;
extern BOOL DVDGetCurrentDir(char* buffer, u32 maxlen);

// MATCH-ONLY: retail has 0x10 bytes of unreferenced sbss after the statics (sbss comes out in reverse
// declaration order, so these sit first), keeps dvd.c's sbss where it is
#pragma force_active on
volatile s64 __DVDFsUnreferencedA;
volatile s64 __DVDFsUnreferencedB;
#pragma force_active reset

static OSBootInfo* BootInfo;
static const DVDNode* FstStart;
static char* FstStringStart;
static u32 MaxEntryNum;

OSThreadQueue __DVDThreadQueue;
BOOL __DVDLongFileNameFlag = TRUE;

static s32 currentDirectory;

static void cbForReadAsync(s32 result, DVDCommandBlock* block);
static void cbForReadSync(s32 result, DVDCommandBlock* block);
static void cbForSeekSync(s32 result, DVDCommandBlock* block);

void __DVDFSInit(void) {
    BootInfo = (OSBootInfo*)OSPhysicalToCached(0);
    FstStart = (const DVDNode*)BootInfo->FSTLocation;

    if (FstStart != NULL) {
        // the root entry's size field holds the entry count
        MaxEntryNum = FstStart->size;
        // the string table follows the nodes
        FstStringStart = (char*)&FstStart[MaxEntryNum];
    }
}

static BOOL isSame(const char* lhs, const char* rhs) {
    while (*rhs != '\0') {
        if (tolower(*lhs++) != tolower(*rhs++)) {
            return FALSE;
        }
    }

    if (*lhs == '/' || *lhs == '\0') {
        return TRUE;
    }

    return FALSE;
}

s32 DVDConvertPathToEntrynum(const char* path) {
    const DVDNode* new_var;
    const char* stringPtr;
    const char* name_end;
    u32 it;
    u32 anchor;
    const char* item_name;
    BOOL name_delimited_by_slash;
    s32 name_length;
    const char* backup_path;
    const char* extension_start;
    BOOL illegal_format;
    BOOL has_extension;

    backup_path = path;
    it = currentDirectory;

    while (TRUE) {
        // end of the string: whatever we have is the answer
        if (*path == '\0') {
            return it;
        }

        // a leading slash restarts from the root
        if (*path == '/') {
            it = 0;
            path++;
            continue;
        }

        // ../ .. ./ .
        if (path[0] == '.') {
            if (path[1] == '.') {
                if (path[2] == '/') {
                    it = FstStart[it].folder.parent;
                    path += 3;
                    continue;
                }
                if (path[2] == '\0') {
                    return FstStart[it].folder.parent;
                }
                goto check_format;
            }

            if (path[1] == '/') {
                path += 2;
                continue;
            }

            if (path[1] == '\0') {
                return it;
            }
        }

    check_format:
        if (!__DVDLongFileNameFlag) {
            has_extension = FALSE;
            illegal_format = FALSE;

            name_end = path;
            while (*name_end != '\0' && *name_end != '/') {
                if (*name_end == '.') {
                    // 8.3 names: at most 8 characters in front of the dot, only one dot
                    if (name_end - path > 8 || has_extension == TRUE) {
                        illegal_format = TRUE;
                        goto validate_format;
                    } else {
                        has_extension = TRUE;
                        extension_start = name_end + 1;
                    }
                } else if (*name_end == ' ') {
                    illegal_format = TRUE;
                }

                name_end++;
            }

        validate_format:
            if (has_extension == TRUE && name_end - extension_start > 3) {
                illegal_format = TRUE;
            }

            if (!(!illegal_format)) {
                OSPanic("dvdfs.c", 443,
                        "DVDConvertEntrynumToPath(possibly DVDOpen or DVDChangeDir or DVDOpenDir): specified directory or "
                        "file (%s) doesn't match standard 8.3 format. This is a temporary restriction and will be removed "
                        "soon\n",
                        backup_path);
            }
        } else {
            name_end = path;
            while (*name_end != '\0' && *name_end != '/') {
                name_end++;
            }
        }

        // the ternary is deliberate, an enum or a plain compare changes the codegen
        name_delimited_by_slash = (*name_end == '\0') ? 0 : 1;
        name_length = name_end - path;

        // walk the children of the current folder
        item_name = path;
        new_var = FstStart;
        for (anchor = it + 1; anchor < new_var[it].folder.sibling_next;
             anchor = DVDNodeIsFolder(FstStart[anchor]) ? FstStart[anchor].folder.sibling_next : anchor + 1) {
            if (DVDNodeIsFolder(FstStart[anchor]) == FALSE && name_delimited_by_slash == TRUE) {
                continue;
            }

            stringPtr = FstStringStart + DVDNodeGetName(FstStart[anchor]);

            if (isSame(item_name, stringPtr) == TRUE) {
                goto descend;
            }
        }

        return -1;

    descend:
        // these blocks sit at the end and are only reached through the goto
        if (!name_delimited_by_slash) {
            return anchor;
        }

        it = anchor;
        path += name_length + 1;
    }
}

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo* info) {
    if (entrynum < 0 || entrynum >= MaxEntryNum || DVDNodeIsFolder(FstStart[entrynum])) {
        return FALSE;
    }

    info->startAddr = FstStart[entrynum].offset >> __DVDLayoutFormat;
    info->length = FstStart[entrynum].size;
    info->callback = NULL;
    info->cb.state = DVD_STATE_END;

    return TRUE;
}

BOOL DVDOpen(const char* path, DVDFileInfo* info) {
    s32 entrynum = DVDConvertPathToEntrynum(path);

    if (entrynum < 0) {
        char dir[128];
        DVDGetCurrentDir(dir, sizeof(dir));

        OSReport("Warning: DVDOpen(): file '%s' was not found under %s.\n", path, dir);

        return FALSE;
    }

    if (DVDNodeIsFolder(FstStart[entrynum])) {
        return FALSE;
    }

    info->startAddr = FstStart[entrynum].offset >> __DVDLayoutFormat;
    info->length = FstStart[entrynum].size;
    info->callback = NULL;
    info->cb.state = DVD_STATE_END;

    return TRUE;
}

BOOL DVDClose(DVDFileInfo* info) {
    DVDCancel(&info->cb);
    return TRUE;
}

static u32 myStrncpy(char* dst, const char* src, u32 maxlen) {
    u32 i;
    for (i = maxlen; i != 0 && *src != '\0'; --i) {
        *dst++ = *src++;
    }

    return maxlen - i;
}

static u32 entryToPath(s32 entrynum, char* string, u32 maxlen) {
    const char* name;
    u32 written;

    if (entrynum == 0) {
        return 0;
    }

    name = FstStringStart + DVDNodeGetName(FstStart[entrynum]);

    written = entryToPath(FstStart[entrynum].folder.parent, string, maxlen);
    if (written == maxlen) {
        return written;
    }

    string[written++] = '/';
    return written + myStrncpy(string + written, name, maxlen - written);
}

static BOOL DVDConvertEntrynumToPath(s32 entrynum, char* string, u32 maxlen) {
    u32 written = entryToPath(entrynum, string, maxlen);

    if (written == maxlen) {
        string[maxlen - 1] = '\0';
        return FALSE;
    }

    if (DVDNodeIsFolder(FstStart[entrynum])) {
        if (written == maxlen - 1) {
            string[written] = '\0';
            return FALSE;
        }

        string[written++] = '/';
    }

    string[written] = '\0';
    return TRUE;
}

BOOL DVDGetCurrentDir(char* buffer, u32 maxlen) {
    return DVDConvertEntrynumToPath(currentDirectory, buffer, maxlen);
}

BOOL DVDReadAsyncPrio(DVDFileInfo* info, void* dst, s32 size, s32 offset, DVDCallback callback, s32 prio) {
    if (!(offset >= 0 && offset <= info->length)) {
        OSPanic("dvdfs.c", 823, "DVDReadAsync(): specified area is out of the file  ");
    }

    if (!(offset + size >= 0 && offset + size < info->length + 32)) {
        OSPanic("dvdfs.c", 829, "DVDReadAsync(): specified area is out of the file  ");
    }

    info->callback = callback;

    DVDReadAbsAsyncPrio(&info->cb, dst, size, info->startAddr + (offset >> 2), cbForReadAsync, prio);

    return TRUE;
}

static void cbForReadAsync(s32 result, DVDCommandBlock* block) {
    DVDFileInfo* info = (DVDFileInfo*)block;

    if (info->callback != NULL) {
        info->callback(result, info);
    }
}

s32 DVDReadPrio(DVDFileInfo* info, void* dst, s32 size, s32 offset, s32 prio) {
    DVDCommandBlock* block;
    BOOL enabled;
    s32 ret;

    if (!(offset >= 0 && offset <= info->length)) {
        OSPanic("dvdfs.c", 893, "DVDRead(): specified area is out of the file  ");
    }

    if (!(offset + size >= 0 && offset + size < info->length + 32)) {
        OSPanic("dvdfs.c", 899, "DVDRead(): specified area is out of the file  ");
    }

    block = &info->cb;
    if (!DVDReadAbsAsyncPrio(block, dst, size, info->startAddr + (offset >> 2), cbForReadSync, prio)) {
        return DVD_RESULT_FATAL;
    }

    enabled = OSDisableInterrupts();

    while (TRUE) {
        s32 state = block->state;

        if (state == DVD_STATE_END) {
            ret = block->transferredSize;
            break;
        } else if (state == DVD_STATE_FATAL_ERROR) {
            ret = DVD_RESULT_FATAL;
            break;
        } else if (state == DVD_STATE_CANCELED) {
            ret = DVD_RESULT_CANCELED;
            break;
        }

        OSSleepThread(&__DVDThreadQueue);
    }

    OSRestoreInterrupts(enabled);
    return ret;
}

static void cbForReadSync(s32 result, DVDCommandBlock* block) {
#pragma unused(result)
#pragma unused(block)

    OSWakeupThread(&__DVDThreadQueue);
}

s32 DVDSeekPrio(DVDFileInfo* info, s32 offset, s32 prio) {
    DVDCommandBlock* block;
    BOOL enabled;
    s32 ret;

    block = &info->cb;
    if (!DVDSeekAbsAsyncPrio(block, (info->startAddr + offset) >> 2, cbForSeekSync, prio)) {
        return DVD_RESULT_FATAL;
    }

    enabled = OSDisableInterrupts();

    while (TRUE) {
        s32 state = block->state;

        if (state == DVD_STATE_END) {
            ret = 0;
            break;
        } else if (state == DVD_STATE_FATAL_ERROR) {
            ret = DVD_RESULT_FATAL;
            break;
        } else if (state == DVD_STATE_CANCELED) {
            ret = DVD_RESULT_CANCELED;
            break;
        }

        OSSleepThread(&__DVDThreadQueue);
    }

    OSRestoreInterrupts(enabled);
    return ret;
}

static void cbForSeekSync(s32 result, DVDCommandBlock* block) {
#pragma unused(result)
#pragma unused(block)

    OSWakeupThread(&__DVDThreadQueue);
}
