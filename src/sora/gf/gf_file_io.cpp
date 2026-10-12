#include <types.h>
#include <gf/gf_file_io.h>

extern "C" int fn_803EC0CC(const char* path);
extern "C" int fn_803EBE4C();
extern "C" u32 strlen(const char* str);
extern "C" char* strcpy(char* dst, const char* src);

int gfFileIO::gfFACreateDir(const char* filepath)
{
    char dir[0x80];
    int result;
    u32 i;
    u32 end;
    u32 j;

    if (strlen(filepath) < 0x7F) {
        result = fn_803EC0CC(filepath);
        if (result != 0) {
            result = fn_803EBE4C();
            if (result != 0x11) {
                end = strlen(filepath);
                if (filepath[end - 1] == '/') {
                    end -= 1;
                }
                i = 3;
                while (i < end) {
                    strcpy(dir, filepath);
                    for (j = i; j < end; j++) {
                        i = j + 1;
                        if (dir[j] == '/') {
                            dir[j] = 0;
                            break;
                        }
                    }
                    result = fn_803EC0CC(dir);
                    if (result != 0) {
                        result = fn_803EBE4C();
                        if (result != 0x11) {
                            break;
                        }
                    }
                }
            }
        }
    }
    return result;
}
