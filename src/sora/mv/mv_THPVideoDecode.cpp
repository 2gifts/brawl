#include <mv/THPVideo.h>
#include <mv/mv_THPPlayer.h>
#include <mv/mv_THPRead.h>
#include <mv/mv_THPVideoDecode.h>
#include <revolution/OS/OSMessage.h>
#include <revolution/OS/OSThread.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
THPVideoDecWork* lbl_805A0208[2] __attribute__((aligned(8))) = { 0 };

void SetTHPVideoDecWork(THPVideoDecWork* work) {
    if (!lbl_805A0208[0]) {
        lbl_805A0208[0] = work;
        work->m_threadActive = false;
    }
}

bool CreateVideoDecodeThread(s32 prio, void* funcArg) {
    if (funcArg) {
        if (!OSCreateThread(&lbl_805A0208[0]->m_thread, VideoDecoderForOnMemory, funcArg,
            lbl_805A0208[0]->m_stack + THPVideoDecWork::StackSize, THPVideoDecWork::StackSize,
            prio, OS_THREAD_DETACHED)) {
            return false;
        }
    } else {
        if (!OSCreateThread(&lbl_805A0208[0]->m_thread, VideoDecoder, nullptr,
            lbl_805A0208[0]->m_stack + THPVideoDecWork::StackSize, THPVideoDecWork::StackSize,
            prio, OS_THREAD_DETACHED)) {
            return false;
        }
    }
    OSInitMessageQueue(&lbl_805A0208[0]->m_freeTextureQueue, lbl_805A0208[0]->m_freeTextureBuf,
        THPVideoDecWork::BufferCap);
    OSInitMessageQueue(&lbl_805A0208[0]->m_decodedTextureQueue, lbl_805A0208[0]->m_decodedTextureBuf,
        THPVideoDecWork::BufferCap);
    lbl_805A0208[0]->m_threadActive = true;
    lbl_805A0208[0]->m_suspendPending = true;
    return true;
}

void VideoDecodeThreadStart() {
    if (lbl_805A0208[0]->m_threadActive) {
        OSResumeThread(&lbl_805A0208[0]->m_thread);
    }
}

void VideoDecodeThreadCancel() {
    if (lbl_805A0208[0]->m_threadActive) {
        OSCancelThread(&lbl_805A0208[0]->m_thread);
        lbl_805A0208[0]->m_threadActive = false;
    }
}

void* VideoDecoder(void* p1) {
    VideoDecMessage* message;
    s32 skipCount = 0;
    while (true) {
        if (g_thpObject.unkA7Flag) while (skipCount != 0) {
            message = static_cast<VideoDecMessage*>(PopReadedBuffer2());
            u32 x = (message->m_blockNum + g_thpObject.unkC0) % g_thpObject.unk50;
            if (x == g_thpObject.unk50 - 1 && !(g_thpObject.unkA6 & 0x1)) {
                THPReadBuffer::VideoDecode(message);
            }
            PushFreeReadBuffer(message);
            g_thpObject.unkD8++;
            skipCount--;
        }
        if (g_thpObject.unkA7Flag) {
            message = static_cast<VideoDecMessage*>(PopReadedBuffer2());
        } else {
            message = static_cast<VideoDecMessage*>(PopReadedBuffer());
        }
        skipCount = THPReadBuffer::VideoDecode(message);
        PushFreeReadBuffer(message);
    }
}

void* VideoDecoderForOnMemory(void* p1) {
    VideoDecMessage msg;
    msg.m_block = static_cast<THPReadBlock*>(p1);
    u32 blockSize = g_thpObject.unkBC;
    u32 frame = 0;
    s32 skipCount = 0;
    while (true) {
        if (g_thpObject.unkA7Flag) {
            while (skipCount != 0) {
                skipCount--;
                g_thpObject.unkD8++;
                u32 n = g_thpObject.unk50;
                u32 x = (frame + g_thpObject.unkC0) % n;
                if (x == n - 1) {
                    if (!(g_thpObject.unkA6 & 0x1)) {
                        break;
                    }
                    blockSize = msg.m_block->m_nextBlockSize;
                    msg.m_block = reinterpret_cast<THPReadBlock*>(g_thpObject.unkB4);
                } else {
                    u32 nextSize = msg.m_block->m_nextBlockSize;
                    msg.m_block = reinterpret_cast<THPReadBlock*>(reinterpret_cast<char*>(msg.m_block) + blockSize);
                    blockSize = nextSize;
                }
                frame++;
            }
        }
        msg.m_blockNum = frame;
        skipCount = THPReadBuffer::VideoDecode(&msg);
        u32 n = g_thpObject.unk50;
        u32 x = (frame + g_thpObject.unkC0) % n;
        if (x == n - 1) {
            if (g_thpObject.unkA6 & 0x1) {
                blockSize = msg.m_block->m_nextBlockSize;
                msg.m_block = reinterpret_cast<THPReadBlock*>(g_thpObject.unkB4);
            } else {
                OSSuspendThread(&lbl_805A0208[0]->m_thread);
            }
        } else {
            u32 nextSize = msg.m_block->m_nextBlockSize;
            msg.m_block = reinterpret_cast<THPReadBlock*>(reinterpret_cast<char*>(msg.m_block) + blockSize);
            blockSize = nextSize;
        }
        frame++;
    }
}

bool THPVideoDecodeOneShot(u32* p1) {
    THPTextureSet* textures = g_thpObject.unkE8;
    u32 i = 0;
    u32* compSizes = p1 + 2;
    char* compData = reinterpret_cast<char*>(p1 + g_thpObject.unk6C.length + 2);
    for (; i < g_thpObject.unk6C.length; compSizes++, i++) {
        switch (static_cast<u8>(g_thpObject.unk6C.buf[i])) {
            case 0:
                g_thpObject.unkACword = THPVideoDecode(compData, textures->ytexture, textures->utexture, textures->vtexture, g_thpObject.unk9C);
                if (g_thpObject.unkACword != 0) {
                    return false;
                }
                break;
            default:
                break;
        }
        compData += *compSizes;
    }
    return true;
}

void PushFreeTextureSet(void* buffer) {
    OSSendMessage(&lbl_805A0208[0]->m_freeTextureQueue, buffer, 0);
}

OSMessage PopDecodedTextureSet(u32 flags) {
    OSMessage message;
    if (OSReceiveMessage(&lbl_805A0208[0]->m_decodedTextureQueue, &message, flags) == true) {
        return message;
    }
    return nullptr;
}
