#pragma once

// Engine
#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "Timer.hpp"

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

	SpotLight spotlight;
};

