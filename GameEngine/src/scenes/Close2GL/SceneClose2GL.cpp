#include "SceneClose2GL.h"
#include "Lines.hpp"

struct BoundingVolume
{
    glm::vec3 max{ 0.f,0.f,0.f };
    glm::vec3 min{ 0.f,0.f,0.f };
};

BoundingVolume CalculateEnclosingAABB(const std::unique_ptr<Model>& obj)
{
    BoundingVolume r;
    for (const auto& mesh : obj->meshes)
    {
        for (const auto& vertice : mesh.vertices)
        {
            if (vertice.Position.x < r.min.x)
                r.min.x = vertice.Position.x;
            if (vertice.Position.y < r.min.y)
                r.min.y = vertice.Position.y;
            if (vertice.Position.z < r.min.z)
                r.min.z = vertice.Position.z;

            if (vertice.Position.x > r.max.x)
                r.max.x = vertice.Position.x;
            if (vertice.Position.y > r.max.y)
                r.max.y = vertice.Position.y;
            if (vertice.Position.z > r.max.z)
                r.max.z = vertice.Position.z;
        }
    }
    return r;
}

SceneClose2GL::SceneClose2GL()
    :
    OpenGLShader("resources/shaders/ogl_vertex.shader", "resources/shaders/ogl_fragment.shader"),
    screenWidth(pScreenWidth), screenHeight(pScreenHeight),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, oglCamera.Position, oglCamera.Right)
{
    objects.emplace_back(std::make_unique<Model>("resources/models/cube_text.in", glm::vec3{ 1.0f,1.0f,0.0f }));
    DisableCullFace();

    VertexShadingGouraudIndex = OpenGLShader.GetSubroutineIndex(ShaderStage::VERTEX, "Gouraud");
    VertexShadingPhongIndex = OpenGLShader.GetSubroutineIndex(ShaderStage::VERTEX, "Phong");

    FragmentShadingGouraudIndex = OpenGLShader.GetSubroutineIndex(ShaderStage::FRAGMENT, "Gouraud");
    FragmentShadingPhongIndex = OpenGLShader.GetSubroutineIndex(ShaderStage::FRAGMENT, "Phong");

    FragmentColoringSolidIndex = OpenGLShader.GetSubroutineIndex(ShaderStage::FRAGMENT, "SolidColor");
    FragmentColoringTextureIndex = OpenGLShader.GetSubroutineIndex(ShaderStage::FRAGMENT, "TextureColor");
}

SceneClose2GL::~SceneClose2GL() = default;

void SceneClose2GL::OnUpdate(float deltaTime)
{
    if (isOpenGLRendered)
    {
        glm::vec3 lookAtLocation = !objects.empty() ? objects[selectedLookAt]->transform.position : glm::vec3();
        isLookAt ? oglCamera.SetLookAt(lookAtLocation) : oglCamera.UnSetLookAt();
        view = oglCamera.GetViewMatrix();
        projection = oglCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);
    
        OpenGLShader.Bind();
        OpenGLShader.SetUniformMatrix4fv("view", view);
        OpenGLShader.SetUniformMatrix4fv("projection", projection);
        OpenGLShader.SetUniform3f("viewPos", oglCamera.Position);

        spotlight.position = glm::vec3(oglCamera.Position.x, oglCamera.Position.y, oglCamera.Position.z);
        spotlight.direction = glm::vec3(oglCamera.Front.x, oglCamera.Front.y, oglCamera.Front.z);
        
        std::array<unsigned int, 2> fragmentSubroutineIndex;
        fragmentSubroutineIndex[0] = (isGouraudShading ? FragmentShadingGouraudIndex : FragmentShadingPhongIndex);
        fragmentSubroutineIndex[1] = (showTexture ? FragmentColoringTextureIndex : FragmentColoringSolidIndex);
        OpenGLShader.SetUniformSubroutine(ShaderStage::FRAGMENT, 2, fragmentSubroutineIndex.data());
        
        if (isGouraudShading)
        {
            OpenGLShader.SetUniformSubroutine(ShaderStage::VERTEX, 1, &VertexShadingGouraudIndex);
            OpenGLShader.SetUniformLight(dirLight, ShaderStage::VERTEX);
            OpenGLShader.SetUniformLight(spotlight, ShaderStage::VERTEX);
        }
        else
        {
            OpenGLShader.SetUniformSubroutine(ShaderStage::VERTEX, 1, &VertexShadingPhongIndex);
            OpenGLShader.SetUniformLight(dirLight, ShaderStage::FRAGMENT);
            OpenGLShader.SetUniformLight(spotlight, ShaderStage::FRAGMENT);
        }

        for (int i = 0; i < objects.size(); ++i)
        {
            objects[i]->Draw(OpenGLShader, drawPrimitive);
        }
    }
    else
    {
        Rasterizer::SetViewPort(*screenWidth, *screenHeight);
        Pixel clearColor{ (unsigned char)(imguiClearColor[0] * 255), (unsigned char)(imguiClearColor[1] * 255), (unsigned char)(imguiClearColor[2] * 255) };
        Rasterizer::SetClearColor(clearColor);
        Rasterizer::ClearFrameBuffer();
        Rasterizer::ClearZBuffer();

        cgl::vec3 lookAtLocation = !objects.empty() ? objects[selectedLookAt]->transform.position : cgl::vec3();
        isLookAt ? cglCamera.SetLookAt(lookAtLocation) : cglCamera.UnSetLookAt();

        for (const auto& object : objects)
        {
            Rasterizer::DrawSoftwareRasterized(*object, cglCamera, rasterizerViewPort, *screenWidth, *screenHeight,
                drawPrimitive, isEnableCullFace, isCullingClockWise);
        }
    }

    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 1000,0,0 }, glm::vec3{1.0f, 0.0f, 0.0f});
    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 0,1000,0 }, glm::vec3{0.0f, 1.0f, 0.0f});
    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 0,0,1000 }, glm::vec3{0.0f, 0.0f, 1.0f});
}

