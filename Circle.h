#include <string>
#include "Definitions.h"

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    struct Circle
    {
        // position
        float locX;
        float locY;
#ifdef SIMULATION_3D
        float locZ;
#endif

        // speed
        float velocityX;
        float velocityY;
#ifdef SIMULATION_3D
        float velocityZ;
#endif

        // size
        float rad;

        // colour
        float R;
        float G;
        float B;

        // life
        int life;

        // identifiers
        std::string name;

    };
}