// GamesEngineeringAssignmentTLX.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

/** KeyCodes **/
// Game management
const EKeyCode kg_KeyQuit = Key_Escape; // quit button


void main()
{
    /**** Initialization ****/
    // Create a 3D engine (using TLX engine here) and open a window for it
    I3DEngine* p_Engine = New3DEngine (kTLX);
    p_Engine->StartWindowed ();

    // Add media folders
    p_Engine->AddMediaFolder ("C:\\ProgramData\\TL-Engine\\Media");

    /**** Scene set up ****/
    // load meshes
    IMesh* p_MshFloor = p_Engine->LoadMesh ("Floor.x"); // mesh of the floor

                                                        // create models

                                                        // camera set up
    ICamera* p_Cam = p_Engine->CreateCamera (kFPS, 0.0f, 0.0f, 0.0f);


    /**** Game Loop ****/
    while (p_Engine->IsRunning ())
    {
        // Draw the scene
        p_Engine->DrawScene ();

        // stop the game
        if (p_Engine->KeyHit (kg_KeyQuit))
        {
            p_Engine->Stop ();
        }
    }

    /**** Clean up ****/

    // Delete the 3D engine now we are finished with it
    p_Engine->Delete ();
}
