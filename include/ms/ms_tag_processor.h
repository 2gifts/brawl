#pragma once

#include <StaticAssert.h>
#include <nw4r/ut/ut_Color.h>
#include <nw4r/ut/ut_Rect.h>
#include <types.h>

namespace ms {
    template <class T>
    class TextWriterBase;

    // HYPOTHESIS: the game's copy of nw4r::ut::PrintContext; only writer (0x0), str (0x4) and x (0x8) are used here.
    template <typename T>
    struct PrintContext {
        TextWriterBase<T>* writer; // 0x0
        const T* str;              // 0x4
        f32 x;                     // 0x8
        f32 y;                     // 0xc
        u32 flags;                 // 0x10
    };

    template <typename T>
    class TagProcessorBase {
    public:
        typedef PrintContext<T> ContextType;

        enum Operation {
            OPERATION_DEFAULT,
            OPERATION_NO_CHAR_SPACE,
            OPERATION_CHAR_SPACE,
            OPERATION_NEXT_LINE,
            OPERATION_END_DRAW
        };

        TagProcessorBase();
        virtual ~TagProcessorBase();

        virtual Operation Process(u16 ch, ContextType* pCtx);
        virtual Operation CalcRect(nw4r::ut::Rect* pRect, u16 ch, ContextType* pCtx);
        virtual void saveEnv();
        virtual void restoreEnv();

    private:
        void ProcessTab(ContextType* pCtx);
        void ProcessLinefeed(ContextType* pCtx);
    };
} // namespace ms

// HYPOTHESIS: MsgTagProc in sora/ms_message.h is this object seen as raw bytes (Message+0xa4); its
// constructor and destructor are still defined under that name.
class msTagProcessor : public ms::TagProcessorBase<char> {
public:
    struct tagStack {
        nw4r::ut::Color m_color; // 0x0
        f32 m_scaleH;            // 0x4
        f32 m_scaleV;            // 0x8
        u8 m_flags;              // 0xc
    };

    virtual ~msTagProcessor();
    virtual Operation Process(u16 ch, ContextType* pCtx);
    virtual Operation CalcRect(nw4r::ut::Rect* pRect, u16 ch, ContextType* pCtx);
    virtual void saveEnv();
    virtual void restoreEnv();

    // HYPOTHESIS: the flag byte is a u8 (the caller passes an unsigned byte of the tag string).
    bool pushFontTag(ContextType* pCtx, u8 flags);
    bool popFontTag(ContextType* pCtx);

    u8 unk4;
    s32 m_stackCount;            // 0x8
    tagStack m_stack[8];         // 0xc
    s32 m_savedStackCount;       // 0x8c
    tagStack m_savedStack[8];    // 0x90
};
static_assert(sizeof(msTagProcessor::tagStack) == 0x10, "msTagProcessor::tagStack size");
static_assert(sizeof(msTagProcessor) == 0x110, "msTagProcessor size");
