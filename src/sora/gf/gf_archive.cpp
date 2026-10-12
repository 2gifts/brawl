#include <gf/gf_archive.h>
#include <types.h>

void* gfArchive::getData(ARCNodeType nodeType, int fileIndex, u16 archiveId) {
    int nodeSize;

    return getData(0, nodeType, fileIndex, &nodeSize, archiveId);
}
