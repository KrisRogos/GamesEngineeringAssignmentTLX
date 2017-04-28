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
            mr_Circles[i].rad = RandomFloat(20.0f, 50.0f);

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
        }

        for (int i = 0; i < k_BeamWorkers; i++)
        {
            mr_BeamWorkers[i].first.thread = std::thread (&CollisionSystem::ProcessLaserBeam, this, i);
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

                // find an empty laser
                uint_fast32_t laser = MAXUINT32;
                for (int i = 0; i < k_MaxBeams; i++)
                {
                    if (!mr_Lasers[i].active)
                    {
                        laser = i;
                        continue;
                    }
                }

                // only do this if a laser was found
                if (laser < k_MaxBeams)
                {
                    // create the laser
                    mr_Lasers[laser].locX = RandomFloat (-k_GenerationLimitX, k_GenerationLimitX);
                    mr_Lasers[laser].locY = RandomFloat (-k_GenerationLimitY, k_GenerationLimitY);
#ifdef SIMULATION_3D
                    mr_Lasers[laser].locZ = RandomFloat (-k_GenerationLimitZ, k_GenerationLimitZ);
                    mr_Lasers[laser].angleX = RandomFloat (-180, 180);
#endif
                    mr_Lasers[laser].angleZ = RandomFloat (-180, 180);

                    // calculate the direction vector from data above
#ifdef SIMULATION_3D
                    mr_Lasers[laser].vecX = std::cosf (mr_Lasers[0].angleZ)*std::cosf (mr_Lasers[0].angleX);
                    mr_Lasers[laser].vecY = std::sinf (mr_Lasers[0].angleZ)*std::cosf (mr_Lasers[0].angleX);
                    mr_Lasers[laser].vecZ = std::sinf (mr_Lasers[0].angleX);
#else
                    mr_Lasers[laser].vecX = std::cosf (mr_Lasers[0].angleZ)*std::cosf (0.0f);
                    mr_Lasers[laser].vecY = std::sinf (mr_Lasers[0].angleZ)*std::cosf (0.0f);
                    mr_Lasers[laser].vecZ = std::sinf (0.0f);
#endif
                    mr_Lasers[laser].active = true;
                    mr_Lasers[laser].lifeLeft = 9.0f;

                    // distribute work to workers
                    for (int i = 0; i < k_BeamWorkers; i++)
                    {
                        auto & worker = mr_BeamWorkers[i].first;
                        std::unique_lock<std::mutex> lk (worker.lock);
                        if (!worker.busy)
                        {
                            worker.busy = true;
                            auto& task = mr_BeamWorkers[i].second;

                            task.complete = false;
                            task.best_distance = k_GenerationLimitX * 1000000.0f;
                            task.best_sphere = MAXUINT32;
                            task.laser = laser;
                            task.range_Start = k_CircleLotSize * i;
                            task.range_End = k_CircleLotSize * (i + 1);

                            worker.taskReady.notify_one ();
                        }
                    }

                    // prepare for finding the result
                    float best_distance = k_GenerationLimitX * 1000000.0f;
                    int best_sphere = MAXUINT32;

                    // wait for workers to finish
                    for (int i = 0; i < k_BeamWorkers; i++)
                    {
                        auto& worker = mr_BeamWorkers[i].first;
                        auto& task = mr_BeamWorkers[i].second;

                        // wait until ready
                        std::unique_lock<std::mutex> lk (worker.lock);
                        worker.taskReady.wait (lk, [&]() { return task.complete;  });

                        // compare results from this worker
                        if (task.best_distance < best_distance)
                        {
                            best_distance = task.best_distance;
                            best_sphere = task.best_sphere;
                        }
                    }

                    auto timeEnd = GetNow ();

                    // make sure a sphere was found
                    if (best_sphere < MAXUINT32)
                    {
                        mr_Circles[best_sphere].life -= 20.0f;
                        fnp_Print ("Collided with sphere: " + std::to_string (best_sphere) + " remaining hp: " + std::to_string (mr_Circles[best_sphere].life), E_MessageType::E_Warning, 2.5f);
                        fnp_Print ("Resolved with " + std::to_string (k_BeamWorkers) + " threads in " + GetDurationStr (timeStart, timeEnd) + TIME_STR, E_MessageType::E_Info, 2.5f);
                    }
                    else
                    {
                        fnp_Print ("All collisions avoided", E_MessageType::E_Warning, 2.5f);
                        fnp_Print ("Resolved with " + std::to_string (k_BeamWorkers) + " threads in " + GetDurationStr (timeStart, timeEnd) + TIME_STR, E_MessageType::E_Info, 2.5f);
                    }
                }


            }

            // modify laser timers
            int activeLasers = 0;
            for (int i = 0; i < k_MaxBeams; i++)
            {
                if (mr_Lasers[i].active)
                {
                    activeLasers++;
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
                auto& circle = mr_Circles[i];
                auto& laser = mr_Lasers[task.laser];
                // calculate vector to from laser origin to sphere
                float vectX = std::fabsf(laser.locX - circle.locX);
                float vectY = std::fabsf(laser.locY - circle.locY);
#ifdef SIMULATION_3D
                float vectZ = std::fabsf(laser.locZ - circle.locZ);
#endif
                // check if this is the new closest sphere
#ifdef SIMULATION_3D
                if (VectorLenght (vectX, vectY, vectZ) < task.best_distance)
#else
                if (VectorLenght (vectX, vectY) < task.best_distance)
#endif
                {
                    // project the vector onto the laser beam, result is a point on the Laser Beam corresponding with the center of the sphere
                    float projX = 0.0f;
                    float projY = 0.0f;
#ifdef SIMULATION_3D
                    float projZ = 0.0f;

                    VectorProjectOnto (vectX, vectY, vectZ, laser.vecX, laser.vecY, laser.vecZ, projX, projY, projZ);
#else
                    VectorProjectOnto (vectX, vectY, laser.vecX, laser.vecY, projX, projY);
#endif

                    // calculate the distance vector between line and sphere
                    float distX = std::fabsf (projX - circle.locX);
                    float distY = std::fabsf (projY - circle.locY);
#ifdef SIMULATION_3D
                    float distZ = std::fabsf (projZ - circle.locZ);

                    float dist = VectorLenght (distX, distY, distZ);
#else
                    float dist = VectorLenght (distX, distY);
#endif

                    // compare distance with the spheres radius
                    if (dist < circle.rad)
                    {
                        task.best_distance = dist;
                        task.best_sphere = i;
                    }
                }
                // no need to continue the work if it isn't
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