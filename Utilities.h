#pragma once
#include <stdlib.h>
#include "Definitions.h"

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    struct SMatirx4
    {
        float data[16];

        SMatirx4 ();
        SMatirx4 (float a_X, float a_Y, float a_Z);
        SMatirx4 (float a_00, float a_01, float a_02,
            float a_10, float a_11, float a_12,
            float a_20, float a_21, float a_22,
            float a_30, float a_31, float a_32);

        void ScaleX (float a_Factor);
        void ScaleY (float a_Factor);
        void ScaleZ (float a_Factor);
    };

    float RandomFloat (float min = 0, float max = 1);

    std::string GetDurationStr (PRECISE_TIMER a_Start, PRECISE_TIMER a_End);

    long GetDurationLng (PRECISE_TIMER a_Start, PRECISE_TIMER a_End);

    PRECISE_TIMER GetNow ();

    float VectorLenght (float a_X, float a_Y, float a_Z);
    float VectorLenght (float a_X, float a_Y);

    float VectorDot (float a_X, float a_Y, float a_Z);
    float VectorDot (float a_X, float a_Y);

    void VectorCross (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float & o_X, float & o_Y, float & o_Z);

    void VectorNorm (float a_X, float a_Y, float a_Z, float & o_X, float & o_Y, float & o_Z);
    void VectorNorm (float a_X, float a_Y, float & o_X, float & o_Y);

    SMatirx4 FaceTarget (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float a_XU, float a_YU, float a_ZU);


    bool VectorIsZero (float a_X, float a_Y, float a_Z);
    bool VectorIsZero (float a_X, float a_Y);

    // project vector 1 onto vector 2, store result in vector o
    void VectorProjectOnto (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float & o_X, float & o_Y, float & o_Z);
    void VectorProjectOnto (float a_X1, float a_Y1, float a_X2, float a_Y2, float & o_X, float & o_Y);
}