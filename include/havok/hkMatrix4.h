#pragma once

#include <havok/hkTransform.h>
#include <havok/hkVector4.h>

// plain 4x4 float matrix, 4 columns of hkVector4 (0x40 bytes). not in BrawlHeaders so it lives here
struct hkMatrix4 {
    hkVector4 m_col0; // 0x00
    hkVector4 m_col1; // 0x10
    hkVector4 m_col2; // 0x20
    hkVector4 m_col3; // 0x30

    void transpose();
    void setMul(const hkMatrix4& a, const hkMatrix4& b);
    // 16 floats out/in, same column order as the struct (what you'd hand to a renderer)
    void get4x4ColumnMajor(hkReal* p) const;
    void set4x4ColumnMajor(const hkReal* p);
    // rotation goes in the top left 3x3, translation in the last column, w lanes 0 / 1
    void set(const hkTransform& t);
};
