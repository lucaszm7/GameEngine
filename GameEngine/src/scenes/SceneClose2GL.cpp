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
    OpenGLShader("resources/shaders/assignament1_vertex.shader", "resources/shaders/assignament1_fragment.shader"),
    Close2GLShader("resources/shaders/cgl_vertex.shader", "resources/shaders/cgl_fragment.shader"),
    screenWidth(pScreenWidth), screenHeight(pScreenHeight),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, oglCamera.Position, oglCamera.Right)
{
    objects.emplace_back(std::make_unique<Model>("resources/models/cube_text.in"));
    colors.emplace_back(1.0f, 1.0f, 0.0f);
}

SceneClose2GL::~SceneClose2GL() = default;

void SceneClose2GL::OnUpdate(float deltaTime)
{
    if (isOpenGLRendered)
    {
        view = oglCamera.GetViewMatrix(isLookAt && objects.size() > 0 ? &objects[selectedLookAt]->transform.position : nullptr);
        projection = oglCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);
    
        OpenGLShader.Bind();
        OpenGLShader.SetUniformMatrix4fv("view", view);
        OpenGLShader.SetUniformMatrix4fv("projection", projection);
        OpenGLShader.SetUniform3f("viewPos", cglCamera.Position);
        // Lights Sources
        OpenGLShader.SetUniformLight(dirLight);
        spotlight.position = glm::vec3(cglCamera.Position.x, cglCamera.Position.y, cglCamera.Position.z);
        spotlight.direction = glm::vec3(cglCamera.Front.x, cglCamera.Front.y, cglCamera.Front.z);
        OpenGLShader.SetUniformLight(spotlight);

        for (int i = 0; i < objects.size(); ++i)
        {
            OpenGLShader.SetUniform3f("uColor", colors[i]);
            objects[i]->DrawOpenGL(OpenGLShader, drawPrimitive);
        }
    }
    else
    {
        cgl::vec3 lookAtLocation = objects.size() > 0 ? (objects[selectedLookAt]->transform.position) : cgl::vec3();
        view = cglCamera.GetViewMatrix(isLookAt && objects.size() > 0 ? &lookAtLocation : nullptr);
        projection = cglCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);

        Close2GLShader.Bind();
        Close2GLShader.SetUniform3f("viewPos", cglCamera.Position);
        // Lights Sources
        Close2GLShader.SetUniformLight(dirLight);
        spotlight.position = glm::vec3(cglCamera.Position.x, cglCamera.Position.y, cglCamera.Position.z);
        spotlight.direction = glm::vec3(cglCamera.Front.x, cglCamera.Front.y, cglCamera.Front.z);
        Close2GLShader.SetUniformLight(spotlight);

        for (int i = 0; i < objects.size(); ++i)
        {
            Close2GLShader.SetUniform3f("uColor", colors[i]);
            objects[i]->DrawCGL(Close2GLShader, drawPrimitive, view, projection);
        }
    }


    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 1000,0,0 });
    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 0,1000,0 });
    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 0,0,1000 });
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
        cglCamera.updateCameraVectors();
    } 
    ImGui::SameLine();
    if (ImGui::RadioButton("OpenGL",     isOpenGLRendered))
    { 
        isOpenGLRendered = true;
        oglCamera.Position = glm::vec3(cglCamera.Position.x, cglCamera.Position.y, cglCamera.Position.z);
        oglCamera.Yaw = cglCamera.Yaw;
        oglCamera.Pitch = cglCamera.Pitch;
        oglCamera.updateCameraVectors();
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

    ImGui::Separator();
    if (ImGui::Checkbox("Culling BackFace", &isEnableCullFace))
    {
        if (isEnableCullFace)
            EnableCullFace();
        else
            DisableCullFace();
    }
    if (isEnableCullFace)
    {
        if (ImGui::RadioButton("Culling Clock Wise", isCullingClockWise))
        {
            isCullingClockWise = true;
            EnableCullFace();
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Culling Counter Clock Wise", !isCullingClockWise))
        {
            isCullingClockWise = false;
            EnableCullFace();
        }
    }

    const char* possibleObjects[]{ "COW", "CUBE", "BACKPACK" };
    if(ImGui::Combo("Object to Add", &selectedObjectToAdd, possibleObjects, 3))
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
            ImGui::ColorEdit3(std::string("Color of" + (*it)->name).c_str(), &colors[i][0]);
            ImGui::TreePop();
        }
        else
            ImGui::SameLine();

        if (ImGui::Button(std::string("Delete " + (*it)->name).c_str()))
        {
            it = objects.erase(it);
            colors.erase(colors.begin() + i);
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

    if (label == "COW")
    {
        objects.emplace_back(std::make_unique<Model>("resources/models/cow_up_no_text.in", tri));
    }
    else if (label == "CUBE")
    {
        objects.emplace_back(std::make_unique<Model>("resources/models/cube_text.in", tri));

    }
    else if (label == "BACKPACK")
    {
        objects.emplace_back(std::make_unique<Model>("resources/models/backpack/backpack.obj", tri));

    }

    colors.emplace_back(cgl::random_double(0, 1), cgl::random_double(0, 1), cgl::random_double(0, 1));

    // Calculate AABB
    BoundingVolume aabb = CalculateEnclosingAABB(objects.back());

    cglCamera.Reset();
    cglCamera.Position.z = (aabb.max.y - aabb.min.y) / 2;
    cglCamera.Position.y = (aabb.max.y - aabb.min.y) / 2;
    cglCamera.Position.x = (aabb.max.y - aabb.min.y) / 2;
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
