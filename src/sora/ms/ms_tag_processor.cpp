#include <ms/ms_tag_processor.h>
#include <sora/ms_message.h>
#include <string.h>
#include <types.h>

extern "C" void __destroy_arr(void*, void (*)(void*), u32, u32);
extern "C" void fn_8006CE1C(void*);
// HYPOTHESIS: fn_8006E7F0 tears down a sub-object at this, with a delete-flag argument passed as 0.
extern "C" void fn_8006E7F0(void*, s32);

MsgTagProc::~MsgTagProc() {
    __destroy_arr(&m_data[0x90], fn_8006CE1C, 0x10, 8);
    __destroy_arr(&m_data[0xC], fn_8006CE1C, 0x10, 8);
    fn_8006E7F0(this, 0);
}


extern "C" void __construct_array(void*, void (*)(void*), void (*)(void*), u32, u32);
extern "C" void fn_8006CE10(void*);
extern "C" void fn_8006CE1C(void*);
extern "C" void fn_8006E7E0();
extern "C" u8 lbl_80454360[];

MsgTagProc::MsgTagProc() {
    fn_8006E7E0();
    *reinterpret_cast<void**>(m_data) = lbl_80454360;
    __construct_array(&m_data[0xC], fn_8006CE10, fn_8006CE1C, 0x10, 8);
    __construct_array(&m_data[0x90], fn_8006CE10, fn_8006CE1C, 0x10, 8);
}

bool msTagProcessor::pushFontTag(ContextType* pCtx, u8 flags) {
    if (m_stackCount >= 8) {
        m_stackCount = 7;
        for (int i = 1; i < 8; i++) {
            m_stack[i - 1] = m_stack[i];
        }
    }

    tagStack* entry = &m_stack[m_stackCount];
    ms::TextWriterBase<char>* writer = pCtx->writer;
    entry->m_flags = flags;
    if (flags & 2) {
        entry->m_color = writer->GetTextColor();
    }
    if (flags & 1) {
        entry->m_scaleH = writer->GetScaleH();
        entry->m_scaleV = writer->GetScaleV();
    }
    if (flags & 0x80) {
        unk4 = 1;
    }
    m_stackCount++;
    return true;
}

bool msTagProcessor::popFontTag(ContextType* pCtx) {
    if (m_stackCount < 1) {
        return false;
    }
    m_stackCount--;
    if (unk4 != 0) {
        return true;
    }

    tagStack* entry = &m_stack[m_stackCount];
    ms::TextWriterBase<char>* writer = pCtx->writer;
    u8 flags = entry->m_flags;
    if (flags & 2) {
        writer->SetTextColor(entry->m_color);
    }
    if (flags & 1) {
        writer->SetScale(entry->m_scaleH, entry->m_scaleV);
        if (pCtx->writer->IsWidthFixed()) {
            pCtx->writer->SetFixedWidth(32.0f * entry->m_scaleH);
        }
    }
    return true;
}

u8* floatToBytes(u8* out, float f) {
    union {
        float value;
        u8 bytes[4];
    } u;

    u.value = f;
    out[0] = u.bytes[0];
    out[1] = u.bytes[1];
    out[2] = u.bytes[2];
    out[3] = u.bytes[3];
    return out;
}

void msTagProcessor::saveEnv() {
    m_savedStackCount = m_stackCount;
    memcpy(m_savedStack, m_stack, m_stackCount * sizeof(tagStack));
}

void msTagProcessor::restoreEnv() {
    m_stackCount = m_savedStackCount;
    memcpy(m_stack, m_savedStack, m_stackCount * sizeof(tagStack));
}

extern "C" f32 fn_80070C78(const ms::TextWriterBase<char>*);
extern "C" int fn_80070D64(const ms::TextWriterBase<char>*);

namespace ms {

template <typename T>
void TagProcessorBase<T>::ProcessTab(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;

    int tabWidth = fn_80070D64(&rWriter);
    if (tabWidth <= 0) {
        return;
    }

    f32 charWidth = rWriter.IsWidthFixed() ? rWriter.GetFixedWidth() : rWriter.GetFontWidth();

    f32 dx = rWriter.GetCursorX() - pCtx->x;
    f32 tabPixel = tabWidth * charWidth;
    int numTab = static_cast<int>(dx / tabPixel) + 1;
    f32 x = pCtx->x + (tabPixel * numTab);

    rWriter.SetCursorX(x);
}

template <typename T>
void TagProcessorBase<T>::ProcessLinefeed(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;

    f32 x = pCtx->x;
    f32 y = rWriter.GetCursorY() + fn_80070C78(&rWriter);

    rWriter.SetCursor(x, y);
}

template <typename T>
typename TagProcessorBase<T>::Operation TagProcessorBase<T>::Process(u16 ch, ContextType* pCtx) {
    switch (ch) {
    case '\n':
        ProcessLinefeed(pCtx);
        return OPERATION_NEXT_LINE;

    case '\t':
        ProcessTab(pCtx);
        return OPERATION_NO_CHAR_SPACE;
    }

    return OPERATION_DEFAULT;
}

template <typename T>
void TagProcessorBase<T>::saveEnv() {}

template <typename T>
void TagProcessorBase<T>::restoreEnv() {}

template TagProcessorBase<char>::Operation TagProcessorBase<char>::Process(u16, ContextType*);
template void TagProcessorBase<char>::saveEnv();
template void TagProcessorBase<char>::restoreEnv();

} // namespace ms
