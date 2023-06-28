#include "SceneClose2GL.h"
#include "Lines.hpp"

struct BoundingVolume
{
    glm::vec3 max{ 0.f,0.f,0.f };
    glm::vec3 min{ 0.f,0.f,0.f };
};

BoundingVolume CalculateEnclosingAABB(const Model* obj)
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
    cubeModel("resources/models/cube_text.in"),
    lightingShader("resources/shaders/assignament1_vertex.shader", "resources/shaders/assignament1_fragment.shader"),
    screenWidth(pScreenWidth), screenHeight(pScreenHeight),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, oglCamera.Position, oglCamera.Right)
{
    objects.push_back(&cubeModel);
    colors.emplace_back(1.0f, 1.0f, 0.0f);
}

SceneClose2GL::~SceneClose2GL() = default;

void SceneClose2GL::OnUpdate(float deltaTime)
{
    if (isOpenGLRendered)
    {
        view = oglCamera.GetViewMatrix(isLookAt ? &objects[selectedLookAt]->transform.position : nullptr);
        projection = oglCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);
    }
    else
    {
        cgl::vec3 lookAtLocation = (objects[selectedLookAt]->transform.position);
        view = cglCamera.GetViewMatrix(isLookAt ? &lookAtLocation : nullptr);
        projection = cglCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);
    }

    lightingShader.Bind();
    lightingShader.SetUniformMatrix4fv("view", view);
    lightingShader.SetUniformMatrix4fv("projection", projection);
    lightingShader.SetUniform3f("viewPos", cglCamera.Position);
    // Lights Sources
    lightingShader.SetUniformLight(dirLight);
    spotlight.position = glm::vec3(cglCamera.Position.x, cglCamera.Position.y, cglCamera.Position.z);
    spotlight.direction = glm::vec3(cglCamera.Front.x, cglCamera.Front.y, cglCamera.Front.z);
    lightingShader.SetUniformLight(spotlight);

    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 10,0,0 });
    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 0,10,0 });
    Debug::Line::Draw(glm::vec3{ 0,0,0 }, glm::vec3{ 0,0,10 });

    for (int i = 0; i < objects.size(); ++i)
    {
        lightingShader.SetUniform3f("uColor", colors[i]);
        objects[i]->Draw(lightingShader);
    }
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
    for (int i = 0; i < objects.size(); ++i)
    {
        if (isLookAt && ImGui::RadioButton(("LookAt " + objects[i]->name).c_str(), selectedLookAt == i))
        {
            selectedLookAt = i;
            ImGui::SameLine();
        }

        if (ImGui::TreeNode(std::string(objects[i]->name).c_str()))
        {
            objects[i]->OnImGui();
            ImGui::ColorEdit3(std::string("Color of" + objects[i]->name).c_str(), &colors[i][0]);
            ImGui::TreePop();
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
        Model* newModel = new Model("resources/models/cow_up_no_text.in", tri);
        objects.push_back(newModel);
    }
    else if (label == "CUBE")
    {
        Model* newModel = new Model("resources/models/cube_text.in", tri);
        objects.push_back(newModel);
    }
    else if (label == "BACKPACK")
    {
        Model* newModel = new Model("resources/models/backpack/backpack.obj", tri);
        objects.push_back(newModel);
    }

    colors.emplace_back(cgl::random_double(0, 1), cgl::random_double(0, 1), cgl::random_double(0, 1));

    // Calculate AABB
    BoundingVolume aabb = CalculateEnclosingAABB(objects.back());

    auto height = aabb.max.y;

    auto width = aabb.max.x - aabb.min.x;
    auto vertical_FOV = cglCamera.Zoom * (3.1415936 / 180);

    auto max_z = aabb.max.z;

    auto horizontal_FOV = 2 * glm::atan(glm::tan(vertical_FOV / 2) * (*pScreenWidth / *pScreenHeight));

    auto distance_vertical = height / (2 * glm::tan(vertical_FOV / 2));
    // alert ('vertical' + distance_vertical);
    auto distance_horizontal = width / (2 * glm::tan(horizontal_FOV / 2));
    // alert ('horizontal' + distance_horizontal);
    auto z_distance = distance_vertical >= distance_horizontal ? distance_vertical : distance_horizontal;

    

    cglCamera.Position.z = z_distance + max_z;
    cglCamera.Position.y = (aabb.max.y - aabb.min.y) / 2;
    cglCamera.Position.x = width/2;

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
