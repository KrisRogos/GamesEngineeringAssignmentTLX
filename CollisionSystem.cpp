#include "CollisionSystem.h"
#include <assert.h>

// Namespace for the project: Kris Rogos Collision System
namespace KRCS {
    CollisionSystem::CollisionSystem ()
    {
        timeBegin = std::chrono::high_resolution_clock::now ();
        m_SimStat = E_SimulationStatus::ENotInitalized;
        std::srand (std::time (NULL)); // set up the random seed
    }


    CollisionSystem::~CollisionSystem ()
    {
    }

    void CollisionSystem::SetUP (void (*fnp_Print)(std::string a_Message, E_MessageType a_Type, float a_duration))
    {
        auto timeStart = GetNow ();
        // randomly position and size the objects in world space
        for (int i = 0; i < k_CircleCount; i++)
        {
            // position
            mr_Circles[i].locX = RandomFloat (-k_GenerationLimitX, k_GenerationLimitX);
            mr_Circles[i].locY = RandomFloat (-k_GenerationLimitY, k_GenerationLimitY);
#ifdef SIMULATION_3D
            mr_Circles[i].locZ = RandomFloat (-k_GenerationLimitZ, k_GenerationLimitZ);
#endif

            // size
            mr_Circles[i].rad = 1.0f;

            // velocity
            mr_Circles[i].velocityX = RandomFloat (-k_VelociyLimitX, k_VelociyLimitX);
            mr_Circles[i].velocityY = RandomFloat (-k_VelociyLimitY, k_VelociyLimitY);
#ifdef SIMULATION_3D
            mr_Circles[i].velocityZ = RandomFloat (-k_VelociyLimitZ, k_VelociyLimitZ);
#endif
            // colour
            mr_Circles[i].R = 1.0f;
            mr_Circles[i].G = 1.0f;
            mr_Circles[i].B = 1.0f;

            // other parameters
            mr_Circles[i].life = 100;
            mr_Circles[i].name = "circle";
        }

        // create worker threads
        for (int i = 0; i < k_MoveWorkers; i++)
        {
            mr_MoveWorkers[i].first.thread = std::thread (&CollisionSystem::ProcessMovement, this, i);

            //mr_AnimWorkers[i].thread = std::thread (&CollisionSystem::WorkerMain, this, i);
        }

        timeLastLaser = GetNow ();
        m_SimStat = E_SimulationStatus::ERunning;

        // display the time it took to start the process
        auto timeEnd = GetNow();
        fnp_Print ("The collider is ready to use after: " + GetDurationStr(timeStart, timeEnd) + TIME_STR + " with " + std::to_string(k_CircleCount) + " circles", E_MessageType::E_Info, 5.0f);
    }
    void CollisionSystem::Run (void (*fnp_Print)(std::string a_Message, E_MessageType a_Type, float a_duration), float a_DeltaTime)
    {
        if (m_SimStat == E_SimulationStatus::ERunning)
        {
            auto timeStart = GetNow ();
            // is it time for a laser to fire
            if (GetDurationLng (timeLastLaser, timeStart) >= TIME_SECOND)
            {
                fnp_Print (std::to_string(GetDurationLng (timeBegin, timeStart)/TIME_SECOND) + ": Laser fired", E_MessageType::E_Info, 2.5f);
                timeLastLaser = std::chrono::high_resolution_clock::now ();

                // create the laser
                mr_Lasers[0].active = true;
                mr_Lasers[0].locX = RandomFloat (-k_GenerationLimitX, -k_GenerationLimitX);
                mr_Lasers[0].locY = RandomFloat (-k_GenerationLimitY, -k_GenerationLimitY);
                mr_Lasers[0].locZ = RandomFloat (-k_GenerationLimitZ, -k_GenerationLimitZ);
                mr_Lasers[0].angleX = RandomFloat (-90, 90);
                mr_Lasers[0].angleZ = RandomFloat (-90, 90);

                mr_Lasers[0].lifeLeft = 0.5f;

                // distribute work to idle threads, first few are reserved for animation so don't use them
                /*for (int i = k_AnimWorkers; i < k_WorkerCount; i++)
                {

                }*/
            }

            // modify laser timers
            for (int i = 0; i < k_MaxBeams; i++)
            {
                if (mr_Lasers[i].active)
                {
                    mr_Lasers[i].lifeLeft -= a_DeltaTime;

                    if (mr_Lasers[i].lifeLeft <= 0.0f)
                    {
                        mr_Lasers[i].active = false;
                    }
                }
            }

            //--- animate spheres ---
            // send the task
            for (int i = 0; i < k_MoveWorkers; i++)
            {
                auto& worker = mr_MoveWorkers[i].first;
                std::unique_lock<std::mutex> lk (worker.lock);
                if (!worker.busy)
                {
                    worker.busy = true;
                    auto& task = mr_MoveWorkers[i].second;

                    task.complete = false;
                    task.deltaTime = a_DeltaTime;
                    task.range_Start = k_CircleLotSize * i;
                    task.range_End = k_CircleLotSize * (i + 1);

                    worker.taskReady.notify_one ();
                }
            }


            for (int i = 0; i < k_MoveWorkers; i++)
            {
                auto& worker = mr_MoveWorkers[i].first;
                auto& task = mr_MoveWorkers[i].second;

                // wait until ready
                std::unique_lock<std::mutex> lk (worker.lock);
                worker.taskReady.wait (lk, [&]() { return task.complete; });
            }
            
            fnp_Print ("Done", E_MessageType::E_Info, 5.0f);

            /*
            std::cout << "Programmed finished in " << std::chrono::duration_cast<std::chrono::TIME_RES>(timeEnd - timeBegin).count () <<
            TIME_STR << "\nThe circle generation itself took " << std::chrono::duration_cast<std::chrono::TIME_RES>(timeEnd - timeStart).count () <<
            TIME_STR << "\n";

            system ("pause");*/
        }
        
    }
    
