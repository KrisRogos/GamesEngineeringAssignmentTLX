#pragma once
#include <iostream>
#include <string>
#include <array>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "Utilities.h"
#include "Circle.h"

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    enum class E_SimulationStatus : uint_fast8_t
    {
        ENotInitalized, // initial state, make sure to call "SetUP()" to continue
        ERunning, // simulation in progress
        EShuttingDown,  // waiting for workers to shut down and for memory to be released
        EComplete // simulation is no longer running and can be deleted safely
    };

    enum class E_ResolverStatus : uint_fast8_t
    {
        EIdle, // no task present
        ECopying, // data is being transfered to or from workers
        EReady, // task was resolved and results are ready to be collected
    };

    enum class E_WorkerStatus : uint_fast8_t
    {
        E_IDLE, // no task
        E_Working, // worker has a task and is working on it
        E_Complete, // worker has completed it's current task, data is ready for offloading
    };

    struct SWorker
    {
        std::thread thread;
        std::condition_variable conditional;
        std::mutex lock;

        bool complete = true;
        bool readyToStart = false;

        E_WorkerStatus state;
        float deltaTime;
        uint_fast32_t range_Start;
        uint_fast32_t range_End;
        bool (*fn)(uint_fast8_t a_thread);
    };

    class CollisionSystem
    {
    public:
        
        std::array<Circle, k_CircleCount> mr_Circles;

    protected:
        std::array<Circle, k_CircleCount> mr_CirclesLaserBuffer;

        const float k_GenerationLimitX = 1000.0f; // positive world size in X, negative is calculated from this
        const float k_GenerationLimitY = 1000.0f; // positive world size in Y, negative is calculated from this
#ifdef SIMULATION_3D 
        const float k_GenerationLimitZ = 1000.0f; // positive world size in Z, negative is calculated from this
#endif

        const float k_VelociyLimitX = 50.0f; // positive velocity X limit, negative is calculated from this
        const float k_VelociyLimitY = 50.0f; // positive velocity Y limit, negative is calculated from this
#ifdef SIMULATION_3D 
        const float k_VelociyLimitZ = 50.0f; // positive velocity Z limit, negative is calculated from this
#endif

        PRECISE_TIMER timeBegin;
        PRECISE_TIMER timeLastLaser;

        E_SimulationStatus m_SimStat;
        E_ResolverStatus m_ResStat;

        std::array<SWorker, k_AnimWorkers> mr_AnimWorkers;

    public:
        CollisionSystem ();
        virtual ~CollisionSystem ();

        void SetUP (void (*fnp_Print)(std::string a_Message, E_MessageType a_Type, float a_duration));

        // runs a single frame of the simulation, if no delta time specified 60FPS assumed
        void Run (void (*fnp_Print)(std::string a_Message, E_MessageType a_Type, float a_duration), float a_DeltaTime = 1.0f/60.0f);

        void WorkerMain (uint_fast8_t a_thread);
        bool ProcessMovement (uint_fast8_t a_thread);
        bool ProcessLaserBeam (uint_fast8_t a_thread);
    };

}