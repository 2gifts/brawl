#include <types.h>
#include <ms/ms_char_writer.h>

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
