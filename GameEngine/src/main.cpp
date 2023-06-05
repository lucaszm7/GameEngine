
// STL
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Dependencies
#include <IMGUI/imgui.h>                // IMGUI (Interface)
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <GL/glew.h>                    // OpenGL Loader (GLEW)
#include <GLFW/glfw3.h>                 // GLFW (Window handler)
#define STB_IMAGE_IMPLEMENTATION
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
#include "scene.h"
#include "camera.h"
#include "light.h"
// #include "material.h"
#include "line.h"
// #include "engine/mesh.h"
#include "model.h"
#include "Timer.hpp"

// Scenes
#include "scenes/SceneSplineCollisionDetection.h"


void processInputs(GLFWwindow* window, double deltaTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY DebugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
GLFWwindow* InitGLFW();
void InitGLEW();
void InitImGui(GLFWwindow* window);
void UpdateImGui();


std::shared_ptr<unsigned int> pScreenWidth;
std::shared_ptr<unsigned int> pScreenHeight;

std::shared_ptr<Camera> pCamera;

static bool firstMouse = true;
static float lastX = 0.0f;
static float lastY = 0.0f;


int main()
{
    // CreateCilinderSpline("resources/models/cilinder.txt", 10, 32, 0.05);
    // CreateCilinderSpline("resources/models/littleCilinder.txt", 10, 32, 0.01);

    GLFWwindow* window = InitGLFW();
    InitGLEW();
    
    InitImGui(window);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    pScreenWidth = std::make_shared<unsigned int>(800);
    pScreenHeight = std::make_shared<unsigned int>(600);

    pCamera = std::make_shared<Camera>();

    Scene_t* m_CurrentScene = nullptr;
    Menu* m_MainMenu = new Menu(m_CurrentScene);
    m_CurrentScene = m_MainMenu;

    m_MainMenu->pScreenWidth = pScreenWidth;
    m_MainMenu->pScreenHeight = pScreenHeight;
    m_MainMenu->pCamera = pCamera;

    m_MainMenu->RegisterApp<SceneSplineCollisionDetection>("Spline Collision Detection");

    double deltaTime = 0.0f;
    double lastFrame = 0.0f;

    try
    {
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInputs(window, deltaTime);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        ImGui::Begin(m_MainMenu->c_SceneName.c_str());
        
        bool ResetToMainMenu = m_CurrentScene != m_MainMenu && ImGui::Button("<- Main Menu");
            ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.0f, 1.0f), "FPS: %.2f - Take %.2f ms", 1 / deltaTime, deltaTime * 1000);
            ImGui::Separator();

        if (ResetToMainMenu)
        {
            delete m_CurrentScene;
            m_CurrentScene = m_MainMenu;
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glfwSetWindowTitle(window, m_MainMenu->c_SceneName.c_str());
        }
        
        m_CurrentScene->OnUpdate(deltaTime);
        m_CurrentScene->OnImGuiRender();

        ImGui::End();
        UpdateImGui();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    *pScreenHeight = height;
    *pScreenWidth = width;
    glViewport(0, 0, width, height);
}

void processInputs(GLFWwindow* window, double deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pCamera->ProcessKeyboard(CamMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pCamera->ProcessKeyboard(CamMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pCamera->ProcessKeyboard(CamMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pCamera->ProcessKeyboard(CamMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        pCamera->ProcessKeyboard(CamMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        pCamera->ProcessKeyboard(CamMovement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffSet = xpos - lastX;
    float yOffSet = lastY - ypos; // reverse since opengl has the Y component inverted

    lastX = xpos;
    lastY = ypos;
    
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) 
        return;

    pCamera->ProcessMouseMovement(xOffSet, yOffSet);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    pCamera->ProcessMouseScroll((float)yoffset);
}

GLFWwindow* InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    // Tells OpenGL we want the core-profile (the good one, with just the newer stuff)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GameEngine", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::cin.get();
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return window;
}

void InitGLEW()
{
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        std::cin.get();
    }
    std::cout << "\033[1;32m" << glGetString(GL_VERSION) << "\033[0m" << std::endl;
    glDebugMessageCallback(DebugCallBack, nullptr);
}

void InitImGui(GLFWwindow* window)
{
    // Setup ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440 core");
    ImGui::StyleColorsDark();
}

void UpdateImGui()
{
    const ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void APIENTRY
DebugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
#ifndef DEBUG || DEBINFO
    if (severity != GL_DEBUG_SEVERITY_MEDIUM && severity != GL_DEBUG_SEVERITY_HIGH) return;
#endif
    std::cout << "\n\033[1;31m ------------ DEBUG ------------ \033[0m\n" << std::endl;
    std::cout << "Message: \033[1;33m" << message << "]\033[0m\n" << std::endl;
    std::cout << "Type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "OTHER";
        break;
    default:
        std::cout << type;
        break;
    }
    std::cout << std::endl;

    std::cout << "ID: " << id << std::endl;
    std::cout << "Severity: ";
    switch (severity) {
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "HIGH";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Generic Notification";
        break;
    default:
        std::cout << severity;
        break;
    }
    std::cout << std::endl;
    std::cout << "\n\033[1;31m ------------ END OF DEBUG ------------ \033[0m\n" << std::endl;
}
