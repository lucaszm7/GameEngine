#include "SolitudeScene.h"

SolitudeScene::SolitudeScene()
	:camera(), shader("resources/shaders/Solitude/vertex.shader", "resources/shaders/Solitude/fragment.shader"),
    dirlight({1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}),
    spotlight({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f})
{
    scene_objects.emplace_back("resources/models/cube_text.in");
    // scene_objects.emplace_back("resources/models/Solitude/Cave.obj");
    scene_objects.emplace_back("resources/models/Solitude/CaveCollision.obj");

    pointlights.push_back({ { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } });
}

SolitudeScene::~SolitudeScene()
{
}

void SolitudeScene::OnUpdate(float deltaTime)
{
    auto view = camera.GetViewMatrix();
    auto projection = camera.GetProjectionMatrix((float)*pScreenWidth / (float)*pScreenHeight);

    for (const auto& pointLight : pointlights)
    {
        cube.Draw(pointLight.position, pointLight.lightColor, camera, (float)*pScreenWidth / (float)*pScreenHeight);
    }

    shader.Bind();
    shader.SetUniformMatrix4fv("view", view);
    shader.SetUniformMatrix4fv("projection", projection);
    shader.SetUniform3f("viewPos", camera.Position);

    spotlight.position = glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z);
    spotlight.direction = glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z);

    shader.SetUniformLight(spotlight);
    shader.SetUniformLight(pointlights);
    shader.SetUniformLight(dirlight);

    for (const auto& obj : scene_objects)
    {
        obj.Draw(shader);
    }
}

void SolitudeScene::OnImGuiRender()
{
    constexpr auto textCentered = [](std::string text, ImVec4 color = { 44.0f / 255.0f, 209.0f / 255.0f, 195.0f / 255.0f, 1.0f }) -> void {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextColored(color, text.c_str());
        };

#ifdef _DEBUG
    textCentered("DEBUG", {0.0f, 1.0f, 0.0f, 1.0f});

    ImGui::Separator();

    textCentered("LIGHTS");
    for (int i = 0; i < pointlights.size(); ++i)
    {
        if (ImGui::TreeNode(std::string("Point Light " + i).c_str()))
        {
            ImGui::DragFloat3(std::string("Pos" + i).c_str(), &pointlights[i].position[0]);
            ImGui::ColorEdit3(std::string("Cor" + i).c_str(), &pointlights[i].lightColor[0]);
            pointlights[i].SetLightColor();
            ImGui::TreePop();
        }
    }

    textCentered("OBJECTS");
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
#endif
}

