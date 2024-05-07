#pragma once

// Engine
#include "ShaderManager.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "Timer.hpp"
#include "ShadowMap.h"

#include "physics/Player.h"
#include "src/Cube.h"

class SolitudeScene : public Scene_t
{
public:
	SolitudeScene();
	~SolitudeScene() final = default;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	void OnPhysics(float deltaTime);

	BaseCam* GetCamera() override { return (BaseCam*)&*camera; }
private:

	std::shared_ptr<ogl::Camera> camera;

	DirectionalLight dirlight;
	SpotLight spotlight;
	ShadowMap shadowMap;

	std::vector<PointLight> pointlights;
	std::shared_ptr<Player> player;

	std::vector<Model> scene_objects;

	bool isGravity = false;
	bool hasCollisions = false;
	bool isDepthMap = false;
	float totalTime = 0.0f;

	std::vector<Collider::Info> infos;

	Cube cube;
};

