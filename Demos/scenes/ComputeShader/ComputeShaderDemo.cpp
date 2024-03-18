#include "ComputeShaderDemo.h"

ComputeShaderDemo::ComputeShaderDemo()
	: computeShader("../GameEngine/resources/shader/compute_shader_demo.shader")
	, mandelbrotShader("../GameEngine/resources/shader/mandelbrot.shader")
	, outputTexture(1000, 1000, Texture::Type::IMAGE)
	, framebuffer(pScreenWidth, pScreenHeight, FrameBuffer::Type::MULTISAMPLE)
{

}

void ComputeShaderDemo::OnUpdate(float deltaTime)
{
	glm::dvec2 vMouse(gen::GameEngine::GetMousePos());

	static bool isHeld = false;

	// Get the position of the mouse and move the world Final Pos - Inital Pos
	// This make us drag Around the Screen Space, with the OffSet variable
	if (gen::GameEngine::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
	{
		if (!isHeld)
		{
			vStartPan = vMouse;
			isHeld = true;
		}
		else
		{
			vOffset -= (vMouse - vStartPan) / vScale;
			vStartPan = vMouse;
		}
	}
	else if (gen::GameEngine::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_1))
	{
		isHeld = false;
	}

	glm::dvec2 vMouseBeforeZoom;
	ScreenToWorld(vMouse, vMouseBeforeZoom);

	if (gen::GameEngine::IsKeyPressed(GLFW_KEY_E)) 
		vScale *= 1.1;
	if (gen::GameEngine::IsKeyPressed(GLFW_KEY_Q))
		vScale *= 0.9;

	glm::dvec2 vMouseAfterZoom;
	ScreenToWorld(vMouse, vMouseAfterZoom);

	vOffset += (vMouseBeforeZoom - vMouseAfterZoom);

	glm::dvec2 pixel_tl = { 0,  1000 };
	glm::dvec2 pixel_br = { 1000, 0};

	glm::dvec2 frac_tl = { -2.0, 1.0 };
	glm::dvec2 frac_br = {  1.0, -1.0 };

	glm::dvec2 frac_real;
	glm::dvec2 frac_imag;

	ScreenToFrac(pixel_tl, pixel_br, frac_tl, frac_br, frac_real, frac_imag);

	float currentFrame = (float)glfwGetTime();

	outputTexture.Bind();
	if (!isMandelbrot)
	{
		computeShader.Bind();
		computeShader.SetUniform1f("time", currentFrame);
		computeShader.Dispatch(1000/10, 1000/10, 1);
		computeShader.Unbind();
	}
	else
	{
		mandelbrotShader.Bind();
		mandelbrotShader.SetUniform1i("MAX_IT_MULTIPLAYER", mandelbrotIterations);
		mandelbrotShader.SetUniform1d("real_x", frac_real.x);
		mandelbrotShader.SetUniform1d("real_y", frac_real.y);
		mandelbrotShader.SetUniform1d("imag_x", frac_imag.x);
		mandelbrotShader.SetUniform1d("imag_y", frac_imag.y);
		mandelbrotShader.Dispatch(1000/10, 1000/10, 1);
		mandelbrotShader.Unbind();
	}
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	outputTexture.Unbind();

	framebuffer.OnRenderTexture(outputTexture);

}

void ComputeShaderDemo::OnImGuiRender()
{
	ImGui::Text("Shader");
	if (ImGui::RadioButton("Example", !isMandelbrot))
	{
		isMandelbrot = false;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Mandelbrot", isMandelbrot))
	{
		isMandelbrot = true;
	}
	if (isMandelbrot)
	{
		ImGui::SliderInt("Iterations", &mandelbrotIterations, 32, 2048);
	}
}

void ComputeShaderDemo::ScreenToWorld(const glm::dvec2& n, glm::dvec2& v)
{
	v.x = (n.x / vScale.x) + vOffset.x;
	v.y = (n.y / vScale.y) + vOffset.y;
}

double map(double x, double oldLow, double oldHigh, double newLow, double newHigh)
{
	double oldRange = (x - oldLow) / (oldHigh - oldLow);
	return oldRange * (newHigh - newLow) + newLow;
}

void ComputeShaderDemo::ScreenToFrac(const glm::dvec2& screen_tl_before, const glm::dvec2& screen_br_before,
	const glm::dvec2& world_tl_before, const glm::dvec2& world_br_before,
	glm::dvec2& world_real_after, glm::dvec2& world_imag_after)
{
	glm::dvec2 screen_tl_after;
	ScreenToWorld(screen_tl_before, screen_tl_after);
	glm::dvec2 screen_br_after;
	ScreenToWorld(screen_br_before, screen_br_after);

	world_real_after.x = map(screen_tl_after.x, screen_tl_before.x, screen_br_before.x, world_tl_before.x, world_br_before.x);
	world_real_after.y = map(screen_br_after.x, screen_tl_before.x, screen_br_before.x, world_tl_before.x, world_br_before.x);

	world_imag_after.x = map(screen_tl_after.y, screen_tl_before.y, screen_br_before.y, world_tl_before.y, world_br_before.y);
	world_imag_after.y = map(screen_br_after.y, screen_tl_before.y, screen_br_before.y, world_tl_before.y, world_br_before.y);
}
