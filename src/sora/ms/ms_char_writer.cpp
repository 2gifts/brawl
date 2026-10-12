#include <ms/ms_char_writer.h>
#include <types.h>

f32 ms::CharWriter::GetFontHeight() const {
    return m_104 * (m_fontScaleY * static_cast<nw4r::ut::Font*>(m_font)->GetHeight());
}


f32 ms::CharWriter::GetFontAscent() const {
    return m_104 * (m_fontScaleY * static_cast<nw4r::ut::Font*>(m_font)->GetAscent());
}

#include <ms/ms_char_writer.h>

void ms::CharWriter::SetColorMapping(nw4r::ut::Color a, nw4r::ut::Color b) {
    m_colorMapping1 = a;
    m_colorMapping2 = b;
}

#include <ms/ms_char_writer.h>

ms::CharWriter::~CharWriter() {}

#include <ms/ms_char_writer.h>

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

#include <ms/ms_char_writer.h>

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
