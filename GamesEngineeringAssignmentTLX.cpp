// GamesEngineeringAssignmentTLX.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "CollisionSystem.h"
using namespace tle;

/** KeyCodes **/
// Game management
const EKeyCode kg_KeyQuit = Key_Escape; // quit button

// engine must be accessible to all functions
I3DEngine* gp_Engine;

// message struct allows for pooling messages for display
struct SMsg {
    std::string text; // content
    KRCS::E_MessageType type; // colour and severity
    float timer; // used by TLE to know how long to show the message for
    bool printed; // used by CONSOLE to know if this message was already displayed

    SMsg (std::string a_Text, KRCS::E_MessageType a_Type, float a_duration)
    {
        text = a_Text;
        type = a_Type;
        timer = a_duration;
        printed = false;
    }
};

std::vector<SMsg*> g_Messages; // message pool

void PrintText (std::string a_Message, KRCS::E_MessageType a_type, float a_duration = 2.5f)
{
    g_Messages.emplace_back (new SMsg (a_Message, a_type, a_duration));
}

// Based on the function provided my Laurent Noel
// Orient and stretch LineModel to connect the given points. Pass line width (world units) and camera
// Use Line.x for the line model for correct results. Create a model for each line required
void CreateLine (IModel* LineModel, float x1, float y1, float z1, float x2, float y2, float z2, float lineWidth, ICamera* camera)
{
    float xCam = camera->GetX ();
    float yCam = camera->GetY ();
    float zCam = camera->GetZ ();

    float xLnCent = (x1 + x2) * 0.5f;
    float yLnCent = (y1 + y2) * 0.5f;
    float zLnCent = (z1 + z2) * 0.5f;

    float xUp;
    float yUp;
    float zUp;

    float lenght = KRCS::VectorLenght (std::fabsf(x1 - x2), std::fabsf(y1 - y2), std::fabsf(z1 - z2));

    KRCS::VectorNorm (xCam - xLnCent, yCam - yLnCent, zCam - zLnCent, xUp, yUp, zUp);

    KRCS::SMatirx4 lineMtx = KRCS::FaceTarget (x1, y1, z1, x2, y2, z2, xUp, yUp, zUp);

    lineMtx.ScaleX (lineWidth);
    lineMtx.ScaleY (0.0f);
    lineMtx.ScaleZ (lenght);

    LineModel->SetMatrix (&lineMtx.data[0]);
}

