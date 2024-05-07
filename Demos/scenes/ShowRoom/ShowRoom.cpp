#include "ShowRoom.h"
#include <Lines.hpp>

ShowRoom::ShowRoom()
    : dirlight({ 1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f })
{
	camera = std::make_shared<ogl::Camera>();
    scene_objects.reserve(100);

    scene_objects.emplace_back(Model::GetDefaultModel(Model::DEFAULT::PLANE));

    auto& planeObj = scene_objects.back();
    planeObj.AddTexture("resources/textures/checker_8x8.jpg");
    planeObj.transform.scale = { 10.0f, 10.0f, 10.0f };

	scene_objects.emplace_back(Model::GetDefaultModel(Model::DEFAULT::CUBE));

    auto& cubeObj = scene_objects.back();
    cubeObj.AddTexture("resources/textures/container2.png");
}

void ShowRoom::OnUpdate(float deltaTime)
{
    auto& shader = ShaderManager::GetShader(SHADER_TYPE::DEFAULT);
    shader.Bind();
    shader.SetUniformMatrix4fv("view", camera->GetViewMatrix());
    shader.SetUniformMatrix4fv("projection", camera->GetProjectionMatrix((float)*pScreenWidth / (float)*pScreenHeight));
    shader.SetUniform3f("viewPos", camera->Position);
    shader.SetUniformLight(dirlight);

    Debug::Line::Draw({ 0.0f, 0.0f, 0.0f }, dirlight.GetDirection(), { 1.0f, 0.0f, 1.0f });

    for (auto& object : scene_objects)
    {
        shader.SetUniform3f("objectColor", object.objectColor);
		object.Draw(shader);
    }
}

void ShowRoom::OnImGuiRender()
{
    constexpr auto textCentered = [](std::string text, ImVec4 color = { 44.0f / 255.0f, 209.0f / 255.0f, 195.0f / 255.0f, 1.0f }) -> void {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextColored(color, text.c_str());
    };

    int selectedObjectToAdd = 0;
    const char* possibleObjects[]{ "CUBE", "COW", "TEAPOT", "DRAGON", "BUNNY", "PLANE"};
    if (ImGui::Combo("Object to Add", &selectedObjectToAdd, possibleObjects, 6))
        scene_objects.emplace_back(Model::GetDefaultModel(Model::to_default_model(possibleObjects[selectedObjectToAdd])));

    ImGui::Separator();
    textCentered("Scene Objects");
    int i = 0;
    for (auto it = scene_objects.begin(); it != scene_objects.end();)
    {
        if (ImGui::TreeNode(std::string((*it).name).c_str()))
        {
            (*it).OnImGui();
            ImGui::TreePop();
        }
        else
            ImGui::SameLine();

        if (ImGui::Button(std::string("Delete " + (*it).name).c_str()))
        {
            it = scene_objects.erase(it);
        }
        else
        {
            it++;
            i++;
        }
    }

    ImGui::Separator();
    textCentered("Light");
    dirlight.OnImGui("Directional Light");

}

void ShowRoom::OnPhysics(float deltaTime)
{

}