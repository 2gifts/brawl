#include <ms/ms_char_writer.h>
#include <nw4r/ut/ut_Font.h>
#include <types.h>

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


f32 ms::CharWriter::GetFontHeight() const {
    return m_104 * (m_fontScaleY * static_cast<nw4r::ut::Font*>(m_font)->GetHeight());
}


f32 ms::CharWriter::GetFontAscent() const {
    return m_104 * (m_fontScaleY * static_cast<nw4r::ut::Font*>(m_font)->GetAscent());
}

void ms::CharWriter::SetColorMapping(nw4r::ut::Color a, nw4r::ut::Color b) {
    m_colorMapping1 = a;
    m_colorMapping2 = b;
}

ms::CharWriter::~CharWriter() {}

void ms::CharWriter::SetAlpha(unsigned char alpha) {
    nw4r::ut::Color* sel1;
    nw4r::ut::Color* sel2;
    nw4r::ut::Color* sel3;

    m_alpha = alpha;
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

// HYPOTHESIS: returns the font resource pointer stored in m_font
extern "C" void* fn_8006CBA0(int);
extern "C" f32 lbl_805A1A74;
extern "C" f32 lbl_805A1A78;

void SetFontResource(ms::CharWriter* w, int id) {
    w->m_font = fn_8006CBA0(id);
    switch (id) {
    case 4:
        w->m_104 = lbl_805A1A78;
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
    case 7:
    case 8:
        w->m_104 = lbl_805A1A74;
        break;
    }
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80497EB8[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80497EC8[0x10] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A01C0[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A01C4[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A01C5[0x1] = { 0 };
unsigned char lbl_805A01C6[0x1] __attribute__((aligned(2))) = { 0 };
unsigned char lbl_805A01C7[0x1] = { 0 };
