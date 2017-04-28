#pragma once
#include <stdlib.h>
#include "Definitions.h"

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    float RandomFloat (float min = 0, float max = 1);

    std::string GetDurationStr (PRECISE_TIMER a_Start, PRECISE_TIMER a_End);

    long GetDurationLng (PRECISE_TIMER a_Start, PRECISE_TIMER a_End);

    PRECISE_TIMER GetNow ();
}