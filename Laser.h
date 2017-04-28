#pragma once
#include "Definitions.h"
// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    struct Laser
    {
        // this laser being used
        bool active = false;

        // position
        float locX;
        float locY;
#ifdef SIMULATION_3D
        float locZ;
#endif

        // angle - Y axis would just cause a spin so is not needed, in 2D (XY) simulation the axis rotates only on the Z axis
        float angleZ;
#ifdef SIMULATION_3D
        float angleX;
#endif

        // direction vector
        float vecX; 
        float vecY;
        float vecZ;

        // how long until this laser vanishes
        float lifeLeft = 0.0f;

    };
}