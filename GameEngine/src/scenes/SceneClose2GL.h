#pragma once

// Engine
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "line.h"
#include "model.h"
#include "Timer.hpp"

#include <mat4.h>
#include <vec4.h>

class SceneClose2GL : public Scene_t
{
public:
	SceneClose2GL();
	~SceneClose2GL() final;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	BaseCam* GetCamera() override { return showDefaultCamera ? (BaseCam*)&oglCamera : (BaseCam*)&cglCamera; }


private:
	Model cubeModel;
	Shader lightingShader;

	cgl::Camera cglCamera;
	ogl::Camera oglCamera;

	cgl::mat4 view;
	cgl::mat4 projection;

	std::shared_ptr<unsigned int> screenWidth;
	std::shared_ptr<unsigned int> screenHeight;

	DirectionalLight dirLight;
	std::vector<PointLight> pointLights;
	SpotLight spotlight;

	std::vector<Model*> objects;
	std::vector<glm::vec3> colors;

	bool isLookAt = false;
	int selectedLookAt = 0;
	std::vector<std::string> lookAtObjects;

	int selectedObjectToAdd = 0;
	int selectedTriOrientation = 1;

	void AddObject(std::string label);
	void EnableCullFace();
	void DisableCullFace();

	bool showDefaultCamera = false;
};


