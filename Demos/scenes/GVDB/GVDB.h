#pragma once

#include "Plugin.h"
// Dependencies
#include <assimp/Importer.hpp>
#include <list>

#include "scene.h"
#include "camera.h"
#include "light.h"
#include "Lines.hpp"
#include "model.h"
#include "Timer.hpp"


class GVDB : public Scene_t
{
private:
	std::shared_ptr<unsigned int> screenWidth;
	std::shared_ptr<unsigned int> screenHeight;

	Model modelA;
	Model modelB;

	unsigned int modelAID;
	unsigned int modelBID;

	ogl::Camera pCamera;

	glm::mat4 view;
	glm::mat4 projection;

	VertexArray lightVAO;
	Shader lightingShader;

	DirectionalLight dirLight;
	SpotLight spotlight;

	std::list<PluginCollisionData>* collisionDataList;
	unsigned int collisionsCount;

	bool debugCollDet = true;
	bool debugControlPointsColon = false;
	bool debugControlPointsEndo = false;

	bool hasCollisionDetection = true;
	bool isDrawingMeshes = true;
	bool drawAABB = false;
	bool isUsingSplineCollDet = true;

	Timer collDetTimer;

	void EnableCullFace();
	void DisableCullFace();

public:
	GVDB();
	~GVDB();
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	BaseCam* GetCamera() override { return &pCamera; }

};

