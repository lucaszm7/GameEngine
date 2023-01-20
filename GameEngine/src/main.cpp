
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


// Core
#include "core/Shader.h"
#include "core/VertexArray.h"
#include "core/VertexBuffer.h"
#include "core/IndexBuffer.h"
#include "core/VertexBufferLayout.h"
#include "core/Texture.h"

// Engine
#include "engine/camera.h"

void processInputs(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY DebugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);

float smilePercentage = 0.2f;
bool isPressedUp = false;
bool isPressedDown = false;

static unsigned int screenWidth = 800;
static unsigned int screenHeight = 600;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

bool firstMouse = true;

float lastX = screenWidth / 2;
float lastY = screenHeight / 2;

Camera camera;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(0.5f, 1.0f, 0.5f);

int main()
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
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        std::cin.get();
        return -1;
    }
    std::cout << "\033[1;32m" << glGetString(GL_VERSION) << "\033[0m" << std::endl;
    glDebugMessageCallback(DebugCallBack, nullptr);

    Shader shader     ("resources/shaders/vertex.shader",       "resources/shaders/fragment.shader");
    Shader lightShader("resources/shaders/light_vertex.shader", "resources/shaders/light_fragment.shader");

    std::vector<float> vertices = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

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

    std::vector<unsigned int> indices = 
    {
        0, 1, 2,
        0, 2, 3
    };

    VertexArray VAO;
    VAO.Bind();
    VertexBuffer VBO(&vertices[0], static_cast<unsigned int>(vertices.size() * sizeof(float)), GL_STATIC_DRAW);
    VBO.Bind();
    IndexBuffer EBO(&indices[0], static_cast<unsigned int>(indices.size() * sizeof(unsigned int)), GL_STATIC_DRAW);
    EBO.Bind();
    VertexBufferLayout VBL;
    VBL.Push<float>(3); // positions
    VBL.Push<float>(3); // normals
    VAO.AddBuffer(VBO, VBL);

    VBO.Unbind();
    VAO.Unbind();

    VertexArray lightVAO;
    lightVAO.Bind();
    VertexBuffer lightVBO(&cubeVertices[0], static_cast<unsigned int>(cubeVertices.size() * sizeof(float)), GL_STATIC_DRAW);
    VertexBufferLayout lightVBL;
    lightVBL.Push<float>(3); // positions
    lightVAO.AddBuffer(lightVBO, lightVBL);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::mat4 view;
    glm::mat4 projection;
    
    // Setup ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440 core");
    ImGui::StyleColorsDark();

    // Our state
    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInputs(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view       = camera.GetViewMatrix();
        projection = camera.GetProjectionMatrix((float)screenWidth / (float)screenHeight);
        
        glm::vec3 rotatedLightPos = glm::vec3(
            lightPos.x,
            lightPos.y + (10 * sin((glm::pi<float>() / 180) * (float)glfwGetTime() * 100)),
            lightPos.z + (10 * cos((glm::pi<float>() / 180) * (float)glfwGetTime() * 100))
        );

        // Draw Light Source
        {
            lightVAO.Bind();
            lightShader.Bind();

            lightShader.SetUniformMatrix4fv("view", view);
            lightShader.SetUniformMatrix4fv("projection", projection);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, rotatedLightPos);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.SetUniformMatrix4fv("model", model);

            lightShader.SetUniform3f("lightColor", lightColor);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        shader.Bind();
        shader.SetUniformMatrix4fv("view", view);
        shader.SetUniformMatrix4fv("projection", projection);
        
        shader.SetUniform3f("objectColor", 1.0f, 0.5f, 0.31f);
        shader.SetUniform3f("lightColor",  lightColor);
        shader.SetUniform3f("lightPos", rotatedLightPos);
        shader.SetUniform3f("viewPos", camera.Position);

        VAO.Bind();

        for (int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            // model = glm::rotate(model, (glm::pi<float>() / 4) * (i + 1)/2 * (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetUniformMatrix4fv("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

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
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        std::cout << "PRESSED\n";
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_UP && action == GLFW_PRESS && !isPressedUp)
        isPressedUp = true;
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE && isPressedUp)
    {
        isPressedUp = false;
        if (smilePercentage < 1.0f) smilePercentage += 0.2f;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && !isPressedDown)
        isPressedDown = true;
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE && isPressedDown)
    {
        isPressedDown = false;
        if (smilePercentage > -1.0f) smilePercentage -= 0.2f;
    }
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

    camera.ProcessMouseMovement(xOffSet, yOffSet);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
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