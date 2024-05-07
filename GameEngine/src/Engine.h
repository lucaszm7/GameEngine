#pragma once

// STL
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <exception>

// Dependencies
#include <IMGUI/imgui.h>                // IMGUI (Interface)
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <GL/glew.h>                    // OpenGL Loader (GLEW)
#include <GLFW/glfw3.h>                 // GLFW (Window handler)
#include <STB/stb_image.h>              // STB (Image Loader)
#include <GLM/glm.hpp>                  // GLM (Math Library)
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

// Core
//#include "core/Shader.h"
//#include "core/VertexArray.h"
//#include "core/VertexBuffer.h"
//#include "core/IndexBuffer.h"
//#include "core/VertexBufferLayout.h"
//#include "core/Texture.h"

// Engine
#include "Lines.hpp"
#include "scene.h"
#include "camera.h"
#include "light.h"
// #include "material.h"
// #include "engine/mesh.h"
#include "model.h"
#include "Timer.hpp"
#include "FrameBuffer.hpp"
#include "ShaderManager.h"

// Game Engine Namespace
namespace gen
{
    class GameEngine
    {
    private:
        inline static GLFWwindow* pWindow;
        inline static std::shared_ptr<unsigned int> pScreenWidth;
        inline static std::shared_ptr<unsigned int> pScreenHeight;
        inline static BaseCam* pCamera;
        inline static std::unique_ptr<FrameBuffer> pFrameBuffer;
        inline static bool firstMouse;
        inline static float lastX;
        inline static float lastY;
        inline static Scene_t* m_CurrentScene;
        inline static Menu* m_MainMenu;
        inline static std::unique_ptr<Shader> m_DebugLineShader;
        inline static double deltaTime;
        inline static double lastFrame;

        GameEngine() = delete;
        GameEngine(GameEngine const&) = delete;
        void operator = (GameEngine& const) = delete;

    public:
        static void Start(unsigned int width, unsigned int height);

        static void AddScene(const std::string& name, const std::function<Scene_t* ()>& func);

        static void Run();
        static void ResetEngine();

        static glm::dvec2 GetMousePos();
        static bool IsKeyPressed(int key);
        static bool IsMouseButtonPressed(int button);
        static bool IsMouseButtonReleased(int button);


    private:
        static void processInputs(GLFWwindow* window, double deltaTime);
        static void APIENTRY
            DebugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity,
                GLsizei length, const GLchar* message, const void* userParam);
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        static void ImGuiDockSpace();
        static GLFWwindow* InitGLFW(const char* name, unsigned int width, unsigned int height);
        static void InitGLEW();
        static void InitImGui(GLFWwindow* window);
        static void UpdateImGui();
    };
}
