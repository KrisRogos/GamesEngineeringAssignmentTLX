#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <chrono>
#include <vector>
#include <cmath>

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {

// remove (comment) this line to switch to a 2D only mode
#define SIMULATION_3D

    // define which engines and outputs to use, can use both
//#define ENGINE_CONSOLE
#define ENGINE_TLX

#define PRECISE_TIMER std::chrono::time_point<std::chrono::steady_clock>
#define TIME_RES microseconds
#define TIME_SECOND 1000000
#define TIME_STR " microseconds"

    const uint_fast8_t k_MaxBeams = 10; // maximum number of beams that can be seen at once
    const uint_fast32_t k_CircleCount = 1; // number of circles to be simulated
    const uint_fast32_t k_CircleLotSize = 1; // number of circles per thread
    const uint_fast8_t k_BeamWorkers = k_CircleCount / k_CircleLotSize; // amount of workers for beam resolution only
    const uint_fast8_t k_MoveWorkers = k_CircleCount / k_CircleLotSize; // amount of workers for animation only

    const float k_GenerationLimitX = 5.0f; // positive world size in X, negative is calculated from this
    const float k_GenerationLimitY = 5.0f; // positive world size in Y, negative is calculated from this
#ifdef SIMULATION_3D 
    const float k_GenerationLimitZ = 5.0f; // positive world size in Z, negative is calculated from this
#endif

    const float k_VelociyLimitX = 0.0f; // positive velocity X limit, negative is calculated from this
    const float k_VelociyLimitY = 0.0f; // positive velocity Y limit, negative is calculated from this
#ifdef SIMULATION_3D 
    const float k_VelociyLimitZ = 0.0f; // positive velocity Z limit, negative is calculated from this
#endif

    enum class E_MessageType : uint_fast8_t
    {
        E_Info, // generic message
        E_Warning, // a resolvable issue
        E_Error, // a non-critical error
        E_Crit, // a critical error
    };
}