void SceneClose2GL::OnImGuiRender()
{
    ImGui::Text("Render API");

    if (ImGui::RadioButton("Close 2 GL", !isOpenGLRendered))
    { 
        isOpenGLRendered = false;
        cglCamera.Position = oglCamera.Position;
        cglCamera.Yaw = oglCamera.Yaw;
        cglCamera.Pitch = oglCamera.Pitch;
        cglCamera.MovementSpeed = oglCamera.MovementSpeed;
        cglCamera.updateCameraVectors();
    } 
    ImGui::SameLine();
    if (ImGui::RadioButton("OpenGL",     isOpenGLRendered))
    { 
        isOpenGLRendered = true;
        oglCamera.Position = glm::vec3(cglCamera.Position.x, cglCamera.Position.y, cglCamera.Position.z);
        oglCamera.Yaw = cglCamera.Yaw;
        oglCamera.Pitch = cglCamera.Pitch;
        oglCamera.MovementSpeed = cglCamera.MovementSpeed;
        oglCamera.updateCameraVectors();
    }

    if (!isOpenGLRendered)
    {
        ImGui::ColorEdit3(std::string("Close2GL Clear Color").c_str(), imguiClearColor);
    }

    ImGui::Text("Drawing Primitive");
    if (ImGui::RadioButton("Triangle", drawPrimitive == DrawPrimitive::Triangle))
    {
        drawPrimitive = DrawPrimitive::Triangle;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Point", drawPrimitive == DrawPrimitive::Point))
    {
        drawPrimitive = DrawPrimitive::Point;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("WireFrame", drawPrimitive == DrawPrimitive::WireFrame))
    {
        drawPrimitive = DrawPrimitive::WireFrame;
    }

    ImGui::Text("Shading Model");
    if (ImGui::RadioButton("Gouraud Shading", isGouraudShading))
    {
        isGouraudShading = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Phong Shading", !isGouraudShading))
    {
        isGouraudShading = false;
    }

    ImGui::Checkbox("Show Textures", &showTexture);

    ImGui::Separator();
    if (ImGui::Checkbox("Culling BackFace", &isEnableCullFace))
    {
        if (isEnableCullFace && isOpenGLRendered)
            EnableCullFace();
        else
            DisableCullFace();
    }
    if (isEnableCullFace)
    {
        if (ImGui::RadioButton("Culling Clock Wise", isCullingClockWise))
        {
            isCullingClockWise = true;
            isOpenGLRendered ? EnableCullFace() : DisableCullFace();
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Culling Counter Clock Wise", !isCullingClockWise))
        {
            isCullingClockWise = false;
            isOpenGLRendered ? EnableCullFace() : DisableCullFace();
        }
    }

    const char* possibleObjects[]{ "COW", "CUBE", "BACKPACK", "TEAPOT", "DRAGON"};
    if(ImGui::Combo("Object to Add", &selectedObjectToAdd, possibleObjects, 5))
        AddObject(std::string(possibleObjects[selectedObjectToAdd]));

    if (ImGui::RadioButton("Load Clock Wise", isLoadingClockWise))
    {
        isLoadingClockWise = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Load Counter Clock Wise", !isLoadingClockWise))
    {
        isLoadingClockWise = false;
    }

    ImGui::Checkbox("Look At", &isLookAt);

    ImGui::Separator();
    int i = 0;
    for (auto it = objects.begin(); it != objects.end();)
    {
        if (isLookAt && ImGui::RadioButton(("LookAt " + (*it)->name).c_str(), selectedLookAt == i))
        {
            selectedLookAt = i;
            ImGui::SameLine();
        }

        if (ImGui::TreeNode(std::string((*it)->name).c_str()))
        {
            (*it)->OnImGui();
            ImGui::ColorEdit3(std::string("Color of" + (*it)->name).c_str(), &(*it)->color[0]);
            ImGui::TreePop();
        }
        else
            ImGui::SameLine();

        if (ImGui::Button(std::string("Delete " + (*it)->name).c_str()))
        {
            it = objects.erase(it);
        }
        else
        {
            it++;
            i++;
        }
    }
}

void SceneClose2GL::AddObject(std::string_view label)
{
    TriangleOrientation tri;
    if (isLoadingClockWise)
        tri = TriangleOrientation::ClockWise;
    else
        tri = TriangleOrientation::CounterClockWise;

    glm::vec3 color(cgl::random_double(0, 1), cgl::random_double(0, 1), cgl::random_double(0, 1));

    if (label == "COW")
        objects.emplace_back(std::make_unique<Model>("resources/models/cow_up_no_text.in", color, tri));
    else if (label == "CUBE")
        objects.emplace_back(std::make_unique<Model>("resources/models/cube_text.in", color, tri));
    else if (label == "BACKPACK")
        objects.emplace_back(std::make_unique<Model>("resources/models/backpack/backpack.obj", color, tri));
    else if (label == "TEAPOT")
        objects.emplace_back(std::make_unique<Model>("resources/models/teapot.obj", color, tri));
    else if (label == "DRAGON")
        objects.emplace_back(std::make_unique<Model>("resources/models/dragon.obj", color, tri));

    // Calculate AABB
    BoundingVolume aabb = CalculateEnclosingAABB(objects.back());

    float aspectRatio = (float)*pScreenWidth / (float)*pScreenHeight;

    if (isOpenGLRendered)
    {
        oglCamera.Reset();
        float OPP = (aabb.max.y + aabb.min.y) / 2;
        float OPPX = (aabb.max.x + aabb.min.x) / 2;
        oglCamera.Position.x = (aabb.max.x + aabb.min.x) / 2;
        oglCamera.Position.y = (aabb.max.y + aabb.min.y) / 2;
        float TAN = glm::tan(glm::radians(oglCamera.Zoom / 2));
        float yBig = (aabb.max.z + ((aabb.max.y - OPP)  / TAN));
        float xBig = (aabb.max.z + ((aabb.max.x - OPPX) / glm::tan(glm::radians((oglCamera.Zoom * aspectRatio) / 2))));
        oglCamera.Position.z = (xBig > yBig) ? xBig : yBig;
    }
    else
    {
        cglCamera.Reset();
        float OPP = (aabb.max.y + aabb.min.y) / 2;
        float OPPX = (aabb.max.x + aabb.min.x) / 2;
        cglCamera.Position.x = (aabb.max.x + aabb.min.x) / 2;
        cglCamera.Position.y = (aabb.max.y + aabb.min.y) / 2;
        float TAN = glm::tan(glm::radians(cglCamera.Zoom / 2));
        float yBig = (aabb.max.z + ((aabb.max.y - OPP) / TAN));
        float xBig = (aabb.max.z + ((aabb.max.x - OPPX) / glm::tan(glm::radians((cglCamera.Zoom * aspectRatio) / 2))));
        cglCamera.Position.z = (xBig > yBig) ? xBig : yBig;
    }
}

void SceneClose2GL::EnableCullFace()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(isCullingClockWise ? GL_CW : GL_CCW);
}

void SceneClose2GL::DisableCullFace()
{
    glDisable(GL_CULL_FACE);
}
