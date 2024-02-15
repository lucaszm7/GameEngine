#include "SolitudeScene.h"

SolitudeScene::SolitudeScene()
	:camera(), shader("resources/shaders/Solitude/vertex.shader", "resources/shaders/Solitude/fragment.shader"),
    dirlight({1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}),
    spotlight({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}),
    shadowMap(pScreenWidth, pScreenHeight)
{
    scene_objects.emplace_back("resources/models/Solitude/Cave.obj");
    scene_objects.emplace_back("resources/models/cube_text.in");
    scene_objects.emplace_back("resources/models/cube_text.in");

    camera = std::make_shared<ogl::Camera>();
    player = std::make_shared<Player>(camera);
    pointlights.push_back({ { 1.0f, 0.0f, 0.0f }, {   0.0f,  5.0f,  0.0f } });
    pointlights.push_back({ { 0.0f, 1.0f, 0.0f }, {   0.0f, 10.0f, 42.0f } });
    pointlights.push_back({ { 0.0f, 0.0f, 1.0f }, { -18.0f, 14.0f, 35.0f } });
    pointlights.push_back({ { 1.0f, 1.0f, 1.0f }, { -17.0f, 28.0f, 57.0f } });

    spotlight.intensity = 3.0f;
}

SolitudeScene::~SolitudeScene()
{
}

void SolitudeScene::OnUpdate(float deltaTime)
{
#ifdef _DEBUG
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
#else
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    hasCollisions = true;
#endif

    OnPhysics(deltaTime);

    spotlight.position = glm::vec3(camera->Position.x, camera->Position.y, camera->Position.z);
    spotlight.direction = glm::vec3(camera->Front.x, camera->Front.y, camera->Front.z);

    shadowMap.OnUpdate(spotlight.position, spotlight.direction, spotlight.outerCutOff, scene_objects);

    if (isDepthMap)
    {
        shadowMap.DebugShadowMap();
        return;
    }

    auto view = camera->GetViewMatrix();
    auto projection = camera->GetProjectionMatrix((float)*pScreenWidth / (float)*pScreenHeight);

#ifdef _DEBUG
    for (const auto& pointLight : pointlights)
    {
        cube.Draw(pointLight.position, pointLight.lightColor, *camera, (float)*pScreenWidth / (float)*pScreenHeight);
    }
#endif

    shader.Bind();
    shader.SetUniformMatrix4fv("view", view);
    shader.SetUniformMatrix4fv("projection", projection);
    shader.SetUniformMatrix4fv("lightSpaceMatrix", shadowMap.lightSpaceMatrix);
    shader.SetUniform3f("viewPos", camera->Position);
    shadowMap.Bind(shader, 1);

    shader.SetUniformLight(spotlight);
    shader.SetUniformLight(pointlights);
#ifdef _DEBUG
    shader.SetUniformLight(dirlight);
#endif

    for (const auto& obj : scene_objects)
    {
        obj.Draw(shader);
    }
}

void SolitudeScene::OnPhysics(float deltaTime)
{
    totalTime += deltaTime;
    // Lights
    for (auto& light : pointlights)
    {
        light.intensity = 5 * (1 + std::cos(totalTime * 1/3));
    }

    // Physics
    /*if(isGravity)
        player->camera->velocity += (glm::vec3(0.0f, -0.98f, 0.0f) * deltaTime) / 4.0f;*/

    player->collider.center = player->camera->Position;

    if (hasCollisions)
    {
        auto infos = Collider::CheckCollision(player->collider, scene_objects.front());
        for (unsigned int infoCounter = 0; infoCounter < glm::min((int)115, (int)infos.size()); ++infoCounter)
        {
            player->OnPhysics(infos[infoCounter]);
        }
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

    ImGui::Checkbox("Gravity", &isGravity);
    ImGui::Checkbox("Collisions", &hasCollisions);
    ImGui::Checkbox("Depth Map", &isDepthMap);

    ImGui::DragFloat3("Coll Pos", &player->collider.center[0]);

    ImGui::Text("Collision Count: %d", (int)infos.size());

    ImGui::Separator();
    textCentered("LIGHTS");
    for (int i = 0; i < pointlights.size(); ++i)
    {
        if (ImGui::TreeNode(std::string("Point Light " + i).c_str()))
        {
            pointlights[i].OnImGui(std::to_string(i));
            ImGui::TreePop();
        }
    }
    spotlight.OnImGui("cam");

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

