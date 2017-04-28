#include "Utilities.h"

namespace KRCS
{
    float RandomFloat (float min, float max)
    {
        float random = ((float)rand () / (float)RAND_MAX); // generate a number
        float setSize = max - min; // get the size of the set of numbers available
        float result = random * setSize; // calculate the number
        return min + result; // return the number adjusted to the starting point of the random set

    }

    std::string GetDurationStr (PRECISE_TIMER a_Start, PRECISE_TIMER a_End)
    {
        return std::to_string (std::chrono::duration_cast<std::chrono::TIME_RES>(a_End - a_Start).count ());
    }

    long GetDurationLng (PRECISE_TIMER a_Start, PRECISE_TIMER a_End)
    {
        return  std::chrono::duration_cast<std::chrono::TIME_RES>(a_End - a_Start).count ();
    }

    PRECISE_TIMER GetNow ()
    {
        return std::chrono::high_resolution_clock::now ();
    }

    float VectorDot (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2)
    {
        return a_X1 * a_X2 + a_Y1 * a_Y2 + a_Z1 * a_Z2;
    }

    float VectorDot (float a_X1, float a_Y1, float a_X2, float a_Y2)
    {
        return a_X1 * a_X2 + a_Y1 * a_Y2;
    }

    float VectorLenght (float a_X, float a_Y, float a_Z)
    {
        float sqX = a_X * a_X;
        float sqY = a_Y * a_Y;
        float sqZ = a_Z * a_Z;

        float total = sqX + sqY + sqZ;

        return std::sqrtf (total);
    }

    float VectorLenght (float a_X, float a_Y)
    {
        float sqX = a_X * a_X;
        float sqY = a_Y * a_Y;

        float total = sqX + sqY;

        return std::sqrtf (total);
    }

    // project vector 1 onto vector 2, store result in vector o
    void VectorProjectOnto (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float & o_X, float & o_Y, float & o_Z)
    {
        float dp1 = VectorDot (a_X2, a_Y2, a_Z2, a_X2, a_Y2, a_Z2);
        if (dp1 > 0.0f)
        {
            float dp2 = VectorDot (a_X1, a_Y1, a_Z1, a_X2, a_Y2, a_Z2);
            float div = dp2 / dp1;
            o_X = a_X2 * div;
            o_Y = a_Y2 * div;
            o_Z = a_Z2 * div;
            return;
        }
        o_X = 0.0f;
        o_Y = 0.0f;
        o_Z = 0.0f;
        return;
    }

    // project vector 1 onto vector 2, store result in vector o
    void VectorProjectOnto (float a_X1, float a_Y1, float a_X2, float a_Y2, float & o_X, float & o_Y)
    {
        float dp1 = VectorDot (a_X2, a_Y2, a_X2, a_Y2);
        if (dp1 > 0.0f)
        {
            float dp2 = VectorDot (a_X1, a_Y1, a_X2, a_Y2);
            float div = dp2 / dp1;
            o_X = a_X2 * div;
            o_Y = a_Y2 * div;
            return;
        }
        o_X = 0.0f;
        o_Y = 0.0f;
        return;
    }
}