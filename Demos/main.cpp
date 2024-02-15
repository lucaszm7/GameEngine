
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
#include "scenes/Close2GL/SceneClose2GL.h"
#include "scenes/Solitude/SolitudeScene.h"


int main()
{
    gen::GameEngine::Start(1280, 720);
    
    gen::GameEngine::AddScene("Close 2 GL", []() {return new SceneClose2GL(); });
    gen::GameEngine::AddScene("Solitude", []() {return new SolitudeScene(); });

    gen::GameEngine::Run();
}
