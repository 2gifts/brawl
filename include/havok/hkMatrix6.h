#pragma once

#include <havok/hkMatrix3.h>
#include <havok/hkVector4.h>
#include <havok/hkVector8.h>

// 6x6 matrix stored as a 2x2 grid of 3x3 blocks (0xC0 bytes). Each block is an hkMatrix3 (0x30 bytes,
// three columns of four floats): block (0,0) at 0x00, (0,1) at 0x30, (1,0) at 0x60, (1,1) at 0x90.
// Only the free functions below exist in this build.
struct hkMatrix6 {
    hkMatrix3 m_m[2][2]; // 0x00

    // Block-wise helpers used by the free functions. HYPOTHESIS: the names.
    void sub(const hkMatrix6& other);
    void setMul(const hkMatrix6& a, const hkMatrix6& b);
    void setTranspose(const hkMatrix6& other);
    void setInvert(const hkMatrix6& in);
};

// the real entry points: extern "C" free functions, named without a class
extern "C" {
void hkMatrix6Sub(hkMatrix6& out, const hkMatrix6& b);
void hkMatrix6SetMul(hkMatrix6& out, const hkMatrix6& a, const hkMatrix6& b);
void hkMatrix6SetMulV(hkVector8& out, const hkMatrix6& a, const hkVector8& b);
void hkMatrix6SetTranspose(hkMatrix6& out, const hkMatrix6& in);
void hkMatrix6SetInvert(hkMatrix6& out, const hkMatrix6& in);
}
