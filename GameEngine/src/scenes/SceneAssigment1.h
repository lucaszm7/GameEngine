#pragma once

#include "scene.h"
#include "model.h"

class SceneAssigment1 : public Scene_t
{
public:
	SceneAssigment1();
	~SceneAssigment1();

	void OnUpdate(float deltaTime);
	void OnImGuiRender();

private:
	Model m;
	Shader lightingShader;

	glm::mat4 view;
	glm::mat4 projection;

	std::shared_ptr<unsigned int> screenWidth;
	std::shared_ptr<unsigned int> screenHeight;

	DirectionalLight dirLight;
	std::vector<PointLight> pointLights;
	SpotLight spotlight;


};

