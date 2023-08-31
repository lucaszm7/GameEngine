#pragma once

// Engine
#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "Timer.hpp"
#include "ShadowMap.h"

#include "src/Player.h"
#include "src/Cube.h"

class SolitudeScene : public Scene_t
{
public:
	SolitudeScene();
	~SolitudeScene() final;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	void OnPhysics(float deltaTime);

	BaseCam* GetCamera() override { return (BaseCam*)&*camera; }
private:

	std::shared_ptr < Player> player;
	std::shared_ptr<ogl::Camera> camera;
	Shader shader;

	ShadowMap shadowMap;

	Model scene;
	std::vector<Model> scene_objects;

	bool isGravity = false;
	bool hasCollisions = false;
	float totalTime = 0.0f;

	DirectionalLight dirlight;
	SpotLight spotlight;
	std::vector<PointLight> pointlights;
	Cube cube;
};

