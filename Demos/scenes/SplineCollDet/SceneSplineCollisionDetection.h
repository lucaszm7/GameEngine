#pragma once

// Dependencies
#include <assimp/Importer.hpp>

// Engien
#include "scene.h"
#include "camera.h"
#include "light.h"
// #include "material.h"
#include "Lines.hpp"
// #include "engine/mesh.h"
#include "model.h"
#include "Timer.hpp"

// Spline Coll Det
#include "Spline.hpp"
#include "AccelerationStructures.h"
// #include "spline/SplineModel.hpp"

#include "Plugin.h"

class SceneSplineCollisionDetection : public Scene_t
{
private:
	std::shared_ptr<unsigned int> screenWidth;
	std::shared_ptr<unsigned int> screenHeight;

	ogl::Camera pCamera;

	glm::mat4 view;
	glm::mat4 projection;

	VertexArray lightVAO;
	Shader lightingShader;

	DirectionalLight dirLight;
	SpotLight spotlight;
	
	bool debugCollDet = true;
	bool debugControlPointsColon = false;
	bool debugControlPointsEndo = false;

	bool hasCollisionDetection = true;
	bool isDrawingMeshes = true;
	bool drawAABB = false;
	bool isUsingSplineCollDet = true;

	Spline colon;
	Spline endo;

	std::shared_ptr<SplineModel> endoSplineModel;
	std::shared_ptr<SplineModel> colonSplineModel;

	SplineCollDet collDet;
	Timer collDetTimer;

	void EnableCullFace();
	void DisableCullFace();
	void DrawNode(NodeV7* node, const glm::vec3& color);
	void DrawAABB(AABB* aabb, const glm::vec3& color);
	void CreateCilinderSpline(const std::string& filePath, 
		int nControlPoints, int nVectorsPerControlPoints, double CorrectionFactor);

public:
	SceneSplineCollisionDetection();
	~SceneSplineCollisionDetection();
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	BaseCam* GetCamera() override { return &pCamera; }

};

