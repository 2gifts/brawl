#include <st/loader/st_loader_player.h>
#include <types.h>

// TODO: clean up - the flag byte at 0x5D (_0x5b[2]) is tested by hand; name its bits in the header.
bool stLoaderPlayer::isLoad() {
    u32 flag = ((u8)_0x5b[2] >> 3) & 1;
    if (flag == 1) {
        return true;
    }
    return stLoader::isLoad();
}
