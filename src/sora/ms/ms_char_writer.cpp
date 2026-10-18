#include <ms/ms_char_writer.h>
#include <nw4r/ut/ut_Font.h>
#include <revolution/GX.h>
#include <types.h>

// ms::CharWriter is the game's copy of nw4r::ut::CharWriter (src/nw4r/ut/ut_CharWriter.cpp): the same vertex colors,
// GX setup and glyph quad, plus an outline (edge) pass, a draw scale and a per-font scale.

namespace {

void SetupGXCommon() {
    static const nw4r::ut::Color fog = 0;

    GXSetFog(GX_FOG_NONE, fog, 0.0f, 0.0f, 0.0f, 0.0f);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

    GXSetNumTexGens(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX_IDENT);

    GXSetNumIndStages(0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
}

} // namespace

ms::CharWriter::LoadingTexture ms::CharWriter::mLoadingTexture;

ms::CharWriter::CharWriter()
    : m_alpha(255), m_isWidthFixed(false), m_fixedWidth(0.0f), m_font(NULL), m_isDrawScaled(false), m_drawScale(1.0f),
      _84(0), unk55(0), m_unknownFontWidthModifier(1.0f), m_edgeWidth(0.0f), m_edgeColor(nw4r::ut::Color::BLACK), unk64(1),
      m_adjustFontScale(1.0f), unk6C(0) {
    mLoadingTexture.Reset();
    ResetColorMapping();
    SetGradationMode(GRADMODE_NONE);
    SetTextColor(nw4r::ut::Color::WHITE);
    SetScale(1.0f, 1.0f);
    SetCursor(0.0f, 0.0f, 0.0f);
    EnableLinearFilter(true, true);
}

ms::CharWriter::~CharWriter() { }

// HYPOTHESIS: looks the font resource up by id and keeps the per-font scale of the large font (id 4).
extern "C" void* fn_8006CBA0(int);

void SetFontResource(ms::CharWriter* w, int id) {
    w->m_font = static_cast<nw4r::ut::Font*>(fn_8006CBA0(id));
    switch (id) {
    case 4:
        w->m_adjustFontScale = 1.73f;
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
    case 7:
    case 8:
        w->m_adjustFontScale = 1.0f;
        break;
    }
}

void ms::CharWriter::SetFont(const nw4r::ut::ResFont& rFont) {
    m_font = &rFont;
    m_adjustFontScale = 1.0f;
}

const nw4r::ut::Font* ms::CharWriter::GetFont() const {
    return m_font;
}

void ms::CharWriter::SetupGX() {
    ResetTextureCache();

    if (m_colorMapping.min != DEFAULT_COLOR_MAPPING_MIN || m_colorMapping.max != DEFAULT_COLOR_MAPPING_MAX) {
        SetupGXWithColorMapping(m_colorMapping.min, m_colorMapping.max);
        return;
    }

    if (m_font != NULL) {
        switch (m_font->GetTextureFormat()) {
        case GX_TF_I4:
        case GX_TF_I8: {
            SetupGXForI();
            break;
        }

        case GX_TF_IA4:
        case GX_TF_IA8: {
            SetupGXDefault();
            break;
        }

        case GX_TF_RGB565:
        case GX_TF_RGB5A3:
        case GX_TF_RGBA8: {
            SetupGXForRGBA();
            break;
        }

        default: {
            SetupGXDefault();
            break;
        }
        }
    } else {
        SetupGXDefault();
    }
}

void ms::CharWriter::SetColorMapping(nw4r::ut::Color min, nw4r::ut::Color max) {
    m_colorMapping.min = min;
    m_colorMapping.max = max;
}

void ms::CharWriter::SetAlpha(unsigned char alpha) {
    m_alpha = alpha;
    UpdateVertexColor();
}

void ms::CharWriter::SetTextColor(nw4r::ut::Color start) {
    m_textColor.start = start;
    UpdateVertexColor();
}

nw4r::ut::Color ms::CharWriter::GetTextColor() const {
    return m_textColor.start;
}

void ms::CharWriter::SetScale(float x, float y) {
    m_scale.x = x;
    m_scale.y = y;
}

void ms::CharWriter::SetScale(float scale) {
    m_scale.x = scale;
    m_scale.y = scale;
}

float ms::CharWriter::GetScaleH() const {
    return m_scale.x;
}

float ms::CharWriter::GetScaleV() const {
    return m_scale.y;
}

float ms::CharWriter::getAdjustFontScale() const {
    return m_adjustFontScale;
}

float ms::CharWriter::GetFontWidth() const {
    return m_adjustFontScale * (m_scale.x * m_font->GetWidth());
}

float ms::CharWriter::GetFontHeight() const {
    return m_adjustFontScale * (m_scale.y * m_font->GetHeight());
}

float ms::CharWriter::GetFontAscent() const {
    return m_adjustFontScale * (m_scale.y * m_font->GetAscent());
}

float ms::CharWriter::GetFontDescent() const {
    return m_adjustFontScale * (m_scale.y * m_font->GetDescent());
}

bool ms::CharWriter::IsWidthFixed() const {
    return m_isWidthFixed;
}

void ms::CharWriter::EnableFixedWidth(bool enable) {
    m_isWidthFixed = enable;
}

float ms::CharWriter::GetFixedWidth() const {
    return m_fixedWidth;
}

void ms::CharWriter::SetFixedWidth(float width) {
    m_fixedWidth = width;
}

f32 ms::CharWriter::Print(u16 ch) {
    nw4r::ut::Glyph glyph;
    f32 width;
    f32 left;

    m_font->GetGlyph(&glyph, ch);
    if (m_isWidthFixed) {
        f32 margin =
            (m_fixedWidth - (m_unknownFontWidthModifier * (m_scale.x * ((f32)glyph.widths.charWidth * m_adjustFontScale)))) /
            2.0f;
        left = margin + (m_unknownFontWidthModifier * (m_scale.x * ((f32)glyph.widths.left * m_adjustFontScale)));
        width = m_fixedWidth;
    } else {
        width = m_unknownFontWidthModifier * (m_scale.x * ((f32)glyph.widths.charWidth * m_adjustFontScale));
        left = m_unknownFontWidthModifier * (m_scale.x * ((f32)glyph.widths.left * m_adjustFontScale));
    }
    PrintGlyph(m_cursorPos.x + left, m_cursorPos.y, m_cursorPos.z, &glyph);
    m_cursorPos.x += width;
    return width;
}

void ms::CharWriter::SetCursor(float x, float y) {
    m_cursorPos.x = x;
    m_cursorPos.y = y;
}

void ms::CharWriter::SetCursor(float x, float y, float z) {
    m_cursorPos.x = x;
    m_cursorPos.y = y;
    m_cursorPos.z = z;
}

void ms::CharWriter::SetCursorX(float x) {
    m_cursorPos.x = x;
}

void ms::CharWriter::SetCursorY(float y) {
    m_cursorPos.y = y;
}

void ms::CharWriter::SetCursorZ(float z) {
    m_cursorPos.z = z;
}

void ms::CharWriter::MoveCursorX(float dx) {
    m_cursorPos.x += dx;
}

void ms::CharWriter::MoveCursorY(float dy) {
    m_cursorPos.y += dy;
}

float ms::CharWriter::GetCursorX() const {
    return m_cursorPos.x;
}

float ms::CharWriter::GetCursorY() const {
    return m_cursorPos.y;
}

float ms::CharWriter::GetCursorZ() const {
    return m_cursorPos.z;
}

namespace {

// One textured quad of the glyph (x1, y1) - (x2, y2), one color for all four corners.
inline void PrintQuad(float x1, float y1, float x2, float y2, float z, u32 color, u16 texLeft, u16 texTop, u16 texRight,
                      u16 texBottom) {
    GXPosition3f32(x1, y1, z);
    GXColor1u32(color);
    GXTexCoord2s16(texLeft, texTop);

    GXPosition3f32(x2, y1, z);
    GXColor1u32(color);
    GXTexCoord2s16(texRight, texTop);

    GXPosition3f32(x2, y2, z);
    GXColor1u32(color);
    GXTexCoord2s16(texRight, texBottom);

    GXPosition3f32(x1, y2, z);
    GXColor1u32(color);
    GXTexCoord2s16(texLeft, texBottom);
}

} // namespace

void ms::CharWriter::PrintGlyph(float x, float y, float z, const void* pGlyph) {
    const nw4r::ut::Glyph& rGlyph = *static_cast<const nw4r::ut::Glyph*>(pGlyph);

    u16 texLeft = rGlyph.cellX * 0x8000U / rGlyph.texWidth;
    u16 texTop = rGlyph.cellY * 0x8000U / rGlyph.texHeight;
    u16 texRight = (rGlyph.cellX + rGlyph.widths.glyphWidth) * 0x8000U / rGlyph.texWidth;
    u16 texBottom = (rGlyph.cellY + rGlyph.height) * 0x8000U / rGlyph.texHeight;

    float x1;
    float x2;
    float y1;
    float y2;
    float zOffset;
    if (m_isDrawScaled) {
        float drawScale = m_drawScale;
        float h = m_scale.y * ((float)rGlyph.height * m_adjustFontScale);
        float w = m_unknownFontWidthModifier * (m_scale.x * ((float)rGlyph.widths.glyphWidth * m_adjustFontScale));
        x1 = x * drawScale;
        x2 = drawScale * (x + w);
        y1 = -y * drawScale;
        y2 = drawScale * -(y + h);
    } else {
        float h = m_scale.y * ((float)rGlyph.height * m_adjustFontScale);
        float w = m_unknownFontWidthModifier * (m_scale.x * ((float)rGlyph.widths.glyphWidth * m_adjustFontScale));
        x1 = x;
        x2 = x + w;
        y1 = y;
        y2 = y + h;
    }
    zOffset = m_isDrawScaled ? 0.01f : 0.0f;

    LoadTexture(rGlyph, GX_TEXMAP0);

    if (0.0f == m_edgeWidth || _84 != 0) {
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        {
            GXPosition3f32(x1, y1, zOffset);
            GXColor1u32(m_vertexColor.lu);
            GXTexCoord2s16(texLeft, texTop);

            GXPosition3f32(x2, y1, zOffset);
            GXColor1u32(m_vertexColor.ru);
            GXTexCoord2s16(texRight, texTop);

            GXPosition3f32(x2, y2, zOffset);
            GXColor1u32(m_vertexColor.rd);
            GXTexCoord2s16(texRight, texBottom);

            GXPosition3f32(x1, y2, zOffset);
            GXColor1u32(m_vertexColor.ld);
            GXTexCoord2s16(texLeft, texBottom);
        }
        GXEnd();
    } else {
        GXBegin(GX_QUADS, GX_VTXFMT0, 36);
        {
            float drawScale = m_isDrawScaled ? m_drawScale : 1.0f;
            float edgeX = drawScale * (m_unknownFontWidthModifier * (m_adjustFontScale * (m_edgeWidth * m_scale.x)));
            float edgeY = drawScale * (m_unknownFontWidthModifier * (m_adjustFontScale * (m_edgeWidth * m_scale.y)));
            float edgeXNeg = -edgeX;
            float edgeYNeg = -edgeY;
            nw4r::ut::Color edgeColor = m_edgeColor;
            u32 color = edgeColor;
            float edgeZ = z + zOffset;

            PrintQuad(x1 + edgeXNeg, y1 + edgeYNeg, x2 + edgeXNeg, y2 + edgeYNeg, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1, y1 + edgeYNeg, x2, y2 + edgeYNeg, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1 + edgeX, y1 + edgeYNeg, x2 + edgeX, y2 + edgeYNeg, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1 + edgeX, y1, x2 + edgeX, y2, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1 + edgeX, y1 + edgeY, x2 + edgeX, y2 + edgeY, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1, y1 + edgeY, x2, y2 + edgeY, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1 + edgeXNeg, y1 + edgeY, x2 + edgeXNeg, y2 + edgeY, edgeZ, color, texLeft, texTop, texRight, texBottom);
            PrintQuad(x1 + edgeXNeg, y1, x2 + edgeXNeg, y2, edgeZ, color, texLeft, texTop, texRight, texBottom);

            edgeZ = edgeZ + zOffset;
            PrintQuad(x1, y1, x2, y2, edgeZ, m_vertexColor.lu, texLeft, texTop, texRight, texBottom);
        }
        GXEnd();
    }
}

void ms::CharWriter::LoadTexture(const nw4r::ut::Glyph& rGlyph, GXTexMapID slot) {
    LoadingTexture loadingTexture;

    loadingTexture.slot = slot;
    loadingTexture.texture = rGlyph.pTexture;
    loadingTexture.filter = m_filter;

    if (loadingTexture != mLoadingTexture) {
        GXTexObj texObj;
        GXInitTexObj(&texObj, rGlyph.pTexture, rGlyph.texWidth, rGlyph.texHeight, rGlyph.texFormat, GX_CLAMP, GX_CLAMP,
                     FALSE);

        GXInitTexObjLOD(&texObj, m_filter.atSmall, m_filter.atLarge, 0.0f, 0.0f, 0.0f, FALSE, FALSE, GX_ANISO_1);

        GXLoadTexObj(&texObj, slot);

        mLoadingTexture = loadingTexture;
    }
}

void ms::CharWriter::SetupVertexFormat() {
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 15);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

void ms::CharWriter::SetupGXDefault() {
    SetupGXCommon();

    GXSetNumTevStages(1);

    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);

    SetupVertexFormat();
}

void ms::CharWriter::SetupGXWithColorMapping(nw4r::ut::Color min, nw4r::ut::Color max) {
    SetupGXCommon();

    GXSetNumTevStages(2);

    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);

    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);

    GXSetTevColor(GX_TEVREG0, min);
    GXSetTevColor(GX_TEVREG1, max);

    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);

    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);

    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);

    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);

    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);

    SetupVertexFormat();
}

void ms::CharWriter::SetupGXForI() {
    SetupGXCommon();

    GXSetNumTevStages(1);

    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);

    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);

    SetupVertexFormat();
}

void ms::CharWriter::SetupGXForRGBA() {
    SetupGXDefault();
}
