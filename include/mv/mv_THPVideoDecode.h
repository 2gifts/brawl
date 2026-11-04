#pragma once

#include <mv/mv_THPAudioDecode.h>
#include <mv/mv_THPPlayer.h>
#include <mv/mv_THPRead.h>
#include <revolution/OS/OSMessage.h>
#include <revolution/OS/OSThread.h>
#include <types.h>

// HYPOTHESIS: layout derived from the offsets used by the decode thread functions.
struct THPVideoDecWork {
    static const u32 StackSize = 0x1000;
    static const u32 BufferCap = 3;
    BOOL m_threadActive;
    OSThread m_thread;
    char m_stack[StackSize];
    OSMessageQueue m_freeTextureQueue;
    OSMessageQueue m_decodedTextureQueue;
    OSMessage m_freeTextureBuf[BufferCap];
    OSMessage m_decodedTextureBuf[BufferCap];
    BOOL m_suspendPending;
};

// HYPOTHESIS: same layout as AudioDecMessage; the 0x18 size comes from the stack frame of VideoDecoderForOnMemory.
struct VideoDecMessage {
    THPReadBlock* m_block;
    s32 m_blockNum;
    u8 _8[0x10];
};

void SetTHPVideoDecWork(THPVideoDecWork* work);
bool CreateVideoDecodeThread(s32 prio, void* funcArg);
void VideoDecodeThreadStart();
void VideoDecodeThreadCancel();
void* VideoDecoder(void* p1);
void* VideoDecoderForOnMemory(void* p1);
bool THPVideoDecodeOneShot(u32* p1);

void PushFreeTextureSet(void* buffer);
