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
#include "Laser.h"

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

    struct SMoveTask
    {
        bool complete = true;

        float deltaTime;
        uint_fast32_t range_Start;
        uint_fast32_t range_End;

    };

    struct SBeamTask
    {
        bool complete = true;

        uint_fast32_t range_Start;
        uint_fast32_t range_End;
        uint_fast8_t laser;

        float best_distance = k_GenerationLimitX * 1000.0f ;
        uint_fast32_t best_sphere = MAXUINT32;
    };

    struct SWorker
    {
        std::thread thread;
        std::condition_variable taskReady;
        std::mutex lock;

        bool busy;
    };

    class CollisionSystem
    {
    public:
        
        std::array<Circle, k_CircleCount> mr_Circles;
        std::array<Laser, k_MaxBeams> mr_Lasers;

    protected:
        std::array<Circle, k_CircleCount> mr_CirclesLaserBuffer;

        

        

        PRECISE_TIMER timeBegin;
        PRECISE_TIMER timeLastLaser;

        E_SimulationStatus m_SimStat;
        E_ResolverStatus m_ResStat;

        std::pair<SWorker, SMoveTask> mr_MoveWorkers[k_MoveWorkers];
        std::pair<SWorker, SBeamTask> mr_BeamWorkers[k_MoveWorkers];

    public:
        CollisionSystem ();
        virtual ~CollisionSystem ();

        void SetUP (void (*fnp_Print)(std::string a_Message, E_MessageType a_Type, float a_duration));

        // runs a single frame of the simulation, if no delta time specified 60FPS assumed
        void Run (void (*fnp_Print)(std::string a_Message, E_MessageType a_Type, float a_duration), float a_DeltaTime = 1.0f/60.0f);

        bool ProcessMovement (uint_fast8_t a_thread);
        bool ProcessLaserBeam (uint_fast8_t a_thread);
    };

}
