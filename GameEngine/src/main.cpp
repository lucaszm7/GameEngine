
// STL
#include <iostream>
#include <vector>
#include <string>

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
#include <assimp/Importer.hpp>

// Core
#include "core/Shader.h"
#include "core/VertexArray.h"
#include "core/VertexBuffer.h"
#include "core/IndexBuffer.h"
#include "core/VertexBufferLayout.h"
#include "core/Texture.h"

// Engine
#include "engine/camera.h"
#include "engine/light.h"
#include "engine/material.h"
#include "engine/mesh.h"
#include "engine/model.h"

// Spline Coll Det
#include "spline/SplineModel.hpp"

void processInputs(GLFWwindow* window);
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

static unsigned int screenWidth = 800;
static unsigned int screenHeight = 600;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

bool firstMouse = true;

float lastX = screenWidth / 2;
float lastY = screenHeight / 2;

Camera camera;

int main()
{
    GLFWwindow* window = InitGLFW();
    InitGLEW();
    
    Shader lightingShader    ("resources/shaders/vertex.shader",      "resources/shaders/fragment.shader");
    Shader lightSourceShader ("resources/shaders/light_vertex.shader", "resources/shaders/light_fragment.shader");

    std::vector<float> cubeVertices =
    {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    Model backpack("resources/models/backpack/backpack.obj");

    SplineModel spline;
    LoadSplineModel("resources/models/VolumetricSpline.txt", spline);
    // GenerateSplineMesh(spline);
    GenerateSurface(spline);

    VertexArray lightVAO;
    lightVAO.Bind();
    VertexBuffer lightVBO(&cubeVertices[0], static_cast<unsigned int>(cubeVertices.size() * sizeof(float)), GL_STATIC_DRAW);
    VertexBufferLayout lightVBL;
    lightVBL.Push<float>(3); // positions
    lightVAO.AddBuffer(lightVBO, lightVBL);

    glm::mat4 view;
    glm::mat4 projection;

    DirectionalLight dirLight( {0.0f, 0.0f, 0.0f}, { -0.2f, -1.0f, -0.3f });

    std::vector<PointLight> pointLights =
    {
        { {0.3f, 0.3f, 0.3f}, { 0.7f,  0.2f,  2.0f}},
        { {0.3f, 0.3f, 0.3f}, { 2.3f, -3.3f, -4.0f}},
        { {0.3f, 0.3f, 0.3f}, {-4.0f,  2.0f, -12.0f}},
        { {0.3f, 0.3f, 0.3f}, { 0.0f,  0.0f, -3.0f}}
    };

    SpotLight spotlight({1.0f, 1.0f, 1.0f}, camera.Position, camera.Front);

    InitImGui(window);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInputs(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        view       = camera.GetViewMatrix();
        projection = camera.GetProjectionMatrix((float)screenWidth / (float)screenHeight);

        // Draw Light Source
        lightVAO.Bind();
        lightSourceShader.Bind();
        lightSourceShader.SetUniformMatrix4fv("view", view);
        lightSourceShader.SetUniformMatrix4fv("projection", projection);
        for(const auto& pointLight : pointLights)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLight.position);
            model = glm::scale(model, glm::vec3(0.2f));
            lightSourceShader.SetUniformMatrix4fv("model", model);
            lightSourceShader.SetUniform3f("lightColor", pointLight.GetLightColor());

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightingShader.Bind();

        lightingShader.SetUniformMatrix4fv("view", view);
        lightingShader.SetUniformMatrix4fv("projection", projection);

        lightingShader.SetUniform3f("viewPos", camera.Position);

        // Lights Sources
        lightingShader.SetUniformLight(dirLight);
        spotlight.position = camera.Position;
        spotlight.direction = camera.Front;
        lightingShader.SetUniformLight(spotlight);
        lightingShader.SetUniformLight(pointLights);

        backpack.Draw(lightingShader);
        spline.Draw(lightingShader);

        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS: %.2f - Elapsed Time %.2f ms", 1 / deltaTime, deltaTime * 1000);

            ImGui::ColorEdit3("Dir Light", (float*)&dirLight.lightColor);
            dirLight.SetLightColor();
            ImGui::DragFloat3("Dir Light", (float*)&dirLight.direction, 1.0f, -10.0f, 10.0f);

            ImGui::ColorEdit3("Spotlight", (float*)&spotlight.lightColor);
            spotlight.SetLightColor();
            ImGui::DragFloat3("Spotlight", (float*)&spotlight.direction, 1.0f, -10.0f, 10.0f);

            for (int i = 0; i < pointLights.size(); ++i)
            {
                ImGui::ColorEdit3(std::to_string(i).c_str(), (float*)&pointLights[i].lightColor);
                pointLights[i].SetLightColor();
                ImGui::DragFloat3(std::to_string(i).c_str(), (float*)&pointLights[i].position, 1.0f, -10.0f, 10.0f);
            }
        }
        ImGui::End();

        UpdateImGui();

        glfwSwapBuffers(window);
        glfwPollEvents();
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
    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
}

void processInputs(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CamMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CamMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CamMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CamMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(CamMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(CamMovement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) 
        return;

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

    camera.ProcessMouseMovement(xOffSet, yOffSet);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
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
