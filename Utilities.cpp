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
}