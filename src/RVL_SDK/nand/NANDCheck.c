// NANDCheck: space accounting for the nand. how many blocks/files the title's home dir and the user dirs take,
// and whether a given amount would still fit.
#include <revolution/NAND.h>

// built as c++ against the openrvl headers, keep c linkage
extern "C" {

extern s32 ISFS_GetStats(void *stats);

#define NAND_HOME_MAX_BLOCKS 0x400
#define NAND_HOME_MAX_FILES 0x21
#define NAND_USER_MAX_BLOCKS 0x4400
#define NAND_USER_MAX_FILES 0xFA0

static const char *USER_DIR_LIST[] = {"/meta",
                                      "/ticket",
                                      "/title/00010000",
                                      "/title/00010001",
                                      "/title/00010003",
                                      "/title/00010004",
                                      "/title/00010005",
                                      "/title/00010006",
                                      "/title/00010007",
                                      "/shared2/title",
                                      NULL,
                                      NULL};

// copies the fs statistics (cluster size, free/used/bad/reserved clusters, free/used inodes) to the caller
s32 fn_8020D7BC(u32 *stats) {
    u32 raw[7];
    s32 result;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    result = ISFS_GetStats(raw);
    if (result == IPC_RESULT_OK) {
        stats[0] = raw[0];
        stats[1] = raw[1];
        stats[2] = raw[2];
        stats[3] = raw[3];
        stats[4] = raw[4];
        stats[5] = raw[5];
        stats[6] = raw[6];
    }

    return nandConvertErrorCode(result);
}

static s32 nandCalcUsage(s32 *blockCountOut, s32 *fileCountOut, const char **dirList) {
    s32 result = -117;

    *blockCountOut = 0;
    *fileCountOut = 0;

    for (; *dirList != NULL; dirList++) {
        s32 blockCount = 0;
        s32 fileCount = 0;

        result = ISFS_GetUsage(*dirList, &blockCount, &fileCount);
        if (result == IPC_RESULT_OK) {
            *blockCountOut += blockCount;
            *fileCountOut += fileCount;
        } else if (result == IPC_RESULT_NOEXISTS) {
            result = IPC_RESULT_OK;
        } else {
            break;
        }
    }

    return result;
}

static s32 nandCalcUserUsage(s32 *blockCountOut, s32 *fileCountOut) {
    return nandCalcUsage(blockCountOut, fileCountOut, USER_DIR_LIST);
}

static u32 nandCheck(u32 neededBlocks, u32 neededFiles, u32 homeDirBlocks, u32 homeDirFiles, u32 userBlocks,
                     u32 userFiles) {
    u32 answer = 0;

    if (homeDirBlocks + neededBlocks > NAND_HOME_MAX_BLOCKS) {
        answer |= NAND_CHECK_TOO_MANY_APP_BLOCKS;
    }

    if (homeDirFiles + neededFiles > NAND_HOME_MAX_FILES) {
        answer |= NAND_CHECK_TOO_MANY_APP_FILES;
    }

    if (userBlocks + neededBlocks > NAND_USER_MAX_BLOCKS) {
        answer |= NAND_CHECK_TOO_MANY_USER_BLOCKS;
    }

    if (userFiles + neededFiles > NAND_USER_MAX_FILES) {
        answer |= NAND_CHECK_TOO_MANY_USER_FILES;
    }

    return answer;
}

s32 NANDCheck(u32 neededBlocks, u32 neededFiles, u32 *answer) {
    s32 result;
    s32 homeDirBlocks = -1;
    s32 homeDirFiles = -1;
    s32 userBlocks = -1;
    s32 userFiles = -1;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    result = ISFS_GetUsage(nandGetHomeDir(), &homeDirBlocks, &homeDirFiles);
    if (result != IPC_RESULT_OK) {
        return nandConvertErrorCode(result);
    }

    result = nandCalcUserUsage(&userBlocks, &userFiles);
    if (result != IPC_RESULT_OK) {
        return nandConvertErrorCode(result);
    }

    *answer = nandCheck(neededBlocks, neededFiles, homeDirBlocks, homeDirFiles, userBlocks, userFiles);

    return NAND_RESULT_OK;
}

// how many more blocks and files fit before NANDCheck would start complaining (the smaller of the app and
// user budgets). HYPOTHESIS: the error codes are converted and then dropped, retail always returns 0 here.
s32 fn_8020DA08(u32 *blocksLeft, u32 *filesLeft) {
    s32 userBlocks = -1;
    s32 userFiles = -1;
    s32 homeDirBlocks = -1;
    s32 homeDirFiles = -1;
    s32 result;
    u32 homeBlocksLeft;
    u32 homeFilesLeft;
    u32 userBlocksLeft;
    u32 userFilesLeft;

    result = ISFS_GetUsage(nandGetHomeDir(), &homeDirBlocks, &homeDirFiles);
    if (result != IPC_RESULT_OK) {
        nandConvertErrorCode(result);
    }

    homeDirFiles--;

    result = nandCalcUserUsage(&userBlocks, &userFiles);
    if (result != IPC_RESULT_OK) {
        nandConvertErrorCode(result);
    }

    userBlocksLeft = (u32)userBlocks > NAND_USER_MAX_BLOCKS ? 0 : NAND_USER_MAX_BLOCKS - userBlocks;
    userFilesLeft = (u32)userFiles > NAND_USER_MAX_FILES ? 0 : NAND_USER_MAX_FILES - userFiles;
    homeBlocksLeft = (u32)homeDirBlocks > NAND_HOME_MAX_BLOCKS ? 0 : NAND_HOME_MAX_BLOCKS - homeDirBlocks;
    homeFilesLeft = (u32)homeDirFiles > 0x20 ? 0 : 0x20 - homeDirFiles;

    *blocksLeft = userBlocksLeft < homeBlocksLeft ? userBlocksLeft : homeBlocksLeft;
    *filesLeft = userFilesLeft < homeFilesLeft ? userFilesLeft : homeFilesLeft;
    return NAND_RESULT_OK;
}

}