    void CollisionSystem::WorkerMain (uint_fast8_t a_thread)
    {
        //auto& worker = mr_MoveWorkers[a_thread].first;
        //auto& task = mr_MoveWorkers[a_thread].second;
        //while (true)
        //{
        //    // wait until prompted to do work
        //    {
        //        std::unique_lock<std::mutex> l (worker.lock);
        //        worker.taskReady.wait (l, [&]() { std::cout << "Wait ";  return !worker.complete;  });
        //        std::cout << "The wait is over";
        //        /*worker.complete = false;
        //        l.unlock ();*/
        //    }

        //    // carry out the task
        //    std::cout << "Work start ";
        //    worker.fn (a_thread);
        //    std::cout << "Work end ";

        //    // notify main thread of completion
        //    {
        //        std::unique_lock<std::mutex> l (worker.lock);

        //        //worker.readyToStart = false;
        //        worker.complete = true;
        //        
        //        std::cout << "Release ";

        //        //l.unlock ();
        //        worker.taskReady.notify_one ();
        //    }
        //}
    }

    bool CollisionSystem::ProcessMovement (uint_fast8_t a_thread)
    {
        auto& worker = mr_MoveWorkers[a_thread].first;
        auto& task = mr_MoveWorkers[a_thread].second;

        while (true)
        {
            // wait
            {
                std::unique_lock<std::mutex> lk (worker.lock);
                worker.taskReady.wait (lk, [&]() { return !task.complete;  });
            }

            // for all circles or spheres
            for (int i = task.range_Start; i < task.range_End; i++)
            {
                mr_Circles[i].locX += mr_Circles[i].velocityX * task.deltaTime;
                mr_Circles[i].locY += mr_Circles[i].velocityY * task.deltaTime;
    #ifdef SIMULATION_3D
                mr_Circles[i].locZ += mr_Circles[i].velocityZ * task.deltaTime;
    #endif
            }

            // send the return signal
            {
                std::unique_lock<std::mutex> lk (worker.lock);
                worker.busy = false;
                task.complete = true;
                worker.taskReady.notify_one ();
            }
        }

        return true;
    }

    bool CollisionSystem::ProcessLaserBeam (uint_fast8_t a_thread)
    {
        auto& worker = mr_BeamWorkers[a_thread].first;
        auto& task = mr_BeamWorkers[a_thread].second;

        while (true)
        {
            // wait
            {
                std::unique_lock<std::mutex> lk (worker.lock);
                worker.taskReady.wait (lk, [&]() { return !task.complete;  });
            }

            // for all circles or spheres
            for (int i = task.range_Start; i < task.range_End; i++)
            {

            }

            // send the return signal
            {
                std::unique_lock<std::mutex> lk (worker.lock);
                worker.busy = false;
                task.complete = true;
                worker.taskReady.notify_one ();
            }
        }

        return true;
    }

}