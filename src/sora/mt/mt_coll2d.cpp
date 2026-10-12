#include <mt/mt_coll2d.h>
#include <types.h>

void clRhombus2D::move(const Vec2f& displacement) {
    m_centerPos.m_x += displacement.m_x;
    m_centerPos.m_y += displacement.m_y;
    m_upPos.m_x += displacement.m_x;
    m_upPos.m_y += displacement.m_y;
    m_downPos.m_x += displacement.m_x;
    m_downPos.m_y += displacement.m_y;
    m_leftPos.m_x += displacement.m_x;
    m_leftPos.m_y += displacement.m_y;
    m_rightPos.m_x += displacement.m_x;
    m_rightPos.m_y += displacement.m_y;
    m_aabBox.m_unk1 += displacement.m_x;
    m_aabBox.m_unk2 += displacement.m_y;
    m_aabBox.m_unk3 += displacement.m_x;
    m_aabBox.m_unk4 += displacement.m_y;
}

void clCircle2D::getAABBox(clAABBox2D& aabBox) const {
    float x;
    float r;
    float y;

    y = m_centerPos.m_y;
    r = m_radius;
    x = m_centerPos.m_x;

    float xPlusR;
    float yPlusR;
    float yMinusR;
    float xMinusR;

    yPlusR = y + r;
    xPlusR = x + r;
    yMinusR = y - r;
    xMinusR = x - r;

    aabBox.m_unk4 = yPlusR;
    aabBox.m_unk3 = xPlusR;
    aabBox.m_unk1 = xMinusR;
    aabBox.m_unk2 = yMinusR;
}
