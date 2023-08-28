
// STL
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION

#include "Engine.h"

// Scenes
#include "scenes/SplineCollDet/SceneSplineCollisionDetection.h"
#include "scenes/Close2GL/SceneClose2GL.h"


int main()
{
    gen::GameEngine::Start(1280, 720);
    
    gen::GameEngine::AddScene("Close 2 GL", []() {return new SceneClose2GL(); });
    gen::GameEngine::AddScene("Spline Coll Det", []() {return new SceneSplineCollisionDetection(); });

    gen::GameEngine::Run();
}
