#include <types.h>
#include <ms/ms_char_writer.h>
#include <nw4r/ut/ut_Font.h>

void ms::CharWriter::SetTextColor(nw4r::ut::Color textColor) {
    nw4r::ut::Color* sel1;
    nw4r::ut::Color* sel2;
    nw4r::ut::Color* sel3;

    m_textColor = textColor;
    m_colorRect.m_topLeft = m_textColor;
    // MATCH-ONLY: m_32 is declared char but the code reads it as a 32-bit word
    if (*(s32*)&m_32 != 1) {
        sel1 = &m_textColor;
    } else {
        sel1 = &m_unkColor;
    }
    m_colorRect.m_topRight = *sel1;
    if (*(s32*)&m_32 != 2) {
        sel2 = &m_textColor;
    } else {
        sel2 = &m_unkColor;
    }
    m_colorRect.m_bottomLeft = *sel2;
    if (*(s32*)&m_32 == 0) {
        sel3 = &m_textColor;
    } else {
        sel3 = &m_unkColor;
    }
    m_colorRect.m_bottomRight = *sel3;
    m_colorRect.m_topLeft.a = (m_colorRect.m_topLeft.a * m_alpha) / 255;
    m_colorRect.m_topRight.a = (m_colorRect.m_topRight.a * m_alpha) / 255;
    m_colorRect.m_bottomLeft.a = (m_colorRect.m_bottomLeft.a * m_alpha) / 255;
    m_colorRect.m_bottomRight.a = (m_colorRect.m_bottomRight.a * m_alpha) / 255;
}

extern "C" void PrintGlyph__Q22ms10CharWriterFfffCPv(ms::CharWriter*, f32, f32, f32, const void*);

f32 ms::CharWriter::Print(u16 character) {
    nw4r::ut::Glyph glyph;
    f32 width;
    f32 left;

    static_cast<nw4r::ut::Font*>(m_font)->GetGlyph(&glyph, character);
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

f32 ms::CharWriter::GetFontWidth() const {
    return m_104 * (m_fontScaleX * (f32) static_cast<nw4r::ut::Font*>(m_font)->GetWidth());
}
