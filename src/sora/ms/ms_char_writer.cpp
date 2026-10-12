#include <types.h>
#include <nw4r/ut/ut_Font.h>

namespace ms {
class CharWriter {
public:
    u8 unk00[0x24];
    f32 m_fontScaleX;
    u8 unk28[0x20];
    nw4r::ut::Font* m_font;
    u8 unk4C[0x1C];
    f32 unk68;

    f32 GetFontWidth() const;
};
}

f32 ms::CharWriter::GetFontWidth() const {
    return unk68 * (m_fontScaleX * (f32) m_font->GetWidth());
}
