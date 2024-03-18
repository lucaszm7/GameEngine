
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
#include "scenes/ASCIIMAGE/ASCIIMAGE.h"
#include "scenes/ComputeShader/ComputeShaderDemo.h"


int main()
{
    gen::GameEngine::Start(1280, 720);
    
    gen::GameEngine::AddScene("Close 2 GL", []() {return new SceneClose2GL(); });
    gen::GameEngine::AddScene("Solitude", []() {return new SolitudeScene(); });
    gen::GameEngine::AddScene("ASCII to Image", []() {return new ASCIIMAGE(); });
    gen::GameEngine::AddScene("Compute Shader", []() {return new ComputeShaderDemo(); });

    gen::GameEngine::Run();
}
