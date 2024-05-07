#pragma once

#include "model.h"
#include "scene.h"
#include "ShaderManager.h"

#include <vector>

class ShowRoom : public Scene_t
{
public:
	ShowRoom();
	~ShowRoom() final = default;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	void OnPhysics(float deltaTime);

	BaseCam* GetCamera() override { return (BaseCam*)&*camera; }

private:

	DirectionalLight dirlight;

	std::shared_ptr<ogl::Camera> camera;
	std::vector<Model> scene_objects;
};

