#include <ms/ms_tag_processor.h>
#include <ms/ms_text_writer_base.h>
#include <sora/ms_message.h>
#include <string.h>
#include <types.h>

msTagProcessor::msTagProcessor() { }

// A fresh stack entry is opaque white.
msTagProcessor::tagStack::tagStack() : m_color(0xFFFFFFFF) { }

msTagProcessor::tagStack::~tagStack() { }

msTagProcessor::~msTagProcessor() { }

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

void msTagProcessor::saveEnv(ContextType* pCtx) {
    m_savedStackCount = m_stackCount;
    memcpy(m_savedStack, m_stack, m_stackCount * sizeof(tagStack));
}

void msTagProcessor::restoreEnv(ContextType* pCtx) {
    m_stackCount = m_savedStackCount;
    memcpy(m_stack, m_savedStack, m_stackCount * sizeof(tagStack));
}

namespace ms {

template <typename T>
void TagProcessorBase<T>::ProcessTab(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;

    int tabWidth = rWriter.GetTabWidth();
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
    f32 y = rWriter.GetCursorY() + rWriter.GetLineHeight();

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
TagProcessorBase<T>::TagProcessorBase() { }

template <typename T>
TagProcessorBase<T>::~TagProcessorBase() { }

template <typename T>
void TagProcessorBase<T>::saveEnv(ContextType* pCtx) {}

template <typename T>
void TagProcessorBase<T>::restoreEnv(ContextType* pCtx) {}

template TagProcessorBase<char>::TagProcessorBase();
template TagProcessorBase<char>::~TagProcessorBase();
template TagProcessorBase<char>::Operation TagProcessorBase<char>::Process(u16, ContextType*);
template void TagProcessorBase<char>::saveEnv(ContextType*);
template void TagProcessorBase<char>::restoreEnv(ContextType*);

} // namespace ms
