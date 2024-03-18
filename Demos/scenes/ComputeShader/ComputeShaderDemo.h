#pragma once

// Engine
#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "Timer.hpp"
#include "Texture.h"
#include "ComputeShader.h"
#include "FrameBuffer.hpp"
#include "GLFW/glfw3.h"
#include "Engine.h"

class ComputeShaderDemo : public Scene_t
{
public:
	ComputeShaderDemo();
	~ComputeShaderDemo() final = default;

	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;

	BaseCam* GetCamera() override { return (BaseCam*)&Camera; }
	ogl::Camera Camera;

private:
	void ScreenToWorld(const glm::dvec2& n, glm::dvec2& v);
	void ScreenToFrac(const glm::dvec2& screen_tl_before, const glm::dvec2& screen_br_before,
		const glm::dvec2& world_tl_before, const glm::dvec2& world_br_before,
		glm::dvec2& world_real_after, glm::dvec2& world_imag_after);

	ComputeShader computeShader;
	ComputeShader mandelbrotShader;
	Texture outputTexture;
	FrameBuffer framebuffer;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int fCounter	= 0;

	bool isMandelbrot = false;
	int mandelbrotIterations = 32;

	glm::dvec2 vStartPan = { 0.0f, 0.0f };
	glm::dvec2 vOffset = { 0.0f, 0.0f };
	glm::dvec2 vScale = { 1.0f, 1.0f };
};


