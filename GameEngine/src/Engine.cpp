#include "Engine.h"

void gen::GameEngine::Start(unsigned int width, unsigned int height)
{
    pScreenWidth = std::make_shared<unsigned int>(width);
    pScreenHeight = std::make_shared<unsigned int>(height);

    pWindow = InitGLFW("Game Engine", *pScreenWidth, *pScreenHeight);

    InitGLEW();
    InitImGui(pWindow);

    ResetEngine();

    pFrameBuffer = std::make_unique<FrameBuffer>(pScreenWidth, pScreenHeight, FrameBuffer::Type::MULTISAMPLE);

    m_CurrentScene = nullptr;
    m_MainMenu = new Menu(m_CurrentScene);
    m_CurrentScene = m_MainMenu;

    m_DebugLineShader = std::make_unique<Shader>("../GameEngine/resources/shader/Lines_vertex.shader", "../GameEngine/resources/shader/Lines_fragment.shader");

    Scene_t::pScreenWidth = pScreenWidth;
    Scene_t::pScreenHeight = pScreenHeight;

    deltaTime = 0.0f;
    lastFrame = 0.0f;

    lastX = 0.0f;
    lastY = 0.0f;

    firstMouse = true;
}

void gen::GameEngine::AddScene(const std::string& name, const std::function<Scene_t* ()>& func)
{
    m_MainMenu->RegisterApp(name, func);
}

void gen::GameEngine::Run()
{
    while (!glfwWindowShouldClose(pWindow))
    {
        if(m_CurrentScene != m_MainMenu)
            processInputs(pWindow, deltaTime);

        pFrameBuffer->Bind();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        pCamera = m_CurrentScene->GetCamera();

        ImGuiDockSpace();

        ImGui::Begin(std::string(m_MainMenu->c_SceneName + " | Scene").c_str());
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.1f, 0.1f, 1.0f)); // Menu bar background color
            bool ResetToMainMenu = m_CurrentScene != m_MainMenu && ImGui::Button("<- Main Menu");
            ImGui::PopStyleColor(1);

            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.51f, 0.82f, 0.345f, 1.0f), "FPS: %.2f\tTake %.2f ms", 1 / deltaTime, deltaTime * 1000);
            ImGui::TextColored(ImVec4(0.0f, 0.82f, 0.0f, 1.0f), "MSAA 4x - Active");
            ImGui::Separator();
            pCamera->OnImGui();
            ImGui::Separator();

            if (ResetToMainMenu)
            {
                delete m_CurrentScene;
                m_CurrentScene = m_MainMenu;
                pCamera = m_CurrentScene->GetCamera();
                glfwSetWindowTitle(pWindow, m_MainMenu->c_SceneName.c_str());
                ResetEngine();
            }

            m_CurrentScene->OnUpdate(deltaTime);
            m_CurrentScene->OnImGuiRender();

#ifdef _DEBUG
            Debug::Line::Draw({ 0.0f,0.0f,0.0f }, { 100.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
            Debug::Line::Draw({ 0.0f,0.0f,0.0f }, { 0.0f, 100.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
            Debug::Line::Draw({ 0.0f,0.0f,0.0f }, { 0.0f, 0.0f, 100.0f }, { 0.0f, 0.0f, 1.0f });
#endif
            Debug::Line& m_LinesDrawer = Debug::Line::OnStart();
            m_LinesDrawer.OnUpdate(*m_DebugLineShader, ogl::Camera(pCamera->GetBaseInfo()), (float)*pScreenWidth / (float)*pScreenHeight);
        }
        ImGui::End();

        pFrameBuffer->Unbind(); // back to default
        pFrameBuffer->OnImGuiRender();

        UpdateImGui();

        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void gen::GameEngine::ResetEngine()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);
}

void gen::GameEngine::processInputs(GLFWwindow* window, double deltaTime)
{
    if(pCamera)
        pCamera->ProcessKeyboard(CamMovement::NONE, deltaTime);
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
}

void APIENTRY
gen::GameEngine::DebugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity,
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

void gen::GameEngine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    *pScreenWidth = width;
    *pScreenHeight = height;
    glViewport(0, 0, width, height);
    pFrameBuffer->Reset();
}

void gen::GameEngine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // glfwSetWindowShouldClose(window, true);
}

void gen::GameEngine::mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

void gen::GameEngine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    pCamera->ProcessMouseScroll((float)yoffset);
}

void gen::GameEngine::ImGuiDockSpace()
{
    // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
    // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
    // In this specific demo, we are not using DockSpaceOverViewport() because:
    // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
    // - we allow the host window to have padding (when opt_padding == true)
    // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
    // TL;DR; this demo is more complicated than what you would normally use.
    // If we removed all the options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }

    static bool dockingSpaceOpen = true;
    static bool showDemoWindow = false;

    if (showDemoWindow)
        ImGui::ShowDemoWindow();

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    static bool isVsync = true;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockingSpaceOpen, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("Vsync"))
            {
                isVsync = !isVsync;
                isVsync ? glfwSwapInterval(1) : glfwSwapInterval(0);
            }

            ImGui::Separator();
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Print Screen", NULL);

            ImGui::Separator();

            if (ImGui::MenuItem("Show Demo Window"))
                showDemoWindow = !showDemoWindow;

            if (ImGui::MenuItem("EXIT"))
                glfwSetWindowShouldClose(pWindow, 1);

            ImGui::Separator();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

GLFWwindow* gen::GameEngine::InitGLFW(const char* name, unsigned int width, unsigned int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    // Tells OpenGL we want the core-profile (the good one, with just the newer stuff)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::cin.get();
    }

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void gen::GameEngine::InitGLEW()
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

void gen::GameEngine::InitImGui(GLFWwindow* window)
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

void gen::GameEngine::UpdateImGui()
{
    const ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
