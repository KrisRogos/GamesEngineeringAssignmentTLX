#include "CollisionSystem.h"


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
        for (int i = 0; i < k_AnimWorkers; i++)
        {
            mr_AnimWorkers[i].thread = std::thread (&CollisionSystem::WorkerMain, this, i);
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

                // distribute work to idle threads, first few are reserved for animation so don't use them
                /*for (int i = k_AnimWorkers; i < k_WorkerCount; i++)
                {

                }*/
            }

            //--- animate spheres ---
            // how much work was assigned already
            uint_fast8_t assigned = 0;

            // distribute work to all idle threads
            for (int i = 0; i < k_AnimWorkers && assigned * k_CircleLotSize < k_CircleCount; i++)
            {
                std::unique_lock<std::mutex> l (mr_AnimWorkers[i].lock, std::defer_lock);
                // check worker status
                //if (!mr_AnimWorkers[i].readyToStart)
                {
                    // prepare the work
                    mr_AnimWorkers[i].deltaTime = a_DeltaTime;
                    mr_AnimWorkers[i].range_Start = assigned * k_CircleLotSize;
                    mr_AnimWorkers[i].range_End = ++assigned * k_CircleLotSize;
                    mr_AnimWorkers[i].complete = false;
                    //mr_AnimWorkers[i].readyToStart = true;

                    //l.unlock ();

                    // notify the worker
                    mr_AnimWorkers[i].conditional.notify_one ();
                }
                //else
                {
                    //l.unlock ();
                }
            }

            // wait for the animation to finish
            for (int i = 0; i < k_AnimWorkers; i++)
            {
                auto& worker = mr_AnimWorkers[i];

                {
                    std::unique_lock<std::mutex> l (worker.lock);
                    worker.conditional.wait (l, [&]() { return worker.complete;  });
                    

                    //l.unlock ();
                }
                
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
        auto& worker = mr_AnimWorkers[a_thread];
        while (true)
        {
            // wait until prompted to do work
            {
                std::unique_lock<std::mutex> l (worker.lock);
                worker.conditional.wait (l, [&]() { std::cout << "Wait ";  return !worker.complete;  });
                std::cout << "The wait is over";
                /*worker.complete = false;
                l.unlock ();*/
            }

            // carry out the task
            std::cout << "Work start ";
            worker.fn (a_thread);
            std::cout << "Work end ";

            // notify main thread of completion
            {
                std::unique_lock<std::mutex> l (worker.lock);

                //worker.readyToStart = false;
                worker.complete = true;
                
                std::cout << "Release ";

                //l.unlock ();
                worker.conditional.notify_one ();
            }
        }
    }
    bool CollisionSystem::ProcessMovement (uint_fast8_t a_thread)
    {
        auto& worker = mr_AnimWorkers[a_thread];
        //std::cout << "test";

        // for all circles or spheres
        for (int i = worker.range_Start; i < worker.range_End; i++)
        {
            mr_Circles[i].locX += mr_Circles[i].velocityX * worker.deltaTime;
            mr_Circles[i].locY += mr_Circles[i].velocityY * worker.deltaTime;
#ifdef SIMULATION_3D
            mr_Circles[i].locZ += mr_Circles[i].velocityZ * worker.deltaTime;
#endif
        }

        return true;
    }

    bool CollisionSystem::ProcessLaserBeam (uint_fast8_t a_thread)
    {
        return false;
    }

}