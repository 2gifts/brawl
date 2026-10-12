#include <mt/mt_coll2d.h>
#include <types.h>

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
