#include <types.h>
#include <nw4r/ut/ut_Font.h>

namespace ms {
class CharWriter {
public:
    u8 unk00[0x24];
    f32 m_fontScaleX;
    u8 unk28[0x4];
    f32 m_xPos;
    f32 m_yPos;
    f32 m_zPos;
    u8 unk38[0xB];
    bool m_isFixedWidth;
    f32 m_fixedWidth;
    nw4r::ut::Font* m_font;
    u8 unk4C[0xC];
    f32 m_unknownFontWidthModifier;
    u8 unk5C[0xC];
    f32 m_104;

    f32 Print(u16 character);
};
}

extern "C" void PrintGlyph__Q22ms10CharWriterFfffCPv(ms::CharWriter*, f32, f32, f32, const void*);

f32 ms::CharWriter::Print(u16 character) {
    nw4r::ut::Glyph glyph;
    f32 width;
    f32 left;

    m_font->GetGlyph(&glyph, character);
    if (m_isFixedWidth) {
        f32 margin = (m_fixedWidth -
                      (m_unknownFontWidthModifier * (m_fontScaleX * ((f32) glyph.widths.charWidth * m_104)))) /
                     2.0f;
        left = margin + (m_unknownFontWidthModifier * (m_fontScaleX * ((f32) glyph.widths.left * m_104)));
        width = m_fixedWidth;
    } else {
        width = m_unknownFontWidthModifier * (m_fontScaleX * ((f32) glyph.widths.charWidth * m_104));
        left = m_unknownFontWidthModifier * (m_fontScaleX * ((f32) glyph.widths.left * m_104));
    }
    PrintGlyph__Q22ms10CharWriterFfffCPv(this, m_xPos + left, m_yPos, m_zPos, &glyph);
    m_xPos += width;
    return width;
}
