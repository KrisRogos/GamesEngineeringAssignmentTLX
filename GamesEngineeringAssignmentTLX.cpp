// GamesEngineeringAssignmentTLX.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "CollisionSystem.h"
using namespace tle;

/** KeyCodes **/
// Game management
const EKeyCode kg_KeyQuit = Key_Escape; // quit button

// engine must be accessible to all functions
I3DEngine* gp_Engine;

#ifdef ENGINE_TLX
// required for printing text in TLE
struct SMsg {
    std::string text;
    KRCS::E_MessageType type;
    float timer;

    SMsg (std::string a_Text, KRCS::E_MessageType a_Type, float a_duration)
    {
        text = a_Text;
        type = a_Type;
        timer = a_duration;
    }
};
std::vector<SMsg*> g_Messages;
#endif

void PrintText (std::string a_Message, KRCS::E_MessageType a_type, float a_duration = 2.5f)
{
#ifdef ENGINE_TLX

    g_Messages.emplace_back (new SMsg (a_Message, a_type, a_duration));

#endif

#ifdef ENGINE_CONSOLE

    HANDLE  hConsole;
    hConsole = GetStdHandle (STD_OUTPUT_HANDLE);

    switch (a_type)
    {
    default:
        break;
    case KRCS::E_MessageType::E_Info:
        SetConsoleTextAttribute (hConsole, 15);
        std::cout << a_Message << '\n';
        break;
    case KRCS::E_MessageType::E_Warning:
        SetConsoleTextAttribute (hConsole, 14);
        std::cout << a_Message << '\n';
        SetConsoleTextAttribute (hConsole, 15);
        break;
    case KRCS::E_MessageType::E_Error:
        SetConsoleTextAttribute (hConsole, 12);
        std::cout << a_Message << '\n';
        SetConsoleTextAttribute (hConsole, 15);
        break;
    case KRCS::E_MessageType::E_Crit:
        SetConsoleTextAttribute (hConsole, 192);
        std::cout << a_Message << '\n';
        SetConsoleTextAttribute (hConsole, 15);
        break;
    }
#endif
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
    gp_Engine->AddMediaFolder ("C:\\ProgramData\\TL-Engine\\Media");
    gp_Engine->AddMediaFolder ("Media");

    uint_fast8_t fontSize = 18;
    // prepare and load the font
    IFont* p_Font = gp_Engine->LoadFont("Arial", fontSize);

    // load meshes
    IMesh* p_MshBeam = gp_Engine->LoadMesh ("LaserBeam.x");
    IMesh* p_MshSphere = gp_Engine->LoadMesh ("Sphere.x");

    /**** Scene set up ****/
    // camera set up
    ICamera* p_Cam = gp_Engine->CreateCamera (kFPS, 0.0f, 0.0f, -1000.0f);
    p_Cam->SetRotationSpeed (25.0f);
    p_Cam->SetFarClip (std::fmaxf(KRCS::k_GenerationLimitY * 10.0f, 10000.0f));
    // create models
    
    // beams
    std::array<std::pair<IModel*, bool>, KRCS::k_MaxBeams> pr_ModBeams;
    for (int i = 0; i < KRCS::k_MaxBeams; i++)
    {
        pr_ModBeams[i].second = false;
    }
    
    // spheres
    std::array<IModel*, KRCS::k_CircleCount> pr_ModSpheres;
    for (int i = 0; i < KRCS::k_CircleCount; i++)
    {
        pr_ModSpheres[i] = p_MshSphere->CreateModel ();
        pr_ModSpheres[i]->Scale (p_Collision->mr_Circles[i].rad / 20.0f);
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
#ifdef SIMULATION_3D
            pr_ModSpheres[i]->SetPosition (current.locX, current.locY, current.locZ);
#else
            pr_ModSpheres[i]->SetPosition (current.locX, current.locY, 0.0f);
#endif
        }

        // display the lasers
        for (int i = 0; i < KRCS::k_MaxBeams; i++)
        {
            auto& current = p_Collision->mr_Lasers[i];
            // check if the laser is active
            if (current.active)
            {
                // make a model for it if needed
                if (!pr_ModBeams[i].second)
                {
#ifdef SIMULATION_3D
                    pr_ModBeams[i].first = p_MshBeam->CreateModel (current.locX, current.locY, current.locZ);
                    pr_ModBeams[i].first->RotateLocalX (current.angleX);
                    pr_ModBeams[i].first->RotateLocalZ (current.angleZ);
                    pr_ModBeams[i].first->ScaleY (KRCS::k_GenerationLimitY);
                    pr_ModBeams[i].first->ScaleX (0.1f);
                    pr_ModBeams[i].first->ScaleZ (0.1f);
#else
                    pr_ModBeams[i].first = p_MshBeam->CreateModel (current.locX, current.locY, 0.0f);
                    pr_ModBeams[i].first->RotateLocalZ (current.angleZ);
                    pr_ModBeams[i].first->ScaleY (KRCS::k_GenerationLimitY);
#endif
                    pr_ModBeams[i].second = true;
                }

            }
            // otherwise make sure to not keep it's model
            else if (pr_ModBeams[i].second)
            {
                p_MshBeam->RemoveModel (pr_ModBeams[i].first);
                pr_ModBeams[i].second = false;
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
                case KRCS::E_MessageType::E_Info:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffeeeeee);
                    break;
                case KRCS::E_MessageType::E_Warning:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffffff00);
                    break;
                case KRCS::E_MessageType::E_Error:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffff8800);
                    break;
                case KRCS::E_MessageType::E_Crit:
                    p_Font->Draw (current->text, 11, 11 + i * fontSize, 0xff000000);
                    p_Font->Draw (current->text, 10, 10 + i * fontSize, 0xffff0000);
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
