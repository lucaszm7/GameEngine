#include "SceneAssigment1.h"

SceneAssigment1::SceneAssigment1()
	:m("resources/models/cow_up_no_text.in", ModelType::CUSTOM),
	lightingShader("resources/shaders/vertex.shader", "resources/shaders/fragment.shader"),
	screenWidth(pScreenWidth), screenHeight(pScreenHeight),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, pCamera->Position, pCamera->Front)
{
    pointLights =
    {
        { {0.3f, 0.3f, 0.3f}, { 0.7f,  0.2f,  2.0f}},
        { {0.3f, 0.3f, 0.3f}, { 2.3f, -3.3f, -4.0f}},
        { {0.3f, 0.3f, 0.3f}, {-4.0f,  2.0f, -12.0f}},
        { {0.3f, 0.3f, 0.3f}, { 0.0f,  0.0f, -3.0f}}
    };
}

SceneAssigment1::~SceneAssigment1()
{

}

void SceneAssigment1::OnUpdate(float deltaTime)
{
	view = pCamera->GetViewMatrix();
	projection = pCamera->GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);

    lightingShader.Bind();

    lightingShader.SetUniformMatrix4fv("view", view);
    lightingShader.SetUniformMatrix4fv("projection", projection);

    lightingShader.SetUniform3f("viewPos", pCamera->Position);

    // Lights Sources
    lightingShader.SetUniformLight(dirLight);
    spotlight.position = pCamera->Position;
    spotlight.direction = pCamera->Front;
    lightingShader.SetUniformLight(spotlight);
    lightingShader.SetUniformLight(pointLights);

    m.Draw(lightingShader);
}

void SceneAssigment1::OnImGuiRender()
{
    m.OnImGui();
}