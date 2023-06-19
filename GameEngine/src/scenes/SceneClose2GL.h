#pragma once

// Engine
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "line.h"
#include "model.h"
#include "Timer.hpp"

#include <math/vec3.h>

class SceneClose2GL : public Scene_t
{

public:
	SceneClose2GL();
	~SceneClose2GL();
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
};


