#pragma once

#include <gf/gf_task.h>
#include <types.h>

class muRecorder : public gfTask {
public:
    class Data {
    public:
        void* unk0;
        Data() : unk0(0) {}
        ~Data();
        void reset(void* data);
    };
    Data getAdvData();
    // HYPOTHESIS: only the adventure request words are reconstructed here.
    u8 unk40[0xC0];
    int unk100;
    int unk104;
    void* unk108;
    u32 unk10C;

    // HYPOTHESIS: the adventure slot is a word, narrowed by gfSysRecorder2.
    void saveAdv(u32 slot);
    void loadAdv(u32 slot);
};
