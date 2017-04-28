#pragma once
#include <stdlib.h>
#include "Definitions.h"

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    float RandomFloat (float min = 0, float max = 1);

    std::string GetDurationStr (PRECISE_TIMER a_Start, PRECISE_TIMER a_End);

    long GetDurationLng (PRECISE_TIMER a_Start, PRECISE_TIMER a_End);

    PRECISE_TIMER GetNow ();

    float VectorLenght (float a_X, float a_Y, float a_Z);
    float VectorLenght (float a_X, float a_Y);

    float VectorDot (float a_X, float a_Y, float a_Z);
    float VectorDot (float a_X, float a_Y);

    // project vector 1 onto vector 2, store result in vector o
    void VectorProjectOnto (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float & o_X, float & o_Y, float & o_Z);
    void VectorProjectOnto (float a_X1, float a_Y1, float a_X2, float a_Y2, float & o_X, float & o_Y);
}