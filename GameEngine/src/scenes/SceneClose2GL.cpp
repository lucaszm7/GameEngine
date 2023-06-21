#include "SceneClose2GL.h"

SceneClose2GL::SceneClose2GL()
    :cubeModel("resources/models/cube_text.in"),
    lightingShader("resources/shaders/assignament1_vertex.shader", "resources/shaders/assignament1_fragment.shader"),
    screenWidth(pScreenWidth), screenHeight(pScreenHeight),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, 
        glm::vec3(pCamera.Position.x, pCamera.Position.y, pCamera.Position.z), 
        glm::vec3(pCamera.Front.x, pCamera.Front.y, pCamera.Front.z))
{
    pointLights =
    {
        { {0.3f, 0.3f, 0.3f}, { 0.7f,  0.2f,  2.0f}},
        { {0.3f, 0.3f, 0.3f}, { 2.3f, -3.3f, -4.0f}},
        { {0.3f, 0.3f, 0.3f}, {-4.0f,  2.0f, -12.0f}},
        { {0.3f, 0.3f, 0.3f}, { 0.0f,  0.0f, -3.0f}}
    };

    objects.push_back(&cubeModel);
    colors.emplace_back(1.0f, 1.0f, 0.0f);
}

SceneClose2GL::~SceneClose2GL() = default;

void SceneClose2GL::OnUpdate(float deltaTime)
{
    view = isLookAt ? pCamera.GetViewMatrix(&cgl::vec3(objects[selectedLookAt]->transform.position)) : pCamera.GetViewMatrix();
    projection = pCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);

    lightingShader.Bind();

    lightingShader.SetUniformMatrix4fv("view", view);
    lightingShader.SetUniformMatrix4fv("projection", projection);

    lightingShader.SetUniform3f("viewPos", pCamera.Position);

    // Lights Sources
    lightingShader.SetUniformLight(dirLight);
    spotlight.position = glm::vec3(pCamera.Position.x, pCamera.Position.y, pCamera.Position.z);
    spotlight.direction = glm::vec3(pCamera.Front.x, pCamera.Front.y, pCamera.Front.z);
    lightingShader.SetUniformLight(spotlight);
    lightingShader.SetUniformLight(pointLights);

    for (int i = 0; i < objects.size(); ++i)
    {
        lightingShader.SetUniform3f("uColor", colors[i]);
        objects[i]->Draw(lightingShader);
    }
}

void SceneClose2GL::OnImGuiRender()
{
    const char* possibleObjects[]{ "COW", "CUBE", "BACKPACK" };
    if (ImGui::Button("Add Object"))
        AddObject(possibleObjects[selectedObjectToAdd]);

    const char* triangleOrientations[]{ "Clock Wise", "Counter Clock Wise" };
    ImGui::Combo("Triangle Orientation", &selectedTriOrientation, triangleOrientations, 2);
    ImGui::Combo("Object to Add", &selectedObjectToAdd, possibleObjects, 3);

    ImGui::Separator();

    lookAtObjects.clear();
    for (int i = 0; i < objects.size(); ++i)
    {
        objects[i]->OnImGui();
        lookAtObjects.push_back(objects[i]->name);
        ImGui::ColorPicker3(std::string("Color of" + objects[i]->name).c_str(), &colors[i][0]);
    }

    ImGui::Checkbox("Look At", &isLookAt);
    ImGui::SameLine();
    if (ImGui::BeginCombo("Object Look At", lookAtObjects[selectedLookAt].c_str()))
    {
        for (int i = 0; i < lookAtObjects.size(); ++i) {
            const bool isSelected = (selectedLookAt == i);
            if (ImGui::Selectable(lookAtObjects[i].c_str(), isSelected)) {
                selectedLookAt = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (ImGui::Button("Enable Cull Face"))
        EnableCullFace();
    ImGui::SameLine();
    if (ImGui::Button("Disable Cull Face"))
        DisableCullFace();
}

void SceneClose2GL::AddObject(std::string label)
{
    TriangleOrientation tri;
    if (selectedTriOrientation == 0)
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

    colors.emplace_back(0.0f, 1.0f, 1.0f);

    // Calculate AABB

}

void SceneClose2GL::EnableCullFace()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void SceneClose2GL::DisableCullFace()
{
    glDisable(GL_CULL_FACE);
}
