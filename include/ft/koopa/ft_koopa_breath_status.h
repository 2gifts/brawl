#pragma once

// Shared by the two fire breath status files (Bowser's in sora_melee, Giga Bowser's in the fighter module).

// The rumble the breath asks the controller for. BYTES: the kind is an enum in the original (the status code selects it
// with a branch; an int or bool local compiles to a branchless select).
// HYPOTHESIS: names; a charged breath (ratio above one half) takes kind 0.
enum ftKoopaBreathRumbleKind {
    FtKoopaBreathRumble_Strong,
    FtKoopaBreathRumble_Weak
};