void main()
{
    /**** Initialization ****/
    bool running = true;

    //// set up the collision simulation
    KRCS::CollisionSystem* p_Collision = new KRCS::CollisionSystem ();
    p_Collision->SetUP (PrintText);

    // Create a 3D engine (using TLX engine here) and open a window for it
#ifdef ENGINE_TLX
    gp_Engine = New3DEngine (kTLX);
    gp_Engine->StartWindowed ();
    gp_Engine->StopMouseCapture ();

    // Add media folders
    gp_Engine->AddMediaFolder ("Media");

    uint_fast8_t fontSize = 18;
    // prepare and load the font
    IFont* p_Font = gp_Engine->LoadFont("Arial", fontSize);

    // load meshes
    IMesh* p_MshSphere = gp_Engine->LoadMesh ("sphere.x");
    IMesh* p_MshLine = gp_Engine->LoadMesh ("Line.x");

    /**** Scene set up ****/
    // camera set up
    ICamera* p_Cam = gp_Engine->CreateCamera (kFPS, 0.0f, 0.0f, -1000.0f);
    p_Cam->SetRotationSpeed (25.0f);
    p_Cam->SetFarClip (std::fmaxf(KRCS::k_GenerationLimitY * 10.0f, 10000.0f));

    //-- create models
    
    // spheres
    std::array<std::pair<IModel*, bool>, KRCS::k_CircleCount> pr_ModSpheres;
    for (int i = 0; i < KRCS::k_CircleCount; i++)
    {
        pr_ModSpheres[i].first = p_MshSphere->CreateModel ();
        pr_ModSpheres[i].first->Scale (p_Collision->mr_Circles[i].rad / 5.0f); // physical size adjusted for better presentation
        pr_ModSpheres[i].second = true;
    }

    // lasers
    std::array<std::pair<IModel*, bool>, KRCS::k_MaxBeams> pr_ModLasers;
    for (uint_fast8_t i; i < KRCS::k_MaxBeams; i++)
    {
        pr_ModLasers[i].second = false;
    }

#endif

    float simulationTime = 0.0f;

    /**** Game Loop ****/
    while (
#ifdef ENGINE_TLX 
        gp_Engine->IsRunning ()
#else
        true
#endif 
        && running)
    {
        p_Collision->Run (PrintText, simulationTime);

        // console output
#ifdef ENGINE_CONSOLE
        HANDLE  hConsole;
        hConsole = GetStdHandle (STD_OUTPUT_HANDLE);

        // print all text
        for (int i = 0; i < g_Messages.size (); i++)
        {
            auto current = g_Messages[i];

            if (!current->printed)
            {
                switch (current->type)
                {
                default:
                    break;
                case KRCS::E_MessageType::E_White:
                    SetConsoleTextAttribute (hConsole, 15);
                    std::cout << current->text << '\n';
                    break;
                case KRCS::E_MessageType::E_Yellow:
                    SetConsoleTextAttribute (hConsole, 14);
                    std::cout << current->text << '\n';
                    SetConsoleTextAttribute (hConsole, 15);
                    break;
                case KRCS::E_MessageType::E_Red:
                    SetConsoleTextAttribute (hConsole, 12);
                    std::cout << current->text << '\n';
                    SetConsoleTextAttribute (hConsole, 15);
                    break;
                }

                current->printed = true;
            }
        }
#ifndef ENGINE_TLX
        // when TLX is not in use, remove the messages
        g_Messages.clear ();
#endif
#endif

        // visual output in TLX
#ifdef ENGINE_TLX
        // Draw the scene
        gp_Engine->DrawScene ();
        float frameTime = gp_Engine->Timer ();
        gp_Engine->SetWindowCaption ("Delta: " + std::to_string (frameTime) + " | FPS: " + std::to_string (1 / frameTime));

        // display the spheres
        for (int i = 0; i < KRCS::k_CircleCount; i++)
        {
            auto current = p_Collision->mr_Circles[i];

            // is alive
            if (current.life > 0 && pr_ModSpheres[i].second)
            {
#ifdef SIMULATION_3D
                pr_ModSpheres[i].first->SetPosition (current.locX, current.locY, current.locZ);
#else
                pr_ModSpheres[i].first->SetPosition (current.locX, current.locY, 0.0f);
#endif
            }
            // should it be destroyed
            else if (current.life <= 0 && pr_ModSpheres[i].second)
            {
                p_MshSphere->RemoveModel (pr_ModSpheres[i].first);
                pr_ModSpheres[i].second = false;
            }

        }

        // display the lasers
        for (int i = 0; i < KRCS::k_MaxBeams; i++)
        {
            auto& current = p_Collision->mr_Lasers[i];
            // check if the laser is active
            if (current.active)
            {
                // make a model for it if needed
                if (!pr_ModLasers[i].second)
                {
                    pr_ModLasers[i].first = p_MshLine->CreateModel ();
#ifdef SIMULATION_3D
                    CreateLine (pr_ModLasers[i].first, current.startX, current.startY, current.startZ, current.endX, current.endY, current.endZ, 5.0f, p_Cam);

#else
                    CreateLine (pr_ModLasers[i].first, current.startX, current.startY, 0.0f, current.endX, current.endY, 0.0f, 5.0f, p_Cam);
#endif

                    pr_ModLasers[i].second = true;
                }

            }
            // otherwise make sure to not keep it's model
            else if (pr_ModLasers[i].second)
            {
                p_MshLine->RemoveModel (pr_ModLasers[i].first);
                pr_ModLasers[i].second = false;
            }
        }

        // display messages
        for (int i = 0; i < g_Messages.size(); i++)
        {
            auto current = g_Messages[i];

            current->timer -= frameTime;
            if (current->timer < 0.0f)
            {
                g_Messages.erase (g_Messages.begin () + i);
                i--;
                delete current;
            }
            else
            {
                switch (current->type)
                {
                default:
                    break;
                case KRCS::E_MessageType::E_White:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffeeeeee);
                    break;
                case KRCS::E_MessageType::E_Yellow:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffffff00);
                    break;
                case KRCS::E_MessageType::E_Red:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffff0000);
                    break;
                    break;
                }
            }

        }

        // stop the game
        if (gp_Engine->KeyHit (kg_KeyQuit))
        {
            gp_Engine->Stop ();
        }

        // set the time for the simulation to either frame time or 60FPS
        simulationTime = frameTime;
#else
        simulationTime = 1.0f / 60.0f;
#endif


    }

    /**** Clean up ****/

    // Delete the 3D engine now we are finished with it
    gp_Engine->Delete ();
}
