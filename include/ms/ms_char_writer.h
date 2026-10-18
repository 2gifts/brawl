#pragma once

// Local shadow of the BrawlHeaders header. ms::CharWriter is the game's copy of nw4r::ut::CharWriter (same member
// order, the same vertex-color/gradation logic and the same GX setup), with the edge (outline) rendering, a draw scale
// and a per-font scale added. Member names follow nw4r where the code is the same.

#include <StaticAssert.h>
#include <nw4r/ut/ut_Color.h>
#include <nw4r/ut/ut_ResFont.h>
#include <revolution/GX.h>
#include <types.h>

namespace nw4r {
namespace ut {
class Font;
}
} // namespace nw4r

namespace ms {
    class CharWriter {
    public:
        enum GradationMode {
            GRADMODE_NONE,
            GRADMODE_H,
            GRADMODE_V,

            GRADMODE_MAX
        };

        struct ColorMapping {
            nw4r::ut::Color min; // 0x0
            nw4r::ut::Color max; // 0x4
        };

        struct VertexColor {
            nw4r::ut::Color lu; // 0x0
            nw4r::ut::Color ru; // 0x4
            nw4r::ut::Color ld; // 0x8
            nw4r::ut::Color rd; // 0xC
        };

        struct TextColor {
            nw4r::ut::Color start;       // 0x0
            nw4r::ut::Color end;         // 0x4
            GradationMode gradationMode; // 0x8
        };

        struct TextureFilter {
            GXTexFilter atSmall; // 0x0
            GXTexFilter atLarge; // 0x4

            bool operator!=(const TextureFilter& rOther) const {
                return atSmall != rOther.atSmall || atLarge != rOther.atLarge;
            }
        };

        struct LoadingTexture {
            GXTexMapID slot;      // 0x0
            void* texture;        // 0x4
            TextureFilter filter; // 0x8

            bool operator!=(const LoadingTexture& rOther) const {
                return slot != rOther.slot || texture != rOther.texture || filter != rOther.filter;
            }

            void Reset() {
                slot = GX_TEXMAP_NULL;
                texture = NULL;
            }
        };

        static const u32 DEFAULT_COLOR_MAPPING_MIN = 0x00000000;
        static const u32 DEFAULT_COLOR_MAPPING_MAX = 0xFFFFFFFF;

        ColorMapping m_colorMapping; // 0x0
        VertexColor m_vertexColor;   // 0x8
        TextColor m_textColor;       // 0x18
        struct {
            float x; // 0x24
            float y; // 0x28
        } m_scale;
        struct {
            float x; // 0x2C
            float y; // 0x30
            float z; // 0x34
        } m_cursorPos;
        TextureFilter m_filter;           // 0x38
        u8 unk40[2];                      // 0x40, never written by this class
        u8 m_alpha;                       // 0x42
        bool m_isWidthFixed;              // 0x43
        float m_fixedWidth;               // 0x44
        const nw4r::ut::Font* m_font;     // 0x48
        bool m_isDrawScaled;              // 0x4C, HYPOTHESIS: 1 = positions are multiplied by m_drawScale, y flipped
        float m_drawScale;                // 0x50
        u8 _84;                           // 0x54, HYPOTHESIS: 1 = the edge (outline) quads are not drawn
        u8 unk55;                         // 0x55
        float m_unknownFontWidthModifier; // 0x58, extra width scale (ms::Message writes it from the window ratio)
        float m_edgeWidth;                // 0x5C, outline offset in font units (0 = no outline)
        nw4r::ut::Color m_edgeColor;      // 0x60
        int unk64;                        // 0x64
        float m_adjustFontScale;          // 0x68, per-font-resource scale (SetFontResource)
        u8 unk6C;                         // 0x6C

        CharWriter();
        ~CharWriter();

        void SetFont(const nw4r::ut::ResFont& font);
        const nw4r::ut::Font* GetFont() const;
        void SetupGX();
        void SetColorMapping(nw4r::ut::Color min, nw4r::ut::Color max);
        void SetAlpha(unsigned char alpha);
        void SetTextColor(nw4r::ut::Color start);
        nw4r::ut::Color GetTextColor() const;
        void SetScale(float x, float y);
        void SetScale(float scale);
        float GetScaleH() const;
        float GetScaleV() const;
        float getAdjustFontScale() const;
        float GetFontWidth() const;
        float GetFontHeight() const;
        float GetFontAscent() const;
        float GetFontDescent() const;
        bool IsWidthFixed() const;
        void EnableFixedWidth(bool enable);
        float GetFixedWidth() const;
        void SetFixedWidth(float width);
        f32 Print(u16 ch);
        void SetCursor(float x, float y);
        void SetCursor(float x, float y, float z);
        void SetCursorX(float x);
        void SetCursorY(float y);
        void SetCursorZ(float z);
        void MoveCursorX(float dx);
        void MoveCursorY(float dy);
        float GetCursorX() const;
        float GetCursorY() const;
        float GetCursorZ() const;
        void PrintGlyph(float x, float y, float z, const void* glyph);

        // Text outline (defined in ms_message.cpp).
        void SetEdge(float width, nw4r::ut::Color color);

        void SetGradationMode(GradationMode mode) {
            m_textColor.gradationMode = mode;
            UpdateVertexColor();
        }

        void EnableLinearFilter(bool atSmall, bool atLarge) {
            m_filter.atSmall = atSmall ? GX_LINEAR : GX_NEAR;
            m_filter.atLarge = atLarge ? GX_LINEAR : GX_NEAR;
        }

        void ResetColorMapping() {
            SetColorMapping(DEFAULT_COLOR_MAPPING_MIN, DEFAULT_COLOR_MAPPING_MAX);
        }

    private:
        static void SetupVertexFormat();
        static void SetupGXDefault();
        static void SetupGXWithColorMapping(nw4r::ut::Color min, nw4r::ut::Color max);
        static void SetupGXForI();
        static void SetupGXForRGBA();

        void UpdateVertexColor() {
            m_vertexColor.lu = m_textColor.start;
            m_vertexColor.ru = m_textColor.gradationMode != GRADMODE_H ? m_textColor.start : m_textColor.end;
            m_vertexColor.ld = m_textColor.gradationMode != GRADMODE_V ? m_textColor.start : m_textColor.end;
            m_vertexColor.rd = m_textColor.gradationMode == GRADMODE_NONE ? m_textColor.start : m_textColor.end;

            m_vertexColor.lu.a = (m_vertexColor.lu.a * m_alpha) / 255;
            m_vertexColor.ru.a = (m_vertexColor.ru.a * m_alpha) / 255;
            m_vertexColor.ld.a = (m_vertexColor.ld.a * m_alpha) / 255;
            m_vertexColor.rd.a = (m_vertexColor.rd.a * m_alpha) / 255;
        }

        void LoadTexture(const nw4r::ut::Glyph& rGlyph, GXTexMapID slot);
        void ResetTextureCache() {
            mLoadingTexture.Reset();
        }

        static LoadingTexture mLoadingTexture;
    };
    static_assert(sizeof(CharWriter) == 0x70, "Class is the wrong size!");
}
