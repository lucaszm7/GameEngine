#pragma once

// Engine
#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "Timer.hpp"

class ASCIIMAGE : public Scene_t
{
public:
	ASCIIMAGE();
	~ASCIIMAGE() final = default;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	BaseCam* GetCamera() override { return (BaseCam*)&Camera; }

	ogl::Camera Camera;

	bool light_to_dark = true;
	int shading = 1;
};

