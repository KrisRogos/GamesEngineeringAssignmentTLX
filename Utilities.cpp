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



    void VectorCross (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float & o_X, float & o_Y, float & o_Z)
    {
        o_X = a_Y1 * a_Z2 - a_Z1 * a_Y2;
        o_Y = a_Z1 * a_X2 - a_X1 * a_Z2;
        o_Z = a_X1 * a_Y2 - a_Y1 * a_X2;
    }

    void VectorNorm (float a_X, float a_Y, float a_Z, float & o_X, float & o_Y, float & o_Z)
    {
        float lenght = VectorLenght (a_X, a_Y, a_Z);

        o_X = a_X / lenght;
        o_Y = a_Y / lenght;
        o_Z = a_Z / lenght;

        return;
    }

    void VectorNorm (float a_X, float a_Y, float & o_X, float & o_Y)
    {
        float lenght = VectorLenght (a_X, a_Y);

        o_X = a_X / lenght;
        o_Y = a_Y / lenght;

        return;
    }

    SMatirx4 FaceTarget (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float a_XU, float a_YU, float a_ZU)
    {
        float XaxisZ;
        float YaxisZ;
        float ZaxisZ;

        float XaxisY;
        float YaxisY;
        float ZaxisY;

        float XaxisX;
        float YaxisX;
        float ZaxisX;

        // calculate Z axis
        VectorNorm (a_X2 - a_X1, a_Y2 - a_Y1, a_Z2 - a_Z1, XaxisZ, YaxisZ, ZaxisZ);
        if (VectorIsZero (XaxisZ, YaxisZ, ZaxisZ))
            return SMatirx4 (a_X1, a_Y1, a_Z1);

        // calculate X axis
        float crossX;
        float crossY;
        float crossZ;

        VectorCross (a_XU, a_YU, a_ZU, XaxisZ, YaxisZ, ZaxisZ, crossX, crossY, crossZ);
        VectorNorm (crossX, crossY, crossZ, XaxisX, YaxisX, ZaxisX);
        if (VectorIsZero (XaxisX, YaxisX, ZaxisX))
            return SMatirx4 (a_X1, a_Y1, a_Z1);

        // calculate Y axis
        VectorCross (XaxisX, YaxisX, ZaxisX, XaxisZ, YaxisZ, ZaxisZ, XaxisY, YaxisY, ZaxisY);

        return SMatirx4 (XaxisX, YaxisX, ZaxisX, XaxisY, YaxisY, ZaxisY, XaxisZ, YaxisZ, ZaxisZ, a_X1, a_Y1, a_Z1);
    }

    // project vector 1 onto vector 2, store result in vector o, s and e are the starting and end positions of the 2nd vector
    void VectorProjectOnto (float a_X1, float a_Y1, float a_Z1, float a_X2, float a_Y2, float a_Z2, float a_sX, float a_sY, float a_sZ, float a_eX, float a_eY, float a_eZ, float & o_X, float & o_Y, float & o_Z)
    {
        float nX;
        float nY;
        float nZ;
        // get normal vector of the laser beam
        VectorNorm (a_X2, a_Y2, a_Z2, nX, nY, nZ);

        // get the length of the projected vector
        float projectionLenght = VectorDot (a_X1, a_Y1, a_Z1, nX, nY, nZ);

        // handle edge cases
        if (projectionLenght < 0)
        {
            o_X = a_sX;
            o_Y = a_sY;
            o_Z = a_sZ;
            return;
        }
        else if (projectionLenght > VectorLenght (a_X2, a_Y2, a_Z2))
        {
            o_X = a_eX;
            o_Y = a_eY;
            o_Z = a_eZ;
            return;
        }

        // calculate the projected vector
        float pX = projectionLenght * nX;
        float pY = projectionLenght * nY;
        float pZ = projectionLenght * nZ;

        o_X = a_sX + pX;
        o_Y = a_sY + pY;
        o_Z = a_sZ + pZ;
        return;
    }

    // project vector 1 onto vector 2, store result in vector o, s and e are the starting and end positions of the 2nd vector
    void VectorProjectOnto (float a_X1, float a_Y1, float a_X2, float a_Y2, float a_sX, float a_sY, float a_eX, float a_eY, float & o_X, float & o_Y)
    {

        float nX;
        float nY;
        // get normal vector of the laser beam
        VectorNorm (a_X2, a_Y2, nX, nY);

        // get the length of the projected vector
        float projectionLenght = VectorDot (a_X1, a_Y1, nX, nY);

        // handle edge cases
        if (projectionLenght < 0)
        {
            o_X = a_sX;
            o_Y = a_sY;
            return;
        }
        else if (projectionLenght > VectorLenght (a_X2, a_Y2))
        {
            o_X = a_eX;
            o_Y = a_eY;
            return;
        }

        // calculate the projected vector
        float pX = projectionLenght * nX;
        float pY = projectionLenght * nY;

        o_X = a_sX + pX;
        o_Y = a_sY + pY;
        return;
    }

    bool VectorIsZero (float a_X, float a_Y, float a_Z)
    {
        return (a_X * a_X + a_Y * a_Y + a_Z * a_Z) < 0.000001f;
    }

    bool VectorIsZero (float a_X, float a_Y)
    {
        return (a_X * a_X + a_Y * a_Y) < 0.000001f;
    }

    SMatirx4::SMatirx4 ()
    {
        data[0] = 1.0f;
        data[1] = 0.0f;
        data[2] = 0.0f;
        data[3] = 0.0f;

        data[4] = 0.0f;
        data[5] = 1.0f;
        data[6] = 0.0f;
        data[7] = 0.0f;

        data[8] = 0.0f;
        data[9] = 0.0f;
        data[10] = 1.0f;
        data[11] = 0.0f;

        data[12] = 0.0f;
        data[13] = 0.0f;
        data[14] = 0.0f;
        data[15] = 1.0f;
    }

    SMatirx4::SMatirx4 (float a_X, float a_Y, float a_Z)
    {
        data[0] = 1.0f;
        data[1] = 0.0f;
        data[2] = 0.0f;
        data[3] = 0.0f;

        data[4] = 0.0f;
        data[5] = 1.0f;
        data[6] = 0.0f;
        data[7] = 0.0f;

        data[8] = 0.0f;
        data[9] = 0.0f;
        data[10] = 1.0f;
        data[11] = 0.0f;

        data[12] = a_X;
        data[13] = a_Y;
        data[14] = a_Z;
        data[15] = 1.0f;
    }
    SMatirx4::SMatirx4 (float a_00, float a_01, float a_02, float a_10, float a_11, float a_12, float a_20, float a_21, float a_22, float a_30, float a_31, float a_32)
    {

        data[0] = a_00;
        data[1] = a_01;
        data[2] = a_02;
        data[3] = 0.0f;

        data[4] = a_10;
        data[5] = a_11;
        data[6] = a_12;
        data[7] = 0.0f;

        data[8]  = a_20;
        data[9]  = a_21;
        data[10] = a_22;
        data[11] = 0.0f;

        data[12] = a_30;
        data[13] = a_31;
        data[14] = a_32;
        data[15] = 1.0f;

    }

    void SMatirx4::ScaleX (float a_Factor)
    {
        data[0] *= a_Factor;
        data[1] *= a_Factor;
        data[2] *= a_Factor;
    }

    void SMatirx4::ScaleY (float a_Factor)
    {
        data[4] *= a_Factor;
        data[5] *= a_Factor;
        data[6] *= a_Factor;
    }

    void SMatirx4::ScaleZ (float a_Factor)
    {
        data[8] *= a_Factor;
        data[9] *= a_Factor;
        data[10] *= a_Factor;
    }
}