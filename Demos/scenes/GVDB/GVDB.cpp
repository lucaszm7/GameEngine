#include "GVDB.h"

GVDB::GVDB()
    :
    screenWidth(pScreenWidth),
    screenHeight(pScreenHeight),
    modelA("resources/models/cube.obj"),
    modelB("resources/models/cube.obj"),
    lightingShader("resources/shaders/vertex.shader", "resources/shaders/fragment.shader"),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, pCamera.Position, pCamera.Front)
{
    InitCollisionDetection();

    modelAID = ConstructComplexShape(modelA.meshes.front().vertices.data(), modelA.meshes.front().indices.data(), modelA.meshes.front().indices.size());
    modelBID = ConstructComplexShape(modelB.meshes.front().vertices.data(), modelB.meshes.front().indices.data(), modelB.meshes.front().indices.size());
}

GVDB::~GVDB() = default;

void GVDB::OnUpdate(float deltaTime)
{
    collisionsCount = 0u;
    collDetTimer.reset_hard();

    if (hasCollisionDetection)
    {
        auto max_iterations = 1u;
        UpdateObjectModelMatrix(modelAID, modelA.GetModelMatrix().get_pointer());
        UpdateObjectModelMatrix(modelBID, modelB.GetModelMatrix().get_pointer());
        while (max_iterations-- > 0 && CollisionCheck())
            ++collisionsCount;
    }

    collDetTimer.stop();

    view = pCamera.GetViewMatrix();
    projection = pCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);

    if (debugCollDet && hasCollisionDetection)
    {
        collisionDataList = GetVectorCollisionResult();
        for (int i = 0; !collisionDataList->empty() && i < collisionDataList->size(); i++)
        {
            auto collPoints = collisionDataList->front();
            collisionDataList->pop_front();

            Debug::Line::Draw(
                glm::vec3(collPoints.point1[0], collPoints.point1[1], collPoints.point1[2]),
                glm::vec3(collPoints.point2[0], collPoints.point2[1], collPoints.point2[2]),
                glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    if (drawAABB)
    {
        /*DrawNode(collDet.rootColon, glm::vec3(0.0f, 0.0f, 1.0f));
        DrawNode(collDet.rootEndo, glm::vec3(1.0f, 0.0f, 0.0f));*/
    }

    lightingShader.Bind();

    lightingShader.SetUniformMatrix4fv("view", view);
    lightingShader.SetUniformMatrix4fv("projection", projection);

    lightingShader.SetUniform3f("viewPos", pCamera.Position);

    // Lights Sources
    lightingShader.SetUniformLight(dirLight);
    spotlight.position = pCamera.Position;
    spotlight.direction = pCamera.Front;
    lightingShader.SetUniformLight(spotlight);

    if (isDrawingMeshes)
    {
        modelA.Draw(lightingShader);
        modelB.Draw(lightingShader);
    }
}

void GVDB::OnImGuiRender()
{
    ImGui::Checkbox("Turn Collision Detection", &hasCollisionDetection);

    modelA.OnImGui();
    modelB.OnImGui();

    ImGui::Checkbox("Draw Meshs", &isDrawingMeshes);
    ImGui::Checkbox("Draw AABBs", &drawAABB);
    if (ImGui::Button("Enable Cull Face"))
        EnableCullFace();
    ImGui::SameLine();
    if (ImGui::Button("Disable Cull Face"))
        DisableCullFace();

    ImGui::Text("Collisions Count %d", collisionsCount);
    ImGui::Text("Collisions Time Taken: %f ms", collDetTimer.duration_ms());

    ImGui::Checkbox("Debug Collision Detection", &debugCollDet);

    if (ImGui::TreeNode("Lights"))
    {
        ImGui::ColorEdit3("Dir Light", (float*)&dirLight.lightColor);
        dirLight.SetLightColor();
        ImGui::DragFloat3("Dir Light", (float*)&dirLight.direction, 1.0f, -10.0f, 10.0f);

        ImGui::ColorEdit3("Spotlight", (float*)&spotlight.lightColor);
        spotlight.SetLightColor();
        ImGui::DragFloat3("Spotlight", (float*)&spotlight.direction, 1.0f, -10.0f, 10.0f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Collision Points"))
    {
        for (auto i = 0u; i < collisionDataList->size(); ++i)
        {
            auto it = collisionDataList->begin();
            std::advance(it, i);
            auto& collPoints = *it;
            ImGui::Text(("Point 1: " + std::to_string(i) + "  %.3f, %.3f, %.3f").c_str(), collPoints.point1[0], collPoints.point1[1], collPoints.point1[2]);
        }
        ImGui::TreePop();
    }
}

void GVDB::EnableCullFace()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void GVDB::DisableCullFace()
{
    glDisable(GL_CULL_FACE);
}

