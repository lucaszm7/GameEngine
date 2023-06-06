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
	Model cubeModel;
	Shader lightingShader;

	glm::mat4 view;
	glm::mat4 projection;

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

};

