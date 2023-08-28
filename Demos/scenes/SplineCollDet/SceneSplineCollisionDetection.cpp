#include "SceneSplineCollisionDetection.h"

SceneSplineCollisionDetection::SceneSplineCollisionDetection()
    : 
    screenWidth(pScreenWidth),
    screenHeight(pScreenHeight),
    colon("resources/models/VolumetricSpline.txt"),
    endo("resources/models/VolumetricEndoscope.txt"),
    lightingShader("resources/shaders/vertex.shader", "resources/shaders/fragment.shader"),
    lightSourceShader("resources/shaders/light_vertex.shader", "resources/shaders/light_fragment.shader"),
    dirLight({ 1.0f, 1.0f, 1.0f }, { -0.2f, -1.0f, -0.3f }),
    spotlight({ 1.0f, 1.0f, 1.0f }, pCamera.Position, pCamera.Front)
{
    std::vector<float> cubeVertices =
    {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    // Model backpack("resources/models/backpack/backpack.obj");

    colon.GenerateSplineMesh("resources/textures/4x_tex.png", TriangleOrientation::ClockWise);
    endo.GenerateSplineMesh("resources/textures/black_image.png", TriangleOrientation::ClockWise);

    lightVAO.Bind();
    VertexBuffer lightVBO(&cubeVertices[0], static_cast<unsigned int>(cubeVertices.size() * sizeof(float)), GL_STATIC_DRAW);
    VertexBufferLayout lightVBL;
    lightVBL.Push<float>(3); // positions
    lightVAO.AddBuffer(lightVBO, lightVBL);

    pointLights =
    {
        { {0.3f, 0.3f, 0.3f}, { 0.7f,  0.2f,  2.0f}},
        { {0.3f, 0.3f, 0.3f}, { 2.3f, -3.3f, -4.0f}},
        { {0.3f, 0.3f, 0.3f}, {-4.0f,  2.0f, -12.0f}},
        { {0.3f, 0.3f, 0.3f}, { 0.0f,  0.0f, -3.0f}}
    };
}

SceneSplineCollisionDetection::~SceneSplineCollisionDetection() = default;

void SceneSplineCollisionDetection::OnUpdate(float deltaTime)
{
    endoSplineModel = endo.GetSplineModelTransform();
    colonSplineModel = colon.GetSplineModelTransform();

    collDetTimer.reset_hard();
    if (hasCollisionDetection)
        collDet.CollisionCheck(*(endoSplineModel.get()), *(colonSplineModel.get()));
    collDetTimer.stop();

    view = pCamera.GetViewMatrix();
    projection = pCamera.GetProjectionMatrix((float)*screenWidth / (float)*screenHeight);

    if (debugControlPointsColon || debugControlPointsEndo)
    {
        if (debugControlPointsEndo)
        {
            for (int i = 0; i < endoSplineModel->controlPoints.size() - 1; i+=2)
            {
                auto& controlPointA = endoSplineModel->controlPoints[i];
                auto& controlPointB = endoSplineModel->controlPoints[i + 1];
                Debug::Line::Draw(
                    glm::vec3(controlPointA.x(), controlPointA.y(), controlPointA.z()),
                    glm::vec3(controlPointB.x(), controlPointB.y(), controlPointB.z()), 
                    glm::vec3(0.0f, 1.0f, 0.0f));
            }
            for (int i = 0; i < endoSplineModel->controlPointsVectorPos.size() - 1; i += 2)
            {
                auto& controlPointA = endoSplineModel->controlPoints[i];
                for (int j = 0; j < endoSplineModel->controlPointsVectorPos[0].size() - 1; j += 2)
                {
                    auto& controlPointB = endoSplineModel->controlPointsVectorPos[i][j];
                    Debug::Line::Draw(
                        glm::vec3(controlPointA.x(), controlPointA.y(), controlPointA.z()),
                        glm::vec3(controlPointB.x(), controlPointB.y(), controlPointB.z()),
                        glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
        }
        if (debugControlPointsColon)
        {
            for (int i = 0; i < colonSplineModel->controlPoints.size() - 1; i += 2)
            {
                auto& controlPointA = colonSplineModel->controlPoints[i];
                auto& controlPointB = colonSplineModel->controlPoints[i + 1];
                Debug::Line::Draw(
                    glm::vec3(controlPointA.x(), controlPointA.y(), controlPointA.z()),
                    glm::vec3(controlPointB.x(), controlPointB.y(), controlPointB.z()),
                    glm::vec3(0.0f, 0.0f, 1.0f));
            }
            for (int i = 0; i < colonSplineModel->controlPointsVectorPos.size() - 1; i += 2)
            {
                auto& controlPointA = colonSplineModel->controlPoints[i];
                for (int j = 0; j < colonSplineModel->controlPointsVectorPos[0].size() - 1; j+=2)
                {
                    auto& controlPointB = colonSplineModel->controlPointsVectorPos[i][j];
                    Debug::Line::Draw(
                        glm::vec3(controlPointA.x(), controlPointA.y(), controlPointA.z()),
                        glm::vec3(controlPointB.x(), controlPointB.y(), controlPointB.z()),
                        glm::vec3(0.0f, 0.0f, 1.0f));
                }
            }
        }
    }

    if (debugCollDet && hasCollisionDetection)
    {
        for (int i = 0; !collDet.collisionResults.collisionVectors.empty() &&
                        i < collDet.collisionResults.collisionVectors.size() - 1; i+=2)
        {
            auto& controlPointA = collDet.collisionResults.collisionVectors[i];
            auto& controlPointB = collDet.collisionResults.collisionVectors[i + 1];
            Debug::Line::Draw(
                glm::vec3(controlPointA.x(), controlPointA.y(), controlPointA.z()),
                glm::vec3(controlPointB.x(), controlPointB.y(), controlPointB.z()),
                glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    if (drawAABB)
    {
        DrawNode(collDet.rootColon, glm::vec3(0.0f, 0.0f, 1.0f));
        DrawNode(collDet.rootEndo, glm::vec3(1.0f, 0.0f, 0.0f));
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
    lightingShader.SetUniformLight(pointLights);

    // backpack.Draw(lightingShader);

    if (isDrawingMeshes)
    {
        colon.Draw(lightingShader);
        endo.Draw(lightingShader);
    }

}

void SceneSplineCollisionDetection::OnImGuiRender()
{
    ImGui::Text("Radius: %.2f", endoSplineModel->uniformRadius);
    ImGui::Checkbox("Turn Collision Detection", &hasCollisionDetection);
    ImGui::Checkbox("Draw Meshs", &isDrawingMeshes);
    ImGui::Checkbox("Draw AABBs", &drawAABB);
    if (ImGui::Button("Enable Cull Face"))
        EnableCullFace();
    ImGui::SameLine();
    if (ImGui::Button("Disable Cull Face"))
        DisableCullFace();

    ImGui::Text("Collisions Count %d", collDet.collisionResults.collisionVectors.size());
    ImGui::Text("Collisions Time Taken: %f ms", collDetTimer.duration_ms());

    ImGui::Checkbox("Debug Collision Detection", &debugCollDet);
    ImGui::Checkbox("Debug Control Points Colon", &debugControlPointsColon);
    ImGui::Checkbox("Debug Control Points Endo", &debugControlPointsEndo);

    ImGui::DragInt("Spline Precision", (int*)&collDet.collisionResults.nInterpolatedControlPoints, 1, 1, 1000);
    ImGui::Checkbox("Is Detection Outside Collisions", &collDet.collisionResults.isDetectingOutsideCollisions);

    colon.OnImGui();
    endo.OnImGui();

    if (ImGui::TreeNode("Lights"))
    {
        ImGui::ColorEdit3("Dir Light", (float*)&dirLight.lightColor);
        dirLight.SetLightColor();
        ImGui::DragFloat3("Dir Light", (float*)&dirLight.direction, 1.0f, -10.0f, 10.0f);

        ImGui::ColorEdit3("Spotlight", (float*)&spotlight.lightColor);
        spotlight.SetLightColor();
        ImGui::DragFloat3("Spotlight", (float*)&spotlight.direction, 1.0f, -10.0f, 10.0f);

        for (int i = 0; i < pointLights.size(); ++i)
        {
            ImGui::ColorEdit3(std::to_string(i).c_str(), (float*)&pointLights[i].lightColor);
            pointLights[i].SetLightColor();
            ImGui::DragFloat3(std::to_string(i).c_str(), (float*)&pointLights[i].position, 1.0f, -10.0f, 10.0f);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Endo Control Points"))
    {
        for (int i = 0; i < endoSplineModel->controlPoints.size(); ++i)
        {
            ImGui::Text((std::to_string(i) + ")  %.3f, %.3f, %.3f").c_str(), endoSplineModel->controlPoints[i].x(), endoSplineModel->controlPoints[i].y(), endoSplineModel->controlPoints[i].z());
            if (ImGui::TreeNode(((std::to_string(i) + ". Endo Vectors Control Points")).c_str()))
            {
                for (int j = 0; j < endoSplineModel->controlPointsVectorPos.size(); ++j)
                {
                    ImGui::Text((std::to_string(j) + ")  %.3f, %.3f, %.3f").c_str(), endoSplineModel->controlPointsVectorPos[i][j].x(), endoSplineModel->controlPointsVectorPos[i][j].y(), endoSplineModel->controlPointsVectorPos[i][j].z());
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Collision Points"))
    {
        for (int i = 0; i < collDet.collisionResults.collisionVectors.size(); ++i)
        {
            ImGui::Text((std::to_string(i) + "  %.3f, %.3f, %.3f").c_str(), collDet.collisionResults.collisionVectors[i].x(), collDet.collisionResults.collisionVectors[i].y(), collDet.collisionResults.collisionVectors[i].z());
        }
        ImGui::TreePop();
    }
}

void SceneSplineCollisionDetection::EnableCullFace()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void SceneSplineCollisionDetection::DisableCullFace()
{
    glDisable(GL_CULL_FACE);
}

void SceneSplineCollisionDetection::DrawNode(NodeV7* node, const glm::vec3& color)
{
    DrawAABB(&(node->bv), color);

    if (node->type != NodeType::LEAF)
    {
        if (node->left != nullptr)
            DrawNode(node->left, color);
        if (node->right != nullptr)
            DrawNode(node->right, color);
    }
}

void SceneSplineCollisionDetection::DrawAABB(AABB* aabb, const glm::vec3& color)
{
    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->min.y(), aabb->min.z()), 
        glm::vec3(aabb->max.x(), aabb->min.y(), aabb->min.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->min.y(), aabb->min.z()), 
        glm::vec3(aabb->min.x(), aabb->min.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->min.y(), aabb->min.z()),
        glm::vec3(aabb->min.x(), aabb->max.y(), aabb->min.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->min.y(), aabb->max.z()),
        glm::vec3(aabb->max.x(), aabb->min.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->min.y(), aabb->max.z()),
        glm::vec3(aabb->min.x(), aabb->max.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->max.x(), aabb->min.y(), aabb->max.z()),
        glm::vec3(aabb->min.x(), aabb->min.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->max.x(), aabb->min.y(), aabb->min.z()),
        glm::vec3(aabb->max.x(), aabb->max.y(), aabb->min.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->max.y(), aabb->min.z()),
        glm::vec3(aabb->min.x(), aabb->max.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->min.x(), aabb->max.y(), aabb->min.z()),
        glm::vec3(aabb->max.x(), aabb->max.y(), aabb->min.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->max.x(), aabb->max.y(), aabb->max.z()),
        glm::vec3(aabb->min.x(), aabb->max.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->max.x(), aabb->max.y(), aabb->max.z()),
        glm::vec3(aabb->max.x(), aabb->min.y(), aabb->max.z()), color);

    Debug::Line::Draw(glm::vec3(aabb->max.x(), aabb->max.y(), aabb->max.z()),
        glm::vec3(aabb->max.x(), aabb->max.y(), aabb->min.z()), color);
}

void SceneSplineCollisionDetection::CreateCilinderSpline(const std::string& filePath, int nControlPoints, int nVectorsPerControlPoints, double CorrectionFactor)
{
    std::ofstream file(filePath);
    file << nControlPoints << " " << nVectorsPerControlPoints << std::endl;
    for (int x = -(nControlPoints / 2); x < (nControlPoints / 2); ++x)
    {
        file << (x * CorrectionFactor) << " " << 0 << " " << 0 << std::endl;
        for (int j = 0; j < nVectorsPerControlPoints; ++j)
        {
            double z = -cos(((j * 360) / (nVectorsPerControlPoints - 1)) * (glm::pi<double>() / 180));
            double y = sin(((j * 360) / (nVectorsPerControlPoints - 1)) * (glm::pi<double>() / 180));
            file << 0 << " " << (y * CorrectionFactor) << " " << (z * CorrectionFactor) << std::endl;
        }
    }
    file.close();
}