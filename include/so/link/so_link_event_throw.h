#pragma once

#include <so/link/so_link_event_presenter.h>

class soLinkEventThrow : public soLinkEventArgs {
public:
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    float unk18;
    bool unk1C;

    // HYPOTHESIS: the final byte parameter is a Boolean selector.
    soLinkEventThrow(int eventKind, int value, bool enabled);
};
