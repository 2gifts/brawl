#pragma once

// The fixed-capacity module arrays that more than one enemy class owns. The specializations are classes of sora_melee
// (the enemies only call their constructors and destructors); declared once here so that the enemy headers can be
// included together (em/em_all.h).

#include <ft/builder/ft_dol_instances.h>
#include <so/camera/so_camera_subject.h>

FT_DOL_ARRAY_VECTOR(soCameraSubject, 1);
