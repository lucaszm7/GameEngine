#include "SolitudeScene.h"

SolitudeScene::SolitudeScene()
	:camera(), shader("resources/shaders/Solitude/vertex.shader", "resources/shaders/Solitude/fragment.shader"),
    spotlight({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f})
{
    scene_objects.emplace_back("resources/models/cube_text.in");
}

SolitudeScene::~SolitudeScene()
{
}

void SolitudeScene::OnUpdate(float deltaTime)
{
    auto view = camera.GetViewMatrix();
    auto projection = camera.GetProjectionMatrix((float)*pScreenWidth / (float)*pScreenHeight);

    shader.Bind();
    shader.SetUniformMatrix4fv("view", view);
    shader.SetUniformMatrix4fv("projection", projection);
    shader.SetUniform3f("viewPos", camera.Position);

    spotlight.position = glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z);
    spotlight.direction = glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z);

    shader.SetUniformLight(spotlight);

    for (const auto& obj : scene_objects)
    {
        obj.Draw(shader);
    }
}

void SolitudeScene::OnImGuiRender()
{

}

