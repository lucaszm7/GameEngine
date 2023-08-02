#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "mesh.h"
#include "mat.hpp"
#include "mat4.h"
#include "vec4.h"
#include "ViewPort.hpp"
#include "model.h"
#include "camera.h"
#include "shader.h"
#include "light.h"
#include "Lines.hpp"
#include "Timer.hpp"


struct Pixel
{
	union
	{
		struct
		{
			unsigned char r, g, b;
		};
		unsigned char p[3];
	};
};

inline std::ostream& operator << (std::ostream& out, const Pixel& p)
{
	return out << (unsigned int)p.r << ','
		<< (unsigned int)p.g << ','
		<< (unsigned int)p.b;
}

template <typename _T>
struct Slope
{
	_T curr;
	_T step;

	// starting position + distance between start & end * how far we have traveled / how far we are going to travel
	// x_start + (x_end - x_start) * [(y - y_start) / (y_end - y_start)]
	// the multiplication gives the t value (percentage traveled)
	// For this Y where is the X?
	Slope(_T begin, _T end, int n_steps)
	{
		float inv_steps = 1.0f / (float)n_steps;
		step = (end - begin) * inv_steps;
		curr = begin;
	}

	_T get() const { return curr; }
	void advance() { curr += step; }
};

class Rasterizer 
{
public:
	static void DrawSoftwareRasterized(
		const Model& model,
		const cgl::Camera& camera,
		DirectionalLight& DirectionalLight,
		PRIMITIVE primitive = PRIMITIVE::Triangle,
		SHADING shading = SHADING::NONE,
		bool showTextures = false,
		bool isCulling = false,
		bool isCullingClockWise = false,
		Texture::Filtering textureFiltering = Texture::Filtering::NEAREST_NEIGHBOR
	);

	static void SetViewPort(const unsigned int screenWidth, const unsigned int screenHeight);
	static void ClearFrameBuffer();
	static void SetClearColor(const Pixel& p) { m_ClearColor = p; };
	static void ClearZBuffer();
	static cgl::mat<Pixel>* GetFrameBuffer() { return &m_FrameBuffer; };

	static double GetTexturingTime() { return timer_fragment_shader.duration(); };

private:
	Rasterizer();
	Rasterizer(const Rasterizer&);

	static void Rasterize(
		std::vector<cgl::vec4>& pixelCoordinates, 
		std::vector<cgl::vec4>& pixelColors, 
		std::vector<cgl::vec4>& pixelNormals,
		std::vector<cgl::vec3>& pixelUVs);

	static void Scanline(unsigned int y, 
		int left_x, int right_x,
		float left_z, float right_z,
		cgl::vec4 color_left, cgl::vec4 color_right,
		cgl::vec4 normal_left, cgl::vec4 normal_right,
		cgl::vec3 uv_left, cgl::vec3 uv_right);

	inline static SHADING m_Shading;
	inline static PRIMITIVE m_Primitive;
	inline static Texture::Filtering m_Filtering;
	inline static bool m_ShowTexture;

	inline static std::shared_ptr<Texture> m_CurrentTexture;

	inline static DirectionalLight m_DirectionalLight;

	inline static unsigned int m_screenWidth;
	inline static unsigned int m_screenHeight;

	inline static std::unique_ptr<Texture> m_TextureToDrawOn;
	inline static std::unique_ptr<ViewPort> m_ViewportToDrawOn;

	inline static Pixel m_ClearColor = Pixel{ 255,255,255 };
	inline static cgl::mat<Pixel> m_FrameBuffer;
	inline static cgl::mat<float> m_ZBuffer;

	inline static Timer timer_fragment_shader;
};