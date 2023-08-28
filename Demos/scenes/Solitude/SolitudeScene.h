#pragma once

// Engine
#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "Timer.hpp"

#include "Cube.h"

class SolitudeScene : public Scene_t
{
public:
	SolitudeScene();
	~SolitudeScene() final;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	BaseCam* GetCamera() override { return (BaseCam*)&camera; }
private:

	ogl::Camera camera;
	Shader shader;

	std::vector<Model> scene_objects;

	DirectionalLight dirlight;
	SpotLight spotlight;
	std::vector<PointLight> pointlights;
	Cube cube;
};